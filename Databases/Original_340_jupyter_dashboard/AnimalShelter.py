import firebase_admin
from firebase_admin import credentials, firestore
import atexit

class AnimalShelter(object):
    """CRUD operations for Animal collection in Firebase Firestore"""

    def __init__(self, _username=None, _password=None):
        # Username/password are not used for Firebase but kept for compatibility
        cred = credentials.Certificate("serviceAccountKey.json")
        firebase_admin.initialize_app(cred)
        self.db = firestore.client()
        self.collection = self.db.collection("animals")
        atexit.register(self.close)

    def getNextRecordNum(self):
        # Get the highest rec_num and add 1
        docs = self.collection.order_by("rec_num", direction=firestore.Query.DESCENDING).limit(1).stream()
        for doc in docs:
            return doc.to_dict().get("rec_num", 0) + 1
        return 1

    def create(self, data):
        if not data:
            raise Exception("Data parameter is empty, entry not created.")

        if isinstance(data, dict):
            data = [data]

        for doc in data:
            if not isinstance(doc, dict):
                raise TypeError("Records must be dictionaries.")
            doc["rec_num"] = self.getNextRecordNum()
            doc.pop("_id", None)
            self.collection.add(doc)

        return True

    def read(self, query):
        if query is None:
            raise Exception("Nothing to search, because query parameter is empty")

        docs = self.collection.stream()
        results = []
        for doc in docs:
            data = doc.to_dict()
            if all(data.get(k) == v for k, v in query.items()):
                results.append(data)
        return results

    def update(self, query, new_values, multiple=True):
        if query is None or new_values is None:
            raise Exception("Nothing to update, because query parameter is empty")

        docs = self.collection.stream()
        modified_count = 0
        for doc in docs:
            data = doc.to_dict()
            if all(data.get(k) == v for k, v in query.items()):
                self.collection.document(doc.id).update(new_values)
                modified_count += 1
                if not multiple:
                    break
        return modified_count

    def delete(self, query, multiple=False):
        if query is None:
            raise Exception("Query must be non-None")

        docs = self.collection.stream()
        deleted_count = 0
        for doc in docs:
            data = doc.to_dict()
            if all(data.get(k) == v for k, v in query.items()):
                self.collection.document(doc.id).delete()
                deleted_count += 1
                if not multiple:
                    break
        return deleted_count

    def close(self):
        # Firebase doesn't need an explicit close
        pass

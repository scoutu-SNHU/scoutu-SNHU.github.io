import firebase_admin
from firebase_admin import credentials, firestore, auth
import atexit
import re


class AnimalShelter(object):
    """
    AnimalShelter class provides CRUD operations for the 'animals' collection in Firestore.
    Includes Firebase authentication and ensures proper validation for secure access.
    """

    def __init__(self, email=None, password=None):
        """
        Initialize the Firestore client and authenticate the user.
        The Firebase Admin SDK is initialized once using a local service account key.
        """
        if not firebase_admin._apps:
            # Load service credentials for Firebase connection
            cred = credentials.Certificate("db-enhancement-340-firebase-adminsdk-fbsvc-828a04652c.json")
            firebase_admin.initialize_app(cred)

        # Firestore client and target collection
        self.db = firestore.client()
        self.collection = self.db.collection("animals")

        # Ensure graceful cleanup on exit
        atexit.register(self.close)

        # Validate and authenticate user
        if email and password:
            if not self.validate_email(email):
                raise ValueError(f"Invalid email format: {email}")
            self.user = self.authenticate_user(email, password)
            if not self.user:
                raise Exception("Authentication failed. Please check your email/password.")
        else:
            raise Exception("Email and password must be provided.")

    # ---------------------------
    # Utility / Validation Methods
    # ---------------------------

    def validate_email(self, email):
        """Validate email format before attempting authentication."""
        pattern = r"[^@]+@[^@]+\.[^@]+"
        return re.match(pattern, email) is not None

    def authenticate_user(self, email, password):
        """
        Authenticate a user based on Firebase Authentication.
        Note: Password validation occurs in Firebase; only email lookup is performed here.
        """
        try:
            user = auth.get_user_by_email(email)
            print(f"Authenticated as: {user.email}")
            return user
        except Exception as e:
            print(f"Login failed: {e}")
            return None

    # ---------------------------
    # CRUD Methods
    # ---------------------------

    def getNextRecordNum(self):
        """
        Retrieve the next available record number.
        Sorts existing entries in descending order by 'rec_num' and increments the highest value.
        """
        docs = self.collection.order_by("rec_num", direction=firestore.Query.DESCENDING).limit(1).stream()
        for doc in docs:
            return doc.to_dict().get("rec_num", 0) + 1
        return 1

    def create(self, data):
        """Insert new document(s) into Firestore, automatically assigning a record number."""
        if not data:
            raise Exception("Data parameter is empty, entry not created.")
        if isinstance(data, dict):
            data = [data]
        for doc in data:
            if not isinstance(doc, dict):
                raise TypeError("Records must be dictionaries.")
            doc["rec_num"] = self.getNextRecordNum()
            doc.pop("_id", None)  # Remove MongoDB legacy field if present
            self.collection.add(doc)
        return True

    def read(self, query):
        """Read all documents matching the specified query parameters."""
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
        """
        Update existing documents in the Firestore collection that match the given query.
        This method searches for documents where all query key-value pairs match the document's fields.
        If `multiple` is True, it updates all matching documents; otherwise, it updates only the first match found. 
        """
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
        """
        Delete document(s) that match the query.
        The 'multiple' flag is local-only and controls whether all matching
        documents are deleted or just the first one.
        """
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
        """Graceful shutdown hook — reserved for future cleanup logic."""
        pass

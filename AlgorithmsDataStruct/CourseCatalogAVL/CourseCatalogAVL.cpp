//============================================================================
// Name        : CourseCatalogAVL.cpp
// Author      : Sonny Coutu
// Description : AVL Tree Course Catalog with debug and traversal modes.
//               Implements efficient insert/search/delete operations for a
//               balanced binary search tree of courses loaded from CSV.
//               AVL was chosen because of standard BST's efficiency loss  
//               when using a pre-insertion, sorted list.
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <limits>

#define NOMINMAX
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std; // using namespace as this is a small project without external non-standard library

bool DEBUG_MODE = true; // Debug mode toggle to show AVL balancing steps


// Data structure representing a course and its prerequisites
struct Course {
    string courseNumber;       // e.g., "CSCI101"
    string courseName;         // e.g., "Introduction to Programming in C++"
    vector<string> preReqs;    // List of prerequisite course IDs
};

class BinarySearchTree {
private:
    // Tree node structure containing a course
    struct Node {
        Course course;
        Node* left;     // Left subtree pointer
        Node* right;    // Right subtree pointer
        int height;     // Height of this subtree (used for balancing)

        Node(const Course& c) : course(c), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root; // Root node of the AVL tree
    int size;   // Number of courses in the tree

    // Helper: Returns node height (0 if null)
    int nodeHeight(Node* n) { return n ? n->height : 0; }

    // Updates height after insert/delete/rotation
    void updateHeight(Node* n) {
        if (n) n->height = 1 + max(nodeHeight(n->left), nodeHeight(n->right));
    }

    // Calculates balance factor for AVL balancing
    int balanceFactor(Node* n) { return n ? nodeHeight(n->left) - nodeHeight(n->right) : 0; }

    // Performs right rotation to maintain AVL balance
    Node* rotateRight(Node* y) {
        if (DEBUG_MODE) cout << "[DEBUG] Rotate Right at " << y->course.courseNumber << endl;
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Performs left rotation to maintain AVL balance
    Node* rotateLeft(Node* x) {
        if (DEBUG_MODE) cout << "[DEBUG] Rotate Left at " << x->course.courseNumber << endl;
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Recursive insert function maintaining AVL balance
    Node* insertRec(Node* node, const Course& c, bool& inserted) {
        if (!node) {
            if (DEBUG_MODE) cout << "[DEBUG] Insert: " << c.courseNumber << endl;
            inserted = true;
            return new Node(c);
        }

        // Traverse to left or right subtree based on course number
        if (c.courseNumber < node->course.courseNumber) {
            if (DEBUG_MODE) cout << "[DEBUG] Going left from " << node->course.courseNumber << endl;
            node->left = insertRec(node->left, c, inserted);
        }
        else if (c.courseNumber > node->course.courseNumber) {
            if (DEBUG_MODE) cout << "[DEBUG] Going right from " << node->course.courseNumber << endl;
            node->right = insertRec(node->right, c, inserted);
        }
        else {
            if (DEBUG_MODE) cout << "[DEBUG] Duplicate course: " << c.courseNumber << endl;
            inserted = false;
            return node; // No duplicates allowed
        }

        // Update height and check for AVL balance
        updateHeight(node);
        int bf = balanceFactor(node);
        if (DEBUG_MODE) cout << "[DEBUG] Balance factor at " << node->course.courseNumber << " is " << bf << endl;

        // Perform necessary rotations (acceptable balance factor threshold is 0 or |1| )
        if (bf > 1 && c.courseNumber < node->left->course.courseNumber) return rotateRight(node);
        if (bf < -1 && c.courseNumber > node->right->course.courseNumber) return rotateLeft(node);
        if (bf > 1 && c.courseNumber > node->left->course.courseNumber) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (bf < -1 && c.courseNumber < node->right->course.courseNumber) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // Find node with smallest value in subtree
    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current && current->left) current = current->left;
        return current;
    }

    // Recursive delete function maintaining AVL balance
    Node* deleteRec(Node* root, const string& courseNumber, bool& deleted) {
        if (!root) return root;

        if (courseNumber < root->course.courseNumber) {
            if (DEBUG_MODE) cout << "[DEBUG] Going left to delete " << courseNumber << endl;
            root->left = deleteRec(root->left, courseNumber, deleted);
        }
        else if (courseNumber > root->course.courseNumber) {
            if (DEBUG_MODE) cout << "[DEBUG] Going right to delete " << courseNumber << endl;
            root->right = deleteRec(root->right, courseNumber, deleted);
        }
        else {
            if (DEBUG_MODE) cout << "[DEBUG] Deleting " << courseNumber << endl;
            deleted = true;

            // Node with one or no child
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                }
                else *root = *temp;
                delete temp;
            }
            else {
                // Node with two children: get inorder successor
                Node* temp = minValueNode(root->right);
                root->course = temp->course;
                root->right = deleteRec(root->right, temp->course.courseNumber, deleted);
            }
        }

        if (!root) return root; 

        // Update height and rebalance
        updateHeight(root);
        int bf = balanceFactor(root); 
        if (DEBUG_MODE) cout << "[DEBUG] Balance factor after deletion at " << root->course.courseNumber << " is " << bf << endl;
        
        // acceptable threshold 0 or |1| again
        if (bf > 1 && balanceFactor(root->left) >= 0) return rotateRight(root);
        if (bf > 1 && balanceFactor(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (bf < -1 && balanceFactor(root->right) <= 0) return rotateLeft(root);
        if (bf < -1 && balanceFactor(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    // Recursive search for a course by ID
    Course searchRec(Node* node, const string& courseId) {
        if (!node) return {};
        if (node->course.courseNumber == courseId) return node->course;
        if (courseId < node->course.courseNumber) return searchRec(node->left, courseId);
        return searchRec(node->right, courseId);
    }

    // Tree traversal functions
    void inOrderRec(Node* node) {
        if (!node) return;
        inOrderRec(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseName << endl;
        inOrderRec(node->right);
    }

    void preOrderRec(Node* node) {
        if (!node) return;
        cout << node->course.courseNumber << ", " << node->course.courseName << endl;
        preOrderRec(node->left);
        preOrderRec(node->right);
    }

    void postOrderRec(Node* node) {
        if (!node) return;
        postOrderRec(node->left);
        postOrderRec(node->right);
        cout << node->course.courseNumber << ", " << node->course.courseName << endl;
    }

    // Deletes all nodes in the subtree (used in destructor)
    void deleteSubtree(Node* node) {
        if (!node) return;
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        delete node;
    }

public:
    BinarySearchTree() : root(nullptr), size(0) {}
    ~BinarySearchTree() { deleteSubtree(root); }

    // Public traversal wrappers
    void InOrder() { inOrderRec(root); }
    void PreOrder() { preOrderRec(root); }
    void PostOrder() { postOrderRec(root); }

    // Insert a course into the AVL tree
    void Insert(const Course& aCourse) {
        bool inserted = false;
        root = insertRec(root, aCourse, inserted);
        if (inserted) ++size;
    }

    // Delete a course from the AVL tree
    bool Delete(const string& courseNumber) {
        bool deleted = false;
        root = deleteRec(root, courseNumber, deleted);
        if (deleted) --size;
        return deleted;
    }

    // Search for a course by ID
    Course Search(string courseId) {
        transform(courseId.begin(), courseId.end(), courseId.begin(), ::toupper);
        return searchRec(root, courseId);
    }

    int Size() { return size; }
};

// Efficient CSV Split using stringstream
vector<string> Split(const string& lineFeed) {
    vector<string> tokens;
    stringstream ss(lineFeed);
    string token;
    while (getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    return tokens;
}

// Convert string to uppercase
void convertCase(string& toConvert) {
    transform(toConvert.begin(), toConvert.end(), toConvert.begin(),
        [](unsigned char c) { return toupper(c); });
}

// Load courses from CSV file
void loadCourses(const string& filePath, BinarySearchTree* courseList) {
    ifstream inFS(filePath);
    string line;
    if (!inFS.is_open()) {
        cout << "Could not open file (" << filePath << ")." << endl;
        return;
    }
    while (getline(inFS, line)) {
        if (line.empty()) continue;
        vector<string> tokens = Split(line);
        if (tokens.size() < 2) continue;
        Course aCourse{ tokens[0], tokens[1], {} };
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) aCourse.preReqs.push_back(tokens[i]);
        }
        courseList->Insert(aCourse);
    }
    inFS.close();
}

// Display a course and its prerequisites
void displayCourse(const Course& aCourse) {
    cout << aCourse.courseNumber << ", " << aCourse.courseName << endl;
    cout << "Prerequisites: ";
    if (aCourse.preReqs.empty()) cout << "none" << endl;
    else {
        for (size_t i = 0; i < aCourse.preReqs.size(); ++i) {
            cout << aCourse.preReqs[i];
            if (i + 1 < aCourse.preReqs.size()) cout << ", ";
        }
        cout << endl;
    }
}

// ======================== MAIN ========================
int main(int argc, char* argv[]) {
    string filePath, courseKey;

    // Allow optional command-line file path argument
    switch (argc) {
    case 2: filePath = argv[1]; break;
    case 3: filePath = argv[1]; courseKey = argv[2]; break;
    default: filePath = "courses.csv"; // bundled file used for testing.
    }

    BinarySearchTree* courseList = new BinarySearchTree();
    Course course;
    bool readOnce = false; // Sentinel, as to not add courseList repeatedly.
    int choice = 0;

    while (choice != 9) {
        cout << "\nMenu:\n";
        cout << "  1. Load Courses\n";
        cout << "  2. Display All Courses (InOrder)\n";
        cout << "  3. Find Course\n";
        cout << "  4. Delete Course\n";
        cout << "  5. Display PreOrder\n";
        cout << "  6. Display PostOrder\n";
        cout << "  7. Toggle Debug Mode\n";
        cout << "  9. Exit\n";
        cout << "Enter choice: ";

        courseKey.clear();
        choice = 0;

        try {
            if (!(cin >> choice)) throw 1;
            if (!((choice > 0 && choice < 8) || choice == 9)) throw 1;

            switch (choice) {
            case 1:
                if (!readOnce) {
                    loadCourses(filePath, courseList);
                    cout << courseList->Size() << " courses loaded." << endl;
                    readOnce = true;
                }
                else cout << "Courses already loaded.\n";
                break;

            case 2:
                if (readOnce) courseList->InOrder();
                else cout << "Load courses first.\n";
                break;

            case 3:
                cout << "Enter course identifier: ";
                cin >> courseKey;
                convertCase(courseKey);
                course = courseList->Search(courseKey);
                if (!course.courseNumber.empty()) displayCourse(course);
                else cout << "Course not found.\n";
                break;

            case 4:
                if (readOnce) {
                    cout << "Enter course to delete: ";
                    cin >> courseKey;
                    convertCase(courseKey);
                    if (courseList->Delete(courseKey)) cout << "Deleted " << courseKey << endl;
                    else cout << "Course not found.\n";
                }
                else cout << "Load courses first.\n";
                break;

            case 5:
                if (readOnce) courseList->PreOrder();
                else cout << "Load courses first.\n";
                break;

            case 6:
                if (readOnce) courseList->PostOrder();
                else cout << "Load courses first.\n";
                break;

            case 7: // Toggle
                DEBUG_MODE = !DEBUG_MODE;
                cout << "Debug mode " << (DEBUG_MODE ? "ON" : "OFF") << endl;
                break;

            case 9: break;
            
            default: throw 1;
            }
        }
        catch (...) {
            cout << "Invalid input.\n";
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Fully clear buffer until carr_return
#ifdef _WIN32  // Windows OS only
        system("pause");
        system("cls");
#endif
    }

    cout << "Thank you for using the Course Catalog!\n";
    delete courseList;
    return 0;
}
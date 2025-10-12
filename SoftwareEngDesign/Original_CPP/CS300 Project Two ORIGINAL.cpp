//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Sonny Coutu
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Application that reads courses from a pre-arranged file and 
//               will display one or all & their requirements. 
//               Using a tree structure.
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>

using namespace std;

// Struct to hold the course information
struct Course {

    string courseNumber;
    string courseName;
    vector<string> preReqs;
};

class BinarySearchTree {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        Node* right;
        Node* left;

        // default constructor
        Node() {
            left = nullptr;
            right = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) {
            course = aCourse;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;
    void inOrder(Node* node); // inOrder will display course in alphanumerical, already sorted by insert algorithm
    int size = 0;

public:
    BinarySearchTree();
    void InOrder(); 
    void Insert(Course aCourse);
    Course Search(string courseId);
    int Size();
};

 // Default constructor
BinarySearchTree::BinarySearchTree() {
    this->root = nullptr;
}

//Traverse the tree in order (pre-sorted by insert).
void BinarySearchTree::InOrder() {
    inOrder(root);
}

/**
 * Insert a course.
 * This will use courseNumber to "sort" itself in the tree at insertion(inherent benefit of a tree structure),
 * making a post-insert sort function unneeded for this project.
 */
void BinarySearchTree::Insert(Course aCourse) {

    Node* currentNode = root;

    if (root == NULL) {

        root = new Node(aCourse);

    }
    else {

        while (currentNode != NULL) {

            if (aCourse.courseNumber < currentNode->course.courseNumber) {

                if (currentNode->left == nullptr) {

                    currentNode->left = new Node(aCourse);
                    currentNode = NULL;
                }
                else {

                    currentNode = currentNode->left;
                }
            }
            else {

                if (currentNode->right == nullptr) {

                    currentNode->right = new Node(aCourse);
                    currentNode = NULL;
                }
                else {

                    currentNode = currentNode->right;
                }
            }
        }
    }
    size++;
}

 //  Search for a course
Course BinarySearchTree::Search(string courseId) {

    Course aCourse;

    Node* currentNode = root;

    while (currentNode != NULL) {

        if (currentNode->course.courseNumber == courseId) {

            return currentNode->course;
        }
        else if (courseId < currentNode->course.courseNumber) {

            currentNode = currentNode->left;
        }
        else {

            currentNode = currentNode->right;
        }
    }
    return aCourse;
}

//  Print the tree: left then root then right.
void BinarySearchTree::inOrder(Node* node) {

    if (node == NULL) {

        return;
    }
    inOrder(node->left);

    //  print the node
    cout << node->course.courseNumber << ", " << node->course.courseName << endl;

    inOrder(node->right);
}

int BinarySearchTree::Size() {

    return size;
}

/*
credit: www.codegrepper.com/code-examples/cpp/c%2B%2B+split+string+by+delimiter
splits a string into multiple strings, returning a vector<string> object
*/
vector<string> Split(string lineFeed) {

    char delimiter = ',';

    lineFeed += delimiter; //  includes a delimiter at the end so last word is also read
    vector<string> lineTokens;
    string temp = "";
    for (int i = 0; i < lineFeed.length(); i++)
    {
        if (lineFeed[i] == delimiter)
        {
            lineTokens.push_back(temp); //  found delim token, store completed words in token vector
            temp = "";
            i++;
        }
        temp += lineFeed[i];
    }
    return lineTokens;
}

//  function to load courses
void loadCourses(string filePath, BinarySearchTree* courseList) {
    
    ifstream inFS;
    string line; //  line feed 
    vector<string> stringTokens;

    try
    {
       inFS.open(filePath); 

       if (!inFS.is_open()) { //  small error handler
            cout << "Could not open file( " << filePath <<" ). Please check inputs. " << endl;
            return;
        }

        while (!inFS.eof()) {

            Course aCourse;//  create a new struct for each "line"

            getline(inFS, line);
            stringTokens = Split(line); //  split the line into tokens via the delimiter

            if (stringTokens.size() < 2) { //  if there aren't at least 2 tokens the line is misformatted

                cout << "\nError. Skipping line." << endl;
            }

            else {

                aCourse.courseNumber = stringTokens.at(0);
                aCourse.courseName = stringTokens.at(1);

                for (unsigned int i = 2; i < stringTokens.size(); i++) { // token for prereqs are at 2 ++

                    aCourse.preReqs.push_back(stringTokens.at(i));
                }

                //  push this course to the end
                courseList->Insert(aCourse);
            }
        }

        inFS.close(); 
    }
    catch (const std::exception&)
    {
       // if (!inFS.is_open()) {//small error handler
            cout << "Could not open file. Please check inputs. " << endl;
            
        
    }
}

/*
Pass a course object in for display
Print: courseId, courseName, Prereq list 
*/
void displayCourse(Course aCourse) {

    cout << aCourse.courseNumber << ", " << aCourse.courseName << endl;
    cout << "Prerequisites: ";

    if (aCourse.preReqs.empty()) {//  if the list is empty then there are no prereq

        cout << "none" << endl;
    }
    else {

        for (unsigned int i = 0; i < aCourse.preReqs.size(); i++) {

            cout << aCourse.preReqs.at(i);

            if (aCourse.preReqs.size() > 1 && i < aCourse.preReqs.size() - 1) {//  put a comma for any elements greater than 1

                cout << ", "; 
            }
        }
    }

    cout << endl;
}

/*
Pass the string by reference and change the case of any alpha to upper
Simplifying comparisons
*/
void convertCase(string& toConvert) {

    for (unsigned int i = 0; i < toConvert.length(); i++) {

        if (isalpha(toConvert[i])) {

            toConvert[i] = toupper(toConvert[i]);
        }
    }
}

int main(int argc, char* argv[]) {

    // process command line arguments
    string filePath, courseKey;

    switch (argc) {
    case 2:
        filePath = argv[1];
        break;
    case 3:
        filePath = argv[1];
        courseKey = argv[2];
        break;
    default:
        filePath = "ABCU_Advising_Program_Input.txt";
    }

    // Define a table to hold all the courses
    BinarySearchTree* courseList = new BinarySearchTree();

    Course course;
    bool readOnce = false; // switch sentinel
    int choice = 0;

    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Courses" << endl;
        cout << "  2. Display All Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";

        courseKey = ""; //  re-initialize       
        choice = 0; //  re-initialize 

        try {
            cin >> choice;

            if (!((choice > 0 && choice < 5) || (choice == 9))) {// limit the user menu inputs to good values
                throw 1;
            }

            switch (choice) {
            case 1: // loading courses into tree

                // Call method to load courses
                if (!readOnce) { // Not adding courses more than once per application running.
                    loadCourses(filePath, courseList);
                    cout << courseList->Size() << " courses read" << endl;
                    readOnce = true;
                }
                else {
                    cout << "Courses already added to list.";
                } 
                cout << endl;
                system("pause");

                break;

            case 2: // displaying tree structure using inOrder transversal
                if (readOnce) {
                    courseList->InOrder();
                }
                else {
                    cout << "No data entered yet, please load courses first.";
                }
                cout << endl;
                system("pause");

                break;

            case 3: // display single course info, ID entered by user

                cout << "\nEnter the course identifier you would like to inquire about: " << endl;
                cin >> courseKey;

                convertCase(courseKey); //  convert input to uppercase 

                course = courseList->Search(courseKey);

                if (!course.courseNumber.empty()) {
                    displayCourse(course);
                }
                else {
                    cout << "\nCourse \"" << courseKey << "\" not found." << endl;
                }

                system("pause");

                break;

            case 9:
                exit;
                break;

            default:

                throw 1;
            }
        }

        catch (int err) {

            std::cout << "\nPlease enter a valid choice." << endl;
            system("pause");
        }

        // Clearing the buffer of additional "garbage"
        cin.clear();
        cin.ignore();

        // Clear screen to remove previous information
        system("cls");
    }

    cout << "Good bye." << endl;
    return 0;
}
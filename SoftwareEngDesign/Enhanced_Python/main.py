"""
main.py

Author: Sonny Coutu

Main entry point for the Course Catalog BST application.

Provides a menu-driven console interface to:
- Load courses from CSV files
- Add, search, display, and delete courses
- View multiple tree traversals
- Save courses back to CSV
- Displays prerequisites with warnings if missing
"""

from bst import BST
from parser import load_courses_from_csv
from course import Course
from tabulate import tabulate
from colorama import init, Fore, Style
import csv
import re

# Initialize colorama for colored console output
init(autoreset=True)

def display_courses_table(courses, course_tree=None):
    """
    Display a list of courses in a colored table format.

    Args:
        courses (list[Course]): List of Course objects to display.
        course_tree (BST, optional): BST to check if prerequisites exist.
    """
    table = []
    for c in courses:
        colored_prereqs = []
        for prereq in c.prerequisites:
            if course_tree and not course_tree.search(prereq):
                colored_prereqs.append(Fore.RED + prereq)
            else:
                colored_prereqs.append(Fore.YELLOW + prereq)
        prereq_str = ", ".join(colored_prereqs) if colored_prereqs else Fore.YELLOW + "None"
        table.append([Fore.CYAN + c.course_id, Fore.GREEN + c.title, prereq_str])
    print(tabulate(table, headers=[Fore.CYAN + "Course ID", Fore.GREEN + "Title", Fore.YELLOW + "Prerequisites"], tablefmt="grid"))

def print_menu():
    """Print the main menu with color formatting."""
    print(Fore.CYAN + Style.BRIGHT + "\n=== Course Catalog Menu ===")
    print(Fore.YELLOW + "1. Load courses from file")
    print(Fore.YELLOW + "2. Display all courses (in-order)")
    print(Fore.YELLOW + "3. Find a course")
    print(Fore.YELLOW + "4. Add a course manually")
    print(Fore.YELLOW + "5. Delete a course")
    print(Fore.YELLOW + "6. Display all traversals (in/pre/post/level-order)")
    print(Fore.YELLOW + "7. Save courses to CSV")
    print(Fore.RED + "8. Exit\n")

def main():
    """
    Main program loop for interacting with the course catalog.

    Handles user input, menu navigation, and operations on the BST.
    """
    course_tree = BST()

    while True:
        print_menu()
        choice = input(Fore.CYAN + "Enter your choice (1-8): ").strip()

        # --- Load courses ---
        if choice == "1":
            filename = input(Fore.CYAN + "Enter CSV filename: ").strip()
            courses = load_courses_from_csv(filename)
            for c in courses:
                for prereq in c.prerequisites:
                    if not course_tree.search(prereq):
                        print(Fore.MAGENTA + f"Warning: Prerequisite '{prereq}' not found for course '{c.course_id}'")
                course_tree.insert(c)
            print(Fore.GREEN + f"Loaded {len(courses)} courses.")

        # --- Display all courses ---
        elif choice == "2":
            if course_tree.size() > 0:
                display_courses_table(course_tree.inorder(), course_tree)
            else:
                print(Fore.RED + "No courses loaded yet.")

        # --- Find a course ---
        elif choice == "3":
            if course_tree.size() > 0:
                course_id = input(Fore.CYAN + "Enter course ID or partial ID: ").strip().upper()
                results = sorted([c for c in course_tree.inorder() if course_id in c.course_id], key=lambda x: x.course_id)
                if results:
                    display_courses_table(results, course_tree)
                else:
                    print(Fore.RED + f"No courses matching '{course_id}' found.")
            else:
                print(Fore.RED + "No courses loaded yet.")

        # --- Add a course manually ---
        elif choice == "4":
            course_id = input(Fore.CYAN + "Enter new course ID (e.g., CSCI101): ").strip().upper()

            # Validate that course ID follows format: 4 uppercase letters + 3 digits
            if not re.match(r"^[A-Z]{4}\d{3}$", course_id):
                print(Fore.RED + "Invalid course ID format. Must be 4 letters followed by 3 digits.")
            elif course_tree.search(course_id):
                print(Fore.RED + f"Course '{course_id}' already exists.")
            else:
                title = input(Fore.CYAN + "Enter course title: ").strip()
                prereqs_input = input(Fore.CYAN + "Enter prerequisites (comma-separated), or leave empty: ").strip()
                prereqs = [p.strip().upper() for p in prereqs_input.split(",") if p.strip()] if prereqs_input else []

                for p in prereqs:
                    if not course_tree.search(p):
                        print(Fore.MAGENTA + f"Warning: Prerequisite '{p}' not found in current catalog.")

                new_course = Course(course_id, title, prereqs)
                course_tree.insert(new_course)
                print(Fore.GREEN + f"Course '{course_id}' added successfully.")

        # --- Delete a course ---
        elif choice == "5":
            if course_tree.size() > 0:
                course_id = input(Fore.CYAN + "Enter course ID to delete: ").strip().upper()
                course = course_tree.search(course_id)
                if course:
                    confirm = input(Fore.MAGENTA + f"Are you sure you want to delete '{course_id}'? (y/n): ").strip().lower()
                    if confirm == "y":
                        course_tree.delete(course_id)
                        print(Fore.GREEN + f"Course '{course_id}' deleted.")
                    else:
                        print(Fore.YELLOW + "Deletion canceled.")
                else:
                    print(Fore.RED + f"Course '{course_id}' not found.")
            else:
                print(Fore.RED + "No courses loaded yet.")

        # --- Display all traversals ---
        elif choice == "6":
            if course_tree.size() > 0:
                print(Fore.CYAN + Style.BRIGHT + "\nIn-Order Traversal:")
                display_courses_table(course_tree.inorder(), course_tree)
                print(Fore.CYAN + Style.BRIGHT + "\nPre-Order Traversal:")
                display_courses_table(course_tree.preorder(), course_tree)
                print(Fore.CYAN + Style.BRIGHT + "\nPost-Order Traversal:")
                display_courses_table(course_tree.postorder(), course_tree)
                print(Fore.CYAN + Style.BRIGHT + "\nLevel-Order Traversal:")
                display_courses_table(course_tree.level_order(), course_tree)
            else:
                print(Fore.RED + "No courses loaded yet.")

        # --- Save to CSV ---
        elif choice == "7":
            if course_tree.size() > 0:
                filename = input(Fore.CYAN + "Enter filename to save courses: ").strip()
                try:
                    with open(filename, "w", newline="") as f:
                        writer = csv.writer(f)
                        for c in course_tree.inorder():
                            writer.writerow([c.course_id, c.title] + c.prerequisites)
                    print(Fore.GREEN + f"Courses saved to '{filename}'.")
                except Exception as e:
                    print(Fore.RED + f"Error saving file: {e}")
            else:
                print(Fore.RED + "No courses loaded yet.")

        # --- Exit ---
        elif choice == "8":
            print(Fore.RED + Style.BRIGHT + "Goodbye!")
            break

        else:
            print(Fore.RED + "Invalid choice. Please enter a number between 1 and 8.")

if __name__ == "__main__":
    main()

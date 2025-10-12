"""
parser.py

Load courses from CSV files with course ID validation and prerequisite checks.
"""

import csv
import re
from course import Course

def is_valid_course_id(course_id):
    """
    Check if a course ID is valid (4 letters followed by 3 digits).

    Args:
        course_id (str): The course ID to validate.

    Returns:
        bool: True if valid, False otherwise.
    """
    return re.fullmatch(r"[A-Z]{4}\d{3}", course_id.strip().upper()) is not None

def load_courses_from_csv(filename):
    """
    Load courses from a CSV file into Course objects.

    Each row must contain a course ID and title; additional columns
    are treated as prerequisites. Rows with invalid course IDs or
    insufficient data are skipped with a warning.

    Args:
        filename (str): Path to the CSV file.

    Returns:
        list[Course]: List of valid Course objects.
    """
    courses = []
    try:
        with open(filename, newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if len(row) < 2:
                    print(f"Row is invalid and has been ignored: {row}")
                    continue
                course_id = row[0].upper()
                title = row[1]
                prereqs = [p.upper() for p in row[2:]] if len(row) > 2 else []

                if not is_valid_course_id(course_id):
                    print(f"Warning: Course ID '{course_id}' is invalid and has been ignored")
                    continue

                courses.append(Course(course_id, title, prereqs))
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
    except Exception as e:
        print(f"Error reading file '{filename}': {e}")
    return courses

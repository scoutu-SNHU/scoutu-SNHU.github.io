"""
course.py

Defines the Course class to represent an academic course.
Stores course ID, title, and prerequisites.
Used throughout the advising system as the core data object
managed by the BST.
"""

class Course:
    """Represents a single course in the catalog."""

    def __init__(self, course_id, title, prerequisites=None):
        """
        Initialize a Course object.

        Args:
            course_id (str): The unique course identifier (e.g., 'CSCI101').
            title (str): The course title.
            prerequisites (list[str], optional): List of prerequisite course IDs.
        """
        self.course_id = course_id.upper()
        self.title = title
        self.prerequisites = prerequisites if prerequisites else []

    def __str__(self):
        """
        Return a readable string representation of the course,
        including its prerequisites.
        """
        prereq_str = ", ".join(self.prerequisites) if self.prerequisites else "None"
        return f"{self.course_id}: {self.title} | Prerequisites: {prereq_str}"

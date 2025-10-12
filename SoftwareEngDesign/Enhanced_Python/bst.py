"""
bst.py

Binary Search Tree (BST) implementation for Course objects.
Supports insert, search, delete, and multiple traversals
(in-order, pre-order, post-order, and level-order).
Tracks size for validation and menu-driven operations.
"""

from collections import deque

class Node:
    """Node in the BST for storing a Course object."""

    def __init__(self, course):
        self.course = course
        self.left = None
        self.right = None

class BST:
    """Binary Search Tree for storing courses by course ID."""

    def __init__(self):
        self.root = None
        self._size = 0  # Tracks number of courses in the tree

    def size(self):
        """Return the current number of courses in the BST."""
        return self._size

    def insert(self, course):
        """
        Insert a course into the BST. Duplicate course IDs are ignored.
        """
        if self.root is None:
            self.root = Node(course)
            self._size += 1
            return

        current = self.root
        while True:
            if course.course_id == current.course.course_id:
                return  # Duplicate detected, do not insert again
            elif course.course_id < current.course.course_id:
                if current.left is None:
                    current.left = Node(course)
                    self._size += 1
                    return
                current = current.left
            else:
                if current.right is None:
                    current.right = Node(course)
                    self._size += 1
                    return
                current = current.right

    def search(self, course_id):
        """
        Search course by ID (case-insensitive). Returns Course object if found, else None.
        """
        course_id = course_id.upper()
        current = self.root
        while current:
            if course_id == current.course.course_id:
                return current.course
            elif course_id < current.course.course_id:
                current = current.left
            else:
                current = current.right
        return None

    def inorder(self):
        """Return list of courses in in-order traversal (sorted by ID)."""
        result = []

        def _inorder(node):
            if not node:
                return
            _inorder(node.left)
            result.append(node.course)  # Visit node in the middle
            _inorder(node.right)

        _inorder(self.root)
        return result

    def preorder(self):
        """Return list of courses in pre-order traversal."""
        result = []

        def _preorder(node):
            if not node:
                return
            result.append(node.course)  # Visit node first
            _preorder(node.left)
            _preorder(node.right)

        _preorder(self.root)
        return result

    def postorder(self):
        """Return list of courses in post-order traversal."""
        result = []

        def _postorder(node):
            if not node:
                return
            _postorder(node.left)
            _postorder(node.right)
            result.append(node.course)  # Visit root last

        _postorder(self.root)
        return result

    def level_order(self):
        """Return list of courses in level-order (Breadth-first search)."""
        result = []
        if not self.root:
            return result
        queue = deque([self.root])
        while queue:
            node = queue.popleft()
            result.append(node.course)
            if node.left:
                queue.append(node.left)
            if node.right:
                queue.append(node.right)
        return result

    def delete(self, course_id):
        """
        Delete a course by ID. Adjusts tree size if deletion occurs.
        Does nothing if course is not found.
        """

        course_id = course_id.upper()
        deleted = False  # Flag to track if deletion happened

        def _delete(node, course_id):
            nonlocal deleted
            if not node:
                return None
            if course_id < node.course.course_id:
                node.left = _delete(node.left, course_id)
            elif course_id > node.course.course_id:
                node.right = _delete(node.right, course_id)
            else:
                # Node to delete found
                deleted = True
                if not node.left:
                    return node.right
                if not node.right:
                    return node.left
                # Node has two children: replace with in-order successor
                succ = node.right
                while succ.left:
                    succ = succ.left
                node.course = succ.course
                node.right = _delete(node.right, succ.course.course_id)
            return node

        self.root = _delete(self.root, course_id)
        if deleted:
            self._size -= 1
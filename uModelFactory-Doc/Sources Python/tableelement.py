from textdocx import textDocx


class tableElement:
    def __init__(self, col: int, row: int, item: textDocx):
        self.mColumn = col
        self.mRow = row
        self.mVoid = False
        self.mItem = item

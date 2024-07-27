from textdocx import textDocx


class listDocx:
    def __init__(self):
        self.mItems = None
        self.mListItems = []

    def addItem(self, item: textDocx):
        self.mListItems.append(item)

    def getItem(self):
        item = self.mListItems.pop()
        return item

    def hasItems(self):
        if len(self.mListItems) == 0:
            return False
        else:
            return True

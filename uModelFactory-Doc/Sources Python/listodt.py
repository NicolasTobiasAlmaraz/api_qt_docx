from textodt import textOdt


class listOdt:
    def __init__(self):
        self.mItems = None
        self.mListItems = []

    def addItem(self, item: textOdt):
        self.mListItems.append(item)

    def getItem(self):
        item = self.mListItems.pop()
        return item

    def hasItems(self):
        if len(self.mListItems) == 0:
            return False
        else:
            return True

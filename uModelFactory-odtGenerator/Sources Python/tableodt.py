from tableelement import tableElement
from textodt import textOdt


class tableOdt:
    def __init__(self):
        self.mColumns = None
        self.mRows = None
        self.mItems = []

    def addItem(self, item: tableElement):
        self.mItems.append(item)

    def getItem(self):
        item = self.mItems.pop()
        return item

    def hasItems(self):
        if len(self.mItems) == 0:
            return False
        else:
            return True

    def sortItems(self):
        newList = []
        fAddVoidItem = True
        for currentRow in range(0, int(self.mRows)):
            for currentColumn in range(0, int(self.mColumns)):
                for item in self.mItems:
                    if int(item.mColumn) == currentColumn and int(item.mRow) == currentRow:
                        fAddVoidItem = False
                        newList.append(item)
                if fAddVoidItem:
                    voidItem = tableElement(col=currentColumn, row=currentRow, item=textOdt("No importa"))
                    voidItem.mVoid = True
                    newList.append(voidItem)
        newList.reverse()
        self.mItems = newList

from textodt import textOdt


class tableElement:
    def __init__(self, col: int, row: int, item: textOdt):
        self.mColumn = col
        self.mRow = row
        self.mVoid = False
        self.mItem = item

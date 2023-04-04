from tableodt import tableOdt
from textodt import textOdt
from imageodt import imageOdt
from pageodt import pageOdt
from tableelement import tableElement
import xml.etree.ElementTree as ET

# Page set up
PAGE_WIDTH = 0
PAGE_HEIGHT = 1
MARGIN_TOP = 2
MARGIN_LEFT = 3
MARGIN_RIGHT = 4
MARGIN_BOTTOM = 5

# Text
CONTENT = 0
SIZE = 1
FONT = 2
ALIGN = 3
UNDERLINE = 4
BOLD = 5
ITALIC = 6
STRIKETHROUGH = 7
BKG_COLOR = 8
FILL_COLOR = 9

# Image
IMAGE_PATH = 0
IMAGE_HEIGHT = 1
IMAGE_WIDTH = 2

# Table
TABLE_COLUMNS = 0
TABLE_ROWS = 1


def getPageSetUp(nodeXml):
    setUp = pageOdt()
    setUp.mMarginTop = nodeXml[MARGIN_TOP].text
    setUp.mMarginLeft = nodeXml[MARGIN_LEFT].text
    setUp.mMarginRight = nodeXml[MARGIN_RIGHT].text
    setUp.mMarginBottom = nodeXml[MARGIN_BOTTOM].text
    setUp.mSizeWidth = nodeXml[PAGE_WIDTH].text
    setUp.mSizeHeight = nodeXml[PAGE_HEIGHT].text
    return setUp


def getText(nodeXml):
    text = textOdt("")
    text.mContent = nodeXml[CONTENT].text
    text.mLetterSize = nodeXml[SIZE].text
    text.mFont = nodeXml[FONT].text
    text.mAlign = nodeXml[ALIGN].text
    text.mUnderline = nodeXml[UNDERLINE].text
    text.mBold = nodeXml[BOLD].text
    text.mItalic = nodeXml[ITALIC].text
    text.mStrikethrough = nodeXml[STRIKETHROUGH].text
    text.mBackgroundColor = nodeXml[BKG_COLOR].text
    text.mFillColor = nodeXml[FILL_COLOR].text
    return text


def getImage(nodeXml):
    img = imageOdt()
    img.mImgPath = nodeXml[IMAGE_PATH].text
    img.mWidth = nodeXml[IMAGE_WIDTH].text
    img.mHeight = nodeXml[IMAGE_HEIGHT].text
    return img


def getTable(nodeXml):
    table = tableOdt()
    table.mRows = nodeXml[TABLE_ROWS].text
    table.mColumns = nodeXml[TABLE_COLUMNS].text
    for elem in nodeXml:
        if elem.tag != "element":
            continue
        x = elem.attrib["col"]
        y = elem.attrib["row"]
        if elem[0].tag == "text":
            item = getText(elem[0])
        if elem[0].tag == "image":
            item = getImage(elem[0])
        aux = tableElement(x, y, item)
        table.addItem(aux)
    return table


class XmlReader():
    def __init__(self, path):
        self.tree = ET.parse(path)
        self.root = self.tree.getroot()
        self.listItems = []
        for child in self.root:
            if child.tag == "pageSetUp":
                item = getPageSetUp(child)
            if child.tag == "text":
                item = getText(child)
            if child.tag == "image":
                item = getImage(child)
            if child.tag == "table":
                item = getTable(child)
            self.listItems.append((item, child.tag))

    def hasItems(self):
        if len(self.listItems) == 0:
            return False
        else:
            return True

    def getItem(self):
        item = self.listItems.pop(0)
        return item

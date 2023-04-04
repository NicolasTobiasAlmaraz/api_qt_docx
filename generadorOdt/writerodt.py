# Xml items
from pageodt import pageOdt
from tableodt import tableOdt
from textodt import textOdt
from imageodt import imageOdt

# Odt
from odfpyModule.odf.opendocument import OpenDocumentText
from odfpyModule.odf.style import Style, TextProperties, ParagraphProperties, ListLevelProperties, FontFace, PageLayout, \
    PageLayoutProperties, MasterPage, GraphicProperties
from odfpyModule.odf.text import P, H, A, S, List, ListItem, ListStyle, ListLevelStyleBullet, ListLevelStyleNumber, Span
from odfpyModule.odf.table import Table, TableColumn, TableRow, TableCell
from odfpyModule.odf.draw import Frame, Image


class OdtWriter:
    def __init__(self, path):
        self.path = path
        self.textdoc = OpenDocumentText()
        self.cantStyles = 0
        self.cantTables = 0

        # Style para imagenes
        self.styleImg = Style(name="img", family="graphic")
        self.styleImg.addElement(GraphicProperties(attributes={"wrap": "none", "horizontalpos": "center"}))
        self.textdoc.automaticstyles.addElement(self.styleImg)

    def setPageConfig(self, setUp: pageOdt):
        mTop = setUp.mMarginTop + "cm"
        mLeft = setUp.mMarginLeft + "cm"
        mRight = setUp.mMarginRight + "cm"
        mBottom = setUp.mMarginBottom + "cm"
        width = setUp.mSizeWidth + "cm"
        height = setUp.mSizeHeight + "cm"
        attributes = {"margintop": mTop, "marginleft": mLeft, "marginright": mRight, "marginbottom": mBottom,
                      "pagewidth": width, "pageheight": height, "printorientation": "landscape"}
        setup = PageLayout(name="MyLayout")
        setup.addElement(PageLayoutProperties(attributes=attributes))
        self.textdoc.automaticstyles.addElement(setup)
        # Use page layout on master page, to set it:
        masterpage = MasterPage(name="Standard", pagelayoutname="MyLayout")
        self.textdoc.masterstyles.addElement(masterpage)

    def writeText(self, text: textOdt):
        newText = self.generateText(text)
        self.textdoc.text.addElement(newText)

    def generateText(self, text: textOdt):
        styleName = f"myStyle{self.cantStyles}"
        self.cantStyles += 1
        self.textdoc.fontfacedecls.addElement(
            (FontFace(name=text.mFont, fontfamily=text.mFont, fontfamilygeneric="swiss", fontpitch="variable")))
        # Cargo el formato
        style = Style(name=styleName, family="paragraph")
        attributes = {"fontname": text.mFont, "fontsize": f"{text.mLetterSize}pt", "color": text.mFillColor,
                      "backgroundcolor": text.mBackgroundColor}
        if text.mBold == "1":
            attributes["fontweight"] = "bold"
            attributes["fontweightasian"] = "bold"
            attributes["fontweightcomplex"] = "bold"
        # Set italic
        if text.mItalic == "1":
            attributes["fontstyle"] = "italic"
            attributes["fontstyleasian"] = "italic"
            attributes["fontstylecomplex"] = "italic"
        # Set underline
        if text.mUnderline == "1":
            attributes["textunderlinestyle"] = "solid"
            attributes["textunderlinewidth"] = "auto"
            attributes["textunderlinecolor"] = "font-color"
        # Set strikethrough
        if text.mStrikethrough == "1":
            attributes["textlinethroughstyle"] = "solid"
            attributes["textlinethroughtype"] = "single"
        style.addElement(TextProperties(attributes=attributes))
        style.addElement(ParagraphProperties(attributes={"textalign": text.mAlign}))
        self.textdoc.automaticstyles.addElement(style)
        p = P(text=text.mContent, stylename=style)
        return p

    def writeImage(self, image: imageOdt):
        mPath = image.mImgPath
        mWidth = image.mWidth + "cm"
        mHeight = image.mHeight + "cm"
        p = P(stylename=self.styleImg)
        self.textdoc.text.addElement(p)
        href = self.textdoc.addPicture(mPath)
        f = Frame(name="graphics1", anchortype="paragraph", width=mWidth, height=mHeight, zindex="0",
                  stylename=self.styleImg)
        p.addElement(f)
        img = Image(href=href, type="simple", show="embed", actuate="onLoad")
        f.addElement(img)

    def writeTable(self, table: tableOdt):
        cols = int(table.mColumns)
        rows = int(table.mRows)
        table.sortItems()
        tbOdt = Table(name=f"myTable{self.cantTables}")
        self.cantTables += 1
        tbOdt.addElement(TableColumn(numbercolumnsrepeated=cols))
        for row in range(0, rows):
            tr = TableRow()
            tbOdt.addElement(tr)
            for column in range(0, cols):
                if table.hasItems():
                    item = table.getItem()
                    tc = TableCell(valuetype="string")
                    if item.mVoid:
                        tc.addElement(P())
                        tr.addElement(tc)
                    else:
                        newText = self.generateText(item.mItem)
                        tc.addElement(newText)
                        tr.addElement(tc)
            self.textdoc.text.addElement(tbOdt)
            self.textdoc.text.addElement(P(text="\n"))

    def closeFile(self):
        self.textdoc.save(self.path)

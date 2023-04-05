# Xml items
from pageodt import pageOdt
from tableodt import tableOdt
from textodt import textOdt
from imageodt import imageOdt
from listodt import listOdt

# Odt
from odf.opendocument import OpenDocumentText
from odf.style import Style, TextProperties, ParagraphProperties, ListLevelProperties, FontFace, PageLayout, \
    PageLayoutProperties, MasterPage, GraphicProperties, PageLayout, Header, Footer
from odf.text import P, H, A, S, List, ListItem, ListStyle, ListLevelStyleBullet, ListLevelStyleNumber, Span
from odf.table import Table, TableColumn, TableRow, TableCell
from odf.draw import Frame, Image


class OdtWriter:
    def __init__(self, path):
        self.listElementsHeader = []
        self.listElementsFooter = []
        self.path = path
        self.myTextDoc = OpenDocumentText()
        self.cantStyles = 0
        self.cantTables = 0
        self.aux = False

        # Style para listas
        L1style = ListStyle(name="L1")
        # u'\u2022' is the bullet character (http://www.unicode.org/charts/PDF/U2000.pdf)
        bullet1 = ListLevelStyleBullet(level="1", stylename="Numbering_20_Symbols",
                                       numsuffix=".", bulletchar=u'\u2022')
        L1prop1 = ListLevelProperties(spacebefore="0.25in", minlabelwidth="0.25in")
        bullet1.addElement(L1prop1)
        L1style.addElement(bullet1)
        self.myTextDoc.automaticstyles.addElement(L1style)

    # Genera un objeto texto listo para agregar al documento
    def generateText(self, text: textOdt):
        styleName = f"myStyle{self.cantStyles}"
        self.cantStyles += 1
        self.myTextDoc.fontfacedecls.addElement(
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
        self.myTextDoc.automaticstyles.addElement(style)
        p = P(text=text.mContent, stylename=style)
        # Es como hacer un append
        #p.addElement(Span(text="_Agregado", stylename=style))
        return p

    # Genera un objeto imagen listo para agregar al documento
    def generateImage(self, image: imageOdt):
        # Config Style
        styleImg = Style(name="img", family="graphic")
        styleImg.addElement(GraphicProperties(attributes={"wrap": image.mAnchor, "horizontalpos": image.mAlign}))
        self.myTextDoc.automaticstyles.addElement(styleImg)

        mPath = image.mImgPath
        mWidth = image.mWidth + "px"
        mHeight = image.mHeight + "px"
        p = P(stylename=styleImg)
        href = self.myTextDoc.addPicture(mPath)
        f = Frame(name="graphics1", anchortype="paragraph", width=mWidth, height=mHeight, zindex="0",
                  stylename=styleImg)
        p.addElement(f)
        img = Image(href=href, type="simple", show="embed", actuate="onLoad")
        f.addElement(img)
        return p

    # Genera un objeto header/footer listo para agregar al documento
    # Obj debe ser o un Header() o un Footer()
    def writeHeaderFooter(self):
        pass

    # Stea formato
    def setPageConfig(self, setUp: pageOdt):
        # Config size and margins
        mTop = setUp.mMarginTop + "mm"
        mLeft = setUp.mMarginLeft + "mm"
        mRight = setUp.mMarginRight + "mm"
        mBottom = setUp.mMarginBottom + "mm"
        width = setUp.mSizeWidth + "mm"
        height = setUp.mSizeHeight + "mm"
        attributes = {"margintop": mTop, "marginleft": mLeft, "marginright": mRight, "marginbottom": mBottom,
                      "pagewidth": width, "pageheight": height, "printorientation": "landscape"}
        setup = PageLayout(name="MyLayout")
        setup.addElement(PageLayoutProperties(attributes=attributes))
        self.myTextDoc.automaticstyles.addElement(setup)
        mp = MasterPage(name="Standard", pagelayoutname=setup)
        self.myTextDoc.masterstyles.addElement(mp)

    # Escribe Header
    def writeHeader(self, header):
        self.listElementsHeader = header

        # Master Page para header y footer############
        # ->Si lo pongo en el constructor se rompe no se xq
        pl = PageLayout(name="pagelayout")
        self.myTextDoc.automaticstyles.addElement(pl)
        self.mp = MasterPage(name="Standard", pagelayoutname=pl)
        self.myTextDoc.masterstyles.addElement(self.mp)
        ##############################################

        # Header
        header = Header()
        while len(self.listElementsHeader) != 0:
            item = self.listElementsHeader.pop(0)
            if item["type"] == "text":
                text = self.generateText(item["item"])
                header.addElement(text)
            if item["type"] == "image":
                image = self.generateImage(item["item"])
                header.addElement(image)

        self.mp.addElement(header)

    # Escribe Footer
    def writeFooter(self, footer):
        self.listElementsFooter = footer

        # -> Por protocolo siempre primero se define el header
        # Entonces el masterpage ya esta definido siempre
        # Si piso los datos del masterpage se rompe el header

        # Master Page para header y footer
        # pl = PageLayout(name="pagelayout")
        # self.myTextDoc.automaticstyles.addElement(pl)
        # mp = MasterPage(name="Standard", pagelayoutname=pl)
        # self.myTextDoc.masterstyles.addElement(mp)

        # Footer
        footer = Footer()
        while len(self.listElementsFooter) != 0:
            item = self.listElementsFooter.pop(0)
            if item["type"] == "text":
                text = self.generateText(item["item"])
                footer.addElement(text)
            if item["type"] == "image":
                image = self.generateImage(item["item"])
                footer.addElement(image)

        self.mp.addElement(footer)

    # Escribe texto en el body
    def writeText(self, text: textOdt):
        newText = self.generateText(text)
        self.myTextDoc.text.addElement(newText)

    # Escribe imagen en el body
    def writeImage(self, image: imageOdt):
        img = self.generateImage(image)
        self.myTextDoc.text.addElement(img)

    # Escribe tabla en el body
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
            self.myTextDoc.text.addElement(tbOdt)
            self.myTextDoc.text.addElement(P(text="\n"))

    # Escribe lista en el body
    def writeList(self, textList: listOdt):
        objList = List(stylename="L1")
        textList.mListItems.reverse()
        for i in range(0, textList.mItems):
            item = ListItem()
            text = self.generateText(textList.getItem())
            item.addElement(text)
            objList.addElement(item)
        self.myTextDoc.text.addElement(objList)

    # Guarda y cierra el archivo
    def closeFile(self):
        self.myTextDoc.save(self.path)
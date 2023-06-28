from xmlreader import XmlReader
from writerdocx import DocxWriter

from docx import Document
from docx.shared import Cm

readerXml = XmlReader("genDocx.xml")
path = readerXml.getOutputPath()
writer = DocxWriter(path)
while readerXml.hasItems():
    (item, typeItem) = readerXml.getItem()

    if typeItem == "pageSetUp":
        writer.setPageConfig(item)

    if typeItem == "text":
        writer.writeText(item)

    if typeItem == "image":
        writer.writeImage(item)

    if typeItem == "table":
        writer.writeTable(item)

    if typeItem == "list":
        writer.writeList(item)

    if typeItem == "header":
        writer.writeHeader(item)

    if typeItem == "footer":
        writer.writeFooter(item)

writer.closeFile()
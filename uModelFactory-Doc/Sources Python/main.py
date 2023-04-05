from xmlreader import XmlReader
from writerodt import OdtWriter

readerXml = XmlReader("genOdt.xml")
path = readerXml.getOutputPath()
writer = OdtWriter(path)
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
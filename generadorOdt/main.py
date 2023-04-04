from xmlreader import XmlReader
from writerodt import OdtWriter

writer = OdtWriter("archivoPrueba.odt")
readerXml = XmlReader("genOdt.xml")
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

writer.closeFile()

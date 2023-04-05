# odfpy_gen_example.py

"""

  Description:  This program used odfpy to generate a simple ODF text document
  odfpy:  http://opendocumentfellowship.com/projects/odfpy
  documentation for odfpy:  http://opendocumentfellowship.com/files/api-for-
odfpy.odt

"""

from odf.opendocument import OpenDocumentText
from odf.style import Style, TextProperties, ParagraphProperties, ListLevelProperties, FontFace
from odf.text import P, H, A, S, List, ListItem, ListStyle, ListLevelStyleBullet, ListLevelStyleNumber, Span
from odf.text import Note, NoteBody, NoteCitation
from odf.table import Table, TableColumn, TableRow, TableCell
from odf.draw import Frame, Image

# fname is the path for the output file
fname = 'outFile.odt'
# fname='D:\Document\PersonalInfoRemixBook\examples\ch17\odfpy_gen_example.odt'

# instantiate an ODF text document (odt)
textdoc = OpenDocumentText()

# styles
"""
<style:style style:name="Standard" style:family="paragraph" style:class="text"/>
<style:style style:name="Text_20_body" style:display-name="Text body"
 style:family="paragraph"
 style:parent-style-name="Standard" style:class="text">
 <style:paragraph-properties fo:margin-top="0in" fo:margin-bottom="0.0835in"/>
</style:style>
"""

s = textdoc.styles

StandardStyle = Style(name="Standard", family="paragraph")
StandardStyle.setAttribute('class', 'text')
s.addElement(StandardStyle)

TextBodyStyle = Style(name="Text_20_body", family="paragraph", parentstylename='Standard', displayname="Text body")
TextBodyStyle.setAttribute('class', 'text')
TextBodyStyle.addElement(ParagraphProperties(margintop="0in", marginbottom="0.0835in"))
s.addElement(TextBodyStyle)

# font declarations
"""
 <office:font-face-decls>
    <style:font-face style:name="Arial" svg:font-family="Arial"
      style:font-family-generic="swiss"
      style:font-pitch="variable"/>
  </office:font-face-decls>
"""

textdoc.fontfacedecls.addElement((FontFace(name="Arial", fontfamily="Arial", fontfamilygeneric="swiss",
                                           fontpitch="variable")))

# Automatic Style

# P1
"""
<style:style style:name="P1" style:family="paragraph"
      style:parent-style-name="Standard"
      style:list-style-name="L1"/>
"""
P1style = Style(name="P1", family="paragraph", parentstylename="Standard", liststylename="L1")
textdoc.automaticstyles.addElement(P1style)

# L1
"""
<text:list-style style:name="L1">
  <text:list-level-style-bullet text:level="1"
    text:style-name="Numbering_20_Symbols"
    style:num-suffix="." text:bullet-char="•">
    <style:list-level-properties text:space-before="0.25in"
      text:min-label-width="0.25in"/>
    <style:text-properties style:font-name="StarSymbol"/>
  </text:list-level-style-bullet>
</text:list-style>
"""
L1style = ListStyle(name="L1")
# u'\u2022' is the bullet character (http://www.unicode.org/charts/PDF/U2000.pdf)
bullet1 = ListLevelStyleBullet(level="1", stylename="Numbering_20_Symbols",
                               numsuffix=".", bulletchar=u'\u2022')
L1prop1 = ListLevelProperties(spacebefore="0.25in", minlabelwidth="0.25in")
bullet1.addElement(L1prop1)
L1style.addElement(bullet1)
textdoc.automaticstyles.addElement(L1style)

# P6
"""
  <style:style style:name="P6" style:family="paragraph"
      style:parent-style-name="Standard"
      style:list-style-name="L5"/>
"""

P6style = Style(name="P6", family="paragraph", parentstylename="Standard",
                liststylename="L5")
textdoc.automaticstyles.addElement(P6style)

# L5
"""
<text:list-style style:name="L5">
  <text:list-level-style-number text:level="1"
    text:style-name="Numbering_20_Symbols"
    style:num-suffix="." style:num-format="1">
    <style:list-level-properties text:space-before="0.25in"
      text:min-label-width="0.25in"/>
  </text:list-level-style-number>
</text:list-style>
"""

L5style = ListStyle(name="L5")
numstyle1 = ListLevelStyleNumber(level="1", stylename="Numbering_20_Symbols",
                                 numsuffix=".", numformat='1')
L5prop1 = ListLevelProperties(spacebefore="0.25in", minlabelwidth="0.25in")
numstyle1.addElement(L5prop1)
L5style.addElement(numstyle1)
textdoc.automaticstyles.addElement(L5style)

# T1
"""
   <style:style style:name="T1" style:family="text">
      <style:text-properties fo:font-style="italic" style:font-style-asian="italic"
        style:font-style-complex="italic"/>
    </style:style>
"""
T1style = Style(name="T1", family="text")
T1style.addElement(TextProperties(fontstyle="italic", fontstyleasian="italic",
                                  fontstylecomplex="italic"))
textdoc.automaticstyles.addElement(T1style)

# T2
"""
 <style:style style:name="T2" style:family="text">
      <style:text-properties fo:font-weight="bold" style:font-weight-asian="bold"
        style:font-weight-complex="bold"/>
    </style:style>
"""
T2style = Style(name="T2", family="text")
T2style.addElement(TextProperties(fontweight="bold", fontweightasian="bold",
                                  fontweightcomplex="bold"))
textdoc.automaticstyles.addElement(T2style)

# T5
"""
   <style:style style:name="T5" style:family="text">
      <style:text-properties fo:color="#ff0000" style:font-name="Arial"/>
    </style:style>
"""
T5style = Style(name="T5", family="text")
T5style.addElement(TextProperties(color="#ff0000", fontname="Arial"))
textdoc.automaticstyles.addElement(T5style)

# now construct what goes into <office:text>

h = H(outlinelevel=1, text='Purpose (Heading 1)')
textdoc.text.addElement(h)
p = P(text="The following sections illustrate various possibilities in ODF Text", stylename='Text_20_body')
textdoc.text.addElement(p)

textdoc.text.addElement(H(outlinelevel=2, text='A simple series of paragraphs(Heading2)'))
textdoc.text.addElement(P(text="This section contains a series of paragraphs.", stylename='Text_20_body'))
textdoc.text.addElement(P(text="This is a second paragraph.", stylename='Text_20_body'))
textdoc.text.addElement(P(text="And a third paragraph.", stylename='Text_20_body'))

textdoc.text.addElement(H(outlinelevel=2, text='A section with lists (Heading 2)'))
textdoc.text.addElement(P(text="Elements to illustrate:"))

# add the first list (unordered list)
textList = List(stylename="L1")
item = ListItem()
item.addElement(P(text='hyperlinks', stylename="P1"))
textList.addElement(item)

item = ListItem()
item.addElement(P(text='italics and bold text', stylename="P1"))
textList.addElement(item)

item = ListItem()
item.addElement(P(text='lists (ordered and unordered)', stylename="P1"))
textList.addElement(item)

textdoc.text.addElement(textList)

# add the second (ordered) list

textdoc.text.addElement(P(text="How to figure out ODF"))

textList = List(stylename="L5")
# item = ListItem(startvalue=P(text='item 1'))
item = ListItem()
item.addElement(P(text='work out the content.xml tags', stylename="P5"))
textList.addElement(item)

item = ListItem()
item.addElement(P(text='work styles into the mix', stylename="P5"))
textList.addElement(item)

item = ListItem()
item.addElement(P(text='figure out how to apply what we learned to spreadsheets and presentations', stylename="P5"))
textList.addElement(item)

textdoc.text.addElement(textList)

# A paragraph with bold, italics, font change, and hyperlinks
"""
      <text:p>The <text:span text:style-name="T1">URL</text:span> for <text:span
          text:style-name="T5">Flickr</text:span> is <text:a xlink:type="simple"
          xlink:href="http://www.flickr.com/"
          >http://www.flickr.com</text:a>. <text:s/>The <text:span
          text:style-name="T2"
          >API page</text:span> is <text:a xlink:type="simple"
          xlink:href="http://www.flickr.com/services/api/"
        >http://www.flickr.com/services/api/</text:a></text:p>
"""
p = P(text='The ')
# italicized URL
s = Span(text='URL', stylename='T1')
p.addElement(s)
p.addText(' for ')
# Flickr in red and Arial font
p.addElement(Span(text='Flickr', stylename='T5'))
p.addText(' is ')
# link
link = A(type="simple", href="http://www.flickr.com", text="http://www.flickr.com")
p.addElement(link)
p.addText('.  The ')
# API page in bold
s = Span(text='API page', stylename='T2')
p.addElement(s)
p.addText(' is ')
link = A(type="simple", href="http://www.flickr.com/services/api",
         text="http://www.flickr.com/services/api")
p.addElement(link)

textdoc.text.addElement(p)

# add the table
"""
<table:table-column table:number-columns-repeated="3"/>
"""

textdoc.text.addElement(H(outlinelevel=1, text='A Table (Heading 1)'))

table = Table(name="Table 1")

table.addElement(TableColumn(numbercolumnsrepeated="3"))

# first row
tr = TableRow()
table.addElement(tr)

tc = TableCell(valuetype="string")
tc.addElement(P(text='Website'))
tr.addElement(tc)

tc = TableCell(valuetype="string")
tc.addElement(P(text='Description'))
tr.addElement(tc)

tc = TableCell(valuetype="string")
tc.addElement(P(text='URL'))
tr.addElement(tc)

# second row
tr = TableRow()
table.addElement(tr)

tc = TableCell(valuetype="string")
tc.addElement(P(text='Flickr'))
tr.addElement(tc)

tc = TableCell(valuetype="string")
tc.addElement(P(text='A social photo sharing site'))
tr.addElement(tc)

tc = TableCell(valuetype="string")
link = A(type="simple", href="http://www.flickr.com", text="http://www.flickr.com")
p = P()
p.addElement(link)
tc.addElement(p)

tr.addElement(tc)

# third row
tr = TableRow()
table.addElement(tr)
tc = TableCell(valuetype="string")
tc.addElement(P(text='Google Maps'))
tr.addElement(tc)
tc = TableCell(valuetype="string")
tc.addElement(P(text='An online map'))
tr.addElement(tc)
tc = TableCell(valuetype="string")

link = A(type="simple", href="http://maps.google.com", text="http://maps.google.com")
p = P()
p.addElement(link)
tc.addElement(p)
tr.addElement(tc)

textdoc.text.addElement(table)


# save the document
textdoc.save(fname)

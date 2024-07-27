/**
* @file qt_docx_api.cpp
* @date 19/4/23
* @brief Clases correspondientes a la escritura de un archivo DOCX
* @author Nicolás Almaraz
*
* En este archivo se incluyen las declaraciones de las clases correspondeintes a la API QT DOCX
* Esta API brinda soporte a las funcionalidades:
* - Pagina con tamaño y margenes
* - Encabezado y pie de pagina
* - Texto con formato
* - Imagenes
* - Listas
* - Tablas
*/

//==============================================
// Dependencias
//==============================================

#include <qt_docx_api.h>

//==============================================
//Defines privados
//==============================================

//Tag Root
#define TAG_ROOT            "docDocx"
//Tag Output Path
#define TAG_OUTPUT_PATH     "outputPath"
//Tags Page Set Up
#define TAG_PAGE_SETUP      "pageSetUp"
#define TAG_PSU_WIDTH       "sizeWidth"
#define TAG_PSU_HEIGHT      "sizeHeight"
#define TAG_PSU_TOP         "marBottom"
#define TAG_PSU_BOTTOM      "marTop"
#define TAG_PSU_LEFT        "marLeft"
#define TAG_PSU_RIGHT       "marRight"
//Tags Text
#define TAG_TEXT            "text"
#define TAG_TXT_CONT        "content"
#define TAG_TXT_SIZE        "size"
#define TAG_TXT_FONT        "font"
#define TAG_TXT_ALIGN       "align"
#define TAG_TXT_UNDERLINE   "underline"
#define TAG_TXT_BOLD        "bold"
#define TAG_TXT_ITALIC      "italic"
#define TAG_TXT_STRIKETHROW "strikethrough"
#define TAG_TXT_BKG_COLOR   "bkgColor"
#define TAG_TXT_FILL_COLOR  "fillColor"
//Tags Img
#define TAG_IMAGE           "image"
#define TAG_IMG_PATH        "imgPath"
#define TAG_IMG_HEIGHT      "imgHeight"
#define TAG_IMG_WIDTH       "imgWidth"
#define TAG_IMG_ANCHOR      "imgAnchor"
#define TAG_IMG_ALIGN       "imgAlign"
//Tags Table
#define TAG_TABLE           "table"
#define TAG_TABLE_ELEMENT   "element"
#define TAG_TABLE_COLUMNS   "columns"
#define TAG_TABLE_ROWS      "rows"
//Tags List
#define TAG_LIST            "list"
#define TAG_LIST_ELEMENT    "element"
//Tags Footer
#define TAG_HEADER          "header"
//Tags Header
#define TAG_FOOTER          "footer"

//==============================================
// Implementaciones de métodos pageDocx
//==============================================

pageDocx::pageDocx() {
    mMarginTop = 20;
    mMarginLeft = 30;
    mMarginRight = 20;
    mMarginBottom = 20;

    //Size
    mSizeWidth = 210;
    mSizeHeight = 297;
}

//==============================================
// Implementaciones de métodos textDocx
//==============================================

//Fonts
const QString textDocx::FONT_CALIBRI = "Calibri";
const QString textDocx::FONT_ARIAL = "Arial";
const QString textDocx::FONT_TIMES_NEW_ROMAN = "Times New Roman";

// Align
const QString textDocx::ALIGN_LEFT = "left";
const QString textDocx::ALIGN_CENTER = "center";
const QString textDocx::ALIGN_RIGHT = "right";
const QString textDocx::ALIGN_JUSTIFY = "justify";

textDocx::textDocx(QString txt) {
    mContent = txt;
    mLetterSize = 11;
    mFont = FONT_CALIBRI;
    mAlign = ALIGN_LEFT;
    mUnderline = false;
    mBold = false;
    mItalic = false;
    mStrikethrough = false;
    mFillColor = "000000";
    mBackgroundColor = "FFFFFF";
}

//==============================================
// Implementaciones de métodos tableElement
//==============================================

tableElement::tableElement(textDocx text) {
    mColumn = 0;
    mRow = 0;
    mText = new textDocx(text);
}



/************************************************
Clase Tabla
Permite crear una tabla de tamaño FILAS x COLUMNAS
***************************************************/
const int tableDocx::APPEND = 0;
const int tableDocx::INDEX_ERROR = 1;
const int tableDocx::REPLACE = 2;
tableDocx::tableDocx(int rows, int columns) {
    mRows = rows;
    mColumns = columns;
}

int tableDocx::addElement(tableElement item) {
    int size = listElements.size();
    int row=item.getRow();
    int col=item.getColumn();
    int ret = APPEND;
    if(col>mColumns || row>mRows)
        return INDEX_ERROR;
    bool fAppend = true;
    for(int i=0 ; i<size ; i++) {
        //Tomo elemento
        tableElement aux = listElements.front();
        listElements.pop_front();
        if(col == aux.getColumn() && row == aux.getRow() ) {
            //Me fijo si debo reemplazarlo por el nuevo item
            listElements.push_back(item);
            fAppend = false;
            ret = REPLACE;
        } else
            //Sino lo vuelvo a meter
            listElements.push_back(aux);
    }
    //Si no reemplace ninguno entonces lo agrego al final
    if(fAppend)
        listElements.push_back(item);
    return ret;
}



//==============================================
// Implementaciones de métodos listDocx
//==============================================

listDocx::listDocx() {}

void listDocx::addElement(textDocx item){
    listElements.push_back(item);
}



//==============================================
// Implementaciones de métodos imageDocx
//==============================================

const QString imageDocx::LEFT = "left";
const QString imageDocx::RIGHT = "right";
const QString imageDocx::CENTER = "center";
const QString imageDocx::PARALEL = "paralel";
const QString imageDocx::NONE = "none";

imageDocx::imageDocx(QString imgPath, QString anchor, QString align) {
    mImgPath = imgPath;
    QImage myImg(imgPath);
    mWidth  = myImg.width();
    mHeight = myImg.height();
    mAlign = align;
    mAnchor = anchor;
}

void imageDocx::scaleToWidth(int newWidth) {
    double relacionAspecto = (double) mHeight / mWidth;
    mWidth = newWidth;
    mHeight = newWidth * relacionAspecto;
}



//==============================================
// Implementaciones de métodos headerFooterDocx
//==============================================

headerFooterDocx::headerFooterDocx() {
    mVoid = true;
}

void headerFooterDocx::addElement(textDocx text) {
    mVoid = false;
    imageDocx image("",imageDocx::NONE, imageDocx::LEFT);
    footer_header_element_t aux = {text,image,TEXT};
    mElements.push_back(aux);
}

void headerFooterDocx::addElement(imageDocx image) {
    mVoid = false;
    textDocx text("");
    footer_header_element_t aux = {text,image,IMAGE};
    mElements.push_back(aux);
}


//==============================================
// Implementaciones de métodos headerFooterDocx
//==============================================

writerDocx::writerDocx(QString exePath, QString docxPath, pageDocx pageSetUp, headerFooterDocx header, headerFooterDocx footer) {
    mOutputPath = docxPath;

    //Genero path del directorio donde esta el ejecutable
    QString path = exePath;
    int aux = exePath.lastIndexOf("/");
    path.remove(aux+1,exePath.length()-1);
    mDirPath = path;

    //Genero path del xml
    mXmlPath = path+"genDocx.xml";

    //Guardo nombre del ejecutable
    path = exePath;
    path.remove(0,mDirPath.length());
    mExePath = path;

    //Abro doc auxiliar
    mXmlFile = new QFile(mXmlPath);
    if(!mXmlFile->open(QFile::WriteOnly | QFile::Text)) {
        puts("Error al abrir");
        mXmlFile->close();
        return;
    }

    //Creo declaracion
    mXml = QDomDocument();
    QDomProcessingInstruction def = mXml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"");
    mXml.appendChild(def);

    //Creo tag raiz
    mRoot = mXml.createElement(TAG_ROOT);
    mXml.appendChild(mRoot);

    //Creo ruta de salida
    writeOutputFile(docxPath);

    //Creo config de pagina
    writePageSetUp(pageSetUp);

    //Creo header
    if(!header.isVoid())
        writeHeaderFooter(TAG_HEADER, header);

    //Creo footer
    if(!footer.isVoid())
        writeHeaderFooter(TAG_FOOTER, footer);

}


writerDocx::~writerDocx() {
    mXmlFile->close();
}

void writerDocx::writeHeaderFooter(QString tag, headerFooterDocx obj) {
    //Creo <header/footer> y lo anido al <root>
    QDomElement tagHeaderFooter = mXml.createElement(tag);
    mRoot.appendChild(tagHeaderFooter);
    std::list<headerFooterDocx::footer_header_element_t> myList = obj.getElements();
    while(myList.size()!=0) {
        headerFooterDocx::footer_header_element_t aux = myList.front();
        myList.pop_front();
        if(aux.type == headerFooterDocx::TEXT)
            writeTextFrom(aux.text,tagHeaderFooter);
        if(aux.type == headerFooterDocx::IMAGE)
            writeImageFrom(aux.image,tagHeaderFooter);
    }
}

void writerDocx::writeTagWithText(QDomElement tagAnterior, QString tagNuevo, QString contenido) {
    QDomElement tag= mXml.createElement(tagNuevo);
    tagAnterior.appendChild(tag);
    QDomText domText = mXml.createTextNode(contenido);
    tag.appendChild(domText);
}

void writerDocx::writeOutputFile(QString path) {
    writeTagWithText(mRoot,TAG_OUTPUT_PATH,path);
}

void writerDocx::writePageSetUp(pageDocx pageSetUp) {

    //Obtengo la config del pageSetUp
    QString width = QString::number( pageSetUp.getSizeWidth() );
    QString height = QString::number( pageSetUp.getSizeHeight() );
    QString marTop = QString::number( pageSetUp.getMarginTop() );
    QString marBottom = QString::number( pageSetUp.getMarginBottom() );
    QString marLeft = QString::number( pageSetUp.getMarginLeft() );
    QString marRight = QString::number( pageSetUp.getMarginRight() );

    //Creo <pageSetUp> y lo anido al <root>
    QDomElement tagPageSetUp = mXml.createElement(TAG_PAGE_SETUP);
    mRoot.appendChild(tagPageSetUp);

    //Creo todos los tags internos
    writeTagWithText(tagPageSetUp,TAG_PSU_WIDTH,width);
    writeTagWithText(tagPageSetUp,TAG_PSU_HEIGHT,height);
    writeTagWithText(tagPageSetUp,TAG_PSU_BOTTOM,marBottom);
    writeTagWithText(tagPageSetUp,TAG_PSU_LEFT,marLeft);
    writeTagWithText(tagPageSetUp,TAG_PSU_RIGHT,marRight);
    writeTagWithText(tagPageSetUp,TAG_PSU_TOP,marTop);
}

void writerDocx::writeText(textDocx text) {
    writeTextFrom(text, mRoot);
}

void writerDocx::writeImage(imageDocx image) {
    writeImageFrom(image, mRoot);
}

void writerDocx::writeImageFrom(imageDocx image, QDomElement tagAnterior) {
    //Recupero datos
    QString path = image.getImgPath();
    QString height = QString::number( image.getHeight() );
    QString width = QString::number( image.getWidht() );
    QString anchor = image.getAnchor();
    QString align = image.getAlign();

    //Creo <Text> y lo anido al <root>
    QDomElement tagImg = mXml.createElement(TAG_IMAGE);
    tagAnterior.appendChild(tagImg);

    //Creo todos los demas tags
    writeTagWithText(tagImg,TAG_IMG_PATH,path);
    writeTagWithText(tagImg,TAG_IMG_HEIGHT,height);
    writeTagWithText(tagImg,TAG_IMG_WIDTH,width);
    writeTagWithText(tagImg,TAG_IMG_ANCHOR,anchor);
    writeTagWithText(tagImg,TAG_IMG_ALIGN,align);
}

void writerDocx::writeTextFrom(textDocx text, QDomElement tagAnterior) {
    //Recupero datos
    QString cont = text.getContent();
    QString size = QString::number( text.getLetterSize() );
    QString font = text.getFont();
    QString align = text.getAlign();
    QString underline = QString::number( text.getUnderline() );
    QString bold = QString::number( text.getBold() );
    QString italic = QString::number( text.getItalic() );
    QString strikethrough = QString::number( text.getStrikethrough() );
    QString bkgColor = text.getBackgroundColor();
    QString fillColor = text.getFillColor();

    //Creo <Text> y lo anido al <root>
    QDomElement tagText = mXml.createElement(TAG_TEXT);
    tagAnterior.appendChild(tagText);

    //Creo tag de contenido mediante un CDATA
    QDomElement tagCont = mXml.createElement(TAG_TXT_CONT);
    tagText.appendChild(tagCont);
    QDomCDATASection cdata = mXml.createCDATASection(cont);
    tagCont.appendChild(cdata);

    //Creo todos los demas tags
    writeTagWithText(tagText,TAG_TXT_SIZE,size);
    writeTagWithText(tagText,TAG_TXT_FONT,font);
    writeTagWithText(tagText,TAG_TXT_ALIGN,align);
    writeTagWithText(tagText,TAG_TXT_UNDERLINE,underline);
    writeTagWithText(tagText,TAG_TXT_BOLD,bold);
    writeTagWithText(tagText,TAG_TXT_ITALIC,italic);
    writeTagWithText(tagText,TAG_TXT_STRIKETHROW,strikethrough);
    writeTagWithText(tagText,TAG_TXT_BKG_COLOR,bkgColor);
    writeTagWithText(tagText,TAG_TXT_FILL_COLOR,fillColor);
}

void writerDocx::writeTable(tableDocx table) {
    //Recupero datos
    QString row = QString::number(table.getRows());
    QString col = QString::number(table.getColumns());
    std::list<tableElement> myList = table.getElements();

    //Creo <table> y lo anido al <root>
    QDomElement tagTable = mXml.createElement(TAG_TABLE);
    mRoot.appendChild(tagTable);

    writeTagWithText(tagTable,TAG_TABLE_COLUMNS,col);
    writeTagWithText(tagTable,TAG_TABLE_ROWS,row);

    while(myList.size() != 0) {
        tableElement aux(myList.front());
        myList.pop_front();
        QString auxCol = QString::number(aux.getColumn());
        QString auxRow = QString::number(aux.getRow());

        //Creo <element> y lo anido al <root>
        QDomElement tagElement = mXml.createElement(TAG_TABLE_ELEMENT);
        tagElement.setAttribute("col",auxCol);
        tagElement.setAttribute("row",auxRow);
        tagTable.appendChild(tagElement);
        writeTextFrom(aux.getText(),tagElement);
    }
}

void writerDocx::writeList(listDocx list) {
    std::list<textDocx> textList = list.getElements();

    //Creo <list> y lo anido al <root>
    QDomElement tagList = mXml.createElement(TAG_LIST);
    mRoot.appendChild(tagList);
    while(textList.size() != 0) {
        textDocx aux(textList.front());
        textList.pop_front();

        //Creo <element> y lo anido al <root>
        QDomElement tagElement = mXml.createElement(TAG_LIST_ELEMENT);
        tagList.appendChild(tagElement);
        writeTextFrom(aux, tagElement);
    }
}

void writerDocx::generateDocx() {
    // Save
    QTextStream output(mXmlFile);
    output.setCodec("UTF-8");
    output << mXml.toString();
    mXmlFile->close();

    //Corro el python
    QProcess p;
    p.setProgram(mDirPath+mExePath);
    p.setWorkingDirectory(mDirPath);
    p.start();
    p.waitForFinished();
}

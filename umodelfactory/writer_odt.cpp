/************************************************
-Fecha: 19/4/23

-Descripcion: En este archivo voy a incluir todas las clases
correspondientes a la escritura de un archivo ODT. Esto se va
a usar para implementar la clase de documentación

-Autor: Nicolás Almaraz
***************************************************/



#include <writer_odt.h>

/************************************************
Clase Page Set Up
***************************************************/
pageOdt::pageOdt() {
    mMarginTop = 20;
    mMarginLeft = 30;
    mMarginRight = 20;
    mMarginBottom = 20;

    //Size
    mSizeWidth = 210;
    mSizeHeight = 297;
}



/************************************************
Clase Texto
***************************************************/
//Fonts
const QString textOdt::FONT_CALIBRI = "Calibri";
const QString textOdt::FONT_ARIAL = "Arial";
const QString textOdt::FONT_TIMES_NEW_ROMAN = "Times New Roman";
// Align
const QString textOdt::ALIGN_LEFT = "left";
const QString textOdt::ALIGN_CENTER = "center";
const QString textOdt::ALIGN_RIGHT = "right";
const QString textOdt::ALIGN_JUSTIFY = "justify";

textOdt::textOdt(QString txt) {
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



/************************************************
Clase Elemento Tabla
***************************************************/
tableElement::tableElement(textOdt text) {
    mColumn = 0;
    mRow = 0;
    mText = new textOdt(text);
}



/************************************************
Clase Tabla
***************************************************/
const int tableOdt::APPEND = 0;
const int tableOdt::INDEX_ERROR = 1;
const int tableOdt::REPLACE = 2;
tableOdt::tableOdt(int rows, int columns) {
    mRows = rows;
    mColumns = columns;
}

int tableOdt::addElement(tableElement item) {
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



/************************************************
Clase Lista
***************************************************/
listOdt::listOdt() {}

void listOdt::addElement(textOdt item){
    listElements.push_back(item);
}



/************************************************
Clase Imagen
***************************************************/
const QString imageOdt::LEFT = "left";
const QString imageOdt::RIGHT = "right";
const QString imageOdt::CENTER = "center";
const QString imageOdt::PARALEL = "paralel";
const QString imageOdt::NONE = "none";

imageOdt::imageOdt(QString imgPath, QString anchor, QString align) {
    mImgPath = imgPath;
    QImage myImg(imgPath);
    mWidth  = myImg.width();
    mHeight = myImg.height();
    mAlign = align;
    mAnchor = anchor;
}

void imageOdt::scaleToWidth(int newWidth) {
    double relacionAspecto = (double) mHeight / mWidth;
    mWidth = newWidth;
    mHeight = newWidth * relacionAspecto;
}



/************************************************
Clase Header/Footer
***************************************************/
headerFooterOdt::headerFooterOdt() {
    mVoid = true;
}

void headerFooterOdt::addElement(textOdt text) {
    mVoid = false;
    imageOdt image("",imageOdt::NONE, imageOdt::LEFT);
    DATA_ELEMENT aux = {text,image,TEXT};
    mElements.push_back(aux);
}

void headerFooterOdt::addElement(imageOdt image) {
    mVoid = false;
    textOdt text("");
    DATA_ELEMENT aux = {text,image,IMAGE};
    mElements.push_back(aux);
}


/************************************************
Clase Writer
***************************************************/
writerOdt::writerOdt(QString exePath, QString odtPath, pageOdt pageSetUp, headerFooterOdt header, headerFooterOdt footer) {
    mOutputPath = odtPath;

    //Genero path del directorio donde esta el ejecutable
    QString path = exePath;
    int aux = exePath.lastIndexOf("/");
    path.remove(aux+1,exePath.length()-1);
    mDirPath = path;

    //Genero path del xml
    mXmlPath = path+"genOdt.xml";

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
    writeOutputFile(odtPath);

    //Creo config de pagina
    writePageSetUp(pageSetUp);

    //Creo header
    if(!header.isVoid())
        writeHeaderFooter(TAG_HEADER, header);

    //Creo footer
    if(!footer.isVoid())
        writeHeaderFooter(TAG_FOOTER, footer);

}


writerOdt::~writerOdt() {
    mXmlFile->close();
}

void writerOdt::writeHeaderFooter(QString tag, headerFooterOdt obj) {
    //Creo <header/footer> y lo anido al <root>
    QDomElement tagHeaderFooter = mXml.createElement(tag);
    mRoot.appendChild(tagHeaderFooter);
    std::list<DATA_ELEMENT> myList = obj.getElements();
    while(myList.size()!=0) {
        DATA_ELEMENT aux = myList.front();
        myList.pop_front();
        if(aux.type == TEXT)
            writeTextFrom(aux.text,tagHeaderFooter);
        if(aux.type == IMAGE)
            writeImageFrom(aux.image,tagHeaderFooter);
    }
}

void writerOdt::writeTagWithText(QDomElement tagAnterior, QString tagNuevo, QString contenido) {
    QDomElement tag= mXml.createElement(tagNuevo);
    tagAnterior.appendChild(tag);
    QDomText domText = mXml.createTextNode(contenido);
    tag.appendChild(domText);
}

void writerOdt::writeOutputFile(QString path) {
    writeTagWithText(mRoot,TAG_OUTPUT_PATH,path);
}

void writerOdt::writePageSetUp(pageOdt pageSetUp) {

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

void writerOdt::writeText(textOdt text) {
    writeTextFrom(text, mRoot);
}

void writerOdt::writeImage(imageOdt image) {
    writeImageFrom(image, mRoot);
}

void writerOdt::writeImageFrom(imageOdt image, QDomElement tagAnterior) {
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

void writerOdt::writeTextFrom(textOdt text, QDomElement tagAnterior) {
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

void writerOdt::writeTable(tableOdt table) {
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

void writerOdt::writeList(listOdt list) {
    std::list<textOdt> textList = list.getElements();

    //Creo <list> y lo anido al <root>
    QDomElement tagList = mXml.createElement(TAG_LIST);
    mRoot.appendChild(tagList);
    while(textList.size() != 0) {
        textOdt aux(textList.front());
        textList.pop_front();

        //Creo <element> y lo anido al <root>
        QDomElement tagElement = mXml.createElement(TAG_LIST_ELEMENT);
        tagList.appendChild(tagElement);
        writeTextFrom(aux, tagElement);
    }
}

void writerOdt::generateOdt() {
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

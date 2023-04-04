#include "writerodtmanager.h"

writerOdtManager::writerOdtManager(QString exePath, QString odtPath, pageOdt pageSetUp) {
    mOutputPath = odtPath;
    mExePath = exePath;

    //Genero path del xml
    QString path = exePath;
    int aux = exePath.lastIndexOf("/");
    path.remove(aux+1,exePath.length()-1);
    mXmlPath = path+"genOdt.xml";


     //Abro doc auxiliar
    mXmlFile = new QFile(mXmlPath);
    if(!mXmlFile->open(QFile::WriteOnly | QFile::Text)) {
        puts("Error al abrir");
        mXmlFile->close();
        return;
    }

    //Creo declaracion
    mXmlManager = QDomDocument();
    QDomProcessingInstruction def = mXmlManager.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"");
    mXmlManager.appendChild(def);

    //Creo tag raiz
    mRoot = mXmlManager.createElement("docOdt");
    mXmlManager.appendChild(mRoot);

    //Creo ruta de salida
    writeOutputFile(odtPath);

    //Creo config de pagina
    writePageSetUp(pageSetUp);
}


writerOdtManager::~writerOdtManager() {
    mXmlFile->close();
}

void writerOdtManager::writeTagWithText(QDomElement tagAnterior, QString tagNuevo, QString contenido) {
    QDomElement tag= mXmlManager.createElement(tagNuevo);
    tagAnterior.appendChild(tag);
    QDomText domText = mXmlManager.createTextNode(contenido);
    tag.appendChild(domText);
}

void writerOdtManager::writeOutputFile(QString path) {
    writeTagWithText(mRoot,TAG_OUTPUT_PATH,path);
}



void writerOdtManager::writePageSetUp(pageOdt pageSetUp) {

    //Obtengo la config del pageSetUp
    QString width = QString::number( pageSetUp.getSizeWidth() );
    QString height = QString::number( pageSetUp.getSizeHeight() );
    QString marTop = QString::number( pageSetUp.getMarginTop() );
    QString marBottom = QString::number( pageSetUp.getMarginBottom() );
    QString marLeft = QString::number( pageSetUp.getMarginLeft() );
    QString marRight = QString::number( pageSetUp.getMarginRight() );

    //Creo <pageSetUp> y lo anido al <root>
    QDomElement tagPageSetUp = mXmlManager.createElement(TAG_PAGE_SETUP);
    mRoot.appendChild(tagPageSetUp);

    //Creo todos los tags internos
    writeTagWithText(tagPageSetUp,TAG_PSU_WIDTH,width);
    writeTagWithText(tagPageSetUp,TAG_PSU_HEIGHT,height);
    writeTagWithText(tagPageSetUp,TAG_PSU_BOTTOM,marBottom);
    writeTagWithText(tagPageSetUp,TAG_PSU_LEFT,marLeft);
    writeTagWithText(tagPageSetUp,TAG_PSU_RIGHT,marRight);
    writeTagWithText(tagPageSetUp,TAG_PSU_TOP,marTop);
}

void writerOdtManager::writeText(textOdt text) {
    writeTextFrom(text, mRoot);
}

void writerOdtManager::writeImage(imageOdt image) {
    writeImageFrom(image, mRoot);
}

void writerOdtManager::writeImageFrom(imageOdt image, QDomElement tagAnterior) {
    //Recupero datos
    QString path = image.getImgPath();
    QString height = QString::number( image.getHeight() );
    QString width = QString::number( image.getWidht() );

    //Creo <Text> y lo anido al <root>
    QDomElement tagImg = mXmlManager.createElement(TAG_IMAGE);
    tagAnterior.appendChild(tagImg);

    //Creo todos los demas tags
    writeTagWithText(tagImg,TAG_IMG_PATH,path);
    writeTagWithText(tagImg,TAG_IMG_HEIGHT,height);
    writeTagWithText(tagImg,TAG_IMG_WIDTH,width);
}

void writerOdtManager::writeTextFrom(textOdt text, QDomElement tagAnterior) {
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
    QDomElement tagText = mXmlManager.createElement(TAG_TEXT);
    tagAnterior.appendChild(tagText);

    //Creo tag de contenido mediante un CDATA
    QDomElement tagCont = mXmlManager.createElement(TAG_TXT_CONT);
    tagText.appendChild(tagCont);
    QDomCDATASection cdata = mXmlManager.createCDATASection(cont);
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

void writerOdtManager::writeTable(tableOdt table) {
    //Recupero datos
    QString row = QString::number(table.getRows());
    QString col = QString::number(table.getColumns());
    std::list<tableElement> myList = table.getElements();

    //Creo <table> y lo anido al <root>
    QDomElement tagTable = mXmlManager.createElement(TAG_TABLE);
    mRoot.appendChild(tagTable);

    writeTagWithText(tagTable,TAG_TABLE_COLUMNS,col);
    writeTagWithText(tagTable,TAG_TABLE_ROWS,row);

    while(myList.size() != 0) {
        tableElement aux(myList.front());
        myList.pop_front();
        QString auxCol = QString::number(aux.getColumn());
        QString auxRow = QString::number(aux.getRow());

        //Creo <element> y lo anido al <root>
        QDomElement tagElement = mXmlManager.createElement(TAG_TABLE_ELEMENT);
        tagElement.setAttribute("col",auxCol);
        tagElement.setAttribute("row",auxRow);
        tagTable.appendChild(tagElement);
        writeTextFrom(aux.getText(),tagElement);
    }
}

void writerOdtManager::generateOdt() {
    // Save
    QTextStream output(mXmlFile);
    output << mXmlManager.toString();
    mXmlFile->close();

    //Corro el python
    QProcess p;
    p.start(mExePath);
    p.waitForFinished();
}

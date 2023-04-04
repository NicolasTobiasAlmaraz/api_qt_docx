#ifndef WRITERODTMANAGER_H
#define WRITERODTMANAGER_H

//Tag Root
#define TAG_ROOT            "docOdt"

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


#include <textodt.h>
#include <imageodt.h>
#include <tableodt.h>
#include <pageodt.h>
#include <listodt.h>
#include <headerfooterodt.h>
#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QProcess>
#include <QDebug>

class writerOdt {
public:
    writerOdt(QString exePath, QString outputPath, pageOdt pageSetUp, headerFooterOdt header, headerFooterOdt footer);
    ~writerOdt();
    void writeText(textOdt);
    void writeImage(imageOdt);
    void writeTable(tableOdt);
    void writeList(listOdt);
    void generateOdt();

private:
    void writePageSetUp(pageOdt pageSetUp);
    void writeTagWithText(QDomElement tagAnterior, QString tagNuevo, QString contenido);
    void writeImageFrom(imageOdt image, QDomElement tagAnterior);
    void writeTextFrom(textOdt text, QDomElement tagAnterior);
    void writeOutputFile(QString path);
    void writeHeaderFooter(QString tag, headerFooterOdt obj);

    QString mDirPath;
    QString mXmlPath;
    QString mExePath;
    QString mOutputPath;
    QFile *mXmlFile;
    QDomDocument mXml;
    QDomElement mRoot;
    std::list <QDomElement> myList;
};

#endif // WRITERODTMANAGER_H

#ifndef WRITERODTMANAGER_H
#define WRITERODTMANAGER_H.

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

//Tags Table
#define TAG_TABLE           "table"
#define TAG_TABLE_ELEMENT   "element"
#define TAG_TABLE_COLUMNS   "columns"
#define TAG_TABLE_ROWS      "rows"


#include <textodt.h>
#include <imageodt.h>
#include <tableodt.h>
#include <pageodt.h>
#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QProcess>
#include <QDebug>

/*
* NOTA: no estan todos las configuraciones posibles Hay muchas mas
* Si se quiere hacer alguna mas especifica hay que agregarle atributos/clases.
* Esta es una simplificacion de un archivo .odt para poder escribirlo de forma sencilla
* Para ver como son exactamente los nombres de algunas de las configuraciones hay que crear el archivo en cuestrion con la caracteristica deseada
* abrirlo como XML y ver como es la nomenclatura de dicha caractersitica
*/

class writerOdtManager {
public:
    writerOdtManager(QString exePath, QString outputPath, pageOdt pageSetUp);
    ~writerOdtManager();
    void writeText(textOdt);
    void writeImage(imageOdt);
    void writeTable(tableOdt);
    void generateOdt();

private:
    void writePageSetUp(pageOdt pageSetUp);
    void writeTagWithText(QDomElement tagAnterior, QString tagNuevo, QString contenido);
    void writeImageFrom(imageOdt image, QDomElement tagAnterior);
    void writeTextFrom(textOdt text, QDomElement tagAnterior);
    void writeOutputFile(QString path);

    QString mXmlPath;
    QString mExePath;
    QString mOutputPath;
    QFile *mXmlFile;
    QDomDocument mXmlManager;
    QDomElement mRoot;
    std::list <QDomElement> myList;
};

#endif // WRITERODTMANAGER_H

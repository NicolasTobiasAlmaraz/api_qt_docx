/************************************************
-Fecha: 19/4/23

-Descripcion: En este archivo voy a incluir todas las clases
correspondientes a la escritura de un archivo ODT. Esto se va
a usar para implementar la clase de documentación

-Autor: Nicolás Almaraz
***************************************************/



#ifndef WRITER_DOCX_H
#define WRITER_DOCX_H

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

#define TEXT    false
#define IMAGE   true

#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QString>
#include <QImage>
#include <list>



/************************************************
Clase Page Set Up
En esta clase podremos crear un objeto Hoja
En este le aclararemos tamaño de pagina y margenes en mm
***************************************************/
class pageDocx {
public:
    pageDocx();
    //Set
    void inline setMarginTop(int margin){mMarginTop = margin;}
    void inline setMarginLeft(int margin){mMarginLeft = margin;}
    void inline setMarginRight(int margin){mMarginRight = margin;}
    void inline setMarginBottom(int margin){mMarginBottom = margin;}
    void inline setSizeWidth(int size){mSizeWidth = size;}
    void inline setSizeHeight(int size){mSizeHeight = size;}
    //Get
    int inline getMarginTop(){return mMarginTop;}
    int inline getMarginLeft(){return mMarginLeft;}
    int inline getMarginRight(){return mMarginRight;}
    int inline getMarginBottom(){return mMarginBottom;}
    int inline getSizeWidth(){return mSizeWidth;}
    int inline getSizeHeight(){return mSizeHeight;}
private:
    //Margins
    int mMarginTop;
    int mMarginLeft;
    int mMarginRight;
    int mMarginBottom;
    //Size
    int mSizeWidth;
    int mSizeHeight;
};



/************************************************
Clase Texto
En esta clase podremos crear texto con formato:
Tamaño de letra, fuente, alineacion del texto, subrayado
negrita, italica, subrayado, tachado, color de letra,
color de fondo.
***************************************************/
class textDocx {
public:
    static const int R=0;
    static const int G=1;
    static const int B=2;
    // Fonts
    static const QString FONT_CALIBRI;
    static const QString FONT_ARIAL;
    static const QString FONT_TIMES_NEW_ROMAN;
    // Align
    static const QString ALIGN_LEFT;
    static const QString ALIGN_CENTER;
    static const QString ALIGN_RIGHT;
    static const QString ALIGN_JUSTIFY;
    // Constructor
    textDocx(QString txt);
    // Set
    void inline setContent(QString text){mContent = text;}
    void inline setLetterSize(int size){mLetterSize = size;}
    void inline setFont(QString font){mFont = font;}
    void inline setAlign(QString align){mAlign = align;}
    void inline setUnderline(bool underline){mUnderline = underline;}
    void inline setBold(bool bold){mBold = bold;}
    void inline setItalic(bool italic){mItalic = italic;}
    void inline setStrikethrough(bool strikethrough){mStrikethrough = strikethrough;}
    void inline setFillColor(QString color) {mFillColor=color;} //"#RRGGBB"
    void inline setBackgroundColor(QString color) {mBackgroundColor=color;}
    // Get
    QString inline getContent(){return mContent;}
    int inline getLetterSize(){return mLetterSize;}
    QString inline getFont(){return mFont;}
    QString inline getAlign(){return mAlign;}
    bool inline getUnderline(){return mUnderline;}
    bool inline getBold(){return mBold;}
    bool inline getItalic(){return mItalic;}
    bool inline getStrikethrough(){return mStrikethrough;}
    QString inline getFillColor(){return mFillColor;}
    QString inline getBackgroundColor(){return mBackgroundColor;}
private:
    QString mContent;
    int mLetterSize;
    QString mFont;
    QString mAlign;
    bool mUnderline;
    bool mBold;
    bool mItalic;
    bool mStrikethrough;
    QString mFillColor;
    QString mBackgroundColor;
};



/************************************************
Clase Imagen
En esta clase debemos indicar:
Path de la imagen,
Anclaje a la pagina
    texto al rededor: PARALEL
    en un renglón aparte: None,
Alineación (LEFT, RIGHT, CENTER),

***************************************************/
class imageDocx {
public:
    static const QString LEFT;
    static const QString RIGHT;
    static const QString CENTER;
    static const QString PARALEL;
    static const QString NONE;
    //Constructor
    imageDocx(QString imgPath, QString anchor, QString align);

    //Get
    QString inline getImgPath(){return mImgPath;}
    int inline getHeight(){return mHeight;}
    int inline getWidht(){return mWidth;}
    QString inline getAnchor(){return mAnchor;}
    QString inline getAlign(){return mAlign;}

    //Metodo para escalar imagen
    void scaleToWidth(int newWidth);
private:
    QString mImgPath;
    int mHeight;
    int mWidth;
    QString mAlign;
    QString mAnchor;
};



/************************************************
Clase Elemento Tabla
Esta clase sirve para crear objetos que luego añadiremos a una
tableOdt.
Podremos asignar un objeto texto o un objeto imagen
Además tendremos que indicar nro de fila y columna.
Luego dentro de la clase tabla tendremos un método para
añadir los objetos aquí creados
***************************************************/
class tableElement {
public:
    tableElement(imageDocx image);
    tableElement(textDocx text);
    //Set
    void inline setPos(int currentRow, int currentCol){ mColumn = currentCol; mRow = currentRow; }
    void inline setText(textDocx text){ mText = new textDocx(text);}
    //Get
    int inline getColumn(){return mColumn;}
    int inline getRow(){return mRow;}
    textDocx inline getText(){return *mText;}

private:
    int mColumn;
    int mRow;
    textDocx *mText;
};



/************************************************
Clase Tabla
Con esta clase podremos generar un objeto tabla
En el constructor debemos indicar el tamaño (filas x columnas)
Para ir completando la tabla usaremos el metodo addElement
e iremos añadiento obetos tableElement. Estos tendran el atributo
correspondiente a la celda donde hay que escribirlos
***************************************************/
class tableDocx {
public:
    static const int APPEND;
    static const int INDEX_ERROR;
    static const int REPLACE;
    //Construtor
    tableDocx(int rows, int columns);

    //Agrega un elemento a la lista de elementos y retornará APPEND
    //En caso de haber un elemento con la misma coordenada entonces lo reemplaza y retornará REPLACE
    //En caso de que el elemento este en una ubicacion imposible retornará INDEX_ERROR
    int addElement(tableElement item);

    //Devuelve la cantidad de filas que tiene la tabla
    int inline getRows(){return mRows;}

    //Devuelve la cantidad de columnas que tiene la tabla
    int inline getColumns(){return mColumns;}

    //Deuelve el contenido de las celdas de la tabla
    std::list<tableElement> getElements(){return listElements;}
private:
    int mRows;
    int mColumns;
    std::list <tableElement> listElements;
};



/************************************************
Clase Lista
Con esta clase podremos generar añadiendo objetos textOdt
***************************************************/
class listDocx {
public:
    listDocx();
    //Agrega un elemento a la lista de elementos
    void addElement(textDocx item);
    //Deuelve ls lista
    std::list<textDocx> getElements(){return listElements;}
private:
    std::list <textDocx> listElements;
};



/************************************************
Clase Header/Footer
Con esta clase podremos generar el header y footer inclyendo
texto e imágenes a nuestro gusto
***************************************************/
typedef struct {
    textDocx text;
    imageDocx image;
    bool type;
} DATA_ELEMENT;

class headerFooterDocx
{
public:
    headerFooterDocx();
    void addElement(textDocx);
    void addElement(imageDocx);
    std::list <DATA_ELEMENT> inline getElements(){return mElements;}
    bool inline isVoid(){return mVoid;}
private:
    bool mVoid;
    std::list <DATA_ELEMENT> mElements;
};



/************************************************
Clase Writer:
Con esta clase podremos administrar cada objeto de los anteriores
permitiendo escribir el XML auxiliar.
Para poder inicializarlo necesitamos pasarle los datos de escritura
unica (pageSetUp, header y footer) y la ubicacion del
ejecutable de python y ruta de salida

Con los metodos "write" podremos ir escribirendo a nuestro gusto

Con el metodo generateOdt, llamaremos al ejecutable de python que
consumirá el XML y escribirá todo lo que le fuimos indicando
***************************************************/
class writerDocx {
public:
    writerDocx(QString exePath, QString outputPath, pageDocx pageSetUp, headerFooterDocx header, headerFooterDocx footer);
    ~writerDocx();
    void writeText(textDocx);
    void writeImage(imageDocx);
    void writeTable(tableDocx);
    void writeList(listDocx);
    void generateDocx();

private:
    void writePageSetUp(pageDocx pageSetUp);
    void writeTagWithText(QDomElement tagAnterior, QString tagNuevo, QString contenido);
    void writeImageFrom(imageDocx image, QDomElement tagAnterior);
    void writeTextFrom(textDocx text, QDomElement tagAnterior);
    void writeOutputFile(QString path);
    void writeHeaderFooter(QString tag, headerFooterDocx obj);

    QString mDirPath;
    QString mXmlPath;
    QString mExePath;
    QString mOutputPath;
    QFile *mXmlFile;
    QDomDocument mXml;
    QDomElement mRoot;
    std::list <QDomElement> myList;
};



#endif // WRITER_DOCX_H

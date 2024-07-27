/**
* @file qt_docx_api.h
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

#ifndef WRITER_DOCX_H
#define WRITER_DOCX_H

//==============================================
// Dependencias
//==============================================

#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QString>
#include <QImage>
#include <list>

//==============================================
// Clase pageDocx
//==============================================

/**
* @brief Esta clase permite crear un objeto Hoja aclarando tamaño y margenes en mm
*/
class pageDocx {
public:
    /**
     * @brief Constructor. Por default crea una hoja A4
     */
    pageDocx();

    /**
     * @brief Setea el margen superior
     * @param margin Margen superior [mm]
     */
    void inline setMarginTop(int margin){mMarginTop = margin;}

    /**
     * @brief Setea el margen izquierdo
     * @param margin Margen izquierdo [mm]
     */
    void inline setMarginLeft(int margin){mMarginLeft = margin;}

    /**
     * @brief Setea el margen derecho
     * @param margin Margen derecho [mm]
     */
    void inline setMarginRight(int margin){mMarginRight = margin;}

    /**
     * @brief Setea el margen inferior
     * @param margin Margen inferior [mm]
     */
    void inline setMarginBottom(int margin){mMarginBottom = margin;}

    /**
     * @brief Setea el ancho de la hoja
     * @param size Ancho [mm]
     */
    void inline setSizeWidth(int size){mSizeWidth = size;}

    /**
     * @brief Setea la altura de la hoja
     * @param size Altura [mm]
     */
    void inline setSizeHeight(int size){mSizeHeight = size;}

    /**
     * @brief Devuelve margen superior [mm]
     */
    int inline getMarginTop(){return mMarginTop;}

    /**
     * @brief Devuelve margen izquierdo [mm]
     */
    int inline getMarginLeft(){return mMarginLeft;}

    /**
     * @brief Devuelve margen derecho [mm]
     */
    int inline getMarginRight(){return mMarginRight;}

    /**
     * @brief Devuelve margen inferior [mm]
     */
    int inline getMarginBottom(){return mMarginBottom;}

    /**
     * @brief Devuelve ancho de la hoja [mm]
     */
    int inline getSizeWidth(){return mSizeWidth;}

    /**
     * @brief Devuelve altura de la hoja [mm]
     */
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



//==============================================
// Clase textDocx
//==============================================

/**
* @brief Esta clase permite crear un objeto texto e indicarle atributos de formato
* Esta clase provee las fuentes, colores y alineaciones más usuales, sin embargo, pueden utilizarse personalizadamente las fuentes y alineaciones a gusto.
* Para eso se deben indicar el nombre de la fuente / alineacion deseada tal cual como lo escribiría Word en su XML análogo (todos los archivos docx están basados en una estructura XML).
* Para el caso de los colores sería su RGB correspondiente en formato QString "RRGGBB".
*/
class textDocx {
public:
    const QString R="FF0000"; /*<! Color Rojo*/
    const QString G="00FF00"; /*<! Color Verde*/
    const QString B="0000FF"; /*<! Color Azul*/

    static const QString FONT_CALIBRI;          /*<! Fuente Calibri*/
    static const QString FONT_ARIAL;            /*<! Fuente Arial*/
    static const QString FONT_TIMES_NEW_ROMAN;  /*<! Fuente Times New Roman*/

    static const QString ALIGN_LEFT;    /*<! Alineacion izquierda*/
    static const QString ALIGN_CENTER;  /*<! Alineacion centrada*/
    static const QString ALIGN_RIGHT;   /*<! Alineacion derecha*/
    static const QString ALIGN_JUSTIFY; /*<! Alineacion justificada*/

    /**
     * @brief Constructor. Se le indica el contenido del texto a escribir
     */
    textDocx(QString txt);

    /**
     * @brief Metodo para setear el texto a escribir
     * @param text texto
     */
    void inline setContent(QString text){mContent = text;}

    /**
     * @brief Metodo para setear el tamaño en caracteres que quermos que ocupe la escritura
     * @param size tamaño
     */
    void inline setLetterSize(int size){mLetterSize = size;}

    /**
     * @brief Metodo para setear fuente
     * @param font fuente. Puede ser las definidas en esta clase o personalizdas (QString con el nombre de la fuente)
     */
    void inline setFont(QString font){mFont = font;}

    /**
     * @brief Metodo setear alineacion
     * @param align alineacion. Puede ser las definidas en esta clase o personalizdas (QString con el nombre de la alineacion)
     */
    void inline setAlign(QString align){mAlign = align;}

    /**
     * @brief Metodo setear subrayado
     * @param underline true: subrayado false: no subrayado
     */
    void inline setUnderline(bool underline){mUnderline = underline;}

    /**
     * @brief Metodo setear negrita
     * @param bold true: negrita false: sin negrita
     */
    void inline setBold(bool bold){mBold = bold;}

    /**
     * @brief Metodo setear cursiva
     * @param italic true: cursiva false: normal
     */
    void inline setItalic(bool italic){mItalic = italic;}

    /**
     * @brief Metodo setear tachado
     * @param strikethrough true: tachado false: no tachado
     */
    void inline setStrikethrough(bool strikethrough){mStrikethrough = strikethrough;}

    /**
     * @brief Metodo setear color del texto
     * @param color color "RRGGBB"
     */
    void inline setFillColor(QString color) {mFillColor=color;} //"#RRGGBB"

    /**
     * @brief Metodo setear color de fondo del texto
     * @param color color
     */
    void inline setBackgroundColor(QString color) {mBackgroundColor=color;}

    /**
    * @brief devuelve el texto almacenado
    */
    QString inline getContent(){return mContent;}

    /**
    * @brief devuelve el tamaño del texto almacenado
    */
    int inline getLetterSize(){return mLetterSize;}

    /**
    * @brief devuelve la fuente seteada
    */
    QString inline getFont(){return mFont;}

    /**
    * @brief devuelve la alineacion seteada
    */
    QString inline getAlign(){return mAlign;}

    /**
    * @brief devuelve si esta subrayado
    */
    bool inline getUnderline(){return mUnderline;}

    /**
    * @brief devuelve si esta en negrita
    */
    bool inline getBold(){return mBold;}

    /**
    * @brief devuelve si esta en cursiva
    */
    bool inline getItalic(){return mItalic;}

    /**
    * @brief devuelve si esta en tachado
    */
    bool inline getStrikethrough(){return mStrikethrough;}

    /**
    * @brief devuelve el color del texto
    */
    QString inline getFillColor(){return mFillColor;}

    /**
    * @brief devuelve el color del fondo del texto
    */
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



//==============================================
// Clase imageDocx
//==============================================

/**
* @brief Esta clase permite crear un imagen e indicarle su anclaje y alineacion. Además provee un método para escalar una imagen ya existente
*/
class imageDocx {
public:
    static const QString LEFT;      /*<! Alineacion izquierda*/
    static const QString RIGHT;     /*<! Alineacion derecha*/
    static const QString CENTER;    /*<! Alineacion centrado*/
    static const QString PARALEL;   /*<! Anclaje en paralelo con el texto*/
    static const QString NONE;      /*<! Anclaje en bloque  con el texto*/

    /**
    * @brief Constructor. Se le indica ruta, anclaje y alineacion de la imagen
    * @param imgPath Ruta a una imagen en formato .jpg, .jpeg, .png, bmp, etc (todos los soportados por QImage)
    * @param anchor anclaje. Puede ser los definidos por esta clase o bien personalizados (QString con el nombre que lleva el docx en su XML)
    * @param align anineacion con el texto. Puede ser los definidos por esta clase o bien personalizados (QString con el nombre que lleva el docx en su XML)
    */
    imageDocx(QString imgPath, QString anchor, QString align);

    /**
    * @brief Devuelve ruta de la imagen cargada
    */
    QString inline getImgPath(){return mImgPath;}

    /**
    * @brief Devuelve altura de la imagen
    */
    int inline getHeight(){return mHeight;}

    /**
    * @brief Devuelve ancho de la imagen cargada
    */
    int inline getWidth(){return mWidth;}

    /**
    * @brief Devuelve el anclaje de la imagen cargada
    */
    QString inline getAnchor(){return mAnchor;}

    /**
    * @brief Devuelve alineacion de la imagen cargada
    */
    QString inline getAlign(){return mAlign;}

    /**
    * @brief Metodo para escalar imagen
    * @param newWidth Nuevo ancho
    */
    void scaleToWidth(int newWidth);
private:
    QString mImgPath;
    int mHeight;
    int mWidth;
    QString mAlign;
    QString mAnchor;
};



//==============================================
// Clase tableElement
//==============================================

/**
* @brief Esta clase permite crear un objeto que luego será almacenado en la celda de una tabla
*/
class tableElement {
public:

    /**
    * @brief Constructor. Recibe el objeto textDocx que queremos almacenar en una celda
    */
    tableElement(textDocx text);

    /**
    * @brief Se le indica en qué posicion de la tabla va a estar ubicado
    * @param currentRow fila
    * @param currentCol columna
    */
    void inline setPos(int currentRow, int currentCol){ mColumn = currentCol; mRow = currentRow; }

    /**
    * @brief Permite modificar el texto
    * @param text nuevo texto
    */
    void inline setText(textDocx text){ mText = new textDocx(text);}

    /**
    * @brief Devuelve la columna configurada
    */
    int inline getColumn(){return mColumn;}

    /**
    * @brief Devuelve fila
    */
    int inline getRow(){return mRow;}

    /**
    * @brief Devuelve el texto configurado
    */
    textDocx inline getText(){return *mText;}

private:
    int mColumn;
    int mRow;
    textDocx *mText;
};



//==============================================
// Clase tableElement
//==============================================

/**
* @brief Esta clase permite crear un objeto tabla que almacena en cada una de sus celdas un objeto tableElement.
* Para cargar nuevos objetos tableElement tiene el método addElement(tableElement item) que se encarga de validar si el objeto fue cargado correctamente
*/
class tableDocx {
public:
    static const int APPEND;        /*<! Variable de retorno para cuando se añadio correctamente un nuevo objeto a la tabla*/
    static const int INDEX_ERROR;   /*<! Variable de retorno para cuando el objeto que intentamos añadir estaba fuera de rango*/
    static const int REPLACE;       /*<! Variable de retorno para cuando el objeto que añadimos pisó a otro objeto que ya estaba almacenado en ese lugar*/

    /**
     * @brief Constructor. Crea un objeto tabla del tamaño indicado
     * @param rows Filas
     * @param columns Columnas
     */
    tableDocx(int rows, int columns);

    /**
     * @brief Agrega un nuevo elemento a la tabla. Retorna el status de la operacion definidos en esta clase(APPEND, INDEX_ERROR, REPLACE)
     * @param item elemento a agregar
     * @retval status de la operacion
     */
    int addElement(tableElement item);

    /**
     * @brief Devuele cantidad de filas de la tabla
     */
    int inline getRows(){return mRows;}

    /**
     * @brief Devuele cantidad de columnas de la tabla
     */
        int inline getColumns(){return mColumns;}

    /**
     * @brief Devuele lista de elementos cargados
     * @note Recordar que para saber la ubicacion dentro de la tabla tenemos que consultar la posicion (row,clumn) de cada objeto tableElement através de sus métodos get
     */
    std::list<tableElement> getElements(){return listElements;}

private:
    int mRows;
    int mColumns;
    std::list <tableElement> listElements;
};



//==============================================
// Clase listDocx
//==============================================

/**
* @brief Esta clase permite crear un objeto lista. Cada item es un objeto textDocx
*/
class listDocx {
public:

    /**
    * @brief Crea un objeto lista
    */
    listDocx();

    /**
     * @brief Agrega un elemento textDocx a la lista
     */
    void addElement(textDocx item);

    /**
     * @brief Devuelve la lista de elementos
     * @warning No puede modificarse el orden. En caso de querer cambiarlo hay que crear un objeto nuevo y cargarlos de manera ordenada
     */
    std::list<textDocx> getElements(){return listElements;}

private:
    std::list <textDocx> listElements;
};



//==============================================
// Clase headerFooterDocx
//==============================================

/**
* @brief Esta clase permite crear un objeto que puede utitizarse como header o como footer. En este podremos incluir texto e imagenes
*/

class headerFooterDocx
{
public:
    static const bool TEXT = true;
    static const bool IMAGE = false;

    /**
     * @brief Estructura para indicar el tipo de objeto que vamos a escribir dentro del objeto header/footer
     */
    typedef struct {
        textDocx text;      /*<! En caso de que corresponda, objeto texto */
        imageDocx image;    /*<! En caso de que corresponda, objeto imagen */
        bool type;          /*<! terue: texto  -  false: imagen */
    } footer_header_element_t;

    /**
     * @brief Constructor
     */
    headerFooterDocx();

    /**
     * @brief Añade un objeto texto
     */
    void addElement(textDocx);

    /**
     * @brief Añade un objeto imagen
     */
    void addElement(imageDocx);

    /**
     * @brief Retorna la lista de elementos cargados
     * @note Para saber el tipo de objeto que es hay que consultar el flag de la estructura
     */
    std::list <footer_header_element_t> inline getElements(){return mElements;}

    /**
     * @brief Si el objeto esta vacio retorna true
     */
    bool inline isVoid(){return mVoid;}
private:
    bool mVoid;
    std::list <footer_header_element_t> mElements;
};



/**
* @brief Con esta clase podremos administrar cada objeto de los anteriores permitiendo escribir el XML auxiliar y luego generar el docx.
* Para poder inicializarlo necesitamos pasarle los datos de escritura unica (pageSetUp, header y footer) y la ubicacion del ejecutable de python y ruta de salida
* Con los metodos del tipo "write" podremos ir escribirendo a nuestro gusto
* Con el metodo generateOdt, llamaremos al ejecutable de python que consumirá el XML auxliar y escribirá todo lo que le fuimos indicando
*/
class writerDocx {
public:
    /**
     * @brief Constructor. Genera el objeto hanlder de toda la API haciendo uso de todas las demas clases
     * @param exePath Ruta donde se encuentra el ejecutable (con nombre incluido)
     * @param outputPath Ruta donde se encuentra el archivo de salida (con nombre incluido)
     * @param header header del archivo
     * @param footer footer del archivo
     */
    writerDocx(QString exePath, QString outputPath, pageDocx pageSetUp, headerFooterDocx header, headerFooterDocx footer);

    /**
     * @brief Destructor. Eventualmente cierra el archivo XML
     */
    ~writerDocx();

    /**
     * @brief Concatena un objeto texto
     */
    void writeText(textDocx);

    /**
     * @brief Concatena un objeto imagen
     */
    void writeImage(imageDocx);

    /**
     * @brief Concatena un objeto tabla
     */
    void writeTable(tableDocx);

    /**
     * @brief Concatena un objeto lista
     */
    void writeList(listDocx);

    /**
     * @brief Genera el documento docx en la ruta especificada
     * @warning Si el archivo a generar ya existia va a sobreescribirlo
     * @warning Para llamar a este método debe asegurarse de que el archivo docx que intenta generar esté cerrado
     */
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

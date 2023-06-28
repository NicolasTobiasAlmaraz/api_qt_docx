#ifndef DOCUMENTACION_H
#define DOCUMENTACION_H

#ifdef _WIN32
    //#define DOCUMENTACION_EXE_PATH "./EjecutablesDocumentacion/genDocx.exe"
    #define DOCUMENTACION_EXE_PATH "D:/UTN/uModelFactory/uModelFactory-Documentacion/umodelfactory/EjecutablesDocumentacion/genDocx.exe"
#elif __linux__
    #define DOCUMENTACION_EXE_PATH "./Ejecutables/genDocx.out"
#else
    #ERROR_SISTEMA_OPERATIVO_DOCUMENTACION "Sistema operativo no compatible"
#endif

#include <writer_docx.h>
#include <QImage>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QDir>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QtGui/QImage>
#include <QtCore/QFile>

typedef struct {
    QString estados;
    QString transiciones;
    QString eventos;
    QString acciones;
    QString reset;
} InfoTeorica;

typedef struct {
    QString titulo;
    QString diagrama;
    std::list<std::list<QString>> tablaEstTranc;
} MaquinaDeEstados;

class Documentacion
{
public:
    Documentacion();

    //Config documento
    inline void setPathProyecto(QString path) {mPathProyecto = path;}
    inline void setAutores(QString autores) {mAutores=autores;}
    inline void setFecha(QString fecha) {mFecha = fecha;}
    inline void setLogoUtn(QString logo) {mLogoUtn = logo;}
    inline void setTituloProyecto(QString titulo) {mNombreProyecto = titulo;}
    inline void setDescripcion(QString txt) {mDescripcion = txt;}
    inline void setEventos(std::list <QString> eventos) {mEventos = eventos;}
    inline void setAcciones(std::list <QString> acciones) {mAcciones = acciones;}
    inline void setVariables(std::list <QString> variables) {mVariables = variables;}
    inline void setInfoTeorica(InfoTeorica info) {mInfoTeorica = info;}
    inline void addMaquina(MaquinaDeEstados maq) { mMaquinas.push_back(maq); }
    inline void setPathsCodigos(std::list <QString> paths) {mPathsCodigos = paths;}

    inline void setFlagDescripcion(bool fConfig) {fDescripcion = fConfig;}
    inline void setFlagInfoTeorica(bool fConfig) {fIntroTeorica = fConfig;}
    inline void setFlagTabla(bool fConfig) {fTabla = fConfig ;}
    inline void setFlagCodigos(bool fConfig) {fCodigos = fConfig;}

    inline void resetListaMaq() {mMaquinas.clear();}

    // Generar documentacion
    void generarDocumentacionFormatoDocx(QString exePath, QString outPath);
    void generarDocumentacionFormatoHtml();

    inline QString getDescripcion(){return mDescripcion;}

    //Generador de tabla
    static tableDocx generarTablaEstados(std::list<std::list<QString>> filas);

private:

    const static textDocx newLine;

    //Configuraciones
    bool fDescripcion;
    bool fIntroTeorica;
    bool fTabla;
    bool fCodigos;

    //Ejecutable
    QString mExePath;
    QString mOutPath;

    //Atributos
    QString mPathProyecto;
    QString mAutores;
    QString mFecha;
    QString mLogoUtn;
    QString mNombreProyecto;
    QString mDescripcion;
    InfoTeorica mInfoTeorica;
    std::list<MaquinaDeEstados> mMaquinas;
    std::list <QString> mPathsCodigos;
    std::list <QString> mEventos;
    std::list <QString> mVariables;
    std::list <QString> mAcciones;

    //Manager
    writerDocx* mManagerDocx;

    //Metodos generales
    std::list<QString> generarImagenes();
    std::list<QString> generarCodigos();

    //Metodos de generacion de Docx
    writerDocx pageSetUp(QString exePath, QString outPath);

    void escribirTitulo(QString txt,int size);
    void escribirSubtitulo(QString txt);
    void escribirLista(QString subtitulo, std::list<QString> lista);

    void escribirIntroTeorica();
    void escribirDescripcion();
    void escribirTituloMaqGeneral();
    void escribirEventosAccionesVariables();
    void escribirDiagrama(imageDocx img);
    void escribirTablaEstados(tableDocx tabla);
    void escribirCodigoFuente();

    //Metodos de generacion de Html

};

#endif // DOCUMENTACION_H

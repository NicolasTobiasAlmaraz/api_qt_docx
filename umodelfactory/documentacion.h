#ifndef DOCUMENTACION_H
#define DOCUMENTACION_H

#include <writer_odt.h>
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
    tableOdt tablaEstTranc;
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
    inline void setEventos(std::list <QString> eventos) {mEventos = eventos;}
    inline void setAcciones(std::list <QString> acciones) {mAcciones = acciones;}
    inline void setVariables(std::list <QString> variables) {mVariables = variables;}
    inline void setInfoTeorica(InfoTeorica info) {mInfoTeorica = info;}
    inline void addMaquina(MaquinaDeEstados maq) { mMaquinas.push_back(maq); }
    inline void setPathsCodigos(std::list <QString> paths) {mPathsCodigos = paths;}
    inline void setCodigos(bool fConfig) {fCodigos = fConfig;}
    inline void setIntroTeorica(bool fConfig) {fIntroTeorica = fConfig;}

    // Generar documentacion
    void generarDocumentacionFormatoOdt(QString exePath, QString outPath);
    void generarDocumentacionFormatoHtml();

    //Generador de tabla
    static tableOdt generarTablaEstados(std::list<std::list<QString>> filas);

private:

    const static textOdt newLine;

    //Configuraciones
    bool fCodigos;
    bool fIntroTeorica;
    bool fTabla;

    //Ejecutable
    QString mExePath;
    QString mOutPath;

    //Atributos
    QString mPathProyecto;
    QString mAutores;
    QString mFecha;
    QString mLogoUtn;
    QString mNombreProyecto;
    InfoTeorica mInfoTeorica;
    std::list<MaquinaDeEstados> mMaquinas;
    std::list <QString> mPathsCodigos;
    std::list <QString> mEventos;
    std::list <QString> mVariables;
    std::list <QString> mAcciones;

    //Manager
    writerOdt* mManagerOdt;

    //Metodos generales
    std::list<QString> generarImagenes();
    std::list<QString> generarCodigos();

    //Metodos de generacion de Odt
    writerOdt pageSetUp(QString exePath, QString outPath);

    void escribirTitulo(QString txt,int size);
    void escribirSubtitulo(QString txt);
    void escribirLista(QString subtitulo, std::list<QString> lista);

    void escribirIntroTeorica();
    void escribirTituloMaqGeneral();
    void escribirEventosAccionesVariables();
    void escribirDiagrama(imageOdt img);
    void escribirTablaEstados(tableOdt tabla);
    void escribirCodigoFuente();
    void ajustarImagenAncho(const QString& imagePath, int newWidth);

    //Metodos de generacion de Html

};

#endif // DOCUMENTACION_H

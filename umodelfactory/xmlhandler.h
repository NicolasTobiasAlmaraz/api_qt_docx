#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include <QtXml/QtXml>
#include <QDir>
#include <QFile>
#include <QTextStream>

/*!
 * Tags de la estrucura de datos XML
 */
#define PROYECTO        "proyecto"

#define GENERAL         "general"
    #define NOMBRE          "nombre"
    #define AUTORES         "autores"
    #define CREACION        "creacion"
    #define MODIFICACION    "modificacion"
    #define CARPETA         "carpeta"

#define CONFIGURACION   "configuracion"
    #define HARDWARE        "hardware"
    #define TARGET          "target"

#define RECURSOS        "recursos"
    #define ARCHIVOS_C      "archivosC"
    #define ARCHIVOS_H      "archivosH"

#define MAQUINAS         "maquinas"
    #define MAQUINA         "maquina"
    #define MAQUINA_DEFAULT "maquina0"
        #define NOMBRE_MAQUINA  "nombre"
        #define ESTADO_INICIAL  "estadoInicial"
        #define ESTADO          "estado"
            #define DIAGRAMA        "diagrama"
                #define NOMBRE_ESTADO   "nombre"
                #define COLOR_ESTADO    "color"
                #define POS_X_ESTADO    "pos_x"
                #define POS_Y_ESTADO    "pos_y"
                #define ANCHO_ESTADO    "ancho"
                #define ALTO_ESTADO     "alto"
            #define TRANSICIONES       "transiciones"
                #define ID_TRANS            "tr"
                    #define ID_EVENTO           "idEvento"
                    #define ID_ACCIONES         "idAcciones"
                    #define ID_PROX_ESTADO      "idProxEstado"
                    #define MID_POS             "midPos"
                    #define TEXT_POS            "textPos"

#define EVENTOS                     "eventos"
    #define EVENTO                  "ev"
    #define EVENTO_VALOR            "valorEv"

#define ACCIONES                    "acciones"
    #define ACCION                  "acc"
    #define ACCION_VALOR            "valorAcc"

#define VARIABLES        "variables"
    #define VARIABLE          "var"

#define CONSTANTES        "constantes"
    #define CONSTANTE          "cons"

#define COMENTARIOS      "comentarios"
    #define COMENTARIO         "comm"
    #define ATTR_PARA           "for"


#define DEFAULT_INDENT  4

enum XmlType {NUEVO_XML_DATOS, ABRIR_XML_DATOS, NUEVO_XML_PERIF, ABRIR_XML_PERIF};

/*!
 * \brief Clase XmlHandler
 */
class XmlHandler
{
public:
    XmlHandler();
    XmlHandler(const QString &nombreArchivo, XmlType tipo);

    // Funciones para setear y obtener datos generales (nombre, autor, etc.)
    bool setContenidoGeneral(const QString &tagName, const QString &contenido);
    QString getContenidoGeneral(const QString &tagName);
    bool setContenidoConfiguracion(const QString &tagName, const QString &contenido);
    QString getContenidoConfiguracion(const QString &tagName);
    bool setContenidoRecursos(const QString &tagName, const QString &contenido);
    QStringList getContenidoRecursos(const QString &tagName);

    //Funciones para crear y manejar las maquinas de estados
    void addNuevaMaquina(const QString &maquinaId = MAQUINA_DEFAULT);
    void borrarMaquina(const int &indexMaquina);
    void addInfoMaquina(const QString &maquinaId, const QString &nombre);
    bool setContenidoMaquina(const QString &maquinaId, const QString &tagName, const QString &contenido );
    QString getContenidoMaquina(const QString &maquinaId, const QString &tagName);
    int cantidadMaquinas();
    QStringList getNombresMaquinasDefinidas();

    // Funciones para setear y obtener datos de los estados
    void addNuevoEstado(const QString &estadoId, const QString &maquinaId = MAQUINA_DEFAULT);
    void borrarEstado(const int &indexEstado, const QString &maquinaId = MAQUINA_DEFAULT);
    void addInfoEstado(const QString &estadoId, const QString &nombre, const QString &color,
                            const QString &pos_x, const QString &pos_y, const QString &ancho,
                            const QString &alto, const QString &maquinaId = MAQUINA_DEFAULT);
    bool setContenidoEstado(const QString &estadoId, const QString &tagName, const QString &subTagName,
                            const QString &contenido, const QString &maquinaId = MAQUINA_DEFAULT );
    QString getContenidoEstado(const QString &estadoId, const QString &tagName, const QString &subTagName,
                               const QString &maquinaId = MAQUINA_DEFAULT);
    bool setContenidoDiagrama(const QString &estadoId, const QString &subTagName, const QString &contenido,
                              const QString &maquinaId = MAQUINA_DEFAULT);
    QString getContenidoDiagrama(const QString &estadoId, const QString &subTagName,
                                 const QString &maquinaId = MAQUINA_DEFAULT);
    int cantidadEstados(const QString &maquinaId = MAQUINA_DEFAULT);

    // Funciones para manejar los estados iniciales
    bool setInfoEstadoInicial(const QString &pos_x, const QString &pos_y, const QString &accion,
                              const QString &proxEstadoId, const QString &midPos, const QString &textPos,
                              const QString &maquinaId = MAQUINA_DEFAULT);
    bool setContenidoInicial(const QString &tagName, const QString &contenido, const QString &maquinaId);
    QString getContenidoInicial(const QString &tagName, const QString &maquinaId = MAQUINA_DEFAULT);
    QString getEstadoInicial(const QString &maquinaId = MAQUINA_DEFAULT);
    void borrarEstadoInicial(const QString &maquinaId = MAQUINA_DEFAULT);

    // Funciones para manejar los datos de las Transiciones
    bool setContenidoTransicion(const QString &estadoId, const QString &transId,
                                const QString &subTagName, const QString &contenido,
                                const QString &maquinaId = MAQUINA_DEFAULT );
    void addNuevaTransicion(const QString &estadoId, const QString &transId, const QString &evento,
                            const QString &accion, const QString &proxEstadoId, const QString &midPos,
                            const QString &textPos, const QString &maquinaId = MAQUINA_DEFAULT);
    void borrarTransicion(const int &indexEstado, const int &indexTrans,
                          const QString &maquinaId = MAQUINA_DEFAULT);
    QString getContenidoTransicion(const QString &estadoId, const QString &transId,
                                   const QString &tagName, const QString &maquinaId = MAQUINA_DEFAULT);
    int cantidadTransiciones(const QString &estadoId, const QString &maquinaId = MAQUINA_DEFAULT);

    //Funciones para manejar y definir Eventos
    void addNuevoEvento(const QString &nombre);
    void borrarEvento(const int &eventoId);
    QString getContenidoEvento(const QString &eventoId);
    int cantidadEventos();

    // Funciones para manejar y definir Variables
    void addNuevaVariable(const QString &nombre);
    void borrarVariable(const int &variableId);
    QString getContenidoVariable(const QString &variableId);
    int cantidadVariables();

    //Funciones para manejar y definir Acciones
    void addNuevaAccion(const QString &nombre);
    void borrarAccion(const int &accionId);
    QString getContenidoAccion(const QString &accionId);
    int cantidadAcciones();

    //Funciones para manejar y definir Constantes
    void addNuevaConstante(const QString &nombre);
    void borrarConstante(const int &constanteId);
    void editarConstante(const int &constanteId, const QString &nombre);
    QString getContenidoConstante(const QString &constanteId);
    int cantidadConstantes();

    // Funciones para acceder a los nombres definidos
    QStringList getNombresAccionesDefinidas();
    QStringList getNombresEventosDefinidos();
    QStringList getNombresEstadosDefinidos(const QString &maquinaId = MAQUINA_DEFAULT);
    QStringList getNombresVariablesDefinidas();
    QStringList getNombresConstantesDefinidas();

    // Funciones para borrar las estructuras del XML
    void borrarMaquinas();
    void borrarEstados(const QString &maquinaId = MAQUINA_DEFAULT);

    // Funciones para escribir los datos en el archivo XML
    bool save();
    // Recupera los datos del archivo XML
    bool load (QFileDevice &file);

    // Funciones para guardar los comentarios en el .umf
    int cantidadComentarios();
    int setComentario(const QString &elementId, const QString &comentario);
    void clearComentario(const QString &elementId, bool tambienComentario = false);
    QString getComentario(const QString &elementId);
    QString getComentarioArchivo(const QString &elementId);

private:
    // Crea un nuevo 'tag' en un determinado 'parent node' ( puede o no incluir un 'contenido' )
    void addElemento(const QString &parentNode, const QString &tagName,
                     const QString &contenido = QString());
    void crearEstructuraProyecto();
    void crearEstructuraPerifericos();

    bool cambiarElementoComentario(const QString& oldElementId, const QString& newElementId);
    void borrarComentario(const QString &commentId);
    int addNuevoComentario(const QString &comentario);

    QDomDocument m_doc;
    QFile m_file;
    QDir m_dir;
};

#endif // XMLHANDLER_H

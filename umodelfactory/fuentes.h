#ifndef FUENTES_H
#define FUENTES_H

#include <QString>
#include <QTextStream>
#include "dialoggenerarcodigo.h"

// Estructura de una transicion
struct Transiciones
{
    QString idEvento;
    QString idAcciones;
    QString idProxEstado;
};

// Estructura de un estado
struct Estados
{
    QString NombreEstado;
    QList< struct Transiciones > trn;
};

// Estructura de maquina de Estados
struct MdeE
{
    QString NombreMaquina;
    QList< struct Estados > est;
    QStringList variables_x_maquina;

};

//Estructura de los comentarios
struct Comentario
{
    QString textoComentario;
    QString perteneceA;
};

struct Elementos
{
    QStringList nMaquinas;
    QList< QStringList > nEtados;
    QStringList nAcciones;
    QStringList nMacros;
    QStringList nTemporizadores;
    QStringList nEventos;
    QStringList nExpreciones;
    QStringList nVariables;
    QStringList nConstantes;
    QList<Comentario> nComentarios;

    QString Autor;
    QString Fecha;
    QString Proyecto;
    QString Carpetas;
    
    void clear_elementos ( void ) {
        nMaquinas.clear();
        nEtados.clear();
        nAcciones.clear();
        nEventos.clear();
        nExpreciones.clear();
        nVariables.clear();
        nConstantes.clear();
        
        Autor = "";
        Fecha = "";
        Proyecto = "";
        Carpetas = "";        
    }
};


class Fuentes
{
    QList< struct MdeE > nMaquinas;
    struct Elementos elementos;
    QString agregarTextos( const QString & Template );

protected:
    QString  m_carpeta;
    DialogGenerarCodigo m_permiso;
    QStringList m_ultimosEstados;

public:
    Fuentes();
    QString GeneracionDeCodigo_switch( DialogGenerarCodigo &p ) ;
    QString GeneracionDeCodigo_pFuncion( DialogGenerarCodigo &p ) ;

    void GeneracioDeCodigo_C_Funciones_MdeE( QString & );
    void GeneracioDeCodigo_H_Funciones_MdeE( QString & );
    void GeneracioDeCodigo_C_MdeE_Switch( QString & );
    void GeneracioDeCodigo_H_MdeE( QString & );
    void GeneracioDeCodigo_C_Depuracion( QString & );
    void GeneracioDeCodigo_H_Depuracion( QString & );
    void GeneracioDeCodigo_main( QString & );

    void GeneracioDeCodigo_C_pFuncion( QString &cp );

    QString ArmadoDeFuncionesDeEstados   ( void );
    QString ArmadoDeFuncionMaquinaDeEstadosPF  ( void );

    void GeneracioDeCodigo_C_Inicializacion(  QString &  );
    void GeneracioDeCodigo_H_Inicializacion(  QString &  );

    QString ArmarFuncionMaquinaDeEstadosSwitch  ( void );
    QString ArmarTransicion ( struct Transiciones &tr  , int estado );
    QString ArmarTransicion ( struct Transiciones &tr , int estado , int maquina , int transicion );

    QString ArmarTransicionPf ( struct Transiciones &tr  , int estado );
    QString ArmarTransicionPf ( struct Transiciones &tr , int estado , int maquina , int transicion );

    QString IntercamioTexto( QString &destino , QString &TextoNuevo);
    QString CompletarTextos ( const char *Template ,  QString &nombre_funcion );
    QString CompletarTextosTemporizador ( const char *Template ,  QString &nombre_funcion ,QString &t , QString &b );

    void SetValores( QList< struct MdeE > &Maquinas , struct Elementos &Elm );
    void SetLog( bool dep );
    void SetComent( bool com );
    void SetCarpeta( QString & q);

};


#endif // FUENTES_H

#include "fuentes.h"
#include <QString>
#include <string.h>
#include "qfile.h"
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "xmlhandler.h"

Fuentes::Fuentes()
{   
    m_carpeta = "";
}

void Fuentes::SetLog( bool dep )
{
   //Clave::SetDepurar(dep);
}

void Fuentes::SetComent( bool com )
{
    //depurar. m_coment = com;
}

void Fuentes::SetValores( QList< struct MdeE > &Maquinas , struct Elementos &Elm )
{
    nMaquinas = Maquinas;
    elementos = Elm;
}

void Fuentes::SetCarpeta( QString & q)
{
    m_carpeta = q;
}

QString Fuentes::GeneracionDeCodigo_pFuncion( DialogGenerarCodigo &p )
{
    QString crp;

    bool d = p.getDepurar();
    bool c = p.getComentarios();

    m_permiso.SetComentarios(p.getComentarios());
    m_permiso.SetDepurar(p.getDepurar());

    int i = 0;

    if ( d == false && c == false )
        i = 0;
    if ( d == false && c == true )
        i = 2;
    if ( d == true && c == false )
        i = 1;
    if ( d == true && c == true )
        i = 3;

   switch ( i )
   {
        case 0:
            crp = "/Resultados_" + elementos.Proyecto +  "/pFuncion/SinDepuracionSinComentariosExtendidos";
            break;
        case 2:
            crp = "/Resultados_" + elementos.Proyecto +  "/pFuncion/SinDepuracionConComentariosExtendidos";
            break;
        case 1:
            crp = "/Resultados_" + elementos.Proyecto + "/pFuncion/ConDepuracionSinComentariosExtendidos";
            break;
        case 3:
            crp = "/Resultados_" + elementos.Proyecto + "/pFuncion/ConDepuracionConComentariosExtendidos";
            break;
    }

    QDir().mkpath(elementos.Carpetas + crp );
    crp = elementos.Carpetas + crp ;

    GeneracioDeCodigo_C_Funciones_MdeE( crp );
    GeneracioDeCodigo_H_Funciones_MdeE( crp );
    GeneracioDeCodigo_C_pFuncion( crp );
    GeneracioDeCodigo_H_MdeE( crp );
    GeneracioDeCodigo_main( crp );
    GeneracioDeCodigo_C_Inicializacion(  crp );
    GeneracioDeCodigo_H_Inicializacion ( crp );

    if ( d )
    {
        GeneracioDeCodigo_C_Depuracion( crp );
        GeneracioDeCodigo_H_Depuracion( crp );
    }
    return crp;
}


QString Fuentes::GeneracionDeCodigo_switch( DialogGenerarCodigo &p )
{
    QString crp;

    bool d = p.getDepurar();
    bool c = p.getComentarios();

    m_permiso.SetComentarios(p.getComentarios());
    m_permiso.SetDepurar(p.getDepurar());

    int i = 0;

    if ( d == false && c == false )
        i = 0;
    if ( d == false && c == true )
        i = 2;
    if ( d == true && c == false )
        i = 1;
    if ( d == true && c == true )
        i = 3;

   switch ( i )
   {
        case 0:
            crp = "/Resultados_" + elementos.Proyecto +  "/switch/SinDepuracionSinComentariosExtendidos";
            break;
        case 2:
            crp = "/Resultados_" + elementos.Proyecto +  "/switch/SinDepuracionConComentariosExtendidos";
            break;
        case 1:
            crp = "/Resultados_" + elementos.Proyecto + "/switch/ConDepuracionSinComentariosExtendidos";
            break;
        case 3:
            crp = "/Resultados_" + elementos.Proyecto + "/switch/ConDepuracionConComentariosExtendidos";
            break;
    }

    QDir().mkpath(elementos.Carpetas + crp );
    crp = elementos.Carpetas + crp ;

    GeneracioDeCodigo_C_Funciones_MdeE( crp );
    GeneracioDeCodigo_H_Funciones_MdeE( crp );
    GeneracioDeCodigo_C_MdeE_Switch( crp );
    GeneracioDeCodigo_H_MdeE( crp );
    GeneracioDeCodigo_main( crp );
    GeneracioDeCodigo_C_Inicializacion(  crp );
    GeneracioDeCodigo_H_Inicializacion ( crp );

    if ( d )
    {
        GeneracioDeCodigo_C_Depuracion( crp );
        GeneracioDeCodigo_H_Depuracion( crp );
    }
    return crp;
}

void Fuentes::GeneracioDeCodigo_C_Funciones_MdeE( QString &cp )
{
    QFile archivo(cp + "/AP_FuncionesMDE.c");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();

    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }

        QString auxi = "";
        Texto = CompletarTextos ( "2-Descripcion_FuncionesMdeE.c" , auxi);
        stream << Texto << endl;

        Texto = agregarTextos( "3-includes.c");
        stream << Texto ;

        Texto = agregarTextos( "15-IncludeFuncionesMdeE.c");
        stream << Texto ;
        if ( d ==  true )
        {
            Texto = agregarTextos( "16-IncludeDepurar.c");
            stream << Texto << endl;
        }
        else
          stream << endl;

        Texto = agregarTextos( "4-Defines.c");
        stream << Texto << endl;

        stream << endl;
        if ( c )
        {
            Texto = agregarTextos( "5-Macros.c");
            stream << Texto  << endl;

            Texto = agregarTextos( "6-tipos.c");
            stream << Texto << endl;

            Texto = agregarTextos( "10-PrototiposFuncionesPrivadas.c");
            stream << Texto << endl;

            Texto = agregarTextos( "7-Tablas.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "8-VariablesGlobales.c");
        stream << Texto << endl;

        if ( c )
        {
            Texto = agregarTextos( "9-VariablesEstaticas.c");
            stream << Texto << endl;

            Texto = agregarTextos( "11-FuncionesPrivadas.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "12-FuncionesPublicas.c");
        stream << Texto << endl;

        Texto = agregarTextos( "28-EVENTOS.c");
        stream << Texto << endl;

        for ( int i = 0 ; elementos.nEventos[i] != "" ; i++)
        {
            Texto = CompletarTextos ( "19-EventoCompuesto.c" , elementos.nEventos[i] );
            stream << Texto << endl;
        }

        Texto = agregarTextos( "27-ACCIONES.c");
        stream << Texto << endl;

        for ( int i = 0 ; elementos.nAcciones[i] != "" ; i++)
        {
            elementos.nAcciones[i].replace("()" , "");
            Texto = CompletarTextos ( "20-AccionCompuesta.c" , elementos.nAcciones[i] );
            stream << Texto << endl;
        }

        Texto = agregarTextos( "27a-Temporizadores.c");
        stream << Texto << endl;

        for ( int i = 0 ; elementos.nTemporizadores[i] != "" ; i++)
        {
            QStringList tmp;
            QString base,t;
            tmp = elementos.nTemporizadores[i].split('_');

            if (tmp[tmp.count()-1] == "D")
                base = "DECIMAS";
            else
                base = "SEGUNDOS";

            QString aux = "t";
            for ( int m = 1 ; m < tmp.count()-2 ; m++)
            {
                aux += "_";
                aux += tmp[m];
            }

            Texto = CompletarTextosTemporizador ( "20b-Temporizador.c" , aux , tmp[tmp.count()-2] , base);
            stream << Texto << endl;
        }
        archivo.close();
    }
}

void Fuentes::GeneracioDeCodigo_H_Funciones_MdeE( QString &cp )
{
    QFile archivo(cp + "/AP_FuncionesMDE.h");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }
        QString aux = "";
        Texto = CompletarTextos ( "29-Descripcion_FuncionesMdeE_H.c" , aux );

        stream << Texto << endl;
        stream << "#ifndef INC_AP_FUNCIONESMDEE_H_" << endl;
        stream << "#define INC_AP_FUNCIONESMDEE_H_" << endl << endl;

        Texto = agregarTextos( "30-includesGlobales.c");
        stream << Texto << endl;

        Texto = agregarTextos( "31-DefinesGlobales.c");
        stream << Texto ;

        //char auxi[50];
        for ( int i = 0 ; elementos.nConstantes[i]!= "" ; i++ )
        {
            QString str = elementos.nConstantes[i];
            QStringList strSplited = str.split(',');
            Texto = "#define\t\t" + strSplited.at(0) + "\t\t" + strSplited.at(1);
            for (int j = 0; j < elementos.nComentarios.length(); j++) {
                if (elementos.nComentarios[j].perteneceA == elementos.nConstantes[i]) {
                    Texto += "\t//" + elementos.nComentarios[j].textoComentario.replace("\n* ", " ").replace("/*\n", "").replace("/n*/", "");
                }
            }

            /*
            sprintf( auxi ,"#define\t\t%-16s\t\t%-2s" ,strSplited.at(0).toStdString().c_str(), strSplited.at(1).toStdString().c_str() );

            Texto = auxi;
            */
            stream << Texto << endl;
        }
        stream << endl;

        if ( c )
        {
            Texto = agregarTextos( "32-Macros Globales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "33-tipos globales.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "8-VariablesGlobales.c");
        stream << Texto << endl;

        Texto = agregarTextos( "34-PrototiposFuncionesglobales.c");
        stream << Texto << endl;

        Texto = agregarTextos( "28-EVENTOS.c");
        stream << Texto << endl;

        for ( int i = 0 ; elementos.nEventos[i] != "" ; i++)
        {
            Texto = agregarTextos( "35-PrototiposEventos.c");
            Texto = IntercamioTexto ( Texto , elementos.nEventos[i] );
            stream << Texto ;
        }

        stream << endl;

        Texto = agregarTextos( "27-ACCIONES.c");
        stream << Texto << endl;

        for ( int i = 0 ; elementos.nAcciones[i] != "" ; i++)
        {
            Texto = agregarTextos( "36-PrototiposAcciones.c");
            Texto = IntercamioTexto ( Texto , elementos.nAcciones[i] );
            stream << Texto ;
        }
        stream << endl;
        Texto = agregarTextos( "27a-Temporizadores.c");
        stream << Texto << endl;

        for ( int i = 0 ; elementos.nTemporizadores[i] != "" ; i++)
        {
            QStringList l;
            l =  elementos.nTemporizadores[i].split("_");
            Texto = agregarTextos( "36-PrototiposAcciones.c");

            QString aux = "t";
            for ( int m = 1 ; m < l.count()-2 ; m++)
            {
                aux += "_";
                aux += l[m];
            }

            Texto = IntercamioTexto ( Texto , aux );
            stream << Texto ;
        }
        stream << endl;
        stream << "#endif /* INC_AP_FUNCIONESMDEE_H_ */"<< endl;
		archivo.close();
     }
}

void Fuentes::GeneracioDeCodigo_C_MdeE_Switch( QString &cp )
{
    QFile archivo(cp + "/AP_MDE.c");
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }

        QString auxi = "";
        Texto = CompletarTextos ( "38-Descripcion_MdeE.c" , auxi);
        stream << Texto ;

        Texto = agregarTextos( "3-includes.c");
        stream << Texto ;

        Texto = agregarTextos( "14-IncludeMdeE.c");
        stream << Texto ;
        if ( d ==  true )
        {
            Texto = agregarTextos( "16-IncludeDepurar.c");
            stream << Texto << endl;
        }
        else
          stream << endl;

        Texto = agregarTextos( "4-Defines.c");
        stream << Texto ;

        int j = 0 ;
        char aux [ 100];
        /*
        Texto = "RESET";

        sprintf( aux ,"#define\t\t%-25s\t\t0" ,Texto.toStdString().c_str( ) );

        Texto = aux ;*/
        
        Texto = "#define\t\tRESET\t\t0\t";
        stream << Texto << endl  ;

        for ( int i = 0 ; elementos.nEtados[i][0]!= "" ; i++ )
        {
            j = 0;

            stream << "//Maquina: " << elementos.nMaquinas[i] << endl ;

            while ( elementos.nEtados[i][j] != "" )
            {
                Texto = "#define\t\t" + elementos.nEtados[i][j] + "\t\t" + QString::number(j + 1) + "\t// < Maquina: " + elementos.nMaquinas[i];
                for (int p = 0; p < elementos.nComentarios.length(); p++) {
                    if (elementos.nComentarios[p].perteneceA == elementos.nEtados[i][j]) {
                        Texto += ", " + elementos.nComentarios[p].textoComentario.replace("\n* ", " ").replace("/*\n", "").replace("/n*/", "");
                    }
                }
                /*sprintf( aux ,"#define\t\t%-25s\t\t%d\t//!< Maquina: %s, Coloque aqui una descripcion " ,elementos.nEtados[i][j].toStdString().c_str(), (j + 1) , elementos.nMaquinas[i].toStdString().c_str() );
                j++;
                Texto = aux;*/
                
                Texto += " >";
                j++;
                stream << Texto << endl;
            }
            stream << endl;
        }
        Texto = agregarTextos( "5-Macros.c");
        stream << Texto ;

        for ( int i = 0 ; elementos.nMacros[i] != "" ; i++)
        {
            Texto = CompletarTextos ( "20a-AccionSimple.c" , elementos.nMacros[i] );
            for (int p = 0; p < elementos.nComentarios.length(); p++) {
                if (elementos.nComentarios[p].perteneceA == elementos.nMacros[i]) {
                    if (Texto != "") Texto[Texto.size() - 1] = '\t'; // reemplazo el final de linea que es un \n por un \t, cosa de poner
                                                                     // el comentario en la misma linea
                    Texto += "// " + elementos.nComentarios[p].textoComentario.replace("\n* ", " ").replace("/*\n", "").replace("/n*/", "");
                }
            }
            stream << Texto << endl;
        }
        stream << endl;

        if ( c )
        {
            Texto = agregarTextos( "6-tipos.c");
            stream << Texto << endl;
        }
        Texto = agregarTextos( "10-PrototiposFuncionesPrivadas.c");
        stream << Texto ;

        for ( int i = 0 ; elementos.nMaquinas[i] != "" ; i++)
        {
            Texto = agregarTextos( "41-PrototiposMaquinasDeEstado.c");
            Texto = IntercamioTexto ( Texto , elementos.nMaquinas[i] );
            stream << Texto ;
        }
        stream <<  endl;
        if ( c )
        {
            Texto = agregarTextos( "7-Tablas.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "8-VariablesGlobales.c");
        stream << Texto ;

        for ( int i = 0 ; elementos.nVariables[i] != "" ; i ++ )
        {
            Texto = agregarTextos( "44-Declaracion de variables.c");
            Texto = IntercamioTexto( Texto , elementos.nVariables[i] );
            Texto = Texto.replace("\n" , "");
            /*
            sprintf( aux ,"%-25s\t\t//!< Coloque aqui una descripcion" ,Texto.toStdString().c_str());
            Texto = aux ;
            */
            Texto += "\t\t//";
            for (int j = 0; j < elementos.nComentarios.length(); j++) {
                if (elementos.nComentarios[j].perteneceA == elementos.nVariables[i]) {
                    Texto += elementos.nComentarios[j].textoComentario.replace("\n* ", " ").replace("/*\n", "").replace("/n*/", "");
                }
            }
            stream << Texto << endl ;
        }
        stream << endl ;

        if ( c )
        {
            Texto = agregarTextos( "9-VariablesEstaticas.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "11-FuncionesPrivadas.c");
        stream << Texto << endl;
        for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
        {
            Texto = CompletarTextos ( "26-Descripcion funciones Maquinas de estado.c" , nMaquinas[k].NombreMaquina );
            stream << Texto << endl;

            Texto = agregarTextos( "22a-SW-Encabezado.c");
            Texto = IntercamioTexto( Texto , nMaquinas[k].NombreMaquina );

            stream << Texto ;

            if (d)
                Texto = agregarTextos( "22c-SW-Encabezado_depurar.c");
            else
                Texto = agregarTextos( "22b-SW-Encabezado.c");

            stream << Texto << endl;
            int j ;
            for ( j = 0 ; nMaquinas[k].est[j].NombreEstado != ""  ; j++)
            {

                Texto = agregarTextos( "23-SW-case.c" ) ;
                Texto = IntercamioTexto( Texto , nMaquinas[k].est[j].NombreEstado );
                stream << Texto ;

                struct Transiciones t;

                for (int i = 0 ; nMaquinas[k].est[j].trn[i].idEvento != ""  ; i++)
                {
                    t = nMaquinas[k].est[j].trn[i];

                    if (d)
                        Texto = ArmarTransicion ( t , j  , k , i );
                    else
                        Texto = ArmarTransicion ( t , j );

                    //Texto = ArmarTransicion ( t , j );
                    stream << Texto << endl;
                }
                Texto = agregarTextos( "23b-SW-break.c");
                stream << Texto << endl;
            }

            Texto = agregarTextos( "24-SW-default.c");
            stream << Texto;
            if (d)
            {
                QString aux = "Log ( " + QString::number( k ) + " , " + QString::number( j ) + " , " + QString::number( 0 ) + " )";
                Texto = agregarTextos( "45-DepurarSW_default.c");
                Texto.replace( "<Log>" , aux );
                stream << Texto;
            }

            Texto = agregarTextos( "23b-SW-break.c");
            stream << Texto << endl;
            Texto = agregarTextos( "25-SW-salida.c");
            stream << Texto << endl;
        }
        Texto = agregarTextos( "12-FuncionesPublicas.c");
        stream << Texto << endl;

        Texto = ArmarFuncionMaquinaDeEstadosSwitch( );
        stream << Texto << endl;
		archivo.close();
     }
}

void Fuentes::GeneracioDeCodigo_H_MdeE( QString &cp )
{
    QFile archivo(cp + "/AP_MDE.h");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }
        QString aux = "";
        Texto = CompletarTextos ( "40-Descripcion_MdeE_H.c" , aux );

        stream << Texto << endl;
        stream << "#ifndef INC_AP_MDEE_H_" << endl;
        stream << "#define INC_AP_MDEE_H_" << endl << endl;

        Texto = agregarTextos( "30-includesGlobales.c");
        stream << Texto << endl;

        Texto = agregarTextos( "31-DefinesGlobales.c");
        stream << Texto << endl;

        if ( c )
        {
            Texto = agregarTextos( "32-Macros Globales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "33-tipos globales.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "8-VariablesGlobales.c");
        stream << Texto ;

        for ( int i = 0 ; elementos.nVariables[i] != "" ; i ++ )
        {
            Texto = agregarTextos( "44-Declaracion de variables.c");
            Texto = IntercamioTexto( Texto , elementos.nVariables[i] );
            stream <<  "extern "  << Texto ;
        }
        stream << endl;
        Texto = agregarTextos( "34-PrototiposFuncionesglobales.c");
        stream << Texto ;

        QString q = "MaquinaDeEstados";
        Texto = CompletarTextos ( "36-PrototiposAcciones.c" , q );

        stream << Texto << endl;

        stream << "#endif /* INC_AP_MDEE_H_ */"<< endl;
		archivo.close();
     }
}

void Fuentes::GeneracioDeCodigo_C_Inicializacion( QString &cp )
{
    QFile archivo(cp + "/AP_Inicializacion.c");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }

        QString auxi = "";
        Texto = CompletarTextos ( "52-Descripcion_Inicializacion.c" , auxi);
        stream << Texto << endl;

        Texto = agregarTextos( "3-includes.c");
        stream << Texto ;

            Texto = agregarTextos( "14c-IncludeMdeE.c");
            stream << Texto ;
            if ( d ==  true )
            {
                Texto = agregarTextos( "16-IncludeDepurar.c");
                stream << Texto << endl;
            }
            else
              stream << endl;

        Texto = agregarTextos( "4-Defines.c");
        stream << Texto << endl;

        if ( c )
        {
            Texto = agregarTextos( "5-Macros.c");
            stream << Texto << endl;

            Texto = agregarTextos( "6-tipos.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "10-PrototiposFuncionesPrivadas.c");
        stream << Texto ;

        for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
        {
            Texto = agregarTextos( "51-Prototipos Funciones de inicializacion.c");
            Texto.replace( "<nombre>" , nMaquinas[k].NombreMaquina);
            stream << Texto ;
        }
        stream << endl ;

        if ( c )
        {
            Texto = agregarTextos( "7-Tablas.c");
            stream << Texto << endl;

            Texto = agregarTextos( "8-VariablesGlobales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "9-VariablesEstaticas.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "11-FuncionesPrivadas.c");
        stream << Texto << endl;

        QString inMaquinas = "\n\t";
        for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
        {
            Texto = agregarTextos( "50-Funciones de inicializacion.c");
            inMaquinas += "Inicializar_";
            inMaquinas += nMaquinas[k].NombreMaquina ;
            inMaquinas += "( );\n\n\t" ;
            QString vxm = "";
            for ( int i = 0 ; i < nMaquinas[k].variables_x_maquina.length() ; i++)
            {
                vxm += "\t";
                vxm += nMaquinas[k].variables_x_maquina[i];
                vxm += " = 0 ;\n";
            }
            Texto.replace("<nombre>", nMaquinas[k].NombreMaquina );
            Texto.replace("<Autor>",elementos.Autor );
            Texto.replace("<Fecha>",elementos.Fecha );
            Texto.replace("<variables>",vxm );

            stream << Texto << endl;
        }


        Texto = agregarTextos( "12-FuncionesPublicas.c");
        stream << Texto << endl;
        Texto = agregarTextos( "49-funcionInicializar.c");
        Texto.replace("<Autor>",elementos.Autor );
        Texto.replace("<Fecha>",elementos.Fecha );
        Texto.replace("<inicializaciones>",inMaquinas );

        stream << Texto << endl;
		archivo.close();
     }
}
void Fuentes::GeneracioDeCodigo_H_Inicializacion( QString &cp )
{
    QFile archivo(cp + "/AP_Inicializacion.h");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }
        QString aux = "";
        Texto = CompletarTextos ( "53-Descripcion_Inicializacion_H.c" , aux );

        stream << Texto << endl;
        stream << "#ifndef INC_AP_INICIALIZACION_H_" << endl;
        stream << "#define INC_AP_INICIALIZACION_H_" << endl << endl;

        Texto = agregarTextos( "30-includesGlobales.c");
        stream << Texto << endl;

        Texto = agregarTextos( "31-DefinesGlobales.c");
        stream << Texto << endl;

        if ( c )
        {
            Texto = agregarTextos( "32-Macros Globales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "33-tipos globales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "8-VariablesGlobales.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "34-PrototiposFuncionesglobales.c");
        stream << Texto ;

        stream << "void Inicializar( void ) ;" << endl << endl;

        stream << "#endif /* INC_AP_INICIALIZACION_H_ */"<< endl;
		archivo.close();
     }
}
void Fuentes::GeneracioDeCodigo_C_Depuracion( QString &cp )
{
    QFile archivo(cp + "/AP_Depuracion.c");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }

        QString auxi = "";
        Texto = CompletarTextos ( "47-Descripcion_Depuracion.c" , auxi);
        stream << Texto ;

        Texto = agregarTextos( "3-includes.c");
        stream << Texto ;
        stream << "#include " << "\"AP_Depuracion.h\""<< endl;

        Texto = agregarTextos( "16-IncludeDepurar.h");
        stream << Texto << endl;

        Texto = agregarTextos( "4-Defines.c");
        stream << Texto << endl;

        if ( c )
        {
            Texto = agregarTextos( "5-Macros.c");
            stream << Texto << endl;

            Texto = agregarTextos( "6-tipos.c");
            stream << Texto << endl;

            Texto = agregarTextos( "10-PrototiposFuncionesPrivadas.c");
            stream << Texto << endl;

            Texto = agregarTextos( "7-Tablas.c");
            stream << Texto << endl;

            Texto = agregarTextos( "8-VariablesGlobales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "9-VariablesEstaticas.c");
            stream << Texto << endl;

            Texto = agregarTextos( "11-FuncionesPrivadas.c");
            stream << Texto << endl;
        }
        Texto = agregarTextos( "12-FuncionesPublicas.c");
        stream << Texto << endl;

        QString nada ="";
        Texto = CompletarTextos ( "46-funcionDepurar.c" , aux);

        stream << Texto << endl;
		archivo.close();
     }
}
void Fuentes::GeneracioDeCodigo_H_Depuracion( QString &cp )
{
    QFile archivo(cp + "/AP_Depuracion.h");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);

        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }
        QString aux = "";
        Texto = CompletarTextos ( "48-Descripcion_Depuracion_h.c" , aux );

        stream << Texto << endl;
        stream << "#ifndef INC_AP_DEPURACION_H_" << endl;
        stream << "#define INC_AP_DEPURACION_H_" << endl << endl;

        Texto = agregarTextos( "30-includesGlobales.c");
        stream << Texto << endl;

        Texto = agregarTextos( "31-DefinesGlobales.c");
        stream << Texto ;
        stream << "#define   DEPURACION" << endl << endl;

        if ( c )
        {
            Texto = agregarTextos( "32-Macros Globales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "33-tipos globales.c");
            stream << Texto << endl;

            Texto = agregarTextos( "8-VariablesGlobales.c");
            stream << Texto ;
        }
        Texto = agregarTextos( "34-PrototiposFuncionesglobales.c");
        stream << Texto ;

        stream << "void Log( char id_Maquina , char id_Estado , char id_Evento ) ;" << endl << endl;

        stream << "#endif /* INC_AP_DEPURACION_H_ */"<< endl<< endl;
		archivo.close();
     }
}
void Fuentes::GeneracioDeCodigo_main( QString &cp )
{
    QFile archivo(cp + "/main.c");

    QString aux="";
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();
    if (archivo.open(QIODevice::ReadWrite))
    {
        //incluyo header
        QTextStream stream(&archivo);
        QString Texto ;
        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }

        QString auxi = "";
        Texto = CompletarTextos ( "37-ExplicacionMain.c" , auxi);
        stream << Texto << endl;

        Texto = agregarTextos( "3-includes.c");
        stream << Texto << endl;

            Texto = agregarTextos( "14a-IncludeMdeE.c");
            stream << Texto ;
            if ( d ==  true )
            {
                Texto = agregarTextos( "16-IncludeDepurar.c");
                stream << Texto << endl;
            }
            else
              stream << endl;

            Texto = agregarTextos( "13-main.c");
            stream << Texto << endl;
            
            archivo.close();
    }
}

void Fuentes::GeneracioDeCodigo_C_pFuncion( QString &cp )
{
    QFile archivo(cp + "/AP_MDE.c");
    bool d = m_permiso.getDepurar();
    bool c = m_permiso.getComentarios();

    if (archivo.open(QIODevice::ReadWrite))
    {
        QString Texto;
        QTextStream stream(&archivo);

        if ( c )
        {
            Texto = agregarTextos( "1-Declaracion.c");
            stream << Texto << endl;
        }

        QString auxi = "";
        Texto = CompletarTextos ( "38-Descripcion_MdeE.c" , auxi);
        stream << Texto ;

        Texto = agregarTextos( "3-includes.c");
        stream << Texto ;

            Texto = agregarTextos( "14-IncludeMdeE.c");
            stream << Texto ;
            if ( d ==  true )
            {
                Texto = agregarTextos( "16-IncludeDepurar.c");
                stream << Texto << endl;
            }
            else
              stream << endl;

        Texto = agregarTextos( "4-Defines.c");
        stream << Texto ;

        int j = 0 ;
        char aux [ 100 ];
        /*
        Texto = "RESET";

        sprintf( aux ,"#define\t\t%-25s\t\t0" ,Texto.toStdString().c_str( ) );

        Texto = aux ;
        */
        Texto = "#define\t\tRESET\t\t0\t";
        stream << Texto << endl;
        m_ultimosEstados.clear();
        for ( int i = 0 ; elementos.nEtados[i][0]!= "" ; i++ )
        {
            j = 0;

            stream << "//Maquina: " << elementos.nMaquinas[i] << endl ;

            while ( elementos.nEtados[i][j] != "" )
            {
                Texto = "#define\t\t" + elementos.nEtados[i][j] + "\t\t" + QString::number(j + 1) + "\t// < Maquina: " + elementos.nMaquinas[i];
                for (int p = 0; p < elementos.nComentarios.length(); p++) {
                    if (elementos.nComentarios[p].perteneceA == elementos.nEtados[i][j]) {
                        Texto += ", " + elementos.nComentarios[p].textoComentario.replace("\n* ", " ").replace("/*\n", "").replace("/n*/", "");
                    }
                }
                Texto += " >";
                /*
                sprintf( aux ,"#define\t\t%-25s\t\t%d\t//!< Maquina: %s, Coloque aqui una descripcion " ,elementos.nEtados[i][j].toStdString().c_str(), (j + 1) , elementos.nMaquinas[i].toStdString().c_str() );
                Texto = aux;
                */
                if (elementos.nEtados[i][ j + 1] == "")
                    m_ultimosEstados<<elementos.nEtados[i][j];

                j++;
                Texto = aux;

                stream << Texto << endl;
            }
            stream << endl;
        }

        Texto = agregarTextos( "5-Macros.c");
        stream << Texto ;

        for ( int i = 0 ; elementos.nMacros[i] != "" ; i++)
        {
            Texto = CompletarTextos ( "20a-AccionSimple.c" , elementos.nMacros[i] );
            stream << Texto;
        }
        stream << endl;

        if ( c )
        {

            Texto = agregarTextos( "6-tipos.c");
            stream << Texto << endl;
        }
        Texto = agregarTextos( "10-PrototiposFuncionesPrivadas.c");
        stream << Texto ;

        QString prototipo ;

        for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
        {
            int j ;
            stream << "//!< Maquina :" <<nMaquinas[k].NombreMaquina << endl;
            for ( j = 0 ; nMaquinas[k].est[j].NombreEstado != ""  ; j++)
            {
                Texto = agregarTextos( "41-PrototiposMaquinasDeEstado.c");
                prototipo = nMaquinas[k].est[j].NombreEstado;
                prototipo = nMaquinas[k].NombreMaquina+ "_" + prototipo.toLower();
                Texto.replace("<NombreFuncion>" , prototipo);
                stream << Texto;
            }
        }
        stream << endl;

        Texto = agregarTextos( "7-Tablas.c");
        stream << Texto ;
        for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
        {
            int j ;
            Texto = agregarTextos( "55-tablasPf.c");
            QString Texto1 = "\t";

            for ( j = 0 ; nMaquinas[k].est[j].NombreEstado != ""  ; j++)
            {
                prototipo = nMaquinas[k].est[j].NombreEstado;
                Texto1 += nMaquinas[k].NombreMaquina+ "_" + prototipo.toLower();
                if ( nMaquinas[k].est[j+1].NombreEstado == "")
                    Texto1 += " \n";
                else
                    Texto1 += " ,\n\t";
            }

            Texto.replace("<listado>" , Texto1 );
            Texto.replace("<maquina>" , nMaquinas[k].NombreMaquina );

            stream << Texto << endl;

        }

        Texto = agregarTextos( "8-VariablesGlobales.c");
        stream << Texto ;

        for ( int i = 0 ; elementos.nVariables[i] != "" ; i ++ )
        {
            Texto = agregarTextos( "44-Declaracion de variables.c");
            Texto = IntercamioTexto( Texto , elementos.nVariables[i] );
            Texto.replace("\n" , "");
            sprintf( aux ,"%-25s\t\t//!< Coloque aqui una descripcion" ,Texto.toStdString().c_str());
            Texto = aux ;
            stream << Texto << endl ;
        }
        stream << endl ;

        if ( c )
        {
            Texto = agregarTextos( "9-VariablesEstaticas.c");
            stream << Texto << endl;
        }

        Texto = agregarTextos( "11-FuncionesPrivadas.c");
        stream << Texto ;

        Texto = ArmadoDeFuncionesDeEstados  ();
        stream << Texto ;

        Texto = agregarTextos( "12-FuncionesPublicas.c");
        stream << Texto << endl;

        Texto = ArmadoDeFuncionMaquinaDeEstadosPF( );
        stream << Texto << endl;

     }
}
QString Fuentes::IntercamioTexto( QString &destino , QString &TextoNuevo)
{
    QString auxDestino = "" ,auxresto;
    int a;
    a = auxDestino.length();
    char buscar[500],*q , *p;
    memset ( buscar , 0 , 499);
    memcpy( buscar, destino.toStdString().c_str() , ((size_t)destino.size()));
    buscar[ ((size_t)destino.size()) ];
    q = strstr( buscar ,"<" ) ;
    *q = '\0';
    q++;

    auxDestino = buscar;
    auxDestino += TextoNuevo;

    q = strstr( q ,">" ) ;
    q++;

    auxresto = q;
    a = auxresto.length();
    q[ a ] =  '\0';

    auxDestino += q;
    return auxDestino;
}

QString Fuentes::CompletarTextos ( const char *Template , QString &nombre_funcion )
{
    char *p , auxiliar[100];
    QString Texto = Template ;
    QString Nombre = "://templates/" + Texto ;
    QFile file(Nombre);

    QString res ="";

    if(!file.open(QIODevice::ReadOnly)) {
        return "" ;
    }

    QTextStream instream(&file);

    while (!instream.atEnd())
    {
        QString line = instream.readLine();

        memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

        p = auxiliar;
        if ( strstr ( p , "<NombreFuncion>" ) )
            line = IntercamioTexto ( line , nombre_funcion );

        if ( strstr ( p , "<Autor>" ) )
            line = IntercamioTexto ( line , elementos.Autor );

        if ( strstr ( p , "<Maquina>" ) )
            line = IntercamioTexto ( line ,  elementos.Proyecto );

        if ( strstr ( p , "<Proyecto>" ) )
            line = IntercamioTexto ( line ,  elementos.Proyecto );

        if ( strstr ( p , "<Fecha>" ) )
            line = IntercamioTexto ( line ,  elementos.Fecha );

        if ( strstr ( p , "<Comentario>" ) ) {
            QString auxCom = "";
            for(int i = 0; i < elementos.nComentarios.length(); i++) {
                if (elementos.nComentarios[i].perteneceA == nombre_funcion) {
                    auxCom = elementos.nComentarios[i].textoComentario;
                    auxCom = auxCom.prepend("/*\n* ").append("\n*/");
                    break;
                }
            }
            line = IntercamioTexto( line , auxCom );
        }

        memset(auxiliar , 0 , 98 );
        //res = line;
        line+="\n";
        res += line;
    }
    file.close( );

    return res;
}

QString Fuentes::CompletarTextosTemporizador ( const char *Template , QString &nombre_funcion , QString &t , QString &b )
{
    char *p , auxiliar[100];
    QString Texto = Template ;
    QString Nombre = "://templates/" + Texto ;
    QFile file(Nombre);

    QString res ="";

    if(!file.open(QIODevice::ReadOnly)) {
        return "" ;
    }

    QTextStream instream(&file);

    while (!instream.atEnd())
    {
        QString line = instream.readLine();

        memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

        p = auxiliar;
        if ( strstr ( p , "<NombreFuncion>" ) )
            line = IntercamioTexto ( line , nombre_funcion );

        if ( strstr ( p , "<Autor>" ) )
            line = IntercamioTexto ( line , elementos.Autor );

        if ( strstr ( p , "<Maquina>" ) )
            line = IntercamioTexto ( line ,  elementos.Proyecto );

        if ( strstr ( p , "<Proyecto>" ) )
            line = IntercamioTexto ( line ,  elementos.Proyecto );

        if ( strstr ( p , "<Fecha>" ) )
            line = IntercamioTexto ( line ,  elementos.Fecha );

        if ( strstr ( p , "<Tiempo>" ) )
            line = IntercamioTexto ( line ,  t );

        if ( strstr ( p , "<Base>" ) )
            line = IntercamioTexto ( line ,  b );

        if ( strstr ( p , "<Comentario>" ) ) {
            QString auxCom = "";
            for(int i = 0; i < elementos.nComentarios.length(); i++) {
                if (elementos.nComentarios[i].perteneceA == nombre_funcion) {
                    auxCom = elementos.nComentarios[i].textoComentario;
                    auxCom = auxCom.prepend("/*\n* ").append("\n*/");
                    break;
                }
            }
            line = IntercamioTexto( line , auxCom );
        }

        memset(auxiliar , 0 , 98 );
        //res = line;
        line+="\n";
        res += line;
    }
    file.close( );

    return res;
}


/**
 * @brief MainWindow::agregarTextoDeAdvertencia
 * @param evento_entrada
 * @param accion_salida
 * @param estado_salida
 * @return
 */
QString Fuentes::agregarTextos( const QString & Template )
{
    QString Nombre = "://templates/" + Template ;
    QFile file(Nombre);

    QString res ="";

    if(!file.open(QIODevice::ReadOnly)) {return "" ; }

    QTextStream instream(&file);

    while (!instream.atEnd())
    {
        QString line = instream.readLine();
        //res = line;
        line+="\n";
        res += line;
    }
    file.close();

    return res;
}

QString Fuentes::ArmarTransicion ( struct Transiciones &tr , int estado )
{
   char *p , auxiliar[150];
   QString Texto;
   if ( estado )
        Texto = "23a-SW-if.c" ;
    else
        Texto = "23c-SW-Reset.c" ;

   QString Nombre = "://templates/" + Texto ;
   QFile file(Nombre);

   QString res = "";

   if(!file.open(QIODevice::ReadOnly))
   {
       return "" ;
   }

   QTextStream instream(&file);

   while (!instream.atEnd())
   {
       QString line = instream.readLine();

       memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

       p = auxiliar;

       if ( strstr ( p , "<Siguiente>" ) )
           line = IntercamioTexto ( line ,  tr.idProxEstado );

       if ( strstr ( p , "<Evento>" ) )
           line = IntercamioTexto ( line ,  tr.idEvento );

       if ( strstr ( p , "<Acciones>" ) )
       {
           QStringList listAcciones;
           QString trAcciones = "";

           listAcciones = tr.idAcciones.split( ",");
           for ( int i = 0 ; i < listAcciones.size() ; i++ )
           {
               if ( listAcciones[i] != "")
               {
                   trAcciones += IntercamioTexto ( line ,   listAcciones[i] );
                   trAcciones += ";\n";
               }
           }
           line = trAcciones ;
       }

       if ( strstr ( p , "<Caso>" ) )
           line = IntercamioTexto ( line ,  tr.idAcciones );

       memset(auxiliar , 0 , 150 );

       line+="\n";
       res += line;
   }
   file.close();

   return res;

}
QString Fuentes::ArmarTransicionPf ( struct Transiciones &tr , int estado )
{
   char *p , auxiliar[150];
   QString Texto;
   if ( estado )
        Texto = "23d-pf-if.c" ;
    else
        Texto = "23e-pf-Reset.c" ;

   QString Nombre = "://templates/" + Texto ;
   QFile file(Nombre);

   QString res = "";

   if(!file.open(QIODevice::ReadOnly))
   {
       return "" ;
   }

   QTextStream instream(&file);

   while (!instream.atEnd())
   {
       QString line = instream.readLine();

       memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

       p = auxiliar;

       if ( strstr ( p , "<Siguiente>" ) )
           line = IntercamioTexto ( line ,  tr.idProxEstado );

       if ( strstr ( p , "<Evento>" ) )
           line = IntercamioTexto ( line ,  tr.idEvento );

       if ( strstr ( p , "<Acciones>" ) )
       {
           QStringList listAcciones;
           QString trAcciones = "";

           listAcciones = tr.idAcciones.split( ",");
           for ( int i = 0 ; i < listAcciones.size() ; i++ )
           {
               if ( listAcciones[i] != "")
               {
                   trAcciones += IntercamioTexto ( line ,   listAcciones[i] );
                   trAcciones += ";\n";
               }
           }
           line = trAcciones ;
       }

       if ( strstr ( p , "<Caso>" ) )
           line = IntercamioTexto ( line ,  tr.idAcciones );

       memset(auxiliar , 0 , 150 );

       line+="\n";
       res += line;
   }
   file.close();

   return res;

}
QString Fuentes::ArmarTransicionPf ( struct Transiciones &tr , int estado , int maquina , int transicion )
{
    char *p , auxiliar[150];
    QString Texto;

    if ( estado )
        Texto = "42b-DepurarPf.c";
    else
        Texto = "43b-DepurarPfreset.c" ;

   QString Nombre = "://templates/" + Texto ;
   QFile file(Nombre);

   QString res = "";

   if(!file.open(QIODevice::ReadOnly))
   {
       return "" ;
   }

   QTextStream instream(&file);

   while (!instream.atEnd())
   {
       QString line = instream.readLine();

       memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

       p = auxiliar;

       if ( strstr ( p , "<Log>" ) )
       {
           QString aux = "Log ( " + QString::number(maquina ) + " , " + QString::number( estado ) + " , " + QString::number( transicion ) + " )";
           line = IntercamioTexto ( line ,  aux );
       }

       if ( strstr ( p , "<Siguiente>" ) )
           line = IntercamioTexto ( line ,  tr.idProxEstado );

       if ( strstr ( p , "<Evento>" ) )
           line = IntercamioTexto ( line ,  tr.idEvento );

       if ( strstr ( p , "<Acciones>" ) )
       {
           QStringList listAcciones;
           QString trAcciones = "";

           listAcciones = tr.idAcciones.split( ",");
           for ( int i = 0 ; i < listAcciones.size() ; i++ )
           {
               if ( listAcciones[i] != "")
               {
                   QStringList l ;
                   l= listAcciones[i].split("_");

                   if (l[0] == "t")
                   {
                        QString aux = "t";
                        for ( int m = 1 ; m < l.count()-2 ; m++)
                        {
                            aux += "_";
                            aux += l[m];
                        }
                        aux += "()";

                        trAcciones += IntercamioTexto ( line ,   aux );
                   }
                   else
                       trAcciones += IntercamioTexto ( line ,   listAcciones[i] );
                   trAcciones += ";\n";
               }
           }
           line = trAcciones ;
       }

       if ( strstr ( p , "<Caso>" ) )
           line = IntercamioTexto ( line ,  tr.idAcciones );

       memset(auxiliar , 0 , 150 );
       //res = line;
       line+="\n";
       res += line;
   }
   file.close();

   return res;

}
QString Fuentes::ArmarTransicion ( struct Transiciones &tr , int estado , int maquina , int transicion )
{
    char *p , auxiliar[150];
    QString Texto;
    if ( estado )
        Texto = "42-DepurarSW.c";
    else
        Texto = "43-DepurarSWreset.c" ;


   QString Nombre = "://templates/" + Texto ;
   QFile file(Nombre);

   QString res = "";

   if(!file.open(QIODevice::ReadOnly))
   {
       return "" ;
   }

   QTextStream instream(&file);

   while (!instream.atEnd())
   {
       QString line = instream.readLine();

       memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

       p = auxiliar;

       if ( strstr ( p , "<Log>" ) )
       {
           QString aux = "Log ( " + QString::number(maquina ) + " , " + QString::number( estado ) + " , " + QString::number( transicion ) + " )";
           line = IntercamioTexto ( line ,  aux );
       }

       if ( strstr ( p , "<Siguiente>" ) )
           line = IntercamioTexto ( line ,  tr.idProxEstado );

       if ( strstr ( p , "<Evento>" ) )
           line = IntercamioTexto ( line ,  tr.idEvento );

       if ( strstr ( p , "<Acciones>" ) )
       {
           QStringList listAcciones;
           QString trAcciones = "";

           listAcciones = tr.idAcciones.split( ",");
           for ( int i = 0 ; i < listAcciones.size() ; i++ )
           {
                if ( listAcciones[i] != "")
                {
                    QStringList l ;
                    l= listAcciones[i].split("_");

                    if (l[0] == "t")
                    {
                         QString aux = "t";
                         for ( int m = 1 ; m < l.count()-2 ; m++)
                         {
                             aux += "_";
                             aux += l[m];
                         }
                         aux += "()";

                         trAcciones += IntercamioTexto ( line ,   aux );
                    }
                    else
                        trAcciones += IntercamioTexto ( line ,   listAcciones[i] );
                    trAcciones += ";\n";
                }
           }
           line = trAcciones ;
       }

       if ( strstr ( p , "<Caso>" ) )
           line = IntercamioTexto ( line ,  tr.idAcciones );

       memset(auxiliar , 0 , 150 );
       line+="\n";
       res += line;
   }
   file.close();

   return res;

}

QString Fuentes::ArmarFuncionMaquinaDeEstadosSwitch  ( void )
{
   char *p , auxiliar[150];
   QString Texto;
   Texto = "39-Descripcion MdeE switch.c" ;

   QString Nombre = "://templates/" + Texto ;
   QFile file(Nombre);

   QString res = "";

   if(!file.open(QIODevice::ReadOnly))
   {
       return "" ;
   }

   QTextStream instream(&file);

   while (!instream.atEnd())
   {
       QString line = instream.readLine();

       memcpy( auxiliar, line.toStdString().c_str() , ((size_t)line.size()));

       p = auxiliar;

       if ( strstr ( p , "<VariablesEstado>" ) )
       {
           QString VariablesEstado = "";

           for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
           {
               QString vrb = "static int estados_" ;
               vrb += nMaquinas[k].NombreMaquina;
               vrb += " = RESET";
               VariablesEstado += IntercamioTexto ( line ,  vrb );
               VariablesEstado += ";\n";
           }
           line = VariablesEstado ;
       }

       if ( strstr ( p , "<Maquina>" ) )
       {
           QString VariablesEstado = "";

           for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
           {
               QString vrb = "estados_" ;
               vrb += nMaquinas[k].NombreMaquina;
               vrb += " = ";
               vrb += nMaquinas[k].NombreMaquina;
               vrb += "( ";
               vrb +="estados_";
               vrb += nMaquinas[k].NombreMaquina;
               vrb += " )";
               VariablesEstado += IntercamioTexto ( line ,  vrb );
               VariablesEstado += ";\n";
           }
           line = VariablesEstado ;
       }
       if ( strstr ( p , "<Autor>" ) )
           line = IntercamioTexto ( line , elementos.Autor );

       if ( strstr ( p , "<Fecha>" ) )
           line = IntercamioTexto ( line ,  elementos.Fecha );

       memset(auxiliar , 0 , 150 );
       //res = line;
       line+="\n";
       res += line;
   }
   file.close();

   return res;

}

QString Fuentes::ArmadoDeFuncionMaquinaDeEstadosPF  ( void )
{
   QString Texto;

   Texto = agregarTextos("39a-Descripcion MdeEPf.c");
   QString Nombre = "://templates/" + Texto ;


   QString est = "\t" , llm = "\t";
   for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
   {
       char n[3];
       n[0] = (k / 10) + '0';
       n[1] = (k % 10) + '0';
       n[2] = '\0';
       est += "static int estados_";
       est += nMaquinas[k].NombreMaquina;
       est += " = RESET ;\n\t";
   }

   QString if0 = "\n",if1 = "\n\t";
   for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
   {
       if1 += "if ( estados_";
       char n[3];
       n[0] = (k / 10) + '0';
       n[1] = (k % 10) + '0';
       n[2] = '\0';
       if1 += nMaquinas[k].NombreMaquina;
       if1 += " > ";
       if1 += m_ultimosEstados[k];
       if1 += " )\n\t\testados_";
       if1 += nMaquinas[k].NombreMaquina;
       if1 += " = RESET;\n\n\t";
  }

   for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
   {
       char n[3];
       n[0] = (k / 10) + '0';
       n[1] = (k % 10) + '0';
       n[2] = '\0';
       llm +="estados_";
       llm += nMaquinas[k].NombreMaquina;
       llm += " = ( *( ";
       llm += nMaquinas[k].NombreMaquina;
       llm += "[ estados_";
       llm += nMaquinas[k].NombreMaquina;
       llm += " ] ) )( estados_";
       llm += nMaquinas[k].NombreMaquina;
       if ( nMaquinas[k+1].NombreMaquina != "" )
            llm += " ) ;\n\t";
       else
           llm += " ) ;\n";
   }

   Texto.replace("<variables>", est) ;
   Texto.replace("<erroeres>", if1 ) ;
   Texto.replace("<funciones>", llm) ;
   Texto.replace("<Autor>", elementos.Autor ) ;
   Texto.replace("<Fecha>", elementos.Fecha ) ;

   return Texto;

}

QString Fuentes::ArmadoDeFuncionesDeEstados   ( void )
{
    QString Texto1="",Texto = "",prototipo;
    bool d = m_permiso.getDepurar();

    for (int k = 0 ; nMaquinas[k].NombreMaquina != ""  ; k++)
    {
        Texto += "\n//!< FUNCIONES CORRESPONDIENTES A LOS ESTADOS DE LA MAQUINA : ";
        Texto += nMaquinas[k].NombreMaquina ;
        Texto += " ------------------------\n\n" ;

        int j ;
        for ( j = 0 ; nMaquinas[k].est[j].NombreEstado != ""  ; j++)
        {
            prototipo = nMaquinas[k].est[j].NombreEstado;
            prototipo = nMaquinas[k].NombreMaquina+ "_" + prototipo.toLower();
            Texto1 = agregarTextos( "54-MaquinaDeEstadosPfuncion.c");

            Texto1.replace("<Autor>",elementos.Autor );
            Texto1.replace("<Fecha>",elementos.Fecha );
            Texto1.replace("<funcion>",prototipo );
            Texto1.replace("<estados>" , "Estado" );
            Texto1.replace("<maquina>" , nMaquinas[k].NombreMaquina );

            //Texto = IntercamioTexto( Texto , nMaquinas[k].est[j].NombreEstado );
            Texto += Texto1;

            struct Transiciones t;
            Texto1 = "";
            for (int i = 0 ; nMaquinas[k].est[j].trn[i].idEvento != ""  ; i++)
            {
                t = nMaquinas[k].est[j].trn[i];

                if (d)
                    Texto1 += ArmarTransicionPf ( t , j  , k , i );
                else
                    Texto1 = ArmarTransicionPf ( t , j );

                Texto1 += "\n";
            }
            Texto.replace("<transiciones>" , Texto1 );
        }
    }
    return Texto;
}


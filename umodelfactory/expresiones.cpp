#include "expresiones.h"
#include <string.h>
#include <QString>
#include <QList>

expresiones::expresiones()
{

}

int expresiones::Analisis_Sensores_y_Variable ( QString expresion )
{
    struct variables_de_proceso v;

    for ( int j = 0 ; j < VariablesDeProceso.count() ; j++)
    {
        if (expresion.contains(VariablesDeProceso.value(j).texto_variable ))
        {
            QStringList aux;
            aux = expresion.split(" ");
            int valor = aux.value(2).toInt();

            if ( expresion.contains("==") )
            {
                if (  VariablesDeProceso.value(j).valor_variable == valor )
                     return 1;
            }

            if ( expresion.contains(">=") )
            {
                if (  VariablesDeProceso.value(j).valor_variable >= valor )
                     return 1;
            }

            if ( expresion.contains("<=") )
            {
                if (  VariablesDeProceso.value(j).valor_variable <= valor )
                     return 1;
            }

            if ( expresion.contains("!=") )
            {
                if (  VariablesDeProceso.value(j).valor_variable != valor )
                     return 1;
            }

            if (expresion.contains("<"))
            {
                if (!(expresion.contains("=")))
                    if (  VariablesDeProceso.value(j).valor_variable < valor )
                        return 1;
            }

            if (expresion.contains(">"))
            {
                if (!(expresion.contains("=")))
                    if (  VariablesDeProceso.value(j).valor_variable > valor )
                        return 1;
            }
            return  0;
        }
    }

    for ( int j = 0 ; j < PulsadoresDeProceso.count() ; j++)
    {
        struct pulsadores_de_proceso aux;
        aux = PulsadoresDeProceso.value(j);
        if (expresion.contains(PulsadoresDeProceso.value(j).texto_pulsadores ))
        {
            if ( PulsadoresDeProceso.value(j).valor_pulsadores)
            {
                aux = PulsadoresDeProceso.value(j);

                return 1;
            }

            return  0;
        }
    }
    return -1;
}
//-+-+-+-+-+-+
bool expresiones::ResultadoExpresiones ( QStringList &expreciones , QStringList &operadores )
{
    QList <int> salidafinal;

    for ( int i = 0 ; i < expreciones.count() ; i ++)
    {
        QStringList expreciones1 , operadores1;
        int analisis;
        AnalisisDeExpresiones ( expreciones[i] , expreciones1 , operadores1 );
        if (expreciones1.count() > 1 )
        {
            QList <int> salida;

            for (int j = 0 ; j < expreciones1.count() ; j++ )
            {
                analisis = Analisis_Sensores_y_Variable ( expreciones1[j] );
                if ( analisis != -1)
                    salida << analisis;
            }
            int resultado = salida[0];
// el ultimo bug
            for ( int j = 0 ; j < operadores1.count() ; j++)
            {
                if ( operadores1 [j] == "&&")
                {
                    if ( j != 0)
                        resultado *= salida[j];
                }
                if ( operadores1 [j] == "||")
                {
                    if ( j != 0)
                        resultado += salida[j];
                }
            }
            salidafinal << resultado;
        }
        else
        {
            analisis = Analisis_Sensores_y_Variable ( expreciones1[0] );
            if ( analisis != -1)
               salidafinal << analisis;
        }
    }

    int resultado = salidafinal[0];
// el otro ultimo bug
    for ( int j = 0 ; j < operadores.count() ; j++)
    {
        if ( operadores [j] == "&&")
        {
            if ( j != 0)
                resultado *= salidafinal[j];
        }

        if ( operadores [j] == "||")
        {
            if ( j != 0)
                resultado += salidafinal[j];
        }
    }

    return ( resultado == 0 ) ? false : true;
}
//-*-*-*-*-*
void expresiones::AnalisisDeExpresiones ( QString proposicion , QStringList &expreciones , QStringList &operadores )
{
    QStringList exp,prueba;
    proposicion.replace("&amp;&amp;" , "&&");

    prueba = proposicion.split("(");
    QStringList prueba1;
    for ( int i = 0 ; i < prueba.count() ; i++)
    {
        if (prueba[i].contains("||") && !prueba[i].contains(")"))
        {
            prueba1 = prueba[i].split(" || ");
            for ( int j = 0 ; j < prueba1.count() ; j++)
                expreciones << prueba1[j];
            operadores << "||";
            continue;
        }

        if (prueba[i].contains("&&") && !prueba[i].contains(")"))
        {
            prueba1 = prueba[i].split(" && ");
            for ( int j = 0 ; j < prueba1.count() ; j++)
                expreciones << prueba1[j];
            operadores << "&&";
            continue;
        }
       prueba[i].replace(")","");
       expreciones << prueba[i];
    }
    return ;
}

void expresiones::setNuevoValorDeVariable ( QString accion )
{
    struct variables_de_proceso v;

    for ( int i = 0 ; i < VariablesDeProceso.count() ; i++)
    {
        QStringList aux;
        aux = accion.split(" ");
        v.texto_variable = aux.value(0);
        if (  v.texto_variable == VariablesDeProceso.value(i).texto_variable)
        {
            v = VariablesDeProceso.value(i);
            //VariablesDeProceso.replace(i,v);

            aux = accion.split(" ");
            v.texto_variable = aux.value(0);
            if (accion.contains("+="))
               v.valor_variable += aux.value(2).toInt();
            else if (accion.contains("-="))
               v.valor_variable -= aux.value(2).toInt();
            else if (accion.contains("*="))
               v.valor_variable *= aux.value(2).toInt();
            else if (accion.contains("/="))
               v.valor_variable /= aux.value(2).toInt();
            else if (accion.contains("%="))
               v.valor_variable %= aux.value(2).toInt();
            else if ( accion.contains("="))
               v.valor_variable = aux.value(2).toInt();

            VariablesDeProceso.replace(i , v);
        }
    }
}

void expresiones::ActivaPulsador(QString auxEvento)
{
    struct pulsadores_de_proceso v;

    for ( int i = 0 ; i < PulsadoresDeProceso.count( ) ; i++)
    {
        v = PulsadoresDeProceso.value(i);
        if ( v.texto_pulsadores == auxEvento )
        {
            v.valor_pulsadores = 1;
            PulsadoresDeProceso.replace(i,v) ;
        }
    }
}

void expresiones::DesActivaPulsador(void)
{
    struct pulsadores_de_proceso v;

    for ( int i = 0 ; i < PulsadoresDeProceso.count( ) ; i++)
    {
        v = PulsadoresDeProceso.value(i);
        v.valor_pulsadores = 0;
        PulsadoresDeProceso.replace(i,v);
    }
}
void expresiones::setVariables ( struct variables_de_proceso v)
{
    VariablesDeProceso << v;
}

void expresiones::getVariables ( struct variables_de_proceso &v)
{
    v = VariablesDeProceso.value(0);
    VariablesDeProceso.pop_front();
}
void expresiones::getVariables ( struct variables_de_proceso &v , int pos)
{
    v = VariablesDeProceso.value(pos);
}
void expresiones::setPulsadores(struct pulsadores_de_proceso p)
{
    PulsadoresDeProceso << p;
}
void expresiones::setPulsadores(struct pulsadores_de_proceso p,int pos)
{
    PulsadoresDeProceso.replace(pos , p);
}

void expresiones::getPulsadores(struct pulsadores_de_proceso &p)
{
    p = PulsadoresDeProceso.value(0);
    PulsadoresDeProceso.pop_front();
}
void expresiones::getPulsadores(struct pulsadores_de_proceso &p , int pos)
{
    p = PulsadoresDeProceso.value(pos);
}

void expresiones::clear_variables ( void )
{
    VariablesDeProceso.clear();
    PulsadoresDeProceso.clear();
}

#include "variablesdeproceso.h"

variablesdeproceso::variablesdeproceso()
{

}

void variablesdeproceso::set_variables ( struct variables_de_proceso & dato )
{
    VariablesDeProceso << dato;
}

void variablesdeproceso::clear_variables ( void )
{
    VariablesDeProceso.clear();
}

void variablesdeproceso::set_valor ( QString accion )
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

void variablesdeproceso::Analisis_VariablesDeProceso (QStringList & EventosPosibles , QStringList &nExp)
{
    struct variables_de_proceso v;

    for ( int j = 0 ; j < VariablesDeProceso.count() ; j++)
    {
        for ( int i = 0 ; i < nExp.count() ; i++)
        {
            if ( nExp.value(i).contains(VariablesDeProceso.value(j).texto_variable ))
            {
                QStringList aux;
                aux = nExp.value(i).split(" ");
                int valor = aux.value(2).toInt();

                if (nExp.value(i).contains("=="))
                {
                    if (  VariablesDeProceso.value(j).valor_variable == valor )
                         EventosPosibles << nExp.value(i);
                }

                if (nExp.value(i).contains(">="))
                {
                    if (  VariablesDeProceso.value(j).valor_variable >= valor )
                         EventosPosibles << nExp.value(i);
                }

                if (nExp.value(i).contains("<="))
                {
                    if (  VariablesDeProceso.value(j).valor_variable <= valor )
                         EventosPosibles << nExp.value(i);
                }

                if (nExp.value(i).contains("!="))
                {
                    if (  VariablesDeProceso.value(j).valor_variable != valor )
                         EventosPosibles << nExp.value(i);
                }

                if (nExp.value(i).contains("<"))
                {
                    if (!(nExp.value(i).contains("=")))
                    if (  VariablesDeProceso.value(j).valor_variable < valor )
                         EventosPosibles << nExp.value(i);
                }

                if (nExp.value(i).contains(">"))
                {
                    if (!(nExp.value(i).contains("=")))
                    if (  VariablesDeProceso.value(j).valor_variable > valor )
                         EventosPosibles << nExp.value(i);
                }
            }
        }
    }
}

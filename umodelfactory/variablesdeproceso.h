#ifndef VARIABLESDEPROCESO_H
#define VARIABLESDEPROCESO_H
#include <QString>
#include <QList>
#include "expresiones.h"

//struct variables_de_proceso
//{
//    QString texto_variable;
//    int valor_variable;
//    variables_de_proceso () { texto_variable = "" ; valor_variable = 0; }
//};

class variablesdeproceso
{
    QList < struct variables_de_proceso >  VariablesDeProceso;

public:
    variablesdeproceso();
    void set_variables ( struct variables_de_proceso & dato );
    void set_valor ( QString accion );
    void Analisis_VariablesDeProceso (QStringList & EventosPosibles , QStringList &nExp);
    void clear_variables ( void );

};

#endif // VARIABLESDEPROCESO_H

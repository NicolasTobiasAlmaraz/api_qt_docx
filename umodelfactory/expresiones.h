#ifndef EXPRESIONES_H
#define EXPRESIONES_H
#include <QString>
#include <QList>

struct variables_de_proceso
{
    QString texto_variable;
    int valor_variable;
    variables_de_proceso () { texto_variable = "" ; valor_variable = 0; }
};

struct pulsadores_de_proceso
{
    QString texto_pulsadores;
    int valor_pulsadores;
    pulsadores_de_proceso () { texto_pulsadores = "" ; valor_pulsadores = 0; }
};

class expresiones
{
    QList <struct variables_de_proceso> VariablesDeProceso;
    QList <struct pulsadores_de_proceso> PulsadoresDeProceso;

public:
    expresiones();
    int Analisis_Sensores_y_Variable ( QString expresion );
    bool ResultadoExpresiones ( QStringList &expreciones , QStringList &operadores );
    void AnalisisDeExpresiones ( QString proposicion , QStringList &expreciones , QStringList &operadores );
    void setNuevoValorDeVariable ( QString accion );
    void ActivaPulsador(QString auxEvento);
    void DesActivaPulsador(void);

    void setVariables ( struct variables_de_proceso v );
    void getVariables ( struct variables_de_proceso &v );
    void getVariables ( struct variables_de_proceso &v , int pos);

    void setPulsadores ( struct pulsadores_de_proceso p );
    void setPulsadores ( struct pulsadores_de_proceso p ,int pos);
    void getPulsadores ( struct pulsadores_de_proceso &p );
    void getPulsadores ( struct pulsadores_de_proceso &p , int pos);

    void clear_variables ( void );
};

#endif // EXPRESIONES_H

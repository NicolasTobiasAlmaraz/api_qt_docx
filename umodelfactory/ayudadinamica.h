#ifndef AYUDADINAMICA_H
#define AYUDADINAMICA_H

#include <QStringList>
#include <QtXml>

class AyudaDinamica
{
public:
    enum NivelAyuda { AYUDA_USR_AVANZADO = 0, AYUDA_USR_INTERMEDIO, AYUDA_USR_BASICO, DESACTIVADA};

    AyudaDinamica();

    void cargarContenidos();

    QString getTextoEstadoInicial();
    QString getTextoEstadoSimple();
    QString getTextoTransicion();

    NivelAyuda nivelAyuda() const;
    void setNivelAyuda(NivelAyuda nuevoNivelAyuda);

private:
    NivelAyuda m_nivelAyuda;

    QStringList contenidoAyudaEstadoSimple;
    QStringList contenidoAyudaTransicion;
    QStringList contenidoAyudaEstadoReset;

    QDomDocument m_docAyudaDinamica;
};

#endif // AYUDADINAMICA_H

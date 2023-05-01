#include "ayudadinamica.h"
#include "xmlhandler.h"

AyudaDinamica::AyudaDinamica()
{
    m_nivelAyuda = DESACTIVADA;
}

void AyudaDinamica::cargarContenidos()
{
    //verificamos el archivo
    QDomElement helpRoot, helpElement;
    QFile file(":/help/help.xml");
    QString typeAttr, h1, h2, h3;
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!m_docAyudaDinamica.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    if (m_docAyudaDinamica.isNull()) return; //aca hay que hacer algo con la info
    helpRoot = m_docAyudaDinamica.documentElement();
    if (helpRoot.hasChildNodes() == false) return;
    if (helpRoot.elementsByTagName("help").isEmpty()) return;
    helpElement = helpRoot.elementsByTagName("help").item(0).toElement();

    helpElement = helpElement.firstChild().toElement();
    typeAttr = helpElement.attribute("type", "none");

    while (!helpElement.isNull()) {
        if (typeAttr != "none") {
            h1 = helpElement.elementsByTagName("h1").at(0).toElement().text();  // Avanzado
            h2 = helpElement.elementsByTagName("h2").at(0).toElement().text();  // Intermedio
            h3 = helpElement.elementsByTagName("h3").at(0).toElement().text();  // Básico
            if (typeAttr == ESTADO) {
                contenidoAyudaEstadoSimple << h1 << h2 << h3;
            }
            if (typeAttr == TRANSICIONES) {
                contenidoAyudaTransicion << h1 << h2 << h3;
            }
            if (typeAttr == ESTADO_INICIAL) {
                contenidoAyudaEstadoReset << h1 << h2 << h3;
            }
        }
        helpElement = helpElement.nextSibling().toElement();
        typeAttr = helpElement.attribute("type", "none");
    }
}

QString AyudaDinamica::getTextoEstadoInicial()
{
    QString contenido;

    if(m_nivelAyuda != DESACTIVADA)
        return contenidoAyudaEstadoReset.at(m_nivelAyuda);

    return contenido;
}

QString AyudaDinamica::getTextoEstadoSimple()
{
    QString contenido;

    if(m_nivelAyuda != DESACTIVADA)
        contenido = contenidoAyudaEstadoSimple.at(m_nivelAyuda);

    return contenido;
}

QString AyudaDinamica::getTextoTransicion()
{
    QString contenido;

    if(m_nivelAyuda != DESACTIVADA)
        contenido = contenidoAyudaTransicion.at(m_nivelAyuda);

    return contenido;
}

AyudaDinamica::NivelAyuda AyudaDinamica::nivelAyuda() const
{
    return m_nivelAyuda;
}

void AyudaDinamica::setNivelAyuda(NivelAyuda nuevoNivelAyuda)
{
    m_nivelAyuda = nuevoNivelAyuda;
}



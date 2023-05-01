#include "xmlhandler.h"

/*!
 * \brief XmlHandler::XmlHandler
 */
XmlHandler::XmlHandler()
{
    // Define el nombre del archivo por default
    m_file.setFileName(QString::null);
}

/*!
 * \brief XmlHandler::XmlHandler
 * \param nombreArchivo
 */
XmlHandler::XmlHandler(const QString &nombreArchivo, XmlType tipo)
{
    switch (tipo) {
        case NUEVO_XML_DATOS:
            m_file.setFileName(nombreArchivo+".umf");
            crearEstructuraProyecto();
            break;
    
        case NUEVO_XML_PERIF:
            m_file.setFileName(nombreArchivo+".umf");
            crearEstructuraPerifericos();
            break;
    
        case ABRIR_XML_DATOS:
            m_file.setFileName(nombreArchivo);
            break;
    
        case ABRIR_XML_PERIF:
            break;
    }
}

/*!
 * \brief XmlHandler::setContenidoGeneral
 * \param tagName
 * \param contenido
 * \return
 */
bool XmlHandler::setContenidoGeneral(const QString &tagName, const QString &contenido)
{
    QDomElement element = m_doc.documentElement().firstChildElement(GENERAL).firstChildElement(tagName);

    if(!element.isNull()) {
        if (element.hasChildNodes() == true)
            element.firstChild().setNodeValue(contenido);
        else
            element.appendChild(m_doc.createTextNode(contenido));

        return true;
    } else
        return false;
}

/*!
 * \brief XmlHandler::getContenidoGeneral
 * \param tagName
 * \return
 */
QString XmlHandler::getContenidoGeneral(const QString &tagName)
{
    QDomElement element = m_doc.documentElement().firstChildElement(GENERAL).firstChildElement(tagName);
    if(!element.isNull())
        return (element.text());
    else
        return (QString::number(-1));
}

/*!
 * \brief XmlHandler::addNuevaMaquina
 * \param maquinaId
 */
void XmlHandler::addNuevaMaquina(const QString &maquinaId)
{
   if( m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).isNull() )
   {
       QDomElement newElement = m_doc.createElement(maquinaId);
       m_doc.documentElement().firstChildElement(MAQUINAS).appendChild(newElement);

       //Agrego el tag para el nombre y para el estado inicial
       newElement = m_doc.createElement(NOMBRE_MAQUINA);
       m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).appendChild(newElement);

       newElement = m_doc.createElement(ESTADO_INICIAL);
       m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).appendChild(newElement);
   }
}

/*!
 * \brief XmlHandler::borrarMaquina
 * \param indexMaquina
 */
void XmlHandler::borrarMaquina(const int &indexMaquina)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS);

    if( !element.isNull() )
    {
        if (getComentario(MAQUINA+QString::number(indexMaquina)) != QString("")) {
            clearComentario(MAQUINA+QString::number(indexMaquina),true);
        }
        if(element.childNodes().count() == 1)   // si quiero borrar la única máquina
        {
            element.removeChild(element.firstChildElement(MAQUINA+QString::number(indexMaquina)));
            //Creo una maquina por default para que no quede vacia?
        }
        else if( (element.childNodes().count() - 1) == indexMaquina ) // si quiero borrar la última máquina
        {
            element.removeChild(element.firstChildElement(MAQUINA+QString::number(indexMaquina)));
        }
        else    // si quiero borrar otro maquina cualquiera
        {
           QDomNode newChild = element.firstChildElement(MAQUINA+QString::number(indexMaquina+1));   //borra el estado y lo reemplaza con el siguiente

           if(!newChild.isNull())
            {
                element.replaceChild(newChild,element.firstChildElement(MAQUINA+QString::number(indexMaquina)));

                for(int j=indexMaquina+1; j<=element.childNodes().count(); j++) {
                    if (getComentario(MAQUINA+QString::number(j)) != QString("")){
                        cambiarElementoComentario(MAQUINA+QString::number(j), MAQUINA+QString::number(j-1));
                    }
                    element.firstChildElement(MAQUINA+QString::number(j)).setTagName(MAQUINA+QString::number(j-1));

                }
            }

        }

    }
}

/*!
 * \brief XmlHandler::addNuevoInfoMaquina
 * \param maquinaId
 * \param nombre
 */
void XmlHandler::addInfoMaquina(const QString &maquinaId, const QString &nombre)
{
    setContenidoMaquina(maquinaId, NOMBRE_MAQUINA, nombre);
}

/*!
 * \brief XmlHandler::setContenidoMaquina
 * \param maquinaId
 * \param tagName
 * \param contenido
 * \return
 */
bool XmlHandler::setContenidoMaquina(const QString &maquinaId, const QString &tagName, const QString &contenido )
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(tagName);

    if(!element.isNull()) {
        if (element.hasChildNodes() == true)
            element.firstChild().setNodeValue(contenido);
        else
            element.appendChild(m_doc.createTextNode(contenido));

        return true;
    } else
        return false;
}

/*!
 * \brief XmlHandler::getContenidoMaquina
 * \param maquinaId
 * \param tagName
 * \return
 */
QString XmlHandler::getContenidoMaquina(const QString &maquinaId, const QString &tagName)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(tagName);
    if( !element.isNull() )
        return ( element.text() );
    else
        return (QString::number(-1));

}
/*!
 * \brief XmlHandler::cantidadMaquinas
 * \return
 */
int XmlHandler::cantidadMaquinas()
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS);

    if( !element.isNull() )
        return ( element.childNodes().count());
    else
        return ( -1 );
}

/*!
 * \brief XmlHandler::getNombresMaquinasDefinidos
 * \return
 */
QStringList XmlHandler::getNombresMaquinasDefinidas()
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS);
    if( !element.isNull() )
    {
        QStringList retList;
        for(int k=0; k < element.childNodes().count(); k++)
        {
            retList.append(element.firstChildElement(MAQUINA+QString::number(k)).firstChildElement(NOMBRE).text());
        }
        return ( retList );
    }
    else
        return (QStringList());
}

/*!
 * \brief XmlHandler::addNuevoEstado
 * \param estadoId
 * \param maquinaId
 */
void XmlHandler::addNuevoEstado(const QString &estadoId, const QString &maquinaId)
{

    if( m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId).isNull() )
    {
        QDomElement newElement = m_doc.createElement(estadoId);
        m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).appendChild(newElement);
    }
}

/*!
 * \brief XmlHandler::borrarEstado
 * \param indexEstado
 * \param maquinaId
 */
void XmlHandler::borrarEstado(const int &indexEstado, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId);

    if( !element.isNull() )
    {
        if (getComentario(ESTADO+QString::number(indexEstado)) != QString("")){
            clearComentario(ESTADO+QString::number(indexEstado, true));
        }
        if(element.childNodes().count() == 1)   // si quiero borrar el un único estado
        {
            element.removeChild(element.firstChildElement(ESTADO+QString::number(indexEstado)));

            if( getEstadoInicial() == ESTADO+QString::number(indexEstado))  // chequa por el estado inicial
                m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL).firstChild().clear();
        }
        else if( (element.childNodes().count() - 1) == indexEstado ) // si quiero borrar el último estado
        {
            element.removeChild(element.firstChildElement(ESTADO+QString::number(indexEstado)));

            if( getEstadoInicial() == ESTADO+QString::number(indexEstado))  // chequa por el estado inicial
                m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL).firstChild().clear();
        }
        else    // si quiero borrar otro estado cualquiera
        {
           QDomNode newChild = element.firstChildElement(ESTADO+QString::number(indexEstado+1));   //borra el estado y lo reemplaza con el siguiente
            if(!newChild.isNull())
            {
                element.replaceChild(newChild,element.firstChildElement(ESTADO+QString::number(indexEstado)));

                for(int j=indexEstado+1; j<=element.childNodes().count(); j++) {
                    if (getComentario(ESTADO+QString::number(j)) != QString("")){
                        cambiarElementoComentario(ESTADO+QString::number(j), ESTADO+QString::number(j-1));
                    }
                    element.firstChildElement(ESTADO+QString::number(j)).setTagName(ESTADO+QString::number(j-1));

                }

                for(int i=0; i < element.childNodes().count(); i++)
                {
                    QDomNode transNode = element.firstChildElement(ESTADO+QString::number(i)).firstChildElement(TRANSICIONES);
                    for(int k=0; k < transNode.childNodes().count(); k++)
                    {
                        int indexProx = transNode.childNodes().at(k).firstChildElement(ID_PROX_ESTADO).text().right(1).toInt();
                        if( indexProx > indexEstado )
                        {
                            transNode.childNodes().at(k).firstChildElement(ID_PROX_ESTADO).firstChild().setNodeValue(ESTADO+QString::number(indexProx-1));
                        }
                    }
                }
            }
        }
    }
}

/*!
 * \brief XmlHandler::addInfoEstado
 * \param estadoId
 * \param nombre
 * \param color
 * \param pos_x
 * \param pos_y
 * \param ancho
 * \param alto
 * \param maquinaId
 */
void XmlHandler::addInfoEstado(const QString &estadoId,const QString &nombre,const QString &color,
                                    const QString &pos_x,const QString &pos_y, const QString &ancho,
                                    const QString &alto, const QString &maquinaId)
{
    setContenidoEstado(estadoId, DIAGRAMA, NOMBRE_ESTADO, nombre, maquinaId);
    setContenidoEstado(estadoId, DIAGRAMA, COLOR_ESTADO, color, maquinaId);
    setContenidoEstado(estadoId, DIAGRAMA, POS_X_ESTADO, pos_x, maquinaId);
    setContenidoEstado(estadoId, DIAGRAMA, POS_Y_ESTADO, pos_y, maquinaId);
    setContenidoEstado(estadoId, DIAGRAMA, ANCHO_ESTADO, ancho, maquinaId);
    setContenidoEstado(estadoId, DIAGRAMA, ALTO_ESTADO, alto, maquinaId);
}

/*!
 * \brief XmlHandler::setContenidoEstado
 * \param estadoId
 * \param tagName
 * \param subTagName
 * \param contenido
 * \return
 */
bool XmlHandler::setContenidoEstado(const QString &estadoId, const QString &tagName, const QString &subTagName, const QString &contenido, const QString &maquinaId)
{
    QDomElement idElement = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId);
    if (!idElement.isNull()) {
        QDomElement tagElement = idElement.firstChildElement(tagName);
        if(!tagElement.isNull()) {
            QDomElement subTagElement = tagElement.firstChildElement(subTagName);
            if(!subTagElement.isNull()) {
                if (subTagElement.hasChildNodes() == true) {
                    QDomElement newNodeTag = m_doc.createElement(subTagName);
                    QDomText newNodeText = m_doc.createTextNode(contenido);
                    newNodeTag.appendChild(newNodeText);
                    // reemplaza el nodo anterior
                    tagElement.replaceChild(newNodeTag, subTagElement);
                } else
                    subTagElement.appendChild(m_doc.createTextNode(contenido));
            } else {
                subTagElement=m_doc.createElement(subTagName);
                tagElement.appendChild(subTagElement);
                subTagElement.appendChild(m_doc.createTextNode(contenido));
            }
        } else {
            tagElement=m_doc.createElement(tagName);
            idElement.appendChild(tagElement);
            tagElement.appendChild(m_doc.createElement(subTagName));
            tagElement.firstChildElement(subTagName).appendChild(m_doc.createTextNode(contenido));
        }
        return true;
    } else
        return false;
}

/*!
 * \brief XmlHandler::getContenidoEstado
 * \param estadoId
 * \param tagName
 * \param subTagName
 * \return
 */
QString XmlHandler::getContenidoEstado(const QString &estadoId, const QString &tagName, const QString &subTagName, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId).firstChildElement(tagName).firstChildElement(subTagName);
    if( !element.isNull() )
        return ( element.text() );
    else
        return (QString::number(-1));
}

/*!
 * \brief XmlHandler::setContenidoDiagrama
 * \param estadoId
 * \param subTagName
 * \param contenido
 * \return
 */
bool XmlHandler::setContenidoDiagrama(const QString &estadoId, const QString &subTagName, const QString &contenido, const QString &maquinaId)
{
    QDomElement idElement = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId);
    if (!idElement.isNull()) {
        QDomElement tagElement = idElement.firstChildElement(DIAGRAMA);
        if(!tagElement.isNull()) {
            QDomElement subTagElement = tagElement.firstChildElement(subTagName);
            if(!subTagElement.isNull()) {
                if (subTagElement.hasChildNodes() == true){
                    QDomElement newNodeTag = m_doc.createElement(subTagName);
                    QDomText newNodeText = m_doc.createTextNode(contenido);
                    newNodeTag.appendChild(newNodeText);
                    // reemplaza el nodo anterior
                    tagElement.replaceChild(newNodeTag, subTagElement);
                } else
                    subTagElement.appendChild(m_doc.createTextNode(contenido));
            } else {
                subTagElement=m_doc.createElement(subTagName);
                tagElement.appendChild(subTagElement);
                subTagElement.appendChild(m_doc.createTextNode(contenido));
            }
        } else {
            tagElement=m_doc.createElement(DIAGRAMA);
            idElement.appendChild(tagElement);
            tagElement.appendChild(m_doc.createElement(subTagName));
            tagElement.firstChildElement(subTagName).appendChild(m_doc.createTextNode(contenido));
        }
        return true;
    }
    else
        return false;
}

/*!
 * \brief XmlHandler::getContenidoDiagrama
 * \param estadoId
 * \param subTagName
 * \param maquinaId
 * \return
 */
QString XmlHandler::getContenidoDiagrama(const QString &estadoId, const QString &subTagName, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId).firstChildElement(DIAGRAMA).firstChildElement(subTagName);
    if( !element.isNull() )
        return ( element.text() );
    else
        return (QString::number(-1));
}

/*!
 * \brief XmlHandler::cantidadEstados
 * \param maquinaId
 * \return
 */
int XmlHandler::cantidadEstados(const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId);

    if( !element.isNull() )
        return ( element.childNodes().count() -  2); //-2 Por el estado inicial + nombre
    else
        return ( -1 );
}

/*!
 * \brief XmlHandler::setInfoEstadoInicial
 * \param pos_x
 * \param pos_y
 * \param accion
 * \param proxEstadoId
 * \param midPos
 * \param textPos
 * \param maquinaId
 * \return
 */
bool XmlHandler::setInfoEstadoInicial(const QString &pos_x, const QString &pos_y, const QString &accion,
                                      const QString &proxEstadoId, const QString &midPos, const QString &textPos,
                                      const QString &maquinaId)
{
    setContenidoInicial(POS_X_ESTADO, pos_x, maquinaId);
    setContenidoInicial(POS_Y_ESTADO, pos_y, maquinaId);
    setContenidoInicial(ID_ACCIONES, accion, maquinaId);
    setContenidoInicial(ID_PROX_ESTADO, proxEstadoId, maquinaId);
    setContenidoInicial(MID_POS, midPos, maquinaId);
    setContenidoInicial(TEXT_POS, textPos, maquinaId);

    return true;
}

/*!
 * \brief XmlHandler::setContenidoInicial
 * \param tagName
 * \param contenido
 * \param maquinaId
 * \return
 */
bool XmlHandler::setContenidoInicial(const QString &tagName, const QString &contenido, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL);
    if (!element.isNull()) {
        QDomElement subTagElement = element.firstChildElement(tagName);
        if (!subTagElement.isNull()) {
            if ( subTagElement.hasChildNodes() == true )
                subTagElement.firstChild().setNodeValue(contenido);
            else
                subTagElement.appendChild(m_doc.createTextNode(contenido));
        } else {
            subTagElement=m_doc.createElement(tagName);
            element.appendChild(subTagElement);
            subTagElement.appendChild(m_doc.createTextNode(contenido));
        }
        return true;
    } else
        return false;
}

/*!
 * \brief XmlHandler::getContenidoInicial
 * \param tagName
 * \param maquinaId
 * \return
 */
QString XmlHandler::getContenidoInicial(const QString &tagName, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL).firstChildElement(tagName);
    if(!element.isNull())
        return (element.text());
    else
        return (QString::number(-1));
}

/*!
 * \brief XmlHandler::getEstadoInicial
 * \param maquinaId
 * \return
 */
QString XmlHandler::getEstadoInicial(const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL).firstChildElement(ID_PROX_ESTADO);
    if( !element.isNull() ) {
        QDomElement newElement = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).
                firstChildElement(element.text()).firstChildElement(DIAGRAMA).firstChildElement(NOMBRE_ESTADO);
        return (newElement.text());
    } else
        return (QString::number(-1));
}
/*!
 * \brief XmlHandler::borrarEstadoInicial
 */
void XmlHandler::borrarEstadoInicial(const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL);

    if( !element.isNull() )
    {
        m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO_INICIAL).firstChild().clear();
    }
}

/*!
 * \brief XmlHandler::setContenidoTransicion
 * \param estadoId
 * \param transId
 * \param subTagName
 * \param contenido
 * \return
 */
bool XmlHandler::setContenidoTransicion(const QString &estadoId, const QString &transId,
                                        const QString &subTagName, const QString &contenido, const QString &maquinaId)
{
    QDomElement idElementEstado = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId);
    if ( !idElementEstado.isNull() )
    {
        QDomElement tagTransiciones = idElementEstado.firstChildElement(TRANSICIONES);
        if( !tagTransiciones.isNull() )
        {
            QDomElement idElementTrans = tagTransiciones.firstChildElement(transId);
            if( !idElementTrans.isNull() )
            {
                QDomElement subTagElement = idElementTrans.firstChildElement(subTagName);
                if( !subTagElement.isNull() )
                {
                    if ( subTagElement.hasChildNodes() == true )
                        subTagElement.firstChild().setNodeValue(contenido);
                    else
                        subTagElement.appendChild(m_doc.createTextNode(contenido));
                }
                else
                {
                subTagElement=m_doc.createElement(subTagName);
                idElementTrans.appendChild(subTagElement);
                subTagElement.appendChild(m_doc.createTextNode(contenido));
                }
            }
            else
            {
                idElementTrans=m_doc.createElement(transId);
                tagTransiciones.appendChild(idElementTrans);
                idElementTrans.appendChild(m_doc.createElement(subTagName));
                idElementTrans.firstChildElement(subTagName).appendChild(m_doc.createTextNode(contenido));
            }

        }
        else
        {
            tagTransiciones=m_doc.createElement(TRANSICIONES);
            idElementEstado.appendChild(tagTransiciones);
            tagTransiciones.appendChild(m_doc.createElement(transId));
            tagTransiciones.firstChildElement(transId).appendChild(m_doc.createElement(subTagName));
            tagTransiciones.firstChildElement(transId).firstChildElement(subTagName).appendChild(m_doc.createTextNode(contenido));

        }

        return true;
    }
    else
        return false;

}

/*!
 * \brief XmlHandler::addNuevaTransicion
 * \param estadoId
 * \param transId
 * \param evento
 * \param accion
 * \param proxEstadoId
 * \param midPos
 * \param textPos
 * \param maquinaId
 */
void XmlHandler::addNuevaTransicion(const QString &estadoId, const QString &transId,
                                    const QString &evento, const QString &accion,
                                    const QString &proxEstadoId, const QString &midPos,
                                    const QString &textPos, const QString &maquinaId)
{
    setContenidoTransicion(estadoId, transId, ID_EVENTO, evento, maquinaId);
    setContenidoTransicion(estadoId, transId, ID_ACCIONES, accion, maquinaId);
    setContenidoTransicion(estadoId, transId, ID_PROX_ESTADO, proxEstadoId, maquinaId);
    setContenidoTransicion(estadoId, transId, ID_PROX_ESTADO, proxEstadoId, maquinaId);
    setContenidoTransicion(estadoId, transId, MID_POS, midPos, maquinaId);
    setContenidoTransicion(estadoId, transId, TEXT_POS, textPos, maquinaId);

}

/*!
 * \brief XmlHandler::borrarTransicion
 * \param indexEstado
 * \param indexTrans
 * \param maquinaId
 */
void XmlHandler::borrarTransicion(const int &indexEstado, const int &indexTrans, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(ESTADO+QString::number(indexEstado)).firstChildElement(TRANSICIONES);

    if( !element.isNull() ){
        if(element.childNodes().count() == 1)
            element.removeChild(element.firstChildElement(ID_TRANS+QString::number(indexTrans)));
        else if( (element.childNodes().count() - 1) == indexTrans )
            element.removeChild(element.firstChildElement(ID_TRANS+QString::number(indexTrans)));
        else{
           QDomNode newChild = element.firstChildElement(ID_TRANS+QString::number(indexTrans+1));
            if(!newChild.isNull()){
                element.replaceChild(newChild,element.firstChildElement(ID_TRANS+QString::number(indexTrans)));
                for(int j=indexTrans+1; j<=element.childNodes().count(); j++)
                        element.firstChildElement(ID_TRANS+QString::number(j)).setTagName(ID_TRANS+QString::number(j-1));
            }
        }
    }
}

/*!
 * \brief XmlHandler::getContenidoTransicion
 * \param estadoId
 * \param transId
 * \param tagName
 * \param maquinaId
 * \return
 */
QString XmlHandler::getContenidoTransicion(const QString &estadoId, const QString &transId, const QString &tagName, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId).firstChildElement(TRANSICIONES).firstChildElement(transId).firstChildElement(tagName);
    if( !element.isNull() )
        return ( element.text() );
    else
        return (QString::number(-1));
}

/*!
 * \brief XmlHandler::cantidadTransiciones
 * \param estadoId
 * \param maquinaId
 * \return
 */
int XmlHandler::cantidadTransiciones(const QString &estadoId, const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).firstChildElement(estadoId).firstChildElement(TRANSICIONES);

    if (!element.isNull())
        return (element.childNodes().count());
    else
        return ( -1 );
}

/*! Agrega un nuevo evento al XML
 *  \brief XmlHandler::addNuevoEvento
 */
void XmlHandler::addNuevoEvento(const QString &nombre)
{
    int i = cantidadEventos();
    if (i == -1)
        i=0;
    addElemento(EVENTOS,EVENTO+QString::number(i),nombre);
}

/*!
 * \brief XmlHandler::borrarEvento
 * \param eventoId
 */
void XmlHandler::borrarEvento(const int &eventoId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(EVENTOS);

    if (!element.isNull()) {
        if (getComentario(EVENTO+QString::number(eventoId)) != QString("")){
            clearComentario(EVENTO+QString::number(eventoId), true);
        }
        if (element.childNodes().count() == 1)
            element.removeChild(element.firstChildElement(EVENTO+QString::number(eventoId)));
        else if ((element.childNodes().count() - 1) == eventoId)
            element.removeChild(element.firstChildElement(EVENTO+QString::number(eventoId)));
        else {
           QDomNode newChild = element.firstChildElement(EVENTO+QString::number(eventoId+1));
            if (!newChild.isNull()){
                element.replaceChild(newChild,element.firstChildElement(EVENTO+QString::number(eventoId)));
                for (int j=eventoId+1; j<=element.childNodes().count(); j++) {
                    if (getComentario(EVENTO+QString::number(j)) != QString("")){
                        cambiarElementoComentario(EVENTO+QString::number(j), EVENTO+QString::number(j-1));
                    }
                    element.firstChildElement(EVENTO+QString::number(j)).setTagName(EVENTO+QString::number(j-1));
                }
            }
        }
    }
}

/*! Devuelve el nombre del Evento
 *  \brief XmlHandler::getContenidoEvento
 *  \param eventoId
 *  \return nombre del Evento
 */
QString XmlHandler::getContenidoEvento(const QString &eventoId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(EVENTOS).firstChildElement(eventoId);
    if (!element.isNull())
        return (element.text());
    else
        return (QString::number(-1));
}

/*! Devuelve la cantidad de Eventos definidos
 *  \brief XmlHandler::cantidadEventos
 *  \return cantidad de Eventos
 */
int XmlHandler::cantidadEventos()
{
    QDomElement element = m_doc.documentElement().firstChildElement(EVENTOS);

    if (!element.isNull())
        return ( element.childNodes().count() );
    else
        return ( -1 );
}

/*!
 * \brief XmlHandler::addNuevoVariable
 * \param nombre
 */
void XmlHandler::addNuevaVariable(const QString &nombre)
{
    int i = cantidadVariables();

    if ( i == -1 )
        i = 0;

    addElemento(VARIABLES, VARIABLE+QString::number(i), nombre);
}

/*!
 * \brief XmlHandler::borrarVariable
 * \param variableId
 */
void XmlHandler::borrarVariable(const int &variableId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(VARIABLES);

    if (!element.isNull()) {
        if (getComentario(VARIABLE+QString::number(variableId)) != QString("")){
            clearComentario(VARIABLE+QString::number(variableId), true);
        }
        if(element.childNodes().count() == 1)
            element.removeChild(element.firstChildElement(VARIABLE+QString::number(variableId)));
        else if((element.childNodes().count() - 1) == variableId)
            element.removeChild(element.firstChildElement(VARIABLE+QString::number(variableId)));
        else {
            QDomNode newChild = element.firstChildElement(VARIABLE+QString::number(variableId+1));
            if(!newChild.isNull()) {
                element.replaceChild(newChild, element.firstChildElement(VARIABLE+QString::number(variableId)));
                for(int j=variableId+1; j<=element.childNodes().count(); j++) {
                    if (getComentario(VARIABLE+QString::number(j)) != QString("")){
                        cambiarElementoComentario(VARIABLE+QString::number(j), VARIABLE+QString::number(j-1));
                    }
                    element.firstChildElement(VARIABLE+QString::number(j)).setTagName(VARIABLE+QString::number(j-1));
                }
            }
        }

    }
}

/*!
 * \brief XmlHandler::getContenidoVariable
 * \param variableId
 * \return
 */
QString XmlHandler::getContenidoVariable(const QString &variableId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(VARIABLES).firstChildElement(variableId);
    if ( !element.isNull() )
        return ( element.text() );
    else
        return (QString::number(-1));
}

/*!
 * \brief XmlHandler::cantidadVariables
 * \return
 */
int XmlHandler::cantidadVariables()
{
    QDomElement element = m_doc.documentElement().firstChildElement(VARIABLES);

    if( !element.isNull() )
        return ( element.childNodes().count() );
    else
        return ( -1 );
}

/*!
 * \brief XmlHandler::addNuevaAccion
 * \param nombre
 */
void XmlHandler::addNuevaAccion(const QString &nombre)
{
    int i = cantidadAcciones();
    if(i == -1) {
        i = 0;
    }

    addElemento(ACCIONES, ACCION+QString::number(i), nombre);
}

/*!
 * \brief XmlHandler::borrarAccion
 * \param accionId
 */
void XmlHandler::borrarAccion(const int &accionId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(ACCIONES);

    if( !element.isNull() ){
        if (getComentario(ACCION+QString::number(accionId)) != QString("")){
            clearComentario(ACCION+QString::number(accionId), true);
        }
        if(element.childNodes().count() == 1)
            element.removeChild(element.firstChildElement(ACCION+QString::number(accionId)));
        else if( (element.childNodes().count() - 1) == accionId )
            element.removeChild(element.firstChildElement(ACCION+QString::number(accionId)));
        else{
           QDomNode newChild = element.firstChildElement(ACCION+QString::number(accionId+1));
            if(!newChild.isNull()){
                element.replaceChild(newChild,element.firstChildElement(ACCION+QString::number(accionId)));
                for(int j=accionId+1; j<=element.childNodes().count(); j++) {
                    if (getComentario(ACCION+QString::number(j)) != QString("")){
                        cambiarElementoComentario(ACCION+QString::number(j), ACCION+QString::number(j-1));
                    }
                    element.firstChildElement(ACCION+QString::number(j)).setTagName(ACCION+QString::number(j-1));

                }
            }
        }

    }
}

/*!
 *  \brief XmlHandler::getContenidoAccion
 *  \param accionId
 *  \return nombre de la Acción
 */
QString XmlHandler::getContenidoAccion(const QString &accionId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(ACCIONES).firstChildElement(accionId);
    if( !element.isNull() )
        return ( element.text() );
    else
        return (QString::number(-1));
}

/*!
 *  \brief XmlHandler::cantidadAcciones
 *  \return cantidad de Acciones
 */
int XmlHandler::cantidadAcciones()
{
    QDomElement element = m_doc.documentElement().firstChildElement(ACCIONES);

    if( !element.isNull() )
        return ( element.childNodes().count() );
    else
        return ( -1 );
}

/*!
 * \brief XmlHandler::addNuevaConstante
 * \param nombre
 */
void XmlHandler::addNuevaConstante(const QString &nombre)
{
    int i = cantidadConstantes();
    if(i == -1) {
        i = 0;
    }

    addElemento(CONSTANTES, CONSTANTE+QString::number(i), nombre);
}

/*!
 * \brief XmlHandler::borrarConstante
 * \param constanteId
 */
void XmlHandler::borrarConstante(const int &constanteId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(CONSTANTES);

    if (!element.isNull()) {
        if (getComentario(CONSTANTE+QString::number(constanteId)) != QString("")) {
            clearComentario(CONSTANTE+QString::number(constanteId), true);
        }
        if(element.childNodes().count() == 1)
            element.removeChild(element.firstChildElement(CONSTANTE+QString::number(constanteId)));
        else if((element.childNodes().count() - 1) == constanteId)
            element.removeChild(element.firstChildElement(CONSTANTE+QString::number(constanteId)));
        else {
            QDomNode newChild = element.firstChildElement(CONSTANTE+QString::number(constanteId+1));
            if(!newChild.isNull()) {
                element.replaceChild(newChild, element.firstChildElement(CONSTANTE+QString::number(constanteId)));
                for(int j=constanteId+1; j<=element.childNodes().count(); j++) {
                    if (getComentario(newChild.toElement().tagName()) != QString("")) {
                        cambiarElementoComentario(CONSTANTE+QString::number(j), CONSTANTE+QString::number(j-1));
                    }
                    element.firstChildElement(CONSTANTE+QString::number(j)).setTagName(CONSTANTE+QString::number(j-1));

                }
            }
        }
    }
}

void XmlHandler::editarConstante(const int &constanteId, const QString &nombre)
{
    QDomElement element = m_doc.documentElement().firstChildElement(CONSTANTES).firstChildElement(CONSTANTE+QString::number(constanteId));


    if (!element.isNull()) {
        element.firstChild().setNodeValue(nombre);
    }
}

/*!
 *  \brief XmlHandler::getContenidoConstante
 *  \param constanteId
 *  \return constante y su valor
 */
QString XmlHandler::getContenidoConstante(const QString &constanteId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(CONSTANTES).firstChildElement(constanteId);
    if( !element.isNull() )
    {
        return ( element.text() );
    }
    else
        return (QString::number(-1));
}

/*! Devuelve la cantidad de constantes definidas
 *  \brief XmlHandler::cantidadConstantes
 *  \return cantidad de Constantes
 */
int XmlHandler::cantidadConstantes()
{
    QDomElement element = m_doc.documentElement().firstChildElement(CONSTANTES);

    if( !element.isNull() )
        return ( element.childNodes().count() );
    else
        return ( -1 );
}

/*!
 * \brief XmlHandler::getNombresAccionesDefinidas
 * \return
 */
QStringList XmlHandler::getNombresAccionesDefinidas()
{
    QDomElement element = m_doc.documentElement().firstChildElement(ACCIONES);
    if( !element.isNull() )
    {
        QStringList retList;
        for(int k=0; k < element.childNodes().count(); k++)
        {
            retList.append(element.firstChildElement(ACCION+QString::number(k)).text());
        }
        return ( retList );
    }

    else
        return (QStringList());
}

/*!
 * \brief XmlHandler::getNombresEventosDefinidos
 * \return
 */
QStringList XmlHandler::getNombresEventosDefinidos()
{
    QDomElement element = m_doc.documentElement().firstChildElement(EVENTOS);
    if( !element.isNull() )
    {
        QStringList retList;
        for(int k=0; k < element.childNodes().count(); k++)
        {
            retList.append(element.firstChildElement(EVENTO+QString::number(k)).text());
        }
        return ( retList );
    }

    else
        return (QStringList());
}

/*!
 * \brief XmlHandler::getNombresVariablesDefinidas
 * \return
 */
QStringList XmlHandler::getNombresVariablesDefinidas()
{
    QDomElement element = m_doc.documentElement().firstChildElement(VARIABLES);
    if( !element.isNull() )
    {
        QStringList retList;
        for(int k=0; k < element.childNodes().count(); k++)
        {
            retList.append(element.firstChildElement(VARIABLE+QString::number(k)).text());
        }
        return ( retList );
    }

    else
        return (QStringList());
}

/*!
 * \brief XmlHandler::getNombresEstadosDefinidos
 * \return
 */
QStringList XmlHandler::getNombresEstadosDefinidos(const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId);
    if( !element.isNull() )
    {
        QStringList retList;
        for(int k=0; k < element.childNodes().count(); k++)
        {
            retList.append(element.firstChildElement(ESTADO+QString::number(k)).firstChildElement(DIAGRAMA).firstChildElement(NOMBRE_ESTADO).text());
        }
        return ( retList );
    }
    else
        return (QStringList());
}

/*!
 * \brief XmlHandler::getNombresConstantesDefinidas
 * \return
 */
QStringList XmlHandler::getNombresConstantesDefinidas()
{
    // NUEVO: Para devolver solo los nombres de las constantes, me quedo con el contenido previo a la ","
    QDomElement element = m_doc.documentElement().firstChildElement(CONSTANTES);
    if( !element.isNull() )
    {
        QStringList retList;
        for(int k=0; k < element.childNodes().count(); k++)
        {
            retList.append(element.firstChildElement(CONSTANTE+QString::number(k)).text().split(",").first());
        }
        return ( retList );
    }

    else
        return (QStringList());
}

/*!
 * \brief XmlHandler::borrarMaquinas
 */
void XmlHandler::borrarMaquinas()
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS);
    QDomElement newElement = m_doc.createElement(MAQUINAS);
    m_doc.documentElement().replaceChild(newElement, element);
}

/*!
 * \brief XmlHandler::borrarEstados
 */
void XmlHandler::borrarEstados(const QString &maquinaId)
{
    QDomElement element = m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId);
    QDomElement newElement = m_doc.createElement(maquinaId);
    m_doc.documentElement().firstChildElement(MAQUINAS).replaceChild(newElement, element);

    newElement = m_doc.createElement(NOMBRE);
    m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).appendChild(newElement);

    newElement = m_doc.createElement(ESTADO_INICIAL);
    m_doc.documentElement().firstChildElement(MAQUINAS).firstChildElement(maquinaId).appendChild(newElement);
}

/*!
 * \brief XmlHandler::save
 * \return
 */
bool XmlHandler::save()
{
    QFile file(getContenidoGeneral(CARPETA)+"/"+getContenidoGeneral(NOMBRE)+".umf");

    if ( file.open(QIODevice::WriteOnly | QIODevice::Text) == true ) {
        // Escribe los datos en el archivo XML
        QTextStream stream(&file);
        m_doc.save(stream, DEFAULT_INDENT, QDomNode::EncodingFromDocument);
        file.close();
        return true;
    }
    else
        return false;
}

/*!
 * \brief XmlHandler::load
 * \param file
 * \return
 */
bool XmlHandler::load(QFileDevice &file)
{
    return (m_doc.setContent(&file));
}


//TEST

int XmlHandler::cantidadComentarios() {
    if (m_doc.documentElement().firstChildElement(COMENTARIOS).isNull()) return -1;
    return m_doc.documentElement().firstChildElement(COMENTARIOS).childNodes().size();
}

int XmlHandler::addNuevoComentario(const QString &comentario){
    int cant = cantidadComentarios(), i = 0;
    QDomElement newElement, tempElement = m_doc.documentElement().firstChildElement(COMENTARIOS);
    if (cant == -1) {
        m_doc.documentElement().appendChild(m_doc.createElement(COMENTARIOS));
        addElemento(COMENTARIOS, COMENTARIO+QString::number(0), comentario);
        return 0;
    } else {
        while (!tempElement.firstChildElement(COMENTARIO + QString::number(i)).isNull()) {
            i++;
        }
        newElement = m_doc.createElement(COMENTARIO + QString::number(i));
        tempElement.insertAfter(newElement, tempElement.firstChildElement(COMENTARIO + QString::number(i-1)));
        newElement.appendChild(m_doc.createTextNode(comentario));
        return i;
    }
}

void XmlHandler::borrarComentario(const QString &commentId){
    QDomElement element = m_doc.documentElement().firstChildElement(COMENTARIOS);
    if (!element.isNull() && !element.firstChildElement(commentId).isNull()) {
        element.removeChild(element.firstChildElement(commentId));
    }
}

int XmlHandler::setComentario(const QString &elementId, const QString & comentario) {
    QDomNode node = m_doc.documentElement().elementsByTagName(elementId).at(0);
    int commentId = addNuevoComentario(comentario);
    QDomElement commentElement = m_doc.documentElement().firstChildElement(COMENTARIOS).firstChildElement(COMENTARIO + QString::number(commentId));
    if (!node.isNull() && node.isElement()) {
        commentElement.setAttribute(ATTR_PARA, elementId);
    }
    return commentId;
}

void XmlHandler::clearComentario(const QString &elementId, bool tambienComentario) {
    QDomNode node = m_doc.documentElement().elementsByTagName(elementId).at(0);
    QDomNodeList commentList = m_doc.documentElement().firstChildElement(COMENTARIOS).childNodes();
    if (!node.isNull() && node.isElement()) {
        for (int i = 0; i < commentList.length(); i++) {
            if (commentList.at(i).toElement().attribute(ATTR_PARA) == elementId) {
                if (tambienComentario) {
                    borrarComentario(commentList.at(i).toElement().tagName());
                } else {
                    commentList.at(i).toElement().removeAttribute(ATTR_PARA);
                }
                break;
            }
        }
    }
}

QString XmlHandler::getComentario(const QString &elementId) {
    QDomNode node = m_doc.documentElement().elementsByTagName(elementId).at(0);
    QDomNodeList commentList = m_doc.documentElement().firstChildElement(COMENTARIOS).childNodes();
    if (!node.isNull() && node.isElement()) {
        for (int i = 0; i < commentList.length(); i++) {
            if (commentList.at(i).toElement().attribute(ATTR_PARA) == elementId) {
                return commentList.at(i).toElement().text();
            }
        }
    }
    return QString("");
}

QString XmlHandler::getComentarioArchivo(const QString &elementId) {
    QString ret = getComentario(elementId);
    ret = ret.replace(QChar::ParagraphSeparator, "\n* ").replace(QChar::LineSeparator, "\n* ");
    return ret;
}
//------------------------------ private functions ------------------------------------
/*!
 * \brief XmlHandler::addElemento
 * \param parentNode
 * \param tagName
 * \param contenido
 * Crea un nuevo 'tag' en un determinado 'parent node' ( puede o no incluir un 'contenido' )
 */
void XmlHandler::addElemento(const QString &parentNode, const QString &tagName, const QString &contenido)
{

    if( m_doc.documentElement().firstChildElement(parentNode).firstChildElement(tagName).isNull() )
    {
        QDomElement newElement = m_doc.createElement(tagName);

        m_doc.documentElement().firstChildElement(parentNode).appendChild(newElement);

        if( !contenido.isNull() )
            newElement.appendChild(m_doc.createTextNode(contenido));
    }    
}

/*! Genera la estructura base de un nuevo documento XML para Datos
 *  \brief XmlHandler::createEstructuraBase
 */
void XmlHandler::crearEstructuraProyecto()
{
    // Crea el encabezado del archivo XML y lo agrega al documento
    QDomProcessingInstruction instr = m_doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    m_doc.appendChild(instr);

    // Crea el elemento 'raiz' (proyecto) y lo agrega al documento
    QDomElement root = m_doc.createElement(PROYECTO);
    m_doc.appendChild(root);

    // Crea los elementos principales
    root.appendChild(m_doc.createElement(GENERAL));
    root.appendChild(m_doc.createElement(CONFIGURACION));
    root.appendChild(m_doc.createElement(RECURSOS));
    root.appendChild(m_doc.createElement(MAQUINAS));
    root.appendChild(m_doc.createElement(EVENTOS));
    root.appendChild(m_doc.createElement(ACCIONES));
    root.appendChild(m_doc.createElement(VARIABLES));
    root.appendChild(m_doc.createElement(CONSTANTES));
    //TEST
    root.appendChild(m_doc.createElement(COMENTARIOS));

    // Crea los sub-elementos dentro de 'general'
    addElemento(GENERAL, NOMBRE);
    addElemento(GENERAL, AUTORES);
    addElemento(GENERAL, CREACION);
    addElemento(GENERAL, MODIFICACION);
    addElemento(GENERAL, CARPETA);

    // Crea los sub-elementos dentro de 'configuracion'
    addElemento(CONFIGURACION, HARDWARE);
    addElemento(CONFIGURACION, TARGET);

    // Crea los sub-elementos dentro de 'recursos'
    addElemento(RECURSOS, ARCHIVOS_C);
    addElemento(RECURSOS, ARCHIVOS_H);
}

/*! Genera la estructura base de un nuevo documento XML para Periféricos
 *  \brief XmlHandler::createEstructuraPerifericos
 */
void XmlHandler::crearEstructuraPerifericos()
{
    // Crea el encabezado del archivo XML y lo agrega al documento
    QDomProcessingInstruction instr = m_doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    m_doc.appendChild(instr);

    // Crea el elemento 'raiz' (proyecto) y lo agrega al documento
    QDomElement root = m_doc.createElement("perifericos");
    m_doc.appendChild(root);
}

//Comentarios
//Realiza el cambio de indice del comentario por si se eliminan elementos
bool XmlHandler::cambiarElementoComentario(const QString& oldElementId, const QString& newElementId){
    if (m_doc.documentElement().elementsByTagName(newElementId).at(0).isNull()) {
        QDomNodeList commentList = m_doc.documentElement().firstChildElement(COMENTARIOS).childNodes();
            for (int i = 0; i < commentList.length(); i++) {
                if (commentList.at(i).toElement().attribute(ATTR_PARA) == oldElementId) {
                    commentList.at(i).toElement().setAttribute(ATTR_PARA, newElementId);
                    return true;
                }
            }

    }
    return false;
}



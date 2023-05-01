#include "diagramitem.h"
#include "diagramtransition.h"

#include <math.h>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsView>

/**
 * @brief DiagramItem::DiagramItem
 * @param itemType
 * @param contextMenu
 * @param parent
 * Probando Repositorio
 */
DiagramItem::DiagramItem(ItemType itemType, QMenu *contextMenu, QGraphicsItem *parent)
    : QGraphicsRectItem(parent), m_itemType(itemType), m_contextMenu(contextMenu)
{
    QPainterPath path;
    QGraphicsDropShadowEffect *shadow;

    switch (m_itemType) {
    case EstadoSimple:
        m_name = "?";
        setRect(-ESTADO_SIMPLE_DEFAULT_WIDTH/2, -ESTADO_SIMPLE_DEFAULT_HEIGHT/2,
                ESTADO_SIMPLE_DEFAULT_WIDTH, ESTADO_SIMPLE_DEFAULT_HEIGHT);
        setZValue(ESTADO_SIMPLE_ZVALUE);
        path.addRoundedRect(rect().marginsAdded(QMarginsF(1, 1, 1, 1)),
                            ESTADO_SIMPLE_RADIUS, ESTADO_SIMPLE_RADIUS,
                            Qt::RelativeSize);
        m_polygon = path.toFillPolygon();
        m_resizeHandle = new QGraphicsRectItem(rect().bottomRight().x()-3,
                                               rect().bottomRight().y()-3, 6, 6, this);
        m_resizeHandle->setBrush(Qt::white);
        m_resizeHandle->setPen(QPen(QBrush(Qt::black), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        m_resizeHandle->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        m_resizeHandle->setCursor(Qt::SizeFDiagCursor);
        m_resizeHandle->setZValue(ESTADO_SIMPLE_ZVALUE);
        m_resizeHandle->hide();
        m_currentState = ResizeHandleState::Released;        
        // genero el efecto de la sombra
        shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(10);
        shadow->setOffset(4, 4);
        setGraphicsEffect(shadow);

        //obsolete method -> replace with QGraphicsItem::installSceneEventFilter(QGraphicsItem *filterItem)
        setHandlesChildEvents(true);
        break;

    case EstadoReset:
        m_name = "RESET";
        setRect(-ESTADO_RESET_DIAMETER/2, -ESTADO_RESET_DIAMETER/2,
                ESTADO_RESET_DIAMETER, ESTADO_RESET_DIAMETER);
        setZValue(ESTADO_RESET_ZVALUE);
        // genero QPolygon para las intersecciones con las transiciones
        path.addEllipse(-ESTADO_RESET_DIAMETER/2, -ESTADO_RESET_DIAMETER/2,
                        ESTADO_RESET_DIAMETER, ESTADO_RESET_DIAMETER);
        m_polygon = path.toFillPolygon();
        // genero el efecto de la sombra
        shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(10);
        shadow->setOffset(2, 2);
        setGraphicsEffect(shadow);
        break;

    default:
        break;
    }

    m_isEstadoInicial = false;
    m_isActive = false;
    setCursor(Qt::PointingHandCursor);
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);
}

/**
 * @brief DiagramItem::boundingRect
 * @return
 */
QRectF DiagramItem::boundingRect() const
{
    return (rect().marginsAdded(QMarginsF(3, 3, 3, 3)));
}

/**
 * @brief DiagramItem::shape
 * @return
 */
QPainterPath DiagramItem::shape() const
{
    QPainterPath path;

    switch (m_itemType) {
    case EstadoSimple:
        path.addRoundedRect(rect(), ESTADO_SIMPLE_RADIUS,
                            ESTADO_SIMPLE_RADIUS, Qt::RelativeSize);
        break;
    case EstadoReset:
        path.addEllipse(-ESTADO_RESET_DIAMETER/2, -ESTADO_RESET_DIAMETER/2,
                        ESTADO_RESET_DIAMETER, ESTADO_RESET_DIAMETER);
        break;
    default:
        break;
    }
    return path;
}

/**
 * @brief DiagramItem::removeTransition
 * @param tr
 */
void DiagramItem::removeTransition(DiagramTransition *tr)
{
        m_transitions.removeOne(tr);
}

/**
 * @brief DiagramItem::removeAllTransitions
 */
void DiagramItem::removeAllTransitions()
{
    const auto transitionsCopy = m_transitions;
    for (DiagramTransition *tr : transitionsCopy) {
        tr->startItem()->removeTransition(tr);
        tr->endItem()->removeTransition(tr);
        scene()->removeItem(tr);
        delete tr;
    }
}

/**
 * @brief DiagramItem::setAsEstadoInicial
 * @param newValue
 */
void DiagramItem::setAsEstadoInicial(bool newValue)
{
    m_isEstadoInicial = newValue;
}

/**
 * @brief DiagramItem::setActive
 * @param newValue
 */
void DiagramItem::setActive(bool newValue)
{
    m_isActive = newValue;
    update();
}

/**
 * @brief DiagramItem::addTransition
 * @param tr
 */
void DiagramItem::addTransition(DiagramTransition *tr)
{
    m_transitions.append(tr);
}

/**
 * @brief DiagramItem::setName
 * @param name
 */
void DiagramItem::setName(const QString &name)
{
    m_name = name;
    setToolTip(m_name.toUpper());
    update();
}

/**
 * @brief DiagramItem::setSize
 * @param width
 * @param height
 */
void DiagramItem::setSize(const qreal &width, const qreal &height)
{
    QPainterPath roundRectPath;

    switch (m_itemType) {
    case EstadoSimple:
        prepareGeometryChange();
        setRect(QRectF(-width/2.0, -height/2.0, width, height));
        m_resizeHandle->setRect(rect().bottomRight().x()-3, rect().bottomRight().y()-3, 6, 6);
        m_resizeHandle->update();
        roundRectPath.addRoundedRect(rect().marginsAdded(QMarginsF(1,1,1,1)),
                                     ESTADO_SIMPLE_RADIUS, ESTADO_SIMPLE_RADIUS,
                                     Qt::RelativeSize);
        m_polygon = roundRectPath.toFillPolygon();
        break;

    default:
        break;
    }
}

/**
 * @brief DiagramItem::contextMenuEvent
 * @param event
 */
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    switch (m_itemType) {
    case EstadoSimple:
        scene()->clearSelection();
        setSelected(true);
        m_contextMenu->exec(event->screenPos());
        break;

    default:
        break;
    }
}

/**
 * @brief DiagramItem::itemChange
 * @param change
 * @param value
 * @return
 */
QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // actualizo transiciones cuando cambia la posición de un estado
    if (change == QGraphicsItem::ItemPositionChange ||
            change == QGraphicsItem::ItemPositionHasChanged) {
        for (DiagramTransition *tr : qAsConst(m_transitions)) {
            tr->updatePosition();
        }
        if(m_itemType == EstadoSimple) {
            m_resizeHandle->update();
        }
    }

    // muestro/oculto el resizeHandle cuando selecciono un item
    if(m_itemType == EstadoSimple) {   
        if(change == QGraphicsItem::ItemSelectedHasChanged) {
            prepareGeometryChange();
            if (isSelected()) {
                m_resizeHandle->show();
            }
            else if (!m_resizeHandle->isUnderMouse()) {
                m_resizeHandle->hide();
                m_resizeHandle->update();
            }
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

/**
 * @brief DiagramItem::mousePressEvent
 * @param event
 */
void DiagramItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (m_itemType) {
    case EstadoSimple:
        QGraphicsItem::mousePressEvent(event);
        // si se presionó el resizeHandle, actualizo estado y guardo la coordenada del click
        if(m_resizeHandle->isUnderMouse()) {
            m_resizeHandle->setBrush(QBrush(Qt::black));
            m_currentState = ResizeHandleState::Pressed;
            m_handlePressOffset = rect().bottomRight() - event->pos();
            m_handleCenterOffset = rect().center() - event->pos();
            m_resizeHandle->update();
        }
        break;

    case EstadoReset:
        // llamo al handler por default
        QGraphicsItem::mousePressEvent(event);
        break;

    default:
        break;
    }
}

/**
 * @brief DiagramItem::mouseReleaseEvent
 * @param event
 */
void DiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (m_itemType) {
    case EstadoSimple:
        QGraphicsItem::mouseReleaseEvent(event);
        // si se soltó el resizeHandle, actualizo el estado
        if (m_currentState == ResizeHandleState::Pressed) {
            m_resizeHandle->setSelected(false);
            m_currentState = ResizeHandleState::Released;
            m_resizeHandle->setBrush(QBrush(Qt::white));
            m_resizeHandle->update();
        }
        // actualizo las auto-transiciones
        for (DiagramTransition *tr : qAsConst(m_transitions))
            if (tr->startItem() == tr->endItem())
                tr->setHandleSelected(false);
        break;

    case EstadoReset:
        // llamo al handler por default
        QGraphicsItem::mouseReleaseEvent(event);
        break;

    default:
        break;
    }
}

/**
 * @brief DiagramItem::mouseMoveEvent
 * @param event
 */
void DiagramItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF newRect;
    QPainterPath roundRectPath;

    switch (m_itemType) {
    case EstadoSimple:
        if(m_currentState == ResizeHandleState::Pressed) {
            prepareGeometryChange();
            // actualizo el tamaño del estado y la posición
            newRect.setTopLeft(rect().topLeft());
            newRect.setBottomRight(event->pos() + m_handlePressOffset);
            newRect.setSize(newRect.size().expandedTo(QSize(ESTADO_SIMPLE_MIN_WIDTH,
                                                             ESTADO_SIMPLE_MIN_HEIGHT)));
            setPos(pos()+QPointF((newRect.width()-rect().width())/2,(newRect.height()-rect().height())/2));
            setRect(QRectF(-newRect.width()/2, -newRect.height()/2, newRect.width(), newRect.height()));
            // actualizo el resizeHandle
            m_resizeHandle->setRect(rect().bottomRight().x()-3, rect().bottomRight().y()-3, 6, 6);
            m_resizeHandle->update();
            // actualizo el QPolygon del item
            roundRectPath.addRoundedRect(rect().marginsAdded(QMarginsF(1,1,1,1)),
                                         ESTADO_SIMPLE_RADIUS, ESTADO_SIMPLE_RADIUS,
                                         Qt::RelativeSize);
            m_polygon = roundRectPath.toFillPolygon();
        } else {
            // actualizo las auto-transiciones
            for (DiagramTransition *tr : qAsConst(m_transitions))
                if (tr->startItem() == tr->endItem())
                    tr->setHandleSelected(true);
            // llamo al handler por default
            QGraphicsRectItem::mouseMoveEvent(event);
        }
        break;

    case EstadoReset:
        // llamo al handler por default
        QGraphicsRectItem::mouseMoveEvent(event);
        break;

    default:
        break;
    }
    update();
}

/**
 * @brief DiagramItem::mouseDoubleClickEvent
 * @param event
 */
void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    switch (m_itemType) {
    case EstadoSimple:
        QGraphicsItem::mouseDoubleClickEvent(event);
        break;

    case EstadoReset:
        break;

    default:
        break;
    }
}

/**
 * @brief DiagramItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QLinearGradient gradient;
    QFont font("Consolas", 10, QFont::Normal);

    switch (m_itemType) {
    case EstadoSimple:
        // llamo al painter del polygongItem y luego agrego el nombre del estado
        painter->setPen(QPen(m_color.darker(350), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawRoundedRect(rect(), ESTADO_SIMPLE_RADIUS, ESTADO_SIMPLE_RADIUS, Qt::RelativeSize);
        painter->save();
        gradient.setStart(rect().bottomRight());
        gradient.setFinalStop(rect().topLeft());
        gradient.setColorAt(0, QColor(m_color.lighter(180)));
        gradient.setColorAt(1, QColor(m_color));
        painter->fillPath(shape(), gradient);
        painter->restore();

        if (isSelected()) {
            painter->setPen(QPen(m_color.darker(350), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(rect().marginsAdded(QMarginsF(2, 2, 2, 2)),
                                     ESTADO_SIMPLE_RADIUS, ESTADO_SIMPLE_RADIUS,
                                     Qt::RelativeSize);
        } else if (m_isActive == true) {
            painter->save();
            QLinearGradient gradient(rect().bottomRight(), rect().topLeft());
            gradient.setColorAt(0, QColor("salmon"));
            gradient.setColorAt(1, QColor("red"));
            painter->fillPath(shape(), gradient);
            painter->restore();
            painter->setPen(QPen(QColor("red"), 1.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(rect().marginsAdded(QMarginsF(2,2,2,2)),
                                     ESTADO_SIMPLE_RADIUS, ESTADO_SIMPLE_RADIUS,
                                     Qt::RelativeSize);
        }
        painter->setFont(font);
        painter->setPen(Qt::black);
        painter->drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, m_name.toUpper());
        break;

    case EstadoReset:
        painter->save();
        gradient.setStart(rect().bottomRight());
        gradient.setFinalStop(rect().topLeft());
        gradient.setColorAt(0, QColor(Qt::black).lighter(100));
        gradient.setColorAt(1, QColor(Qt::black));
        painter->fillPath(this->shape(), gradient);
        painter->drawEllipse(-ESTADO_RESET_DIAMETER/2, -ESTADO_RESET_DIAMETER/2,
                             ESTADO_RESET_DIAMETER, ESTADO_RESET_DIAMETER);
        painter->restore();

        if (isSelected()) {
            painter->setPen(QPen(Qt::black, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(rect().marginsAdded(QMarginsF(2, 2, 2, 2)),
                                     100, 100, Qt::RelativeSize);
        } else if (m_isActive == true) {
            painter->save();
            QLinearGradient gradient(rect().bottomRight(), rect().topLeft());
            gradient.setColorAt(0, QColor("red"));
            gradient.setColorAt(1, QColor("red"));
            painter->fillPath(this->shape(), gradient);
            painter->setPen(QPen(QColor("darksalmon"), 1.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawEllipse(-ESTADO_RESET_DIAMETER/2, -ESTADO_RESET_DIAMETER/2,
                                 ESTADO_RESET_DIAMETER,ESTADO_RESET_DIAMETER);
            painter->setPen(QPen(Qt::red, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(rect().marginsAdded(QMarginsF(2,2,2,2)),
                                     100, 100, Qt::RelativeSize);
            painter->restore();
        }
        break;

    default:
        break;
    }
}

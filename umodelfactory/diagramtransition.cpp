#include "diagramtransition.h"
#include "diagramitem.h"

#include <math.h>

#include <QPen>
#include <QPainter>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>

/**
 * @brief DiagramTransition::DiagramTransition
 * @param fromItem
 * @param toItem
 * @param contextMenu
 * @param parent
 */
DiagramTransition::DiagramTransition(DiagramItem *fromItem, DiagramItem *toItem, QMenu *contextMenu, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), m_from(fromItem), m_to(toItem), m_contextMenu(contextMenu)
{
    setPen(QPen(m_color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
    setCursor(QCursor(Qt::PointingHandCursor));
    setHandlesChildEvents(true);
    setZValue(TRANSITION_ZVALUE);

    m_positionHandle = new QGraphicsRectItem(QRectF(-3,-3, 6, 6), this);
    if (m_from != m_to) {
        m_positionHandle->setPos((m_from->pos() + m_to->pos())/2.0);
    } else {
        m_positionHandle->setPos(m_from->rect().translated(m_from->pos()).center() -
                                 QPointF(m_from->rect().width()*0.95, -15));
    }
    m_positionHandle->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    m_positionHandle->setCursor(QCursor(Qt::PointingHandCursor));
    m_positionHandle->setPen(QPen(Qt::black, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    m_positionHandle->setBrush(QBrush(Qt::white));
    m_positionHandle->setZValue(HANDLE_ZVALUE);
    m_positionHandle->hide();

    m_textItem = new QGraphicsTextItem(" ? / ? ", this);
    m_textItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    m_textItem->setZValue(TRANSITION_ZVALUE);
    m_event = "?";
    m_actions << "?";
    m_textItem->setFont(QFont("Consolas", 10, QFont::Normal));
    m_textItem->setCursor(QCursor(Qt::PointingHandCursor));
    m_textItemPressed = false;
    m_active = false;
    m_visible = true;

    if(m_from->itemType() == DiagramItem::EstadoReset) {
        m_textRelativePos = QPointF(0, -m_textItem->boundingRect().height());
        QLineF line(m_from->pos(), m_to->pos());
        m_positionHandle->setPos(line.pointAt(0.28));
    } else if (m_from != m_to) {
        QLineF line(m_from->pos(), m_to->pos());
        QLineF norm_line = line.normalVector().translated(line.dx()/2, line.dy()/2);
        m_positionHandle->setPos(norm_line.pointAt(0.1));
        if (line.angle() >= 0.0 && line.angle() < 90.0) {
            m_textRelativePos = QPointF(-m_textItem->boundingRect().width(),
                                        -m_textItem->boundingRect().height());
        } else if (line.angle() >= 90.0 && line.angle() < 180) {
            m_textRelativePos = QPointF(-m_textItem->boundingRect().width(), 0);
        } else if (line.angle() >= 270 && line.angle() < 360.0 ) {
            m_textRelativePos = QPointF(0, -m_textItem->boundingRect().height());
        }
    } else {
        m_textRelativePos = QPointF(-m_textItem->boundingRect().width()/2, 0);
    }
}

/**
 * @brief DiagramTransition::shape
 * @return
 */
QPainterPath DiagramTransition::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(8.0);
    return (stroker.createStroke(this->path()));
}

/**
 * @brief DiagramTransition::setTextRelativePos
 * @param pos
 */
void DiagramTransition::setTextRelativePos(const QPointF &pos)
{
    m_textRelativePos = pos;
    update();
}

/**
 * @brief DiagramTransition::updatePosition
 */
void DiagramTransition::updatePosition()
{
    prepareGeometryChange();

    QPointF beyond = getBeyond();
    QPointF p_head_end = getHead();
    QPainterPath curve;

    if(m_from != m_to) {
        QPointF p_tail_end = getTail();
        curve.moveTo(p_tail_end);
        curve.quadTo(beyond, p_head_end);
        setPath(curve);
    } else {
        QLineF mid_line(m_positionHandle->pos(), p_head_end);
        QLineF mid_line2(p_head_end, mid_line.normalVector().pointAt(0.5));
        QPointF p_tail_end = getAutoTail();
        curve.moveTo(p_tail_end);
        curve.cubicTo(mid_line2.p2(), m_positionHandle->pos(), p_head_end);
        setPath(curve);
    }

    //The angle from midpoint to head
    double arrow_sz = 10.0;   //pixels
    double dx = m_to->pos().x() - beyond.x();
    double dy = m_to->pos().y() - beyond.y();
    double angle = M_PI - (::atan(dx/dy));
    if (dy >= 0.0)
        angle = (1.0 * M_PI) + angle;
    QPointF p0(p_head_end.x(), p_head_end.y());
    QPointF p1 = p0 + QPointF(::sin(angle+0.0+(M_PI*0.1))*arrow_sz, -::cos(angle+0.0+(M_PI*0.1))*arrow_sz);
    QPointF p2 = p0 + QPointF(::sin(angle+0.0-(M_PI * 0.1))*arrow_sz, -::cos(angle+0.0-(M_PI*0.1))*arrow_sz);
    m_arrowHead = QPolygonF() << p0 << p1 << p2;
}

/**
 * @brief DiagramTransition::setHandleSelected
 * @param selected
 */
void DiagramTransition::setHandleSelected(bool selected)
{
    if(m_from == m_to) {
        if(selected) {
            m_positionHandle->show();
            m_positionHandle->setSelected(true);
        } else if (!this->isSelected()) {
            m_positionHandle->hide();
        }
        update();
    }
}

/**
 * @brief DiagramTransition::setText
 * @param event
 * @param actions
 */
void DiagramTransition::setText(const QString &event, const QStringList &actions)
{
    prepareGeometryChange();

    QString str(event + " /");
    foreach (QString act_str, actions) {
        if (!act_str.isEmpty()) {
            if (act_str != "?") {
                str.append("\n" + act_str);
            } else
                str.append(" ? ");
        } else
            str.append(" - ");
    }
    m_event = event;
    m_actions = actions;
    m_textItem->setPlainText(str);
    m_textItem->update();
}

/**
 * @brief DiagramTransition::alignText
 */
void DiagramTransition::alignText()
{
    prepareGeometryChange();

    if (m_from != m_to) {
        QLineF line(m_from->pos(), m_to->pos());
        if (line.angle() >= 0.0 && line.angle() < 90.0) {
            m_textRelativePos = QPointF(-m_textItem->boundingRect().width(),
                                        -m_textItem->boundingRect().height());
        } else if (line.angle() >= 90.0 && line.angle() < 180) {
            m_textRelativePos = QPointF(-m_textItem->boundingRect().width(),0);
        } else if (line.angle() >= 270 && line.angle() < 360.0 ) {
            m_textRelativePos = QPointF(0, -m_textItem->boundingRect().height());
        }
    } else {
        m_textRelativePos = QPointF(-m_textItem->boundingRect().width()/2,0);
    }
}

/**
 * @brief DiagramTransition::setActive
 * @param newValue
 */
void DiagramTransition::setActive(bool newValue)
{
    prepareGeometryChange();

    m_active = newValue;
    if (newValue == true) {
        QString line = m_textItem->toHtml();
        QString alertHtml = "<font color=\"Red\">";
        QString endHtml = "</font><br>";
        line = alertHtml + line;
        line = line + endHtml;
        m_textItem->setHtml(line);
    } else {
        setText(m_event, m_actions);
    }
}

/**
 * @brief DiagramTransition::paint
 * @param painter
 */
void DiagramTransition::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPen myPen = pen();
    myPen.setColor(m_color);

    if(isSelected())
        myPen.setWidthF(1.40);
    else
        myPen.setWidthF(0.90);

    if (m_active == true) {
        if(m_visible == true) {
            myPen.setColor(QColor("red"));
            myPen.setStyle(Qt::DashLine);
            myPen.setWidthF(2.0);
        } else
            myPen.setColor(QColor("white"));
    }

    painter->setPen(myPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(this->path());

    // actualizo la pos del texto, cuando cambia la flecha
    if(m_textItemPressed == false)
        m_textItem->setPos(m_positionHandle->pos()+m_textRelativePos);

    //Arrow head
    if(m_active == false)
        painter->setBrush(Qt::black);
    else {
        if (m_visible == true) {
            painter->setPen(QPen(QColor("red"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(QColor("red"));
        }
        else {
            painter->setPen(QPen(QColor("white")));
            painter->setBrush(QColor("white"));
        }
    }
    painter->drawPolygon(m_arrowHead);

    // Control point (dibujo una linea punteada)
    if (this->isSelected() || m_positionHandle->isSelected()) {
        QLineF line_mid(m_positionHandle->pos(), this->path().pointAtPercent(0.5));
        painter->setPen(QPen(m_color, 0.5, Qt::DashLine));
        painter->drawLine(line_mid);
        //Mid point
        painter->setPen(QPen(m_color, 2.5, Qt::SolidLine));
        painter->drawPoint(this->path().pointAtPercent(0.5));
    }
}

/**
 * @brief DiagramTransition::itemChange
 * @param change
 * @param value
 * @return
 */
QVariant DiagramTransition::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged || change == QGraphicsItem::ItemSelectedChange) {
        if (this->isSelected())
            m_positionHandle->show();
        else if (!m_positionHandle->isUnderMouse())
            m_positionHandle->hide();
    }
    return value;
}

/**
 * @brief DiagramTransition::contextMenuEvent
 * @param event
 */
void DiagramTransition::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    this->setSelected(true);
    m_contextMenu->exec(event->screenPos());
}

/**
 * @brief DiagramTransition::mousePressEvent
 * @param event
 */
void DiagramTransition::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);

    m_textItem->setSelected(true);

    if (m_positionHandle->isUnderMouse()) {
            m_positionHandle->setSelected(true);
            m_positionHandle->setBrush(Qt::black);
    } else if (m_textItem->isUnderMouse()) {
        // guardo el punto del click inicial
        m_textPressOffset = m_textItem->pos() - event->pos();
        // actualizo el estado del texto a "presionado"
        m_textItemPressed = true;
    }
}

/**
 * @brief DiagramTransition::mouseReleaseEvent
 * @param event
 */
void DiagramTransition::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    // lo mantengo como selected para seguir viendo el recuadro
    m_textItem->setSelected(true);
    // actualizo el estado del texto a "no presionado"
    if(m_textItemPressed == true)
        m_textItemPressed = false;

    //actualizo el estado de m_mid
    m_positionHandle->setSelected(false);
    m_positionHandle->setBrush(Qt::white);
}

/**
 * @brief DiagramTransition::mouseMoveEvent
 * @param event
 */
void DiagramTransition::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // actualizo la pos del texto
    if(m_textItemPressed == true) {
        m_textItem->setPos(event->pos() + m_textPressOffset);
        m_textRelativePos = m_textItem->pos()-m_positionHandle->pos();
        updatePosition();
    } else if (m_positionHandle->isSelected()) {
        m_positionHandle->setPos(event->pos());
        updatePosition();
    } else
        QGraphicsItem::mouseMoveEvent(event);
}

/**
 * @brief DiagramTransition::getAngle
 * @param dx
 * @param dy
 * @return
 */
double DiagramTransition::getAngle(const double dx, const double dy)
{
    return (M_PI - (::atan(dx/dy)));
}

/**
 * @brief DiagramTransition::getBeyond
 * @return
 */
QPointF DiagramTransition::getBeyond() const
{
    const QPointF center = getCenter();
    const double dx_mid_center = m_positionHandle->pos().x() - center.x();
    const double dy_mid_center = m_positionHandle->pos().y() - center.y();
    const QPointF beyond(center.x() + dx_mid_center*1.45, center.y() + dy_mid_center*1.5);
    return beyond;
}

/**
 * @brief DiagramTransition::getCenter
 * @return
 */
QPointF DiagramTransition::getCenter() const
{
   const QPointF center((m_from->pos() + m_to->pos()) / 2.0);
   return center;
}

/**
 * @brief DiagramTransition::getHead
 * @return
 */
QPointF DiagramTransition::getHead()
{
    QPointF beyond = getBeyond();
    QLineF line_head(beyond, m_to->pos());
    QPointF p1 = m_to->polygon().constFirst() + m_to->pos();
    QPointF p2;
    QPointF p_head_end;
    QLineF polyLine;
    QLineF::IntersectType intersectType = QLineF::NoIntersection;

    for(int i = 1; i < m_to->polygon().count(); ++i) {
        p2 = m_to->polygon().at(i) + m_to->pos();
        polyLine = QLineF(p1, p2);
        intersectType = polyLine.intersects(line_head, &p_head_end);
        if (intersectType == QLineF::BoundedIntersection) {
            m_prev_head=polyLine.pointAt(0.5);
            break;
        }
        p1 = p2;
    }
    if (intersectType == QLineF::UnboundedIntersection)
        p_head_end = m_prev_head;

    return p_head_end;
}

/**
 * @brief DiagramTransition::getTail
 * @return
 */
QPointF DiagramTransition::getTail()
{
    QPointF beyond = getBeyond();
    QLineF line_tail(m_from->pos(), beyond);
    QPointF p1 = m_from->polygon().constFirst() + m_from->pos();
    QPointF p2;
    QPointF p_tail_end;
    QLineF polyLine;
    QLineF::IntersectType intersectType = QLineF::NoIntersection;

    for(int i = 1; i < m_from->polygon().count(); ++i) {
        p2 = m_from->polygon().at(i) + m_from->pos();
        polyLine = QLineF(p1, p2);
        intersectType = polyLine.intersects(line_tail, &p_tail_end);
        if(intersectType == QLineF::BoundedIntersection) {
            m_prev_tail=polyLine.pointAt(0.5);
            break;
        }
        p1 = p2;
    }
    if (intersectType == QLineF::UnboundedIntersection)
        p_tail_end = m_prev_tail;

    return p_tail_end;
}

/**
 * @brief DiagramTransition::getAutoTail
 * @return
 */
QPointF DiagramTransition::getAutoTail()
{
    QPointF p_head_end = getHead();
    QLineF mid_line(m_positionHandle->pos(), p_head_end);
    QLineF mid_line2(p_head_end, mid_line.normalVector().pointAt(0.5));
    QLineF line_tail(m_from->pos(), mid_line2.pointAt(0.5));
    QPointF p1 = m_from->polygon().constFirst() + m_from->pos();
    QPointF p2;
    QPointF p_tail_end;
    QLineF polyLine;
    QLineF::IntersectType intersectType = QLineF::NoIntersection;

    for(int i = 1; i < m_from->polygon().count(); ++i) {
        p2 = m_from->polygon().at(i) + m_from->pos();
        polyLine = QLineF(p1, p2);
        intersectType = polyLine.intersects(line_tail, &p_tail_end);
        if(intersectType == QLineF::BoundedIntersection) {
            m_prev_tail=polyLine.pointAt(0.5);
            break;
        }
        p1 = p2;
    }
    if (intersectType == QLineF::UnboundedIntersection)
        p_tail_end = m_prev_tail;

    return p_tail_end;
}

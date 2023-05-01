#ifndef DIAGRAMTRANSITION_H
#define DIAGRAMTRANSITION_H

#include <QGraphicsPathItem>

class DiagramItem;
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;

class DiagramTransition : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 4 };

    DiagramTransition(DiagramItem *fromItem, DiagramItem *toItem,
          QMenu *contextMenu, QGraphicsItem *parent = nullptr);

    int type() const { return Type; }
    QPainterPath shape() const;
    void setColor(const QColor &color) { m_color = color; }
    QColor color (void) const { return m_color; }
    DiagramItem *startItem() const { return m_from; }
    DiagramItem *endItem() const { return m_to; }
    QGraphicsRectItem *midItem() const { return m_positionHandle; }
    QGraphicsTextItem *textItem() const { return m_textItem; }
    void setTextRelativePos(const QPointF &pos);
    QPointF textRelativePos() const { return m_textRelativePos; }

    void updatePosition();
    void setHandleSelected(bool selected);
    void setText(const QString &event, const QStringList &actions);
    void alignText();
    QString event() const { return m_event; }
    QStringList actions() const { return m_actions; }

    void setActive(bool newValue);
    void setVisible(bool newValue) { m_visible = newValue; }
    bool isVisible (void) { return m_visible; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    DiagramItem *m_from;
    DiagramItem *m_to;
    QMenu *m_contextMenu;

    QGraphicsRectItem *m_positionHandle;
    QPointF m_handlePressOffset;
    QPointF m_lastOffset;

    QPolygonF m_arrowHead;
    QColor m_color;
    QPointF m_prevPoint;
    QPointF m_prev_tail;
    QPointF m_prev_head;
    QString m_event;
    QStringList m_actions;

    QGraphicsTextItem *m_textItem;
    QPointF m_textRelativePos;
    QPointF m_textPressOffset;
    bool m_textItemPressed;

    bool m_active;
    bool m_visible;

    const qreal TRANSITION_ZVALUE = 5.0;
    const qreal HANDLE_ZVALUE = 6.0;

    static double getAngle(const double dx, const double dy);
    QPointF getBeyond() const;
    QPointF getCenter() const;
    QPointF getHead();
    QPointF getTail();
    QPointF getAutoTail();
    void setPrevTail(const QPointF prev) { m_prev_tail = prev; }
};

#endif // DIAGRAMTRANSITION_H

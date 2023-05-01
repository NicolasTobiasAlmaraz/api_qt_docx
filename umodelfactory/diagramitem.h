#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsRectItem>

class DiagramTransition;
class DiagramScene;

class DiagramItem : public QGraphicsRectItem
{
public:
    enum { Type = UserType + 15 };
    enum ItemType { EstadoSimple, EstadoCompuesto, EstadoReset };
    enum ResizeHandleState { Released, Pressed, Hovered };

    DiagramItem(ItemType itemType, QMenu *contextMenu, QGraphicsItem *parent = 0);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void removeTransition(DiagramTransition *tr);
    void removeAllTransitions();

    void setAsEstadoInicial(bool newValue);
    bool isEstadoInicial() const { return m_isEstadoInicial; }

    void setActive(bool newValue);
    bool isActive(void) const { return m_isActive; }

    ItemType itemType() const { return m_itemType; }
    QPolygonF polygon() const { return m_polygon; }
    void addTransition(DiagramTransition *tr);
    QList<DiagramTransition *> transitions() const { return m_transitions; }
    int type() const override { return Type; }
    void setName(const QString &name);
    QString name() const { return m_name; }
    void setSize (const qreal &width, const qreal &height);
    void setColor (const QColor &color) { m_color = color; update(); }
    QColor color() const { return m_color; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    ItemType m_itemType;
    QMenu *m_contextMenu;
    QList<DiagramTransition *> m_transitions;
    QString m_name;
    QPolygonF m_polygon;
    QColor m_color;

    QGraphicsRectItem* m_resizeHandle;
    QPointF m_handlePressOffset;
    QPointF m_handleCenterOffset;
    ResizeHandleState m_currentState;

    bool m_isActive;
    bool m_isEstadoInicial;

    const qreal ESTADO_SIMPLE_DEFAULT_WIDTH = 110.0;
    const qreal ESTADO_SIMPLE_MIN_WIDTH = 80.0;
    const qreal ESTADO_SIMPLE_DEFAULT_HEIGHT = 65.0;
    const qreal ESTADO_SIMPLE_MIN_HEIGHT = 50.0;
    const qreal ESTADO_SIMPLE_RADIUS = 20.0;
    const qreal ESTADO_RESET_DIAMETER = 20.0;
    const qreal ESTADO_SIMPLE_ZVALUE = 10.0;
    const qreal ESTADO_RESET_ZVALUE = 8.0;
};

#endif // DIAGRAMITEM_H

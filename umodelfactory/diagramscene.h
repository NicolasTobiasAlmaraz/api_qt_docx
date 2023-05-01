#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>

#include "diagramitem.h"
#include "diagramtransition.h"

class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem = 0, InsertLineStart, InsertLineEnd, MoveItem, Simulation };

    explicit DiagramScene(QString name, QMenu *itemMenu, QObject *parent = nullptr);
    void initSceneRect(qreal x, qreal y, qreal w, qreal h);
    QColor itemColor() const { return m_itemColor; }
    QColor lineColor() const { return m_lineColor; }
    void setLineColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setName (const QString &name) { m_name = name; }
    QString name() const { return m_name; }
    DiagramItem::ItemType itemType() const { return m_itemType; }
    QMenu * itemMenu() const { return m_itemMenu; }
    Mode mode() const {return m_sceneMode;}

    // funciones para el manejo de items
    QList<DiagramItem *> itemList() const { return m_itemList; }
    void clearList() { m_itemList.clear(); }
    void addToList(DiagramItem *item) { m_itemList.append(item); }
    void removeFromList (DiagramItem *item);
    void removeSelectedItems();

    // estado inicial
    void insertInitialStateItem(int idx);
    void setResetItem (DiagramItem *item) { m_resetItem = item; }
    DiagramItem* resetItem() { return m_resetItem; }

    //simulación
    void setPreviousArrow(DiagramTransition *prev) { m_prevArrow = prev; }
    DiagramTransition* previousArrow() { return m_prevArrow; }

public slots:
    void setMode(DiagramScene::Mode mode);
    void setItemType(DiagramItem::ItemType type);

signals:
    void itemInserted(DiagramItem *item);
    void transitionInserted(DiagramTransition *tr);
    void itemSelected(QGraphicsItem *item);
    void itemDoubleClicked(int);
    void transitionDoubleClicked(DiagramTransition *tr);
    void rightClicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    bool isItemChange(int type);
    void insertItemAt(const QPointF &pos);
    void insertLine(DiagramItem *startItem, const QPointF &pos);
    void insertArrow(DiagramItem *endItem);

    QString m_name;
    QList<DiagramItem *> m_itemList;
    Mode m_sceneMode;
    DiagramItem::ItemType m_itemType;
    QMenu *m_itemMenu;
    QColor m_itemColor;
    QColor m_lineColor;
    int m_cursor;

    QGraphicsLineItem *m_line;
    DiagramItem *m_startItem;
    DiagramTransition *m_prevArrow;
    DiagramItem *m_resetItem;
};

#endif // DIAGRAMSCENE_H

#include "diagramscene.h"
#include "diagramtransition.h"

#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QDebug>

DiagramScene::DiagramScene(QString name, QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    m_name = name;
    m_itemMenu = itemMenu;
    m_sceneMode = MoveItem;
    m_itemType = DiagramItem::EstadoSimple;
    m_line = nullptr;
    m_itemColor = Qt::black;
    m_lineColor = Qt::black;
    m_resetItem = nullptr;
    m_startItem = nullptr;
    m_prevArrow = nullptr;

    setItemIndexMethod(QGraphicsScene::NoIndex);
}

void DiagramScene::initSceneRect(qreal x, qreal y, qreal w, qreal h)
{
    // background initial item (hidden)
    QGraphicsRectItem *initItem = new QGraphicsRectItem(x, y, w, h);
    this->addItem(initItem);
    initItem->hide();
}

void DiagramScene::setLineColor(const QColor &color)
{
    m_lineColor = color;
    if (isItemChange(DiagramTransition::Type)) {
        DiagramTransition *item = qgraphicsitem_cast<DiagramTransition *>(selectedItems().first());
        item->setColor(m_lineColor);
        update();
    }
}

void DiagramScene::setItemColor(const QColor &color)
{
    m_itemColor = color;
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            DiagramItem *d_item = qgraphicsitem_cast<DiagramItem *>(item);
            d_item->setColor(m_itemColor);
        }
    }
}

void DiagramScene::removeFromList(DiagramItem *item)
{
    m_itemList.removeAt(m_itemList.indexOf(item));
}

void DiagramScene::removeSelectedItems()
{
    foreach (QGraphicsItem *item, this->selectedItems()) {
        if (item->type() == DiagramTransition::Type) {
            this->removeItem(item);
            DiagramTransition *arrow = qgraphicsitem_cast<DiagramTransition *>(item);
            arrow->startItem()->removeTransition(arrow);
            arrow->endItem()->removeTransition(arrow);
            if (arrow->startItem()->itemType() == DiagramItem::EstadoReset) {
                DiagramItem *init = arrow->startItem();
                this->removeItem(init);
                this->setResetItem(nullptr);
                arrow->endItem()->setAsEstadoInicial(false);
            }
            delete item;
        }
    }
    foreach (QGraphicsItem *item, this->selectedItems()) {
        if (item->type() == DiagramItem::Type) {
          int type = qgraphicsitem_cast<DiagramItem *>(item)->itemType();
            if (type == DiagramItem::EstadoSimple) {
                qgraphicsitem_cast<DiagramItem *>(item)->removeAllTransitions();
                this->removeItem(item);
                this->removeFromList(qgraphicsitem_cast<DiagramItem *>(item));
                if(qgraphicsitem_cast<DiagramItem *>(item)->isEstadoInicial()) {
                    this->removeItem(this->resetItem());
                    this->setResetItem(nullptr);
                }
            }
            else if (type == DiagramItem::EstadoReset) {
                qgraphicsitem_cast<DiagramItem *>(item)->transitions().first()->endItem()->setAsEstadoInicial(false);
                qgraphicsitem_cast<DiagramItem *>(item)->removeAllTransitions();
                this->removeItem(item);
                this->setResetItem(nullptr);
            }
        }
    }
}


void DiagramScene::setMode(DiagramScene::Mode mode)
{
    m_sceneMode = mode;
    if((mode == MoveItem || mode == InsertItem) && (m_line != nullptr)) {
        removeItem(m_line);
        delete m_line;
        m_line = nullptr;
    } else if (mode == Simulation) {
        clearSelection();
    }
}

void DiagramScene::setItemType(DiagramItem::ItemType type)
{
    m_itemType = type;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        DiagramItem *startItem;
        DiagramItem *endItem;
        switch (m_sceneMode) {
            case InsertItem:
                insertItemAt(mouseEvent->scenePos());
                break;
            case InsertLineStart:
                startItem = qgraphicsitem_cast<DiagramItem *>(itemAt(mouseEvent->scenePos(),
                                                                     QTransform()));
                if (startItem != nullptr) {
                    if (startItem->itemType() == DiagramItem::EstadoSimple) {
                        m_sceneMode = InsertLineEnd;
                        insertLine(startItem, mouseEvent->scenePos());
                    }
                }
                break;
            case InsertLineEnd:
                endItem = qgraphicsitem_cast<DiagramItem *>(itemAt(mouseEvent->scenePos(),
                                                                   QTransform()));
                if (endItem != nullptr) {
                    if (endItem->itemType() == DiagramItem::EstadoSimple) {
                        insertArrow(endItem);
                    }
                }
                break;
            case Simulation:
                return;

            default:
                break;
        }
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
    else if ((mouseEvent->button() == Qt::RightButton)) {
        if(m_sceneMode != Simulation)
            emit rightClicked();
    }
    this->update();
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DiagramItem *item;
    QLineF newLine;

    switch (m_sceneMode) {
        case InsertLineStart:
            // actualizo el cursor
            item = qgraphicsitem_cast<DiagramItem *>(itemAt(mouseEvent->scenePos(),
                                                            QTransform()));
            if (item != nullptr) {
                if (item->itemType() == DiagramItem::EstadoSimple)
                    views().constFirst()->viewport()->setCursor(Qt::PointingHandCursor);
                else
                    views().constFirst()->viewport()->setCursor(Qt::ForbiddenCursor);
            }
            else
                views().constFirst()->viewport()->setCursor(Qt::ForbiddenCursor);
            break;

        case InsertLineEnd:
            // actualizo la línea para indicar la transición
            newLine = QLineF(m_startItem->scenePos(), mouseEvent->scenePos());
            m_line->setLine(newLine);
            item = qgraphicsitem_cast<DiagramItem *>(itemAt(mouseEvent->scenePos(),
                                                            QTransform()));
            if (item != nullptr) {
                if (item->itemType() == DiagramItem::EstadoSimple)
                    views().constFirst()->viewport()->setCursor(Qt::PointingHandCursor);
                else
                    views().constFirst()->viewport()->setCursor(Qt::ForbiddenCursor);
            }
            else
                views().constFirst()->viewport()->setCursor(Qt::ForbiddenCursor);
            break;

       case MoveItem:
            // mueve los items
            QGraphicsScene::mouseMoveEvent(mouseEvent);
            break;

        case Simulation:
            break;

        default:
            break;
    }
    this->update();
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_sceneMode == MoveItem) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(itemAt(mouseEvent->scenePos(), QTransform()));
        if (item != nullptr) {
            if (item->type() == DiagramItem::Type) {
                int index = m_itemList.indexOf(item);
                if (index != -1) {
                    emit itemDoubleClicked(index);
                    return;
                }
            }
        }
        DiagramTransition *arrow = qgraphicsitem_cast<DiagramTransition *>(itemAt(mouseEvent->scenePos(), QTransform()));
        if (arrow != nullptr) {
            if (arrow->type() == DiagramTransition::Type) {
                emit transitionDoubleClicked(arrow);
                return;
            }
        }
        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem *>(itemAt(mouseEvent->scenePos(), QTransform()));
        if (textItem != nullptr) {
            DiagramTransition *parentArrow = qgraphicsitem_cast<DiagramTransition *>(textItem->parentItem());
            if (parentArrow != nullptr)
                emit transitionDoubleClicked(parentArrow);
                return;
            }
    }
    else if(m_sceneMode == Simulation) {

    }
}

/**
 * @brief DiagramScene::insertItemAt
 * @param pos
 */
void DiagramScene::insertItemAt(const QPointF &pos)
{
    DiagramItem *item = new DiagramItem(m_itemType, m_itemMenu);
    item->setColor(QColor("lightcyan"));
    addItem(item);
    m_itemList.append(item);
    item->setPos(pos);
    item->setSelected(true);

    emit itemInserted(item);
}

/**
 * @brief DiagramScene::insertInitialItem
 */
void DiagramScene::insertInitialStateItem(int idx)
{
    DiagramItem *endItem = m_itemList.at(idx);

    if (m_resetItem != nullptr) {
        m_resetItem->removeAllTransitions();
        delete m_resetItem;
        m_resetItem = new DiagramItem(DiagramItem::EstadoReset, m_itemMenu);
        addItem(m_resetItem);
        m_resetItem->setPos(endItem->pos()-QPointF((endItem->rect().width()/2)+40, (endItem->rect().height()/2)+40));
        m_startItem = m_resetItem;
        insertArrow(endItem);

    } else {
        m_resetItem = new DiagramItem(DiagramItem::EstadoReset, m_itemMenu);
        addItem(m_resetItem);
        m_resetItem->setPos(endItem->pos()-QPointF((endItem->rect().width()/2)+40, (endItem->rect().height()/2)+40));
        m_startItem = m_resetItem;
        insertArrow(endItem);
    }
}

/**
 * @brief DiagramScene::insertLine
 * @param p1
 * @param p2
 */
void DiagramScene::insertLine(DiagramItem *startItem, const QPointF &pos)
{
    m_startItem = startItem;
    m_line = new QGraphicsLineItem(QLineF(startItem->scenePos(), pos));
    m_line->setPen(QPen(m_lineColor, 1.0));
    m_line->setZValue(-1);
    addItem(m_line);
}

/**
 * @brief DiagramScene::insertArrow
 * @param endItem
 */
void DiagramScene::insertArrow(DiagramItem *endItem)
{
    DiagramTransition *arrow = new DiagramTransition(m_startItem, endItem, m_itemMenu);
    arrow->setColor(m_lineColor);
    m_startItem->addTransition(arrow);
    if(m_startItem != endItem)
        endItem->addTransition(arrow);

    if(m_line != nullptr) {
        removeItem(m_line);
        delete m_line;
        m_line = nullptr;
    }

    views().constFirst()->viewport()->setCursor(Qt::ArrowCursor);
    addItem(arrow);
    arrow->updatePosition();
    emit transitionInserted(arrow);
}

/**
 * @brief DiagramScene::isItemChange
 * @param type
 * @return
 */
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}




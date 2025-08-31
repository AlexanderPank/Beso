#include "algoritmitem.h"

#include "arrow.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QPen>

// Creates algorithm item with connectors and title
AlgoritmItem::AlgoritmItem(AlgoritmType diagramType, QMenu *contextMenu, QString title,
                           QList<QPair<QString,QString>> in, QList<QPair<QString,QString>> out,
                           QGraphicsItem *parent)
    : QGraphicsItem(parent), myDiagramType(diagramType), myContextMenu(contextMenu)
{
    QPainterPath path;
    const int spacing = 20; // vertical distance between ports

    titleItem = new QGraphicsTextItem(title, this);
    QFont font = titleItem->font();
    font.setBold(false);
    font.setUnderline(false);
    font.setPointSizeF(font.pointSizeF() * 0.8);
    titleItem->setFont(font);

    int inTextsize = 0;
    int outTextsize = 0;

    for (auto pair : in) {
        auto inItem = new QGraphicsTextItem(pair.first + " (" + pair.second + ")", this);
        inObjText.insert(pair, inItem);
        inTextsize = qMax(inTextsize, int(inItem->boundingRect().width()));
        auto inObj = new QGraphicsEllipseItem(0, 0, 10, 10, this);
        inObj->setData(Qt::UserRole, QString("in"));
        inObjCircle.insert(pair, inObj);
        inObj->setBrush(QBrush(Qt::green));
    }
    for (auto pair : out) {
        auto outItem = new QGraphicsTextItem(pair.first + " (" + pair.second + ")", this);
        outObjText.insert(pair, outItem);
        outTextsize = qMax(outTextsize, int(outItem->boundingRect().width()));
        auto outObj = new QGraphicsEllipseItem(0, 0, 10, 10, this);
        outObj->setData(Qt::UserRole, QString("out"));
        outObjCircle.insert(pair, outObj);
        outObj->setBrush(QBrush(Qt::blue));
    }

    int width = titleItem->boundingRect().width() + 25;
    if (width < 50)
        width = 50;
    if (width < inTextsize + outTextsize)
        width = inTextsize + outTextsize + 25;
    int h_size = qMax(in.size(), out.size());
    int height = (h_size + 1) * (spacing + 10);

    path.addRoundedRect(-width / 2.0, -height / 2.0, width, height, 10, 10);
    myPolygon = path.toFillPolygon();
    polygonItem = new QGraphicsPolygonItem(myPolygon, this);
    polygonItem->setZValue(-10);
    polygonItem->setPen(QPen(Qt::black, 1));
    titleItem->setPos(-width / 2.0 + 5, -height / 2.0 + 10);

    int i = 0;
    for (auto var : inObjCircle) {
        var->setPos(-width / 2.0 + 5, -height / 2.0 + 20 + i * spacing);
        i++;
    }
    i = 0;
    for (auto var : inObjText) {
        var->setPos(-width / 2.0 + 15, -height / 2.0 + 18 + i * spacing);
        i++;
    }
    i = 0;
    for (auto var : outObjCircle) {
        var->setPos(width / 2.0 - 15, -height / 2.0 + 20 + i * spacing);
        i++;
    }
    i = 0;
    for (auto var : outObjText) {
        var->setPos(width / 2.0 - 15 - var->boundingRect().width(), -height / 2.0 + 18 + i * spacing);
        i++;
    }

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

// Removes arrow reference from list
void AlgoritmItem::removeArrow(Arrow *arrow) {
    arrows.removeAll(arrow);
}

// Removes and deletes all arrows connected to this item
void AlgoritmItem::removeArrows() {
    const auto arrowsCopy = arrows;
    for (Arrow *arrow : arrowsCopy) {
        static_cast<AlgoritmItem*>(arrow->startItem()->parentItem())->removeArrow(arrow);
        static_cast<AlgoritmItem*>(arrow->endItem()->parentItem())->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

// Adds arrow to internal list
void AlgoritmItem::addArrow(Arrow *arrow) {
    arrows.append(arrow);
}

// Returns pixmap representation for given type
QPixmap AlgoritmItem::image(AlgoritmType type) {
    QPixmap pixmap(250, 250);
    switch (type) {
    case ALGORITM:
        setBrush(QColor("#E3E3FD"));
        break;
    case CONDITION:
        setBrush(QColor("#FFFFE3"));
        break;
    case EVENT:
        setBrush(QColor("#FFF9A3"));
        break;
    case PARAM:
        setBrush(QColor("#CFFFE5"));
        break;
    default:
        pixmap.fill(Qt::transparent);
        break;
    }
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);
    return pixmap;
}

// Sets fill brush color of polygon
void AlgoritmItem::setBrush(QColor color) {
    polygonItem->setBrush(color);
}

// Returns list of output connector circles
QList<QGraphicsEllipseItem *> AlgoritmItem::getOutItems() {
    return outObjCircle.values();
}

// Returns list of input connector circles
QList<QGraphicsEllipseItem *> AlgoritmItem::getInItems() {
    return inObjCircle.values();
}

// Shows context menu for the item
void AlgoritmItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}

// Updates arrows when item moves or selection changes
QVariant AlgoritmItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Arrow *arrow : qAsConst(arrows))
            arrow->updatePosition();
    } else if (change == QGraphicsItem::ItemSelectedHasChanged) {
        if (value.toBool())
            polygonItem->setPen(QPen(Qt::red, 2));
        else
            polygonItem->setPen(QPen(Qt::black, 1));
    }
    return value;
}

// Bounding rectangle of the item
QRectF AlgoritmItem::boundingRect() const {
    return polygonItem->boundingRect();
}

// No custom painting required
void AlgoritmItem::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {
}

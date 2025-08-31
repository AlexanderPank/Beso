#include "algorithmitem.h"

#include "arrow.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QList>

// Creates algorithm item with connectors and title
AlgorithmItem::AlgorithmItem(AlgorithmType diagramType, QMenu *contextMenu, QString title,
                             QList<QPair<QString,QString>> in, QList<QPair<QString,QString>> out,
                             QGraphicsItem *parent)
    : QGraphicsItem(parent), myDiagramType(diagramType), myContextMenu(contextMenu)
{
    titleItem = new QGraphicsTextItem(title, this);
    QFont font("Roboto");
    font.setPixelSize(12);
    titleItem->setFont(font);

    for (auto &pair : in)
        m_properties.append({pair.first, pair.first, pair.second, 1});
    for (auto &pair : out)
        m_properties.append({pair.first, pair.first, pair.second, 2});

    polygonItem = new QGraphicsPolygonItem(this);
    polygonItem->setZValue(-10);
    polygonItem->setPen(QPen(Qt::black, 1));

    deleteButton = new QGraphicsRectItem(0, 0, 12, 12, this);
    deleteButton->setBrush(Qt::white);
    deleteButton->setPen(QPen(Qt::black, 1));
    auto l1 = new QGraphicsLineItem(0, 0, 12, 12, deleteButton);
    auto l2 = new QGraphicsLineItem(0, 12, 12, 0, deleteButton);
    l1->setPen(QPen(Qt::black,1));
    l2->setPen(QPen(Qt::black,1));
    deleteButton->setVisible(false);

    applyProperties();

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

// Removes arrow reference from list
void AlgorithmItem::removeArrow(Arrow *arrow) {
    arrows.removeAll(arrow);
}

// Removes and deletes all arrows connected to this item
void AlgorithmItem::removeArrows() {
    const auto arrowsCopy = arrows;
    for (Arrow *arrow : arrowsCopy) {
        static_cast<AlgorithmItem*>(arrow->startItem()->parentItem())->removeArrow(arrow);
        static_cast<AlgorithmItem*>(arrow->endItem()->parentItem())->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

// Adds arrow to internal list
void AlgorithmItem::addArrow(Arrow *arrow) {
    arrows.append(arrow);
}

// Returns pixmap representation for given type
QPixmap AlgorithmItem::image(AlgorithmType type) {
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
void AlgorithmItem::setBrush(QColor color) {
    polygonItem->setBrush(color);
}

// Returns list of output connector circles
QList<QGraphicsEllipseItem *> AlgorithmItem::getOutItems() {
    return outObjCircle.values();
}

// Returns list of input connector circles
QList<QGraphicsEllipseItem *> AlgorithmItem::getInItems() {
    return inObjCircle.values();
}

void AlgorithmItem::setProperties(const QList<PropertyInfo> &props)
{
    m_properties = props;
    applyProperties();
}

// Shows context menu for the item
void AlgorithmItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}

// Updates arrows when item moves or selection changes
QVariant AlgorithmItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Arrow *arrow : qAsConst(arrows))
            arrow->updatePosition();
    } else if (change == QGraphicsItem::ItemSelectedHasChanged) {
        if (value.toBool()) {
            polygonItem->setPen(QPen(Qt::red, 2));
            deleteButton->setVisible(true);
        } else {
            polygonItem->setPen(QPen(Qt::black, 1));
            deleteButton->setVisible(false);
        }
    }
    return value;
}

void AlgorithmItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (deleteButton && deleteButton->isVisible()) {
        QPointF p = deleteButton->mapFromItem(this, event->pos());
        if (deleteButton->boundingRect().contains(p)) {
            removeArrows();
            if (scene())
                scene()->removeItem(this);
            delete this;
            return;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

// Bounding rectangle of the item
QRectF AlgorithmItem::boundingRect() const {
    return polygonItem->boundingRect();
}

// No custom painting required
void AlgorithmItem::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {
}

void AlgorithmItem::applyProperties()
{
    // clean previous connectors
    for (auto it : inObjCircle.values()) delete it;
    for (auto it : outObjCircle.values()) delete it;
    for (auto it : inObjText.values()) delete it;
    for (auto it : outObjText.values()) delete it;
    inObjCircle.clear();
    outObjCircle.clear();
    inObjText.clear();
    outObjText.clear();

    const int spacing = 20;
    const int titleMargin = 5;

    int inTextsize = 0;
    int outTextsize = 0;
    int inCount = 0;
    int outCount = 0;

    for (const PropertyInfo &p : m_properties) {
        if (p.direction == 1) {
            auto text = new QGraphicsTextItem(p.title + " (" + p.type + ")", this);
            text->setFont(QFont("Roboto"));
            inObjText.insert({p.name,p.type}, text);
            inTextsize = qMax(inTextsize, int(text->boundingRect().width()));
            auto circ = new QGraphicsEllipseItem(0,0,10,10,this);
            circ->setData(Qt::UserRole, QString("in"));
            circ->setBrush(QBrush(Qt::green));
            inObjCircle.insert({p.name,p.type}, circ);
            inCount++;
        } else if (p.direction == 2) {
            auto text = new QGraphicsTextItem(p.title + " (" + p.type + ")", this);
            text->setFont(QFont("Roboto"));
            outObjText.insert({p.name,p.type}, text);
            outTextsize = qMax(outTextsize, int(text->boundingRect().width()));
            auto circ = new QGraphicsEllipseItem(0,0,10,10,this);
            circ->setData(Qt::UserRole, QString("out"));
            circ->setBrush(QBrush(Qt::blue));
            outObjCircle.insert({p.name,p.type}, circ);
            outCount++;
        }
    }

    int width = titleItem->boundingRect().width() + 25;
    if (width < 50)
        width = 50;
    if (width < inTextsize + outTextsize)
        width = inTextsize + outTextsize + 25;
    int h_size = qMax(inCount, outCount);

    const int topOffset = titleMargin + int(titleItem->boundingRect().height()) + 15;
    const int bottomMargin = 20;
    int height = topOffset + h_size * spacing + bottomMargin;

    QPainterPath path;
    path.addRoundedRect(-width/2.0, -height/2.0, width, height, 10, 10);
    myPolygon = path.toFillPolygon();
    polygonItem->setPolygon(myPolygon);

    titleItem->setPos(-width / 2.0 + 5, -height / 2.0 + titleMargin);
    deleteButton->setPos(width/2.0 - deleteButton->boundingRect().width(), -height/2.0);

    int i = 0;
    for (auto var : inObjCircle) {
        const qreal y = -height / 2.0 + topOffset + i * spacing;
        var->setPos(-width / 2.0 + 5, y);
        i++;
    }
    i = 0;
    for (auto var : inObjText) {
        const qreal y = -height / 2.0 + topOffset + i * spacing;
        var->setPos(-width / 2.0 + 15, y + 5 - var->boundingRect().height() / 2.0);
        i++;
    }
    i = 0;
    for (auto var : outObjCircle) {
        const qreal y = -height / 2.0 + topOffset + i * spacing;
        var->setPos(width / 2.0 - 15, y);
        i++;
    }
    i = 0;
    for (auto var : outObjText) {
        const qreal y = -height / 2.0 + topOffset + i * spacing;
        var->setPos(width / 2.0 - 15 - var->boundingRect().width(),
                    y + 5 - var->boundingRect().height() / 2.0);
        i++;
    }
}

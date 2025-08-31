#include "algorithmitem.h"

#include "arrow.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QTextDocument>
#include <QTextOption>

// Creates algorithm item with connectors and title
AlgorithmItem::AlgorithmItem(AlgorithmType diagramType, QMenu *contextMenu, QString title,
                             QList<QPair<QString,QString>> in, QList<QPair<QString,QString>> out,
                             QGraphicsItem *parent)
    : QGraphicsItem(parent), myDiagramType(diagramType), myContextMenu(contextMenu)
{
    titleItem = new QGraphicsTextItem(title, this);
    QFont font("Roboto");
    font.setPixelSize(14);
    titleItem->setFont(font);

    for (auto &pair : in)
        m_properties.append(PropertyInfo{pair.first, pair.first, pair.second, 1});
    for (auto &pair : out)
        m_properties.append(PropertyInfo{pair.first, pair.first, pair.second, 2});

    polygonItem = new QGraphicsPolygonItem(this);
    polygonItem->setZValue(-10);
    polygonItem->setPen(QPen(Qt::black, 1));
    polygonItem->setBrush(QColor("#D3D3D3"));

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

void AlgorithmItem::setObjectOutput(bool enable)
{
    m_hasSelfOut = enable;
    applyProperties();
}

QString AlgorithmItem::propertyNameForCircle(QGraphicsEllipseItem *circle) const
{
    for (auto it = inObjCircle.constBegin(); it != inObjCircle.constEnd(); ++it) {
        if (it.value() == circle)
            return it.key().first;
    }
    for (auto it = outObjCircle.constBegin(); it != outObjCircle.constEnd(); ++it) {
        if (it.value() == circle)
            return it.key().first;
    }
    if (m_hasSelfOut && selfOut == circle)
        return QString();
    return QString();
}

QGraphicsEllipseItem* AlgorithmItem::circleForProperty(const QString &name, int direction) const
{
    if (direction == 1) {
        for (auto it = inObjCircle.constBegin(); it != inObjCircle.constEnd(); ++it) {
            if (it.key().first == name)
                return it.value();
        }
    } else if (direction == 2) {
        for (auto it = outObjCircle.constBegin(); it != outObjCircle.constEnd(); ++it) {
            if (it.key().first == name)
                return it.value();
        }
        if (m_hasSelfOut && name.isEmpty())
            return selfOut;
    }
    return nullptr;
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
        } else {
            polygonItem->setPen(QPen(Qt::black, 1));
        }
    }
    return value;
}

void AlgorithmItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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
    struct ArrowInfo {
        Arrow *arrow;
        QString property;
        bool isStart; // true if this item is arrow start
    };
    QList<ArrowInfo> infos;
    for (Arrow *arrow : qAsConst(arrows)) {
        if (arrow->startItem()->parentItem() == this) {
            infos.append({arrow, propertyNameForCircle(arrow->startItem()), true});
        } else if (arrow->endItem()->parentItem() == this) {
            infos.append({arrow, propertyNameForCircle(arrow->endItem()), false});
        }
    }

    for (auto it : inObjCircle.values()) delete it;
    for (auto it : outObjCircle.values()) delete it;
    for (auto it : inObjText.values()) delete it;
    for (auto it : outObjText.values()) delete it;
    inObjCircle.clear();
    outObjCircle.clear();
    inObjText.clear();
    outObjText.clear();
    selfOut = nullptr;

    const int spacing = 20;
    const int titleMargin = 5;

    int inTextsize = 0;
    int outTextsize = 0;
    int inCount = 0;
    int outCount = 0;

    for (const PropertyInfo &p : m_properties) {
        if (p.direction == 1) {
            auto text = new QGraphicsTextItem(p.title + " (" + p.type + ")", this);
            text->setFont(QFont("Roboto", 11));
            inObjText.insert({p.name,p.type}, text);
            inTextsize = qMax(inTextsize, int(text->boundingRect().width()));
            auto circ = new QGraphicsEllipseItem(0,0,12,12,this);
            circ->setData(Qt::UserRole, QString("in"));
            circ->setBrush(QBrush(Qt::green));
            inObjCircle.insert({p.name,p.type}, circ);
            inCount++;
        } else if (p.direction == 2) {
            auto text = new QGraphicsTextItem("    " + p.title + " (" + p.type + ")", this);
            text->setFont(QFont("Roboto", 11));
            outObjText.insert({p.name,p.type}, text);
            outTextsize = qMax(outTextsize, int(text->boundingRect().width()));
            auto circ = new QGraphicsEllipseItem(0,0,12,12,this);
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
    if (inCount > 0 || outCount > 0)
        path.addRoundedRect(-width/2.0, -height/2.0, width, height, 4, 4);
    else
        path.addRect(-width/2.0, -height/2.0, width, height);
    myPolygon = path.toFillPolygon();
    polygonItem->setPolygon(myPolygon);

    titleItem->setTextWidth(width - 10);
    QTextOption opt = titleItem->document()->defaultTextOption();
    opt.setAlignment(Qt::AlignCenter);
    titleItem->document()->setDefaultTextOption(opt);
    titleItem->setPos(-width / 2.0 + 5, -height / 2.0 + titleMargin);

    int i = 0;
    for (auto var : inObjCircle) {
        const qreal y = -height / 2.0 + topOffset + i * spacing;
        var->setPos(-width / 2.0 + 5, y);
        i++;
    }
    i = 0;
    for (auto var : inObjText) {
        const qreal y = -height / 2.0 + topOffset + i * spacing;
        var->setPos(-width / 2.0 + 20, y + 5 - var->boundingRect().height() / 2.0);
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
        var->setPos(width / 2.0 - 20 - var->boundingRect().width(),
                    y + 5 - var->boundingRect().height() / 2.0);
        i++;
    }

    if (m_hasSelfOut) {
        selfOut = new QGraphicsEllipseItem(0,0,12,12,this);
        selfOut->setData(Qt::UserRole, QString("out"));
        selfOut->setBrush(QBrush(Qt::blue));
        qreal y = -height / 2.0 + titleMargin + titleItem->boundingRect().height() / 2.0 - 6;
        selfOut->setPos(width / 2.0 - 15, y);
        outObjCircle.insert({QString(), QString()}, selfOut);
    }

    for (const ArrowInfo &info : infos) {
        QGraphicsEllipseItem *circle = circleForProperty(info.property, info.isStart ? 2 : 1);
        if (circle) {
            if (info.isStart)
                info.arrow->setStartItem(circle);
            else
                info.arrow->setEndItem(circle);
            info.arrow->updatePosition();
        } else {
            static_cast<AlgorithmItem*>(info.arrow->startItem()->parentItem())->removeArrow(info.arrow);
            static_cast<AlgorithmItem*>(info.arrow->endItem()->parentItem())->removeArrow(info.arrow);
            if (scene()) scene()->removeItem(info.arrow);
            delete info.arrow;
        }
    }
}

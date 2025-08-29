#ifndef ALGORITMITEM_H
#define ALGORITMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;

class AlgoritmItem : public QGraphicsItem//QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 15 };
    enum AlgoritmType { ALGORITM, CONDITION, EVENT,PARAM };

    AlgoritmItem(AlgoritmType diagramType, QMenu *contextMenu, QString title="", QList<QPair<QString,QString>> in ={}, QList<QPair<QString,QString>> out ={}, QGraphicsItem *parent = nullptr);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    AlgoritmType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    QPixmap image(AlgoritmType type);
    int type() const override { return Type; }
    void setBrush(QColor);

    QList<QGraphicsEllipseItem *> getOutItems();
    QList<QGraphicsEllipseItem *> getInItems();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    AlgoritmType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    QList<Arrow *> arrows;

    QGraphicsPolygonItem *polygonItem;
    QGraphicsTextItem *titleItem;

//    QGraphicsEllipseItem *inObj;
//    QGraphicsEllipseItem *outObj;

    QMap<QPair<QString,QString>,QGraphicsEllipseItem *> inObjCircle;
    QMap<QPair<QString,QString>,QGraphicsEllipseItem *> outObjCircle;
    QMap<QPair<QString,QString>,QGraphicsTextItem *> inObjText;
    QMap<QPair<QString,QString>,QGraphicsTextItem *> outObjText;
    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


#endif // ALGORITMITEM_H

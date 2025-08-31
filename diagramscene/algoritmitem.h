#ifndef ALGORITMITEM_H
#define ALGORITMITEM_H

#include <QGraphicsItem>
#include <QMap>
#include <QPair>

class QMenu;
class QGraphicsPolygonItem;
class QGraphicsTextItem;
class QGraphicsEllipseItem;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Arrow;

// Visual block representing an algorithm with input/output connectors
class AlgoritmItem : public QGraphicsItem {
public:
    enum { Type = UserType + 15 };
    enum AlgoritmType { ALGORITM, CONDITION, EVENT, PARAM };

    // Constructs item with given type, context menu and connector lists
    AlgoritmItem(AlgoritmType diagramType, QMenu *contextMenu, QString title = "",
                 QList<QPair<QString,QString>> in = {},
                 QList<QPair<QString,QString>> out = {},
                 QGraphicsItem *parent = nullptr);

    // Removes a single arrow from this item
    void removeArrow(Arrow *arrow);
    // Deletes all arrows connected to the item
    void removeArrows();
    // Returns type of algorithm
    AlgoritmType diagramType() const { return myDiagramType; }
    // Current polygon representing the item
    QPolygonF polygon() const { return myPolygon; }
    // Adds arrow to internal list
    void addArrow(Arrow *arrow);
    // Creates pixmap preview for specified type
    QPixmap image(AlgoritmType type);
    // QGraphicsItem type id
    int type() const override { return Type; }
    // Sets background brush color
    void setBrush(QColor);

    // Lists of connector circles
    QList<QGraphicsEllipseItem *> getOutItems();
    QList<QGraphicsEllipseItem *> getInItems();

protected:
    // Displays context menu
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    // Handles geometry changes to update arrows
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    AlgoritmType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu{nullptr};
    QList<Arrow *> arrows;

    QGraphicsPolygonItem *polygonItem{nullptr};
    QGraphicsTextItem *titleItem{nullptr};
    QMap<QPair<QString,QString>,QGraphicsEllipseItem *> inObjCircle;
    QMap<QPair<QString,QString>,QGraphicsEllipseItem *> outObjCircle;
    QMap<QPair<QString,QString>,QGraphicsTextItem *> inObjText;
    QMap<QPair<QString,QString>,QGraphicsTextItem *> outObjText;

public:
    // Bounding rectangle of item
    QRectF boundingRect() const override;
    // Custom painting (currently unused)
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // ALGORITMITEM_H

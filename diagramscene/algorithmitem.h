#ifndef ALGORITHMITEM_H
#define ALGORITHMITEM_H

#include <QGraphicsItem>
#include <QMap>
#include <QPair>
#include <QList>

class QMenu;
class QGraphicsPolygonItem;
class QGraphicsTextItem;
class QGraphicsEllipseItem;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsRectItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Arrow;

// Visual block representing an algorithm with input/output connectors
class AlgorithmItem : public QGraphicsItem {
public:
    enum { Type = UserType + 15 };
    enum AlgorithmType { ALGORITM, CONDITION, EVENT, PARAM };

    // Constructs item with given type, context menu and connector lists
    AlgorithmItem(AlgorithmType diagramType, QMenu *contextMenu, QString title = "",
                  QList<QPair<QString,QString>> in = {},
                  QList<QPair<QString,QString>> out = {},
                  QGraphicsItem *parent = nullptr);

    // Removes a single arrow from this item
    void removeArrow(Arrow *arrow);
    // Deletes all arrows connected to the item
    void removeArrows();
    // Returns type of algorithm
    AlgorithmType diagramType() const { return myDiagramType; }
    // Current polygon representing the item
    QPolygonF polygon() const { return myPolygon; }
    // Adds arrow to internal list
    void addArrow(Arrow *arrow);
    // Creates pixmap preview for specified type
    QPixmap image(AlgorithmType type);
    // QGraphicsItem type id
    int type() const override { return Type; }
    // Sets background brush color
    void setBrush(QColor);

    // Returns output connector circles
    QList<QGraphicsEllipseItem *> getOutItems();
    // Returns input connector circles
    QList<QGraphicsEllipseItem *> getInItems();

    struct PropertyInfo {
        QString title;
        QString name;
        QString type;
        int direction = 0; // 0-none,1-in,2-out
        PropertyInfo() {title = ""; name = ""; type = ""; direction = 0;};
        PropertyInfo(QString t, QString n, QString tp, int d) {
            title = t; name = n; type = tp; direction = d;
        };
    };

    QList<PropertyInfo> properties() const { return m_properties; }
    void setProperties(const QList<PropertyInfo> &props);

protected:
    // Displays context menu
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    // Handles geometry changes to update arrows
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    AlgorithmType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu{nullptr};
    QList<Arrow *> arrows;

    QGraphicsPolygonItem *polygonItem{nullptr};
    QGraphicsTextItem *titleItem{nullptr};
    QGraphicsRectItem *deleteButton{nullptr};
    QMap<QPair<QString,QString>,QGraphicsEllipseItem *> inObjCircle;
    QMap<QPair<QString,QString>,QGraphicsEllipseItem *> outObjCircle;
    QMap<QPair<QString,QString>,QGraphicsTextItem *> inObjText;
    QMap<QPair<QString,QString>,QGraphicsTextItem *> outObjText;
    QList<PropertyInfo> m_properties;

    void applyProperties();

public:
    // Bounding rectangle of item
    QRectF boundingRect() const override;
    // Custom painting (currently unused)
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // ALGORITHMITEM_H

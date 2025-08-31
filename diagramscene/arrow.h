#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>
#include <QColor>

class QGraphicsEllipseItem;

// Arrow represents a polyline connection between two circle handles
class Arrow : public QGraphicsLineItem {
public:
    enum { Type = UserType + 4 };

    // Constructs an arrow between start and end items
    Arrow(QGraphicsEllipseItem *startItem, QGraphicsEllipseItem *endItem,
          QGraphicsItem *parent = nullptr);

    // Reimplemented from QGraphicsItem to return custom type id
    int type() const override { return Type; }
    // Returns bounding rectangle of the arrow
    QRectF boundingRect() const override;
    // Precise shape for collision detection
    QPainterPath shape() const override;
    // Changes arrow color
    void setColor(const QColor &color) { myColor = color; }
    // Access start/end items
    QGraphicsEllipseItem *startItem() { return m_startItem; }
    QGraphicsEllipseItem *endItem() { return m_endItem; }
    // Updates line geometry according to attached items
    void updatePosition();

protected:
    // Draws the arrow on the scene
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    QGraphicsEllipseItem *m_startItem{nullptr};
    QGraphicsEllipseItem *m_endItem{nullptr};
    QPolygonF arrowHead;
    QColor myColor = Qt::white;
};

#endif // ARROW_H

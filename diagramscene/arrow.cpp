#include "arrow.h"

#include <QPainter>
#include <QPen>
#include <QtMath>

// Constructs arrow connecting two QGraphicsEllipseItem endpoints
Arrow::Arrow(QGraphicsEllipseItem *startItem, QGraphicsEllipseItem *endItem,
             QGraphicsItem *parent) : QGraphicsLineItem(parent),
    m_startItem(startItem), m_endItem(endItem) {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

// Returns bounding rectangle for the arrow including arrowhead
QRectF Arrow::boundingRect() const {
    qreal extra = (pen().width() + 20) / 2.0;
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

// Provides shape used for collisions (line plus arrow head)
QPainterPath Arrow::shape() const {
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

// Updates geometry based on start/end item positions
void Arrow::updatePosition() {
    if (!m_startItem || !m_endItem)
        return;
    QLineF newLine(mapFromItem(m_startItem, 0, 0),
                   mapFromItem(m_endItem, 0, 0));
    setLine(newLine);
}

// Paints polyline arrow with two bends and arrow head
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *) {
    if (!m_startItem || !m_endItem || m_startItem->collidesWithItem(m_endItem))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    painter->setPen(myPen);
    painter->setBrush(myColor);

    const qreal offset = 15.0;
    QPointF startPoint = m_startItem->pos() + m_startItem->parentItem()->pos() + QPointF(5,5);
    QPointF endPoint = m_endItem->pos() + m_endItem->parentItem()->pos() + QPointF(5,5);

    qreal startDir = (m_startItem->pos().x() < 0) ? -1 : 1;
    qreal endDir = (m_endItem->pos().x() < 0) ? -1 : 1;

    QPointF p1 = startPoint + QPointF(startDir * offset, 0);
    QPointF p3 = endPoint + QPointF(endDir * offset, 0);
    QPointF p2(p1.x(), p3.y());

    QPolygonF polyline;
    polyline << startPoint << p1 << p2 << p3 << endPoint;
    painter->drawPolyline(polyline);

    QLineF arrowLine(p3, endPoint);
    double angle = std::atan2(-arrowLine.dy(), arrowLine.dx());
    QPointF arrowP1 = endPoint - QPointF(std::sin(angle + M_PI / 2.5) * 10,
                                         std::cos(angle + M_PI / 2.5) * 10);
    QPointF arrowP2 = endPoint - QPointF(std::sin(angle + M_PI - M_PI / 2.5) * 10,
                                         std::cos(angle + M_PI - M_PI / 2.5) * 10);
    arrowHead.clear();
    arrowHead << endPoint << arrowP1 << arrowP2;
    painter->drawPolygon(arrowHead);

    if (isSelected()) {
        painter->setPen(QPen(myColor, 1, Qt::DashLine));
        QLineF myLine(startPoint, p1);
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0, -8.0);
        painter->drawLine(myLine);
    }
}

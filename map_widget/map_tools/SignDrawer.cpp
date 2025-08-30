// signdrawer.cpp
#include "SignDrawer.h"
#include "CoordCtx.h"
#include "../../core/QLogStream.h"
#include "../../signs/SignController.h"
#include <QPainter>
#include <QDebug>
#include <QRectF>
#include <QtMath>
#include <QFontMetrics>

// Helper to compute a point outside of the sign's bounding box.
static QPointF outsideAnchor(const SignBase* sign)
{
    if (!sign)
        return {};

    auto coords = sign->getCoordinatesInRadians();
    if (coords.isEmpty())
        return {};

    // Calculate bounding box of the sign geometry
    QRectF bounds(coords.first(), QSizeF(0, 0));
    for (const QPointF &pt : coords) {
        if (pt.x() < bounds.left())
            bounds.setLeft(pt.x());
        if (pt.x() > bounds.right())
            bounds.setRight(pt.x());
        if (pt.y() < bounds.top())
            bounds.setTop(pt.y());
        if (pt.y() > bounds.bottom())
            bounds.setBottom(pt.y());
    }

    QPointF center = bounds.center();
    double offset = qMax(bounds.width(), bounds.height()) * 0.1; // simple outward offset
    return {bounds.right() + offset, center.y()};
}

SignDrawer* SignDrawer::m_instance = nullptr;

SignDrawer::SignDrawer(HMAP hMap, QObject *parent)
        : QObject(parent), m_hMap(hMap), m_state(Idle)
{
    m_instance = this;
}

SignDrawer::~SignDrawer()
{
    if (m_instance == this)
        m_instance = nullptr;
}


SignDrawer* SignDrawer::instance()
{
    if (!m_instance) {
        qCritical() << "SignDrawer инстанс не создан !";
    }
    return m_instance;
}

void SignDrawer::startDrawing(SignBase *sign)
{
    if (!sign) return;
    if (m_state != Idle) {
        qWarning() << "Already drawing a sign";
        return;
    }
    m_drawing_sign = sign;
    m_signType = sign->getGeometryType();
    m_state = DrawingFirstPoint;
    m_points.clear();
    m_currentMousePos = QPointF();
}

void SignDrawer::startDrawing(SignBase::SignType signType)
{
    if (m_state != Idle) {
        qWarning() << "Already drawing a sign";
        return;
    }

    m_signType = signType;
    m_state = DrawingFirstPoint;
    m_points.clear();
    m_currentMousePos = QPointF();
}

void SignDrawer::cancelDrawing()
{
    if (m_state != Idle) {
        clear();
        emit drawingCanceled();
    }
}

void SignDrawer::handleMouseClick(QPointF coordsPic)
{
    if (m_state == Idle) return;
    m_points.append(coordsPic);
    if (m_2PointSigns.contains(m_signType) && m_points.size() >= 2) completeDrawing();
}

void SignDrawer::handleMouseMove(QPointF coordsPic)
{
    if (m_state == Idle || m_state == Complete) return;

    m_currentMousePos = coordsPic;
    if (m_points.size() == 0) return;
    QVector<QPointF> pointsToSend = m_points;
    pointsToSend.push_back(coordsPic);
    pointsToSend = GeoUtil::convertPointsPicToRadian(m_hMap, pointsToSend);
    emit signDrawing(pointsToSend, GeoUtil::distance(pointsToSend));

}

void SignDrawer::handleMouseDoubleClick()
{
    // Проверяем минимальное количество точек
    if (((m_signType == SignBase::LOCAL_LINE) && m_points.size() >= 2) ||
        (m_signType == SignBase::LOCAL_POLYGON && m_points.size() >= 3)) {
        completeDrawing();
    } else {
        qWarning() << "Not enough points for this sign type";
    }
}

void SignDrawer::handlePaint(QPainter *p, int cx, int cy, int cw, int ch)
{
    if (m_state == Idle) return;

    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);
    // Рисуем линии  и точки фигуры
    for (int i = 0; i < m_points.size(); ++i) {
        // Рисуем линии между добавленными точками
        if (i>0 ) {
            p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
            p->drawLine(m_points[i-1] - QPoint(cx, cy), m_points[i] - QPoint(cx, cy));
        }
        p->setPen(QPen(Qt::gray, 1));
        p->setBrush(Qt::white);
        p->drawEllipse(m_points[i] - QPoint(cx, cy), 5, 5);
    }

    // дорисовываем линию для слежения за курсором
    if (m_points.size() > 0 && m_state != Complete) {
        // qLog() << m_currentMousePos.x() << " " << m_currentMousePos.y();
        p->setPen(QPen(Qt::green, 2, Qt::DotLine));
        p->drawLine(m_points.last() - QPoint(cx, cy), m_currentMousePos - QPoint(cx, cy));
    }
    p->restore();
}

void SignDrawer::drawNameLabels(QPainter *p, int cx, int cy)
{
    auto controller = SignController::getInstance();
    if (!controller)
        return;

    auto signs = controller->allSigns();
    p->save();
    QVector<QRect> placedRects; // to track occupied label areas
    QFontMetrics fm(p->font());

    for (auto it = signs.begin(); it != signs.end(); ++it) {
        SignBase *sign = it.value();
        if (!sign || !sign->getVisibility())
            continue;

        auto pts = sign->getCoordinatesInRadians();
        if (pts.isEmpty())
            continue;

        QPointF anchorGeo;
        switch (sign->getGeometryType()) {
        case SignBase::LOCAL_POINT:
            anchorGeo = pts.first();
            break;
        case SignBase::LOCAL_LINE:
            if (pts.size() == 2)
                anchorGeo = pts.last();
            else
                anchorGeo = pts.at(pts.size() / 2);
            break;
        case SignBase::LOCAL_POLYGON:
        case SignBase::LOCAL_CIRCLE:
        case SignBase::LOCAL_RECTANGLE:
            anchorGeo = pts.last();
            break;
        case SignBase::LOCAL_SQUARE:
            anchorGeo = pts.first();
            break;
        case SignBase::LOCAL_TITLE:
            continue;
        default:
            anchorGeo = pts.first();
        }

        CoordCtx ctx(m_hMap, GEO, anchorGeo);
        QPoint anchor = ctx.pic() - QPoint(cx, cy);

        QString text = sign->getName();
        if (text.isEmpty())
            continue;

        QRect textRect = fm.boundingRect(text).adjusted(-2, -2, 2, 2); // add margin

        // Attempt to place label without overlapping previous ones
        const int radius = 20;          // distance from anchor
        const int stepDeg = 15;         // rotation step
        double angle = -45.0;           // start top-right
        QPoint labelCenter;

        for (int attempt = 0; attempt < 360 / stepDeg; ++attempt) {
            double rad = qDegreesToRadians(angle);
            labelCenter = anchor + QPoint(int(radius * std::cos(rad)), int(radius * std::sin(rad)));

            QRect candidate = textRect;
            candidate.moveCenter(labelCenter);

            bool overlap = false;
            for (const QRect &r : placedRects) {
                if (r.intersects(candidate)) {
                    overlap = true;
                    break;
                }
            }

            if (!overlap) {
                textRect = candidate;
                break;
            }

            angle -= stepDeg; // move clockwise
        }

        placedRects.append(textRect);
        p->setPen(QPen(Qt::black, 1));
        p->drawLine(anchor, textRect.center());
        p->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, text);
    }

    p->restore();
}

QVector<QPoint> SignDrawer::toPixPoints(const QVector<QPointF>& geoPoints, int cx, int cy) const
{
    QVector<QPoint> points;
    for (const QPointF& geoPt : geoPoints) {
        CoordCtx ctx(m_hMap, GEO, geoPt);
        points.append(ctx.pic() - QPoint(cx, cy));
    }
    return points;
}

void SignDrawer::completeDrawing()
{
    if (m_state == Idle) return;
    if (m_points.isEmpty()) {
        qWarning() << "No points to complete drawing";
        return;
    }
    // Для точечного знака гарантируем 2 точки
    if ( m_2PointSigns.contains(m_signType) &&   m_points.size() == 1) {
        m_points.append(m_points[0] + QPointF(1, 1));
    }
    if (m_state != Complete) {
        auto points = GeoUtil::convertPointsPicToRadian(m_hMap, m_points);
        if (m_signType == SignBase::LOCAL_TITLE && m_drawing_sign) {
            // Adjust anchor for callout labels so that the text stays outside
            QPointF anchor = calloutAnchor(m_drawing_sign);
            if (!points.isEmpty()) {
                if (points.size() == 1)
                    points.prepend(anchor);
                else
                    points[0] = anchor;
            }
        }
        if (m_drawing_sign) {
            m_drawing_sign->setCoordinatesInRadians(points.toList());
            emit signDrawEnd();
        }
        else
            emit signDrawn(m_signType, points);
    }
    clear();
}

void SignDrawer::clear()
{
    m_state = Idle;
    m_points.clear();
    m_currentMousePos = QPointF();
}

QPointF SignDrawer::calloutAnchor(const SignBase* sign) const
{
    if (!sign)
        return {};

    switch (sign->getGeometryType()) {
    case SignBase::LOCAL_POLYGON:
    case SignBase::LOCAL_CIRCLE:
    case SignBase::LOCAL_RECTANGLE:
    case SignBase::LOCAL_SQUARE:
        return outsideAnchor(sign);
    default:
        {
            auto coords = sign->getCoordinatesInRadians();
            if (coords.isEmpty())
                return {};
            return coords.first();
        }
    }
}
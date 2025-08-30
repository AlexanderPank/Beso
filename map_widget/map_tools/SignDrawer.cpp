// signdrawer.cpp
#include "SignDrawer.h"
#include "CoordCtx.h"
#include "../../core/QLogStream.h"
#include "../../signs/SignController.h"
#include <QPainter>
#include <QDebug>
#include <QRectF>
#include <QtMath>
#include <QRectF>

// Вспомогательная функция, вычисляющая точку вне прямоугольника, описывающего знак.
// Используется для смещения подписи за пределы фигуры, чтобы она не перекрывала геометрию.
static QPointF outsideAnchor(const SignBase* sign)
{
    // Если знак отсутствует – смещать нечего
    if (!sign)
        return {};

    // Получаем координаты фигуры в радианах
    auto coords = sign->getCoordinatesInRadians();
    if (coords.isEmpty())
        return {};

    // Определяем минимальный прямоугольник, охватывающий все точки знака
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

    // Смещаем подпись вправо от фигуры на 10% её размера
    QPointF center = bounds.center();
    double offset = qMax(bounds.width(), bounds.height()) * 0.1;
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

// Запуск рисования существующего знака.
// Подготавливает внутреннее состояние к вводу новой геометрии.
void SignDrawer::startDrawing(SignBase *sign)
{
    if (!sign) return;                        // Без исходного объекта работать нечему
    if (m_state != Idle) {                     // Не допускаем параллельного рисования
        qWarning() << "Already drawing a sign";
        return;
    }
    m_drawing_sign = sign;                     // Запоминаем редактируемый знак
    m_signType = sign->getGeometryType();      // Тип геометрии определяет правила ввода
    m_state = DrawingFirstPoint;               // Переходим в состояние ожидания первой точки
    m_points.clear();                          // Очищаем ранее введённые координаты
    m_currentMousePos = QPointF();             // Сбрасываем позицию курсора
}

// Запуск рисования нового знака по его типу.
void SignDrawer::startDrawing(SignBase::SignType signType)
{
    if (m_state != Idle) {
        qWarning() << "Already drawing a sign";
        return;
    }

    m_signType = signType;                     // Сохраняем выбранный тип
    m_state = DrawingFirstPoint;               // Ожидаем ввод первой точки
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

// Отрисовывает подписи для всех видимых знаков на карте.
// Алгоритм: для каждого знака выбирается опорная точка и подпись смещается
// относительно неё, после чего между точкой и текстом рисуется соединительная линия.
void SignDrawer::drawNameLabels(QPainter *p, int cx, int cy)
{
    auto controller = SignController::getInstance();
    if (!controller)
        return;

    auto signs = controller->allSigns();
    p->save();
    for (auto it = signs.begin(); it != signs.end(); ++it) {
        SignBase *sign = it.value();
        if (!sign || !sign->getVisibility())
            continue;
        auto pts = sign->getCoordinatesInRadians();
        if (pts.isEmpty())
            continue;

        // Определяем географическую точку, около которой будет размещена подпись
        QPointF anchorGeo;
        switch (sign->getGeometryType()) {
        case SignBase::LOCAL_POINT:
            anchorGeo = pts.first();
            break;
        case SignBase::LOCAL_LINE:
            // Для линии выбираем конец или середину в зависимости от количества точек
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
            continue; // Заголовок сам является подписью
        default:
            anchorGeo = pts.first();
        }

        // Переводим географическую точку в координаты окна и смещаем текст
        CoordCtx ctx(m_hMap, GEO, anchorGeo);
        QPoint anchor = ctx.pic() - QPoint(cx, cy);
        QPoint textPos = anchor + QPoint(10, -10); // Смещение подписи от точки

        // Соединяем точку и текст и выводим название знака
        p->setPen(QPen(Qt::black, 1));
        p->drawLine(anchor, textPos);
        p->drawText(textPos + QPoint(2, -2), sign->getName());
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

// Завершает ввод знака:
// 1) преобразует точки в географический формат;
// 2) при необходимости добавляет якорь для подписи;
// 3) отправляет сигнал о создании и сбрасывает состояние.
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
        // Для подписи типа LOCAL_TITLE первая точка хранит позицию текста,
        // поэтому вычисляем дополнительный якорь вне фигуры, чтобы линия
        // указывала изнутри наружу
        if (m_signType == SignBase::LOCAL_TITLE && m_drawing_sign) {
            QPointF anchor = calloutAnchor(m_drawing_sign);
            if (!points.isEmpty()) {
                if (points.size() == 1)
                    points.prepend(anchor); // Если была одна точка, добавляем якорь
                else
                    points[0] = anchor;      // Иначе заменяем существующий
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

// Возвращает точку, из которой будет выходить линия подписи (якорь).
// Для сложных фигур якорь смещается наружу, для остальных используется первая точка.
QPointF SignDrawer::calloutAnchor(const SignBase* sign) const
{
    if (!sign)
        return {};

    switch (sign->getGeometryType()) {
    case SignBase::LOCAL_POLYGON:
    case SignBase::LOCAL_CIRCLE:
    case SignBase::LOCAL_RECTANGLE:
    case SignBase::LOCAL_SQUARE:
        return outsideAnchor(sign);            // смещение подписи за пределы фигуры
    default:
        {
            auto coords = sign->getCoordinatesInRadians();
            if (coords.isEmpty())
                return {};
            return coords.first();             // для простых знаков используем первую точку
        }
    }
}

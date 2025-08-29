// signeditor.cpp
#include "SignEditor.h"
#include "../../signs/SignController.h"
#include "CoordCtx.h"
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>

SignEditor::SignEditor(QObject *parent) : QObject(parent)
{
    m_signController = SignController::getInstance();
    if (m_signController == nullptr) {
        QMessageBox::critical(nullptr, "Критическая ошибка", "Не удалось инициализировать контроллер знаков. Приложение будет закрыто.");
        QTimer::singleShot(0, qApp, &QApplication::quit);
    }
}



void SignEditor::setCurrentSign(int signId, SignBase::SignType type)
{
    m_selectedSignID = signId;
    m_signType = type;
}

void SignEditor::clearSelection()
{
    m_selectedSignID = -1;
    m_editPoints.clear();
    m_editMode = EditMode::None;
}

void SignEditor::showEditPoints(int mode)
{
    m_editPoints.clear();

    if (m_editMode == mode) {
        m_editMode = EditMode::Select;
        return;
    }

    if (m_selectedSignID == -1) return;

    // Получаем все точки объекта
    m_editPoints = m_signController->getSignByID(m_selectedSignID)->getCoordinatesInRadians().toVector();

    // Для LOCAL_POINT добавляем симметричную точку
    if (m_2PointTypes.contains(m_signType) && m_editPoints.size() >= 2) {
        m_centerPoint = m_editPoints[0];
    }

    // Добавляем точки для масштабирования (углы прямоугольника)
    updateBoundingRect();
    // Добавляем центральную точку для вращения
    if ( !m_2PointTypes.contains(m_signType) )  {
        m_centerPoint = m_boundingRect.center();
    }
    m_editMode = mode;
}

void SignEditor::updateBoundingRect()
{
    if (m_editPoints.isEmpty()) {
        m_boundingRect = QRectF();
        return;
    }

    m_boundingRect = QRectF(m_editPoints.first(), QSizeF(0, 0));

    for (const QPointF& pt : m_editPoints) {
        if (pt.x() < m_boundingRect.left()) m_boundingRect.setLeft(pt.x());
        if (pt.x() > m_boundingRect.right()) m_boundingRect.setRight(pt.x());
        if (pt.y() < m_boundingRect.top()) m_boundingRect.setTop(pt.y());
        if (pt.y() > m_boundingRect.bottom()) m_boundingRect.setBottom(pt.y());
    }
}

void SignEditor::onPaint(QPainter *p, int cx, int cy)
{
    if (hasSelection()) {
        p->save();
        if (m_editMode == EditMode::Select) {
            p->setPen(Qt::red);
            p->setBrush(Qt::NoBrush);
            p->drawEllipse(m_centerPoint - QPoint(cx, cy), 5, 5);
        }
        p->restore();
    }
    if (m_editMode == EditMode::None || !hasSelection() || m_editPoints.isEmpty()) return;

    p->save();
    QPen pen(Qt::black, 1);
    p->setPen(pen);

    pen.setColor(Qt::black);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    if (m_2PointTypes.contains(m_signType) ) {
        QLineF line;
        line.setP1(CoordCtx(m_signController->mapHandle(), GEO, m_editPoints.first()).pic() - QPoint(cx, cy));
        line.setP2(CoordCtx(m_signController->mapHandle(), GEO, m_editPoints.last()).pic() - QPoint(cx, cy));
        p->drawLine(line);
    }
    else {
        // Рисуем ограничивающий прямоугольник
        QVector<QPoint> rectPoints;
        rectPoints << CoordCtx(m_signController->mapHandle(), GEO, m_boundingRect.topLeft()).pic() - QPoint(cx, cy)
                   << CoordCtx(m_signController->mapHandle(), GEO, m_boundingRect.topRight()).pic() - QPoint(cx, cy)
                   << CoordCtx(m_signController->mapHandle(), GEO, m_boundingRect.bottomRight()).pic() - QPoint(cx, cy)
                   << CoordCtx(m_signController->mapHandle(), GEO, m_boundingRect.bottomLeft()).pic() - QPoint(cx, cy);
        p->drawPolygon(rectPoints);
    }

    auto points = m_editPoints;
    points.append(m_centerPoint);
    // Рисуем точки объекта
    for (int i = 0; i < points.size(); ++i) {
        CoordCtx ctx(m_signController->mapHandle(), GEO, points[i]);
        QPoint screenPt = ctx.pic() - QPoint(cx, cy);

        // Текущая выделенная точка
        if (i == m_selectedPointIndex) {
            p->setBrush(Qt::yellow);
        } else {
            p->setBrush(Qt::white);
        }

        p->drawEllipse(screenPt, 5, 5);
    }

    p->restore();

    // Для режима ResizeObject рисуем дополнительные элементы
    if (m_editMode == EditMode::ScaleObject) {
        // Рисуем линии от центра к угловым точкам
        CoordCtx centerCtx(m_signController->mapHandle(), GEO, m_centerPoint);
        QPointF centerPt =  centerCtx.pic() - QPoint(cx, cy);

        for (int i = 0; i < m_editPoints.size() ; ++i) {
            CoordCtx ptCtx(m_signController->mapHandle(), GEO, m_editPoints[i]);
            QPoint pt = ptCtx.pic() - QPoint(cx, cy);
            pen.setColor(Qt::gray);
            p->setPen(pen);
            p->drawLine(centerPt, pt);
        }
    }
}

bool SignEditor::handleMousePress(const QPoint &clickPos)
{
    m_prevPressedPos = clickPos;
    // Получаем текущее состояние модификаторов
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

    if (m_editMode == EditMode::Select && (modifiers & Qt::ControlModifier)) {
        return true;
    }

    if (m_editMode == EditMode::RotateObject || m_editMode == EditMode::ScaleObject) {
        // Проверяем клик по точкам редактирования
        for (int i = 0; i < m_editPoints.size(); ++i) {
            CoordCtx ctx(m_signController->mapHandle(), GEO, m_editPoints[i]);
            QPoint pt = ctx.pic();
            auto ln = QLineF(pt, clickPos).length();
            if (ln < 10) {
                m_selectedPointIndex = i;
                m_dragEditPoint = clickPos;
                qDebug() << "Клик по точке " << m_selectedPointIndex;
                return true;
            }
        }
        return false;
    }

    CoordCtx coord(m_signController->mapHandle(), PIC, clickPos);
    auto singID = m_signController->findNearestSignID(coord.geo(), m_currentZoom);

    if (m_editMode == EditMode::Select && singID != m_selectedSignID){
        m_signController->clearHighlight();
        m_editMode = EditMode::None;
        m_selectedSignID = -1;
        if (singID == -1) return true;
    }

    if (singID != -1) {
        m_selectedSignID = singID;
        // Определяем тип знака
        m_signType = m_signController->getSignByID(m_selectedSignID)->getGeometryType();
        m_signController->highlightSignByID(m_selectedSignID);
        m_editMode = EditMode::Select;
        return true;
    }

    return false;
}

bool SignEditor::handleMouseMove(const QPoint &currentPos, const bool isCtrl)
{

    if (m_editMode == EditMode::Select && isCtrl) {
        CoordCtx ctxCurrent(m_signController->mapHandle(), PIC, currentPos);
        CoordCtx ctxPrev(m_signController->mapHandle(), PIC, m_prevPressedPos);
        auto dx = ctxCurrent.geo().x() - ctxPrev.geo().x();
        auto dy = ctxCurrent.geo().y() - ctxPrev.geo().y();
        if (  dx  != 0  ||  dy  != 0) {
            m_signController->DragCurrentObject(dx, dy, m_selectedSignID);
            m_prevPressedPos = currentPos;
            return true;
        }

    } else
    if (m_editMode == EditMode::ScaleObject && m_selectedPointIndex != -1) {
        CoordCtx ctx(m_signController->mapHandle(), PIC, currentPos);
        QPointF newPos = ctx.geo();
        CoordCtx ctxCenter(m_signController->mapHandle(), PIC, m_centerPoint);

        if (m_2PointTypes.contains(m_signType)  || m_signType == SignBase::SignType::LOCAL_LINE) {
            // Центральная точка теперь m_editPoints[1]
            QPointF center = m_centerPoint;

            // Изменяем размер, сохраняя направление
            QLineF line(center, newPos);
            if (m_initialDistance > 0) {
                line.setLength(line.length() / m_initialDistance);
            }

            // Обновляем все точки относительно центра (кроме самой центральной точки)
//            for (int i = 0; i < m_editPoints.size(); ++i) {
//                if (i == 1) continue; // Пропускаем центральную точку
//
//                QLineF oldLine(center, m_editPoints[i]);
//                oldLine.setLength(oldLine.length() * line.length());
//                m_editPoints[i] = oldLine.p2();
//            }
            m_editPoints[m_selectedPointIndex] = newPos;
            // Для LOCAL_POINT поддерживаем симметрию
            if (m_2PointTypes.contains(m_signType)  && m_editPoints.size() >= 3) {
                QPointF p1 = m_editPoints[0];
                QPointF p2 = center - (p1 - center);
                m_editPoints[1] = p2;
            }
            // Применяем изменения к объекту
            m_signController->resizeSelectedObject(m_selectedSignID, m_editPoints);
            updateBoundingRect();
        }
        else if (m_signType == SignBase::SignType::LOCAL_POLYGON) {
            // Для квадрата изменяем только координаты выбранной точки
            if (m_selectedPointIndex >= 0 && m_selectedPointIndex < m_editPoints.size()) {
                // Обновляем только выбранную точку
                m_editPoints[m_selectedPointIndex] = newPos;

                // Для квадрата обновляем bounding rect на основе всех точек
                updateBoundingRect();

                // Обновляем объект на карте
                QVector<QPointF> objectPoints = m_editPoints;
                m_signController->resizeSelectedObject(m_selectedSignID, objectPoints);
            }
        }
        return true;
    }
    else
    if (m_selectedPointIndex >= 0 && m_editMode == EditMode::RotateObject) {
        CoordCtx ctx(m_signController->mapHandle(), PIC, currentPos);
        QPointF newPos = ctx.geo();
        QPointF center = m_centerPoint; // Последняя точка - центр

        qreal angle = atan2(newPos.y() - center.y(), newPos.x() - center.x()) -
                      atan2(m_editPoints[m_selectedPointIndex].y() - center.y(),
                            m_editPoints[m_selectedPointIndex].x() - center.x());


        // Для LOCAL_POINT поддерживаем симметрию точек
        if (m_2PointTypes.contains(m_signType)  && m_editPoints.size() >= 3) {
            QPointF p1 = m_editPoints[1];
            QPointF p2 = center - (p1 - center);
            m_editPoints[2] = p2;
        }

        m_signController->rotateSelectedObject(m_selectedSignID, angle * 180 / M_PI, m_editPoints, m_centerPoint);
        updateBoundingRect();
        return true;
    }
    return false;
}

void SignEditor::handleMouseRelease()
{
//    if (m_editMode != EditMode::ScaleObject && m_editMode != EditMode::RotateObject)
//        m_selectedPointIndex = -1;
}// signeditor.cpp


void SignEditor::startResizing()
{
    if (m_selectedSignID == -1) return;

    showEditPoints(EditMode::ScaleObject);

    if (m_editMode != EditMode::ScaleObject) return;
    // Для LOCAL_POINT и LOCAL_LINE сохраняем начальное расстояние от центра до точки
    /*if (m_signType == SignBase::SignType::LOCAL_POINT || m_signType == SignBase::SignType::LOCAL_LINE) {
        QPointF center =  m_editPoints[2]; // Центральная точка
        QPointF edgePoint = m_editPoints[0];  // Крайняя точка
        m_initialDistance = QLineF(center, edgePoint).length();
    } */
}

void SignEditor::removeSelectedSign(){
    m_signController->removeSign(m_selectedSignID);
    m_selectedSignID = -1;
    m_editMode = EditMode::None;
}


void SignEditor::selectSign(SignBase* sign){
    clearSelection();
    m_selectedSignID = sign->getGisID();
    m_signController->clearHighlight();
    m_signType = sign->getGeometryType();
    m_signController->highlightSignByID(m_selectedSignID);
    m_editMode = EditMode::Select;

}
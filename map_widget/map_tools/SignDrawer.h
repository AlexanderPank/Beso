#pragma once

#ifndef SCENARIOCLIENT_SIGNDRAWER_H
#define SCENARIOCLIENT_SIGNDRAWER_H


#include <QObject>
#include <QVector>
#include <QPointF>
#include "../map_enums.h"
#include "../../signs/SignBase.h"

class QPainter;
class CoordCtx;

class SignDrawer : public QObject
{
Q_OBJECT
public:
    enum DrawState {
        Idle,
        DrawingFirstPoint,
        DrawingNextPoints,
        Complete
    };

    explicit SignDrawer(HMAP hMap, QObject *parent = nullptr);
    ~SignDrawer();

    void startDrawing(SignBase::SignType signType);
    void startDrawing(SignBase* sign);
    void cancelDrawing();
    void handleMouseClick(QPointF coordsPic);
    void handleMouseMove(QPointF coordsPic); // Добавлен обработчик движения мыши
    void handleMouseDoubleClick();
    void handlePaint(QPainter *p, int cx, int cy, int cw, int ch);
    void drawNameLabels(QPainter *p, int cx, int cy);

    bool isDrawing() const { return m_state != Idle; }
    SignBase::SignType currentSignType() const { return m_signType; }
    void clear();
    void setHMap(HMAP hMap){m_hMap = hMap;};

    static SignDrawer* instance();
signals:
    void signDrawn(SignBase::SignType type, const QVector<QPointF>& points);
    void signDrawEnd();
    void drawingCanceled();
    void signDrawing(const QVector<QPointF>& points, const double distance);

private:
    HMAP m_hMap;
    SignBase::SignType m_signType;
    DrawState m_state;
    QVector<QPointF> m_points;
    QPointF m_currentMousePos; // Текущее положение мыши
    SignBase* m_drawing_sign = nullptr;

    QVector<QPoint> toPixPoints(const QVector<QPointF>& geoPoints, int cx, int cy) const;
    void completeDrawing();
    QPointF calloutAnchor(const SignBase* sign) const;

    // Единственный экземпляр класса
    static SignDrawer* m_instance;

    QList<SignBase::SignType> m_2PointSigns = {SignBase::LOCAL_POINT, SignBase::LOCAL_CIRCLE ,SignBase::LOCAL_RECTANGLE , SignBase::LOCAL_SQUARE, SignBase::LOCAL_TITLE}; // Метод для получения списка разрешенных типов знаков

};


#endif //SCENARIOCLIENT_SIGNDRAWER_H

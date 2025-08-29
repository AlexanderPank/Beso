//
// Created by qq on 25.03.25.
//
// signeditor.h
#pragma once
#ifndef SCENARIOCLIENT_SIGNEDITOR_H
#define SCENARIOCLIENT_SIGNEDITOR_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QRectF>
#include "../map_enums.h"
#include "../../signs/SignBase.h"

class SignController;
class QPainter;



class SignEditor : public QObject
{
Q_OBJECT
public:

    explicit SignEditor(QObject *parent = nullptr);

    void setCurrentSign(int signId, SignBase::SignType type);
    void setZoom(int zoom){m_currentZoom = zoom;};
    void clearSelection();

    void showEditPoints(int mode);
    void updateBoundingRect();
    void onPaint(QPainter *p, int cx, int cy);

    bool handleMousePress(const QPoint& clickPos);
    bool handleMouseMove(const QPoint& currentPos, const bool isCtrl);
    void handleMouseRelease();

    int editMode() const { return m_editMode; }
    bool hasSelection() const { return m_selectedSignID != -1; }

    void startResizing();
    void removeSelectedSign();

    void selectSign(SignBase* sign);


private:
    SignController* m_signController = nullptr;
    int m_selectedSignID = -1;
    SignBase::SignType m_signType = SignBase::SignType::LOCAL_POINT;
    int m_editMode = EditMode::None;

    QVector<QPointF> m_editPoints;
    QPointF m_centerPoint;
    QRectF m_boundingRect;
    int m_selectedPointIndex = -1;
    QPoint m_dragEditPoint;
    QPoint m_prevPressedPos;

    double m_initialDistance = 0.0;  // Для хранения начального расстояния при изменении размера
    double m_currentZoom = 0.0;  // текущий zoom
    const QList<SignBase::SignType> m_2PointTypes = {SignBase::LOCAL_POINT,  SignBase::LOCAL_CIRCLE ,SignBase::LOCAL_RECTANGLE, SignBase::LOCAL_SQUARE};

};


#endif //SCENARIOCLIENT_SIGNEDITOR_H

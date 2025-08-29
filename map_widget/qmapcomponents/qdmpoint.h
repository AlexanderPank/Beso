/******* QDMPOINT.H ************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*     ОПИСАНИЕ КЛАССА "КОМПОНЕНТ - ТОЧКА ЭЛЕКТРОННОЙ КАРТЫ"        *
*                                                                  *
*******************************************************************/

#ifndef QDMPOINT_H
#define QDMPOINT_H

#include "qdmcompa.h"
#include "qdmcmp.h"
#include "qdmpoina.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ - ТОЧКА ЭЛЕКТРОННОЙ КАРТЫ"                 ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class  QDMapPoint : public QDMapComponent
{
    Q_OBJECT

    // Система координат на запись
    Q_PROPERTY(int PlaceIn READ GetPlaceIn WRITE SetPlaceIn)
    // Система координат на чтение
    Q_PROPERTY(int PlaceOut READ GetPlaceOut WRITE SetPlaceOut)

public :
    QDMapPoint(QWidget * parent = 0, const char * name = 0);
    ~QDMapPoint();

    int        GetPlaceIn() const;
    int        GetPlaceOut() const;

    QDMapView  *GetMapView() const;
    virtual void SetMapView(QDMapView *value);

    // Записать координату в системе PlaceIn
    bool SetPoint(double x, double y);
    // Считать  координату в системе PlaceOut
    bool GetPoint(double *x, double *y);

// Для служебного пользования !!!!!
    // Обработка событий изменения состава карт на уровне MapViewWindow
    void    MapAction(int typeaction);

public slots:
//!! конец блока для  служебного пользования !!!!!
    void SetPlaceIn(int value);
    void SetPlaceOut(int value);

public :
    // Для служебного пользования !!!!!
    QDMapPointAcces *VarMapPoint;
    QWidget         *ParentHandle;
    QString         VarMapViewName;
};

#endif // QDMPOINT_H

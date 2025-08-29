/******* QDMPOINA.H ************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*     ОПИСАНИЕ БАЗОВОГО КЛАССА ДОСТУПА К КООРДИНАТАМ ТОЧКИ         *
*                     ЭЛЕКТРОННОЙ КАРТЫ                            *
*                                                                  *
*******************************************************************/

#ifndef QDMPOINA_H
#define QDMPOINA_H

#include <qwidget.h>
#include "qdmviewa.h"
#include "maptype.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ БАЗОВЫЙ КЛАСС ДОСТУПА К КООРДИНАТАМ ТОЧКИ ЭЛЕКТРОННОЙ КАРТЫ ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class  QDMapPointAcces : public QWidget
{
    Q_OBJECT

public :
    QDMapPointAcces(QDMapViewAcces* mapview = 0, QWidget * parent = 0, const char * name = 0);
    ~QDMapPointAcces() {};

    // Конвертировать координаты из dframein система координат placein
    // в систему координат placeout
    MAPDFRAME ConvertDFrame(MAPDFRAME dframein, int placein, int placeout);
    // Преобразовать точку метрики с координатами x, y
    // из системы placein в систему placeout
    bool    ConvertMetric(double *x, double *y, int placein, int placeout);
    int     GetPlaceIn() const;
    int     GetPlaceOut() const;
    double  GetX();
    double  GetY();
    QDMapViewAcces *GetMapView() const;
    void SetPlaceIn(int value);
    void SetPlaceOut(int value);
    void SetX(double value);
    void SetY(double value);
    void SetMapView(QDMapViewAcces *value);
    // Записать координату в системе PlaceIn
    bool SetPoint(double x, double y);
    // Считать  координату в системе PlaceOut
    bool GetPoint(double *x, double *y);
    // Для служебного пользования !!!!!
    virtual void Assign(QDMapPointAcces *mappoint);

private :
    void TranslatePlaneToPicture(double *x, double *y);
    void TranslatePictureToPlane(double *x, double *y);

    // ---------------------------------
public  :
    // Пользователю только на чтение
    QDMapViewAcces  *VarMapView;                                    // 22/03/12
    double          VarX;                // Координата в метрах
    double          VarY;                // Координата в метрах
    int             VarPlaceIn;          // Система координат в теминах MAP API
    int             VarPlaceOut;         // Система координат в теминах MAP API
};

#endif // QDMPOINA_H

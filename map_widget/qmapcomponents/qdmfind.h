/******* QDMFIND.H *************************************************
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
*  ОПИСАНИЕ КЛАССА "КОМПОНЕНТ ПОИСКА ОБ'ЕКТА ЭЛЕКТРОННОЙ КАРТА"    *
*                                                                  *
*******************************************************************/

#ifndef QDMFIND_H
#define QDMFIND_H

#include "qdmcmp.h"
#include "qdmobj.h"

class QDMapPointAcces;
class QDMapSelect;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ ПОИСКА ОБ'ЕКТА ЭЛЕКТРОННОЙ КАРТА"          ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapFind : public QDMapComponent
{
    Q_OBJECT

public  :
    QDMapFind(QWidget * parent = 0, const char * name = 0);
    ~QDMapFind();

    bool           GetActive() const;
    QDMapPointAcces *GetPoint();
    double         GetRadius();
    int            GetPlaceIn();
    int            GetPlaceOut();
    void           GetCoordPoint(double *x, double *y);
    void           SetPlaceIn(int value);
    void           SetPlaceOut(int value);
    void           SetCoordPoint(double x, double y);
    // Указатель на объект карты
    void           SetMapObj(QDMapObj *value);
    QDMapObj       *GetMapObj()  const;
    virtual void   SetMapView(QDMapView *value);
    QDMapView      *GetMapView() const;
    // Установить/запросить условия поиска
    void           SetMapSelect(QDMapSelect *value);
    QDMapSelect    *GetMapSelect();
    HSELECT        GetMapSelectHandle();
    // Включить/выключить режим поиска в заданной точке
    void           SetFindPoint(bool value);
    bool           GetFindPoint();
    void           SetRadius(double value);
    HOBJ           SeekInPoint(HOBJ obj, int flag);
    HOBJ           SeekInMap(HOBJ obj, int flag);
    bool           First();
    bool           Prior();
    bool           Next();
    bool           Last();
    // Установить объект в центр окна
    void           Center();
    // Определить - находится ли система поиска на первом элементе
    bool           GetBOF();
    // Определить - вышла ли система за последний элемент
    bool           GetEOF();

    // Для внутреннего использования
    void           MapAction(int acttype);
    virtual void   Notification(QObject *component, int operation);
    // -----------------------------
public slots:
    void           SetActive(bool value);

public :
    // Пользователю на чтение
    QDMapSelect     *VarMapSelect;
    // Для внутреннего использования
    QDMapObj        *VarMapObj;
    QDMapPointAcces *VarMapPoint;
    MAPDFRAME       VarFrame;
    double          VarRadius;
    bool            VarActive;
    bool            VarBOF;
    bool            VarEOF;
    bool            VarFindPoint;
};


#endif //QDMFIND_H

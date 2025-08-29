/******* QDMSELOB.H ************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR QT-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*   ОПИСАНИЕ КЛАССА "КОМПОНЕНТ - ДИАЛОГ ВЫБОРА ОБЪЕКТА КАРТЫ"      *
*                                                                  *
*******************************************************************/

#ifndef QDMSELOB_H
#define QDMSELOB_H

#include "qdmcmp.h"

class QDMapView;
class QDMapObj;
class QDMapSelect;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ ДИАЛОГ ВЫБОРА ОБ'ЕКТА ЭЛЕКТРОННОЙ КАРТА"   ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapDlgObj : public QDMapComponent
{
    Q_OBJECT

public  :
    QDMapDlgObj(QWidget * parent = 0, const char * name = 0);
    ~QDMapDlgObj();

    virtual void   SetMapView(QDMapView *value);
    QDMapView *    GetMapView() const;
    // Выбрать объект карты в точке, используя диалог
    // x,y    - координаты точки в пикселах в клиентской области
    //                 элемнта управления TMapView, на который настроен TMapObj
    // radius - радиус поиска в пикселах (не более 10)
    // Выход = true  - выбрали и заполнили mapobj
    //         false - отказались
    bool SelectObjectInPoint(QDMapObj *mapobj, int x, int y, int radius);
    // Поиск объекта перебором в диалоге
    // mapselect - критерий поиска :
    //       если AllMaps не установлен, то поиск будет выполнен
    //       для первой доступной для поиска в списке;
    // next      - надо ли выполнить продолжение поиска с текущего;
    // Выход = true  - выбрали и заполнили mapobj
    //         false - отказались
    bool ExecuteDlgFind(QDMapObj *mapobj, QDMapSelect *mapselect, bool next);
    // Показать информационный диалог об объекте
    // Выход = false - отказ
    bool ShowDialogInfo(QDMapObj *mapobj);
};

#endif // QDMSELOB_H

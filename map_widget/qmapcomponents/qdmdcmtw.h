/******** QDMDCMTW.H ***********************************************
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
*        КОМПОНЕНТ - ДИАЛОГ "СОЗДАНИЯ МАТРИЦЫ ВЫСОТ"               *
*                                                                  *
*******************************************************************/

#ifndef QDMDCMTW_H
#define QDMDCMTW_H

#include "qdmcmp.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГА                                +
//+++ "СОЗДАНИЯ МАТРИЦЫ ВЫСОТ"                     +
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapCreateMtw : public QDMapComponent
{
    Q_OBJECT

    // Установить\Запросить значение флага выбора области карты
    Q_PROPERTY(bool AllRectActive READ GetAllRectActive WRITE SetAllRectActive)

public :
    // Для пользователя
    //------------------------------
    QDMapCreateMtw(QWidget *parent = 0, const char *name = 0);
    ~QDMapCreateMtw() {};
    // Выполнить диалог
    void         Execute();
    // Указатель на карту
    virtual void SetMapView(QDMapView *value);
    QDMapView    *GetMapView() const;
    // Установить\Запросить значение флага выбора области карты
    // true  - вся карта(по умолчанию)
    // false - прямоугольная область, заданная функцией SetMapRect
    void  SetAllRectActive(bool value);
    bool  GetAllRectActive() const;
    // Установить размеры области карты в пикселах
    // Перед использованием данной фукции необходимо
    // установить флаг выбора области карты в 0
    // left, top     -  левый верхний угол
    // right, bottom -  правый нижний
    void SetMapRect(int left, int top, int right, int bottom);

public:
    // Служебные
    //------------------------------
    // Заполнить структуру для вызова задачи
    void InitTaskParm(TASKPARM *taskparm, void* mtrparm);

    RECT  MapRect;                                                            // 22/03/12
    // Флаг выбора области карты
    // true  - вся карта (по умолчанию)
    // false - прямоугольная область, заданная функцией SetMapRect
    bool  AllRectActive;
};

#endif // QDMDCMTW_H

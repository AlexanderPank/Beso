/******** QDMD3D.H   ***********************************************
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
*  КОМПОНЕНТ - ДИАЛОГ "ОТОБРАЖЕНИЕ ТРЕХМЕРНОЙ МОДЕЛИ МЕСТНОСТИ"    *
*                                                                  *
*******************************************************************/

#ifndef QDMD3D_H
#define QDMD3D_H

#include "qdmcmp.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++          КЛАСС ДИАЛОГА                       +
//+++  "ОТОБРАЖЕНИЕ ТРЕХМЕРНОЙ МОДЕЛИ МЕСТНОСТИ"   +
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMap3D : public QDMapComponent
{
    Q_OBJECT

    // Установить\Запросить значение флага выбора области карты
    Q_PROPERTY(bool AllRectActive READ GetAllRectActive WRITE SetAllRectActive)

public :
    // Для пользователя
    //------------------------------
    QDMap3D(QWidget *parent = 0, const char *name = 0);
    ~QDMap3D() {CloseDialog3D();}
    // Выполнить диалог
    void         Execute();
    // Закрыть диалог
    void         CloseDialog3D();
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

    // Для внутреннего использования
    void MapAction(int acttype);
public:
    // Служебные
    //------------------------------
    // Заполнить структуру для вызова задачи
    void InitTaskParm(TASKPARMEX *taskparm);

    HINSTANCE   LibInst;    // О©?О©?О©?О©?О©?О©?О©?О©?О©?О©?О©?О©?О©? О©?О©?О©?О©?О©?О©?О©?О©?О©?О©? MAP3DLIB        // 22/03/12
    RECT  MapRect;
    // Флаг выбора области карты
    // true  - вся карта (по умолчанию)
    // false - прямоугольная область, заданная функцией SetMapRect
    bool  AllRectActive;
};

#endif // QDMDCMTW_H

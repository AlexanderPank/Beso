/******** QDMACTPT.H ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR QT-DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*  Компонент      : QDGetMapPoint -  компонент интерактивного      *
*                   выбора точки карты                             *
*                                                                  *
*******************************************************************/

#ifndef QDMACTPT_H
#define QDMACTPT_H

#include "qdmact.h"
#include "mapapi.h"

class QDMapObj;


//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ QDGetMapPoint -  КОМПОНЕНТ ИНТЕРАКТИВНОГО    +
//+++                  ВЫБОРА ТОЧКИ КАРТЫ          +
//++++++++++++++++++++++++++++++++++++++++++++++++++
class QDGetMapPoint : public QDGtkAction
 {
  Q_OBJECT

public:

    QDGetMapPoint(QWidget * parent = 0, const char * name = 0);
    ~QDGetMapPoint();
    // Рабочие функции
    //---------------------------------------------------
    void MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect);
    void MapAfterPaint(QObject *sender, QImage * mapImage, RECT mappaintrect);
    void MouseMove(int x, int y, int modkeys);
    void MouseUp(int x, int y, int modkeys);

private :

    void PaintDot(QImage *mapImage);
    void CalcCoordsForObjAndMouse(int aMouseX, int aMouseY);
    // нарисовать/стереть линию
    void LinePaint(QImage *mapImage);

public:

    // Для пользователя
    //---------------------------------------------------
    // подсоединить обработчик к компоненту отображения карты
    void StartAction();
    // отсоединить
    void StopAction();

    // Объект для которого выбирается точка
    QDMapObj     *GetMapObj();
    void         SetMapObj(QDMapObj *mapobj);

    typedef enum TSELECTIONMODE
    {
      GETEXISTPOINT    = 1,      // существующая точка объекта
      GETVIRTUALPOINT  = 2       // ближайшая точка объекта
    } TSELECTIONMODE;

    // Тип точки(TSELECTIONMODE)
    int          GetTypePoint();
    void         SetTypePoint(int type);

signals :
    // Сигнал о выборе точки
    // x, y       - координаты точки объекта в метрах
    // numpoint   - номер точки
    // numsubobj  - номер подобъекта
    void SignalSelectPoint(double x, double y, int numpoint, int numsubobj);

private :
    // Рабочие переменные
    //---------------------------------------------------

    QDMapObj    *FMapObj;
    DOUBLEPOINT FPointMouse;
    DOUBLEPOINT FPointObj;
    DOUBLEPOINT FPlacePoints[2]; // координаты метрики
    PAINTPARM   FImageParm;      // параметры отрисовки
    IMGDOT      FImageObj;
    PLACEDATA   FPlaceData;      // метрика резинки
    int         FNumPoint;       // номер точки на объекте
    int         FNumSubj;        // номер подобъекта для которого точка
    int         FColorDot;       // цвет резинки
    int         FLineWidth;      // толщина резинки
    int         FDashLenght;     // длина штриха резинки
    int         FBlankLenght;    // длина пробела резинки
    int         FPlaceCount;     // количество метрики
    TSELECTIONMODE FTypeGetPoint;   // тип точки
    bool        FIsLine;
};

#endif // QDMACTPT_H

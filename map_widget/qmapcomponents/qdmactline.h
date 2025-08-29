/******** QDMACTLINE.H *********************************************
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
*  Компонент     : QDMapActionLine - компонент                     *
*                  интерактивного заполнения объекта-линии         *
*                                                                  *
*******************************************************************/

#ifndef QDMACTLINE_H
#define QDMACTLINE_H

#include "qdmact.h"
#include "qdmwina.h"

class QDMapObj;

/*!
//  Класс интерактивного заполнения объекта-линии
*/

class QDMapActionLine : public QDGtkAction
{
Q_OBJECT

public:

    QDMapActionLine(QWidget * parent = 0, const char * name = 0);
    ~QDMapActionLine();

    // Функции - обработчики внутренних событий
    //---------------------------------------------------
    void MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect);
    void MapAfterPaint(QObject *sender, QImage * mapImage, RECT mappaintrect);
    void MouseMove(int x, int y, int modkeys);
    void MouseUp(int x, int y, int modkeys);
    void DblClick();

private :

    // Сохранить текущие координаты мыши в объект
    void AppendMouseCoordsToObject();
    // Пересчитать координаты мыши в координаты на плоскости
    void CalcCoordsForMouse(int aMouseX, int aMouseY);
    // нарисовать линию
    void LinePaint(QImage *mapImage);

    bool PaintObject(QDMapViewWindow *mapwin, QPainter * painter, int left, int top);
    bool PaintOneLine(QDMapViewWindow *mapwin, QPainter * painter, DOUBLEPOINT * fromPoint, DOUBLEPOINT * toPoint, int left, int top);

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


signals :
    // Сигнал об окончании действия(даблклик)
    void SignalEndAction();

private :
    // Рабочие переменные
    //---------------------------------------------------

    /*!
     * \brief Переменная хранения создаваемого объекта
     */
    QDMapObj    *FMapObj;

    /*!
     * \brief Цвет отображаемой линии
     */
    COLORREF    ColorOfLine;

    /*!
     * \brief Толщина линии
     */
    int         ThickOfLine;

    /*!
     * \brief Координаты мыши
     */
    DOUBLEPOINT MouseCoords;

    /*!
     * \brief Координаты последней точки объекта
     */
    DOUBLEPOINT LastPointCoords;
};

#endif // QDMACTLINE_H

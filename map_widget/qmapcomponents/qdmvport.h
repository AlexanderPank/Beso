/******* QDMVPORT.H ************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                        FOR QT-DESIGNER                           *
*                                                                  *
********************************************************************
*                                                                  *
*        ОПИСАНИЕ КЛАССА ДОСТУПА К ФУНКЦИЯМ ОТОБРАЖЕНИЯ            *
*                     ЭЛЕКТРОННОЙ КАРТЫ                            *
*                                                                  *
*******************************************************************/

#ifndef QDMVPORT_H
#define QDMVPORT_H

#include "qdmwina.h"

#include <QPainter>
#include <QWidget>

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ КЛАСС "ИНТЕРФЕЙС ОТОБРАЖЕНИЯ ЭЛЕКТРОННОЙ КАРТОЙ" +++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class  QDMapViewPort : public QWidget
{
    Q_OBJECT

public:

    QDMapViewPort(QWidget *parent = 0);
    ~QDMapViewPort();

    void SetMapViewWindow(QDMapViewWindow *mw);

protected :

    //  Системная функция Qt :
    //  Отрисовка окна
    virtual void paintEvent(QPaintEvent * e);
    //  Системная функция Qt :
    //  Обработать сообщение
    //  Перемещение мыши
    virtual void mouseMoveEvent(QMouseEvent * e);
    //  Системная функция Qt :
    //  Обработать сообщение нажатие кнопки мыши
    virtual void mousePressEvent(QMouseEvent * e);
    //  Системная функция Qt :
    //  Обработать отжатие кнопки мыши
    virtual void mouseReleaseEvent(QMouseEvent * e);
    //  Системная функция Qt :
    //  Обработать двойное нажатие кнопки мыши
    virtual void mouseDoubleClickEvent(QMouseEvent * e);

public:
    // Экземпляр класса QDMapViewWindow
    QDMapViewWindow *MapViewWindow;
};

#endif // QDMVPORT_H

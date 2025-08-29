/******** QDMWINW.H ************************************************
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
*   ОПИСАНИЕ КЛАССА : "КОМПОНЕНТ ОТОБРАЖЕНИЯ ОТКРЫТЫХ ДАННЫХ       *
*                      ЭЛЕКТРОННОЙ КАРТЫ"                          *
*                                                                  *
*******************************************************************/

#ifndef QDMWINW_H
#define QDMWINW_H

#include "qdmwina.h"

class QDMapView;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++ КЛАСС "КОМПОНЕНТ ДОСТУПА К ЭЛЕКТРОННОЙ КАРТЕ          +++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapWindow : public QDMapViewWindow
{
    Q_OBJECT

public :
    QDMapWindow(QWidget *parent = 0);
   ~QDMapWindow();

    // Указатель на карту
    void         SetMapView(QDMapView *value);
    QDMapView    *GetMapView() const;
    // Получить объект доступа к карте
    QDMapViewAcces *GetMapViewAcces();
    // установить необходимый масштаб отображенеия для окна
    int          RestoreViewScaleWin();
    // Восстановить масштаб отображенеия для MapView
    void         RestoreViewScale(int scale);
    // Отображение в контурном виде
    bool         GetMapContour();
    void         SetMapContour(bool value);
    // Яркость отображения карты (-4..+4)
    int          GetMapBright();
    void         SetMapBright(int value);
    // Контрастность отображения карты (-4..+4)
    int          GetMapContrast();
    void         SetMapContrast(int value);
    // Текущий масштаб отображения
    int          GetViewScale();
    int          SetViewScale(int value);
    // Идентификатор карты
    HMAP         GetMapHandle();
    // Перерисовка окна
    virtual void Repaint();
    // Преобразовать точку метрики с координатами x, y из системы placein в систему placeout
    int          ConvertMetric(double *x, double *y, int placein, int placeout);
    // Конвертировать координаты из dframein система координат placein
    virtual MAPDFRAME ConvertDFrame(MAPDFRAME dframein, int placein, int placeout);
    //   Запросить высоту, ширину карты в пикселах в текущем масштабе
    void           GetMapHW(long int *height, long int *width);
    // определить габариты всего набора карт в пикселах
    virtual RECT   GetRegionRect();
    // определить габариты всего набора карт в указанной системе координат
    virtual MAPDFRAME GetRegionDFrame(int pointplace);
    // идентификатор карты
//    property     MapHandle : HMap read GetMapHandle; // дублирует MapView.MapHandle
    // Текущий масштаб отображения
//    property     ViewScale : integer read GetViewScale write SetViewScale;
    // Вид отображения карты
//    property     ViewType : integer read GetViewType write SetViewType;

//  published

    // Указатель на карту
//    property     MapView : TMapView read FMapView write SetMapView;
    // Яркость отображения карты (-4..+4)
//    property     MapBright : integer read GetMapBright write SetMapBright;
    // Контрастность отображения карты (-4..+4)
//    property     MapContrast : integer read GetMapContrast write SetMapContrast;
    // Отображение в контурном виде
//    property     MapContour : boolean read GetMapContour write SetMapContour;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Блок функций и переменных для внутреннего использования
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    void         MapAction(int action);
    virtual void Notification(QObject *component, int operation);

protected :
    virtual void drawContents(QPainter* p, int cx, int cy, int cw, int ch);

protected :
    QDMapView   *VarMapView;          // Указатель на карту
    int         VarViewScaleInWindow; // Масштаб отображения в данном окне
    int         VarMapBright;
    int         VarMapContrast;
    bool        VarMapContour;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
};

#endif // QDMWINW_H

/******** QDMCMP.H *************************************************
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
*   ОПИСАНИЕ КЛАССА : "КОМПОНЕНТ ДОСТУПА К ЭЛЕКТРОННОЙ КАРТЕ"      *
*                                                                  *
*******************************************************************/

#ifndef QDMCMP_H
#define QDMCMP_H

#include "qdmwina.h"
#include "qdmlayr.h"
#include "qdmrst.h"
#include "qdmmtr.h"
#include "qdmsite.h"

class QDMapSemantics;
class QDMapWindow;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++ КЛАСС "КОМПОНЕНТ ДОСТУПА К ЭЛЕКТРОННОЙ КАРТЕ          +++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapView : public QDMapViewWindow
{
    Q_OBJECT

    // Для пользователя
    // Имя файла карты
    Q_PROPERTY(QString MapFileName READ GetMapFileName WRITE SetMapFileName)
    // Активизировать карту
    Q_PROPERTY(bool MapActive READ GetMapActive WRITE SetMapActive)
    // Яркость отображения карты (-4..+4)
    Q_PROPERTY(int MapBright READ GetMapBright WRITE SetMapBright)
    // Контрастность отображения карты (-4..+4)
    Q_PROPERTY(int MapContrast READ GetMapContrast WRITE SetMapContrast)
    // Отображение в контурном виде
    Q_PROPERTY(bool MapContour READ GetMapContour WRITE SetMapContour)


public :
    QDMapView(QWidget *parent = 0);
   ~QDMapView();

public slots:
    // Установить новое имя файла карты
    void           SetMapFileName(const QString qstring);
    void           SetMapActive(bool active);
    // Установить/cбросить отображение карты в контурном виде
    void           SetMapContour(bool value);
    // Установить текущее значение яркости карты
    void           SetMapBright(int value);
    // Установить текущее значение контрастности карты
    void           SetMapContrast(int value);

public :
    // Активна ли в данный момент карта
    bool           GetMapActive() const;
    // Запрос имени файла карты
    QString        GetMapFileName() const;
    // Отображена ли карта в контурном виде
    bool           GetMapContour() const;
    // Запрос текущего значения яркости
    int            GetMapBright() const;
    // Запрос текущего значения контрастности
    int            GetMapContrast() const;
    //   Запросить имя карты
    QString        GetMapName();
    //   Запросить номенклатуру карты
    QString        GetMapListName(int listnumber = 1);      // 21/01/16 Савелов
    //   Запросить базовый масштаб карты
    int            GetMapScale();
    //   Запросить тип карты
    int            GetMapType();
    //   Запросить высоту, ширину карты в пикселах в текущем масштабе
    void           GetMapHW(long int *height, long int *width);
    // Открыть карту с именем aMapFileName
    bool MapOpen(const QString filename, bool mapview);
    // Закрыть карту
    void MapClose();
    HSELECT        GetViewSelectHandle();
    // Выполнить обновление границ  изображения района для все компонент
    // связанных с MapView
    void UpdatePictureBorderForAll();
    // Для внутреннего использования !!!!
    // Обработка событий изменения состава карт и их отображения на уровне MapView
    void  AllMapRef(int action);
    // Работа со списком подключенных к нам QDMapWindow
    void AppendMapWindow(QDMapWindow *mapwindow);
    void DeleteMapWindow(QDMapWindow *mapwindow);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    int  GetMapViewType();
    void SetMapViewType(int viewtype); // Сюда надо передавать константы VT_...

public :
    QDMapSites           *VarMapSites;
    QDMapMtrs            *VarMapMtrs;
    QDMapRsts            *VarMapRsts;
    QDMapLayers          *VarMapLayers;
    QDMapSemantics       *VarMapSemantics;
    // Для внутреннего использования
    QString              VarMapFileName;
    QList<QDMapWindow *> VarMapWindows;
    QString              QStrName;
};

#endif // QDMCMP_H

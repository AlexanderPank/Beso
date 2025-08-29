/******* QDMLAYR.H *************************************************
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
*                 КЛАССЫ ДОСТУПА К СЛОЯМ КАРТЫ                     *
*                                                                  *
*******************************************************************/

#ifndef QDMLAYR_H
#define QDMLAYR_H

#include "maptype.h"

class QDMapViewAcces;
class QDMapView;
class QDMapObjAcces;
class QDMapSite;

//++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К СЛОЯМ КАРТЫ     +++
//++++++++++++++++++++++++++++++++++++++++

class QDMapLayer
{

public :
    QDMapLayer(QDMapViewAcces *mapview, QDMapSite *mapsite);
    ~QDMapLayer();

    HRSC  GetRscHandle();
     // Указатель на компонент карты
    QDMapView  *GetMapView();
     // Указатель на компонент пользовательской карты
    QDMapSite  *GetMapSite();
     // Наименование слоя
    const char *GetLayerName();
    // Уникальное наименование слоя - ключ
    const char *GetLayerKey();
    // Количество объектов описанных в классификаторе
    int    GetObjCount();
    // Условное название объектов
    const char *GetObjName(int number);
    // Внешний код объекта
    int    GetObjExCode(int number);
    // Внутренний код объекта
    int    GetObjInCode(int number);
    // Характер локализации оьъекта
    int    GetObjLocal(int number);
    // Уникальное название оьъекта (ключ)
    const char *GetObjKey(int number);

public :
    // Пользователю только на чтение
    QDMapViewAcces *VarMapView;
    QDMapSite      *VarMapSite;      // Site с которым работаем  имеет высший приоритет по сравнениб с VarRsc
    HRSC           VarHRsc;          // RSC с которым работаем (если FMapSite<>nil то rsc вычисляется по Site)
    int            VarNumber;        // текущий номер слоя
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К СЛОЯМ КАРТЫ - КОНТЕЙНЕР    +++
//+++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapLayers
{

public :
    QDMapLayers(QDMapViewAcces *mapview, QDMapSite *mapsite);
    QDMapLayers(HRSC rsc);
    ~QDMapLayers();

    // Количество слоев
    int        GetLayerCount();
    // Слой по порядковому номеру
    QDMapLayer *GetMapLayers(int number);
    // Слой по оьъекту карты
    QDMapLayer *GetObjLayers(QDMapObjAcces *mapobj);
    // Слой по уникальному названию - ключу
    QDMapLayer *GetKeyLayers(char *keyname);
    void       SetRscHandle(HRSC rsc);

public :
   // Для внутреннего использования
   QDMapLayer *VarMapLayer;
};

#endif  // QDMLAYR_H

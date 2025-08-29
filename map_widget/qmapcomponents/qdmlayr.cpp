/****** QDMLAYR.CPP ************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2005              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR Qt5-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*                 КЛАССЫ ДОСТУПА К СЛОЯМ КАРТЫ                     *
*                                                                  *
*******************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>

#include "qdmlayr.h"
#include "qdmsite.h"
#include "qdmviewa.h"
#include "qdmobja.h"
#include "mapapi.h"

//++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К СЛОЯМ КАРТЫ     +++
//++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapLayer::QDMapLayer(QDMapViewAcces *mapview, QDMapSite *mapsite)
{
    VarMapView = mapview;
    VarNumber  = 0;
    VarMapSite = mapsite;
    VarHRsc    = 0;
}

QDMapLayer::~QDMapLayer()
{
}

//-------------------------------------------------------------
// Получить VarHRsc для текущей комбинации параметров
//-------------------------------------------------------------
HRSC QDMapLayer::GetRscHandle()
{
    HSITE site1;
    HMAP  maph1;
    HRSC  ret = 0;

    // если есть VarMapView значит создали для карты
    if (VarMapView != NULL)
    {
        maph1 = VarMapView->GetMapHandle();

        if (maph1 == 0) return ret;

        // если есть site значит создали для пользовательской карты
        if (VarMapSite != NULL)
          site1 = VarMapSite->VarSite;
        else
          site1 = maph1;

        if (site1 == 0)
          return ret;
        ret = mapGetRscIdent(maph1, site1);
        return ret;
    }

    // если есть rsc значит создали для отдельно открытого RSC
    return (ret = VarHRsc);
}

//-------------------------------------------------------------
// Наименование слоя
//-------------------------------------------------------------
const char *QDMapLayer::GetLayerName()
{
    HRSC rsch1;
    const char *ret = 0;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscSegmentName(rsch1, VarNumber);
    return ret;
}

//-------------------------------------------------------------
// Количество объектов описанных в классификаторе
//-------------------------------------------------------------
int QDMapLayer::GetObjCount()
{
    HRSC rsch1;
    int  ret = 0;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscObjectCountInLayer(rsch1, VarNumber);

    return ret;
}

//-------------------------------------------------------------
// Условное название объектов
//-------------------------------------------------------------
const char *QDMapLayer::GetObjName(int number)
{
    HRSC rsch1;
    const char *ret = 0;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscObjectNameInLayer(rsch1, VarNumber, number);
    return ret;
}

//-------------------------------------------------------------
// Внешний код объекта
//-------------------------------------------------------------
int QDMapLayer::GetObjExCode(int number)
{
    HRSC rsch1;
    int  ret = 0;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscObjectExcodeInLayer(rsch1, VarNumber, number);
    return ret;
}

//-------------------------------------------------------------
// Внутренний код объекта
//-------------------------------------------------------------
int QDMapLayer::GetObjInCode(int number)
{
    HRSC rsch1;
    int  ret = 0;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscObjectCodeInLayer(rsch1, VarNumber, number);
    return ret;
}

//-------------------------------------------------------------
// Характер локализации оьъекта
//-------------------------------------------------------------
int QDMapLayer::GetObjLocal(int number)
{
    HRSC rsch1;
    int  ret = LOCAL_LINE;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscObjectLocalInLayer(rsch1, VarNumber, number);
    return ret;
}

//-------------------------------------------------------------
// Уникальное название оьъекта (ключ)
//-------------------------------------------------------------
const char *QDMapLayer::GetObjKey(int number)
{
    HRSC rsch1;
    const char *ret = 0;
    int  incode;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    incode = mapGetRscObjectCodeInLayer(rsch1, VarNumber, number);
    if (incode == 0) return ret;

    ret = mapGetRscObjectKey(rsch1, incode);
    return ret;
}

//-------------------------------------------------------------
// Уникальное наименование слоя - ключ
//-------------------------------------------------------------
const char *QDMapLayer::GetLayerKey()
{
    HRSC rsch1;
    const char *ret = 0;

    rsch1 = GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscSegmentKey(rsch1, VarNumber);
    return ret;
}

//------------------------------------------------------------------
// Указатель на компонент карты
//------------------------------------------------------------------
QDMapView  *QDMapLayer::GetMapView()
{
    return (QDMapView *)VarMapView;
}

//------------------------------------------------------------------
// Указатель на компонент пользовательской карты
//------------------------------------------------------------------
QDMapSite  *QDMapLayer::GetMapSite()
{
    return VarMapSite;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К СЛОЯМ КАРТЫ - КОНТЕЙНЕР    +++
//+++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapLayers::QDMapLayers(QDMapViewAcces *mapview, QDMapSite *mapsite)
{
    VarMapLayer = new QDMapLayer(mapview, mapsite);
}

//------------------------------------------------------------------
//   Конструктор для работы с отдельно открытым RSC
//------------------------------------------------------------------
QDMapLayers::QDMapLayers(HRSC rsc) /*: QDMapLayers(NULL, NULL) c++11 only*/
{
    VarMapLayer = new QDMapLayer(NULL, NULL);
    SetRscHandle(rsc);
}

QDMapLayers::~QDMapLayers()
{
    delete VarMapLayer;
}

void QDMapLayers::SetRscHandle(HRSC rsc)
{
    if (VarMapLayer->VarMapView != NULL) return;

    VarMapLayer->VarHRsc = rsc;
}

//------------------------------------------------------------------
// Количество слоев
//------------------------------------------------------------------
int QDMapLayers::GetLayerCount()
{
    HRSC rsch1;
    int  ret = 0;

    rsch1 = VarMapLayer->GetRscHandle();
    if (rsch1 == 0) return ret;

    ret = mapGetRscSegmentCount(rsch1);
    return ret;
}

//------------------------------------------------------------------
// Слой по порядковому номеру
//------------------------------------------------------------------
QDMapLayer *QDMapLayers::GetMapLayers(int number)
{
    VarMapLayer->VarNumber = number;
    return VarMapLayer;
}

//------------------------------------------------------------------
// Слой по оьъекту карты
//------------------------------------------------------------------
QDMapLayer *QDMapLayers::GetObjLayers(QDMapObjAcces *mapobj)
{
    VarMapLayer->VarNumber = mapSegmentNumber(mapobj->VarObjHandle);

    return VarMapLayer;
}

//------------------------------------------------------------------
// Слой по уникальному названию - ключу
//------------------------------------------------------------------
QDMapLayer *QDMapLayers::GetKeyLayers(char *keyname)
{
    VarMapLayer->VarNumber = mapGetRscSegmentByKey(VarMapLayer->GetRscHandle(), keyname);

    return VarMapLayer;
}


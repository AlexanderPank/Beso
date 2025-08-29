/******** QDMDMET.CPP **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR Qt5-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*           КЛАСС МЕТРИЧЕСКОГО ОПИСАНИЯ ОБЪЕКТА КАРТЫ              *
*                                                                  *
*                                                                  *
*******************************************************************/

#include "qdmmet.h"
#include "qdmobj.h"
#include "qdmpoina.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС МЕТРИЧЕСКОГО ОПИСАНИЯ ОБЪЕКТА КАРТЫ    *
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapMetric::QDMapMetric(QDMapObjAcces *mapobj)
{
  VarMapObj   = mapobj;
  VarMapPoint = new QDMapPointAcces(VarMapObj->VarMapView);
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapMetric::~QDMapMetric()
{
  delete VarMapPoint;
}

//------------------------------------------------------------------
// Запросить/установить количество подобъектов
//------------------------------------------------------------------
int QDMapMetric::GetSubObjCount()
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0)  return 0;

  return mapPolyCount(VarMapObj->GetObjHandle()) - 1;
}

void QDMapMetric::SetSubObjCount(int value)
{
  int OldSub;

  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0) return;

  if (value < 0) return;
  OldSub = mapPolyCount(VarMapObj->GetObjHandle()) - 1;
  if (value == OldSub) return;
  if (value > OldSub)
  {
     while (value != OldSub)
     {
          if (mapCreateSubject(VarMapObj->GetObjHandle()) == 0) return;
          OldSub = mapPolyCount(VarMapObj->GetObjHandle()) - 1;
     }
  }
  else while (value != OldSub)
       {
          if (mapDeleteSubject(VarMapObj->GetObjHandle(), OldSub) == 0) return;
          OldSub = mapPolyCount(VarMapObj->GetObjHandle()) - 1;
       }

  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

//------------------------------------------------------------------
// Запросить/установить количество точек в подобъекте
//------------------------------------------------------------------
int QDMapMetric::GetPointCount(int subobj)
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0)  return 0;
  if (subobj < 0) return 0;

  return mapPointCount(VarMapObj->GetObjHandle(), subobj);
}

void QDMapMetric::SetPointCount(int subobj, int value)
{
  int    Count;
  double DX,DY;

  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0)  return;
  if (subobj < 0) return;
  if (value < 0) return;
  Count = mapPointCount(VarMapObj->GetObjHandle(), subobj);
  if (Count < value)
  {
      DX = mapXPlane(VarMapObj->GetObjHandle(), Count, subobj);
      DY = mapYPlane(VarMapObj->GetObjHandle(), Count, subobj);
      while (Count != value)
      {
          if (mapAppendPointPlane(VarMapObj->GetObjHandle(), DX, DY, subobj) == 0) return;
          Count = mapPointCount(VarMapObj->GetObjHandle(), subobj);
      }
  }
  else while (Count != value)
       {
          if (mapDeletePointPlane(VarMapObj->GetObjHandle(), Count, subobj) == 0) return;
          Count = mapPointCount(VarMapObj->GetObjHandle(), subobj);
       }

  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

//------------------------------------------------------------------
// Запросить/установить точки объекта
//------------------------------------------------------------------
QDMapPointAcces *QDMapMetric::GetPoints(int subobj, int number)
{
  int OldPlane;
  QDMapPointAcces *ret = VarMapPoint;

  if ((VarMapObj != NULL) && (VarMapObj->GetObjHandle() != 0) &&
      (subobj >= 0) && (number > 0))
  {
    VarMapPoint->VarX = mapXPlane(VarMapObj->GetObjHandle(), number, subobj);
    VarMapPoint->VarY = mapYPlane(VarMapObj->GetObjHandle(), number, subobj);
  }
  else
  {
    OldPlane = VarMapPoint->VarPlaceIn;
    VarMapPoint->VarPlaceIn = PP_PICTURE;
    VarMapPoint->SetPoint(0, 0);
    VarMapPoint->VarPlaceIn = OldPlane;
  }

  return ret;
}

void QDMapMetric::SetPoints(int subobj, int number, QDMapPointAcces *mappoint)
{
  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0)  return;
  if (subobj < 0) return;
  if (number < 0) return;
  if (VarMapPoint->VarMapView != mappoint->VarMapView) return;

  if (VarMapPoint != mappoint) VarMapPoint->Assign(mappoint);
  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

//------------------------------------------------------------------
// Добавить точку в объект
//------------------------------------------------------------------
void QDMapMetric::Append(int subobj, QDMapPointAcces *mappoint)
{
  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0)  return;
  if (subobj < 0) return;

  mapAppendPointPlane(VarMapObj->GetObjHandle(), mappoint->VarX, mappoint->VarY, subobj);
  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

//------------------------------------------------------------------
// Удалить точку объекта
//------------------------------------------------------------------
void QDMapMetric::Delete(int subobj, int number)
{
  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0)  return;
  if (subobj < 0) return;
  if (number < 0) return;

  mapDeletePointPlane(VarMapObj->GetObjHandle(), number, subobj);
  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

//------------------------------------------------------------------
// Вставить точку
//------------------------------------------------------------------
void QDMapMetric::Insert(int subobj, int number, QDMapPointAcces *mappoint)
{
  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0)  return;
  if (subobj < 0) return;
  if (number < 0) return;

  mapInsertPointPlane(VarMapObj->GetObjHandle(), mappoint->VarX, mappoint->VarY, number, subobj);
  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

//------------------------------------------------------------------
// Обновить точку объекта
//------------------------------------------------------------------
void QDMapMetric::Update(int subobj, int number, QDMapPointAcces *mappoint)
{
  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0)  return;
  if (subobj < 0) return;
  if (number < 1) return;

  mapUpdatePointPlane(VarMapObj->GetObjHandle(), mappoint->VarX, mappoint->VarY, number, subobj);
  ((QDMapObj *)VarMapObj)->VarOldFrame = TRUE;
}

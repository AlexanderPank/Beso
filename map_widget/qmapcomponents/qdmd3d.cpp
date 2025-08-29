/******** QDMD3D.CPP ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
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

#include "qdmd3d.h"
#include "mapapi.h"
#include "m3dcom.h"
#include "maplib.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

// Главный обработчик сообщений
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++          КЛАСС ДИАЛОГА                       +
//+++  "ОТОБРАЖЕНИЕ ТРЕХМЕРНОЙ МОДЕЛИ МЕСТНОСТИ"   +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMap3D::QDMap3D(QWidget *parent, const char *name)
  :QDMapComponent(parent, name)
{
  hide();
  AllRectActive = true;
  MapRect.UP = 0;
  MapRect.LT = 0;
  MapRect.DN = 0;
  MapRect.RT = 0;
  LibInst    = 0;
}

//------------------------------------------------------------------
// Указатель на карту
//------------------------------------------------------------------
QDMapView *QDMap3D::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMap3D::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void QDMap3D::InitTaskParm(TASKPARMEX *taskparm)
{
  if (taskparm == 0) return;

  // Заполнение параметров прикладной задачи
  memset((char *)taskparm, 0, sizeof(TASKPARMEX));
  taskparm->Language  = mapGetMapAccessLanguage();
  taskparm->Handle = &MessageHandler;

  QDMapView *MapView = GetMapView();
  HMAP mapHandle = MapView->GetMapHandle();

  if (AllRectActive)
  {
    // Область вывода - весь район
    // Определение габаритов района в метрах
    POINT viewPoint;
    viewPoint.x = 0;
    viewPoint.y = 0;
    MapView->TranslatePicturePointFromViewFrame(&viewPoint);

    double x = viewPoint.x;  // Габариты района в метрах
    double y = viewPoint.y;

    mapPictureToPlane(mapHandle, &x, &y);
    MapRect.left = x;
    MapRect.top = y;

    int width, height;  // Габариты района в пикселах
    MapView->GetPictureSize(&width, &height);
    x = width;
    y = height;
    mapPictureToPlane(mapHandle, &x, &y);
    MapRect.right = MapRect.left;
    MapRect.left = x;
    MapRect.bottom = y;
  }
  else
  {
    // Область вывода из Rect
    double x = MapRect.left;
    double y = MapRect.top;
    mapPictureToPlane(mapHandle, &x, &y);
    MapRect.left = x;
    MapRect.top = y;

    x = MapRect.right;
    y = MapRect.bottom;
    mapPictureToPlane(mapHandle, &x, &y);
    MapRect.right = MapRect.left;
    MapRect.left = x;
    MapRect.bottom = y;
  }
}

//------------------------------------------------------------------
// Выполнить диалог
//------------------------------------------------------------------
void QDMap3D::Execute()
{
  const char *NameFunc  = "ShowMap3D";

  long int   (WINAPI * ShowMap3D)(HMAP hMap,TASKPARMEX* taskparm, RECT *rect);

  QDMapView   *MapView;
  TASKPARMEX  TaskParm;

  MapView = GetMapView();
  if (MapView          == NULL) return;
  if (MapView->GetMapHandle() == 0) return;

  if (mapGetListCount(MapView->GetMapHandle()) == 0 &&
      mapGetSiteCount(MapView->GetMapHandle()) == 0)
  {
    return;
  }

  if (LibInst != 0) CloseDialog3D();

  ShowMap3D = (long int (WINAPI *)(HMAP hMap, TASKPARMEX* taskparm, RECT *rect))
      mapLoadLibrary(MAP3DWINLIB, &LibInst, NameFunc);
  if (ShowMap3D == NULL) return;

  // Заполнение параметров прикладной задачи
  InitTaskParm(&TaskParm);

  ShowMap3D(MapView->GetMapHandle(), &TaskParm, &MapRect);

  return;
}

//------------------------------------------------------------------
// Закрыть диалог
//------------------------------------------------------------------
void QDMap3D::CloseDialog3D()
{
  if (LibInst == 0) return;

  const char *NameFunc  = "CloseMap3D";
  long int   (WINAPI * CloseMap3D)();

  CloseMap3D = (long int (WINAPI *)())
      mapGetProcAddress(LibInst, NameFunc);
  if (CloseMap3D == NULL) return;

  CloseMap3D();
  // Выгрузка библиотеки
  mapFreeLibrary(LibInst);
  LibInst = 0;
}

//------------------------------------------------------------------
// Установить размеры области карты в пикселах
//------------------------------------------------------------------
void QDMap3D::SetMapRect(int left, int top, int right, int bottom)
{
  MapRect.UP = top;
  MapRect.LT = left;
  MapRect.DN = bottom;
  MapRect.RT = right;

  GetMapView()->TranslatePictureRectFromViewFrame(&MapRect);
}

//------------------------------------------------------------------
// Установить\Запросить значение флага выбора области карты
//------------------------------------------------------------------
void QDMap3D::SetAllRectActive(bool value)
{
  AllRectActive = value;
}

bool QDMap3D::GetAllRectActive() const
{
  return AllRectActive;
}

void QDMap3D::MapAction(int acttype)
{
  switch (acttype)
  {
    case CM_T3DSTOP:
      CloseDialog3D();
      break;
  }
}

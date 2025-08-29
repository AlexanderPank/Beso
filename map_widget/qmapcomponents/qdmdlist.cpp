/******** QDMDLIST.CPP *********************************************
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
*        КОМПОНЕНТ - ДИАЛОГ "СПИСОК ДАННЫХ ЭЛЕКТРОННОЙ КАРТЫ"      *
*                                                                  *
*******************************************************************/

#include "qdmdlist.h"
#include "qdmcmp.h"
#include "mapapi.h"
#include "maplib.h"

// Главный обработчик сообщений
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГА                                +
//+++ "СПИСОК ДАННЫХ ЭЛЕКТРОННОЙ КАРТЫ"            +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapDataList::QDMapDataList(QWidget *parent, const char *name)
  :QDMapComponent(parent, name)
{
  hide();
}

//------------------------------------------------------------------
// Указатель на карту
//------------------------------------------------------------------
QDMapView *QDMapDataList::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapDataList::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void QDMapDataList::InitTaskParm(TASKPARM *taskparm, MAPLISTPARM* maplistparm)
{
  if (!taskparm || !maplistparm) return;

  // Заполнение параметров прикладной задачи
  memset((char *)taskparm, 0, sizeof(TASKPARM));
  taskparm->Language  = ML_RUSSIAN;
  taskparm->Handle = &MessageHandler;
  memset((char *)maplistparm, 0, sizeof(MAPLISTPARM));
  maplistparm->Window = (HWND)GetMapView();
}

void QDMapDataList::InitTaskParmEx(TASKPARMEX *taskparm, MAPLISTPARMUN* maplistparm)
{
  if (!taskparm || !maplistparm) return;

  // Заполнение параметров прикладной задачи
  memset((char *)taskparm, 0, sizeof(TASKPARMEX));
  taskparm->Language  = ML_RUSSIAN;
  taskparm->Handle = &MessageHandler;
  memset((char *)maplistparm, 0, sizeof(MAPLISTPARMUN));
  maplistparm->Window = (HWND)GetMapView();
}

//------------------------------------------------------------------
// Выполнить диалог
//------------------------------------------------------------------
void QDMapDataList::Execute()
{
  const char *NameFunc  = "LoadMapListDialogUn";

  long int   (WINAPI * LoadMapListDialog)(HMAP hMap,  MAPLISTPARMUN* maplistparm,
                                          TASKPARMEX* parm);
  HINSTANCE   libinst;
  QDMapView   *MapView;
  TASKPARMEX    TaskParm;
  MAPLISTPARMUN maplistparm;

  MapView = GetMapView();
  if (MapView          == NULL) return;
  if (MapView->GetMapHandle() == 0) return;

  MapView->GetMapLeftTopPlane(&MapView->MapLeftTopPlane.X, &MapView->MapLeftTopPlane.Y);

  LoadMapListDialog = (long int (WINAPI *)(HMAP hMap,  MAPLISTPARMUN* maplistparm, // 06/06/06
                                           TASKPARMEX* parm))
      mapLoadLibrary(MAPQTFRMLIB , &libinst, NameFunc);
  if (LoadMapListDialog == NULL) return;

  // Заполнение параметров прикладной задачи
  InitTaskParmEx(&TaskParm, &maplistparm);
  LoadMapListDialog(MapView->GetMapHandle(), &maplistparm, &TaskParm);

  MapView->UpdatePictureBorder();

  // Выгрузка библиотеки
  if (libinst)    mapFreeLibrary(libinst);

  return;
}

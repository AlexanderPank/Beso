/******** QDMRST.CPP ***********************************************
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
*                   КЛАССЫ ДОСТУПА К РАСТРУ                        *
*                                                                  *
*******************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>

#include "qdmrst.h"
#include "qdmviewa.h"
#include "mapapi.h"
#include "maplib.h"

//++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К РАСТРУ          +++
//++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapRst::QDMapRst(QDMapViewAcces* mapview)
{
  VarMapView = mapview;
  VarNumber  = 0;
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapRst::~QDMapRst()
{
}

//------------------------------------------------------------------
// Имя растровой карты
//------------------------------------------------------------------
char *QDMapRst::GetRstFileName()
{
  if ((VarNumber == 0) || (VarMapView == NULL)) return 0;
  if (VarMapView->GetMapHandle() == 0) return 0;
  return (char *)mapGetRstName(VarMapView->GetMapHandle(), VarNumber);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К РАСТРУ - КОНТЕЙНЕР         +++
//+++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapRsts::QDMapRsts(QDMapViewAcces *mapview)
{
  VarMapRst = new QDMapRst(mapview);
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapRsts::~QDMapRsts()
{
  delete VarMapRst;
  VarMapRst = 0;
}

//------------------------------------------------------------------
// Количество файлов растровой информации в списке
//------------------------------------------------------------------
int QDMapRsts::GetRstCount()
{
  if (VarMapRst->VarMapView == NULL) return 0;
  if (VarMapRst->VarMapView->GetMapHandle() == 0) return 0;

  return mapGetRstCount(VarMapRst->VarMapView->GetMapHandle());
}

//------------------------------------------------------------------
// Доступ к файлам растровой информации по порядковому номеру
//------------------------------------------------------------------
QDMapRst *QDMapRsts::GetMapRsts(int number)
{
  VarMapRst->VarNumber = number;
  return VarMapRst;
}

//------------------------------------------------------------------
// Доступ к файлам растровой информации по наименованию
//------------------------------------------------------------------
QDMapRst *QDMapRsts::GetNameRsts(char *rstname)
{
  int  i;
  char *st;

  for (i = 1;i <= GetRstCount();i++)
  {
    VarMapRst->VarNumber = i;
    st = VarMapRst->GetRstFileName();
    if (strcmp(rstname, st) == 0) return VarMapRst;
  }

  VarMapRst->VarNumber = 0;
  return VarMapRst;
}

//------------------------------------------------------------------
// Добавить растровую карту
//------------------------------------------------------------------
int QDMapRsts::Append(char *rstfilename)
{
  int ret;
  if (VarMapRst->VarMapView == NULL) return 0;
  if (VarMapRst->VarMapView->GetMapHandle() == 0) return 0;

  ret = mapOpenRstForMap(VarMapRst->VarMapView->GetMapHandle(), rstfilename, 0);

  if (ret != 0)
  {
    VarMapRst->VarNumber = ret;
//    if Assigned(TMapView(FMapRst.FMapView).FOnAppendRst) then
//    TMapView(FMapRst.FMapView).FOnAppendRst(Self,FMapRst);
    return ret;
  }
  return 0;
}

//------------------------------------------------------------------
// Удалить растровую карту по наименованию
//------------------------------------------------------------------
void QDMapRsts::DeleteName(char *rstfilename)
{
  int  i;
  char *st;

  if (VarMapRst->VarMapView == NULL) return;
  if (VarMapRst->VarMapView->GetMapHandle() == 0) return;

  for (i = 1;i <= GetRstCount();i++)
  {
    VarMapRst->VarNumber = i;
    st = VarMapRst->GetRstFileName();
    if (strcmp(rstfilename, st) == 0)
    {
      mapCloseRstForMap(VarMapRst->VarMapView->GetMapHandle(), i);
      break;
    }
    else VarMapRst->VarNumber = 0;
  }
}

//------------------------------------------------------------------
// Удалить растровую карту по порядковому номеру
// Внимание! При удалениии растровой карты с младшим номером
// старшие номера смещаются.
//------------------------------------------------------------------
void QDMapRsts::Delete(int number)
{
  if (VarMapRst->VarMapView == NULL)
    return;
  if (VarMapRst->VarMapView->GetMapHandle() == 0)
    return;

  VarMapRst->VarNumber = number;
  mapCloseRstForMap(VarMapRst->VarMapView->GetMapHandle(), number);
}

#define IDD_MAPMAINMENU                   0x400
#define IDS_MENU_SEARCH                   IDD_MAPMAINMENU + 75
#define IDS_FILE_IMPORTGDAL               IDS_MENU_SEARCH+24

//------------------------------------------------------------------
// Сконвертировать растр из изображения                // 26/10/16
//------------------------------------------------------------------
int QDMapRsts::ImageToRstDialog(char *inname, char *outname)
{
  if (inname == 0 || *inname == 0 || outname == 0) return 0;

  HINSTANCE libinst = 0;
  TASKPARM  parm;
  typedef long int (WINAPI * LoadImageToRsw)(HMAP hmap, char* lpszsource,
                                             char* lpsztarget, TASKPARM* parm);

  LoadImageToRsw lpfn_LoadImageToRsw = (long int (WINAPI *)(HMAP hmap, char* lpszsource,
                                             char* lpsztarget, TASKPARM* parm))
                              ::mapLoadLibrary(MAPPICTRLIB, &libinst, "LoadImageToRsw");
  memset(&parm, 0, sizeof(parm));
  parm.Resource = (HINSTANCE) VarMapRst->VarMapView->GetMapViewAcces();

  int ret = 0;
  if (lpfn_LoadImageToRsw)
  {
    HMAP hmap = 0;
    if (VarMapRst->VarMapView && VarMapRst->VarMapView->GetMapHandle())
      hmap = VarMapRst->VarMapView->GetMapHandle();
    ret = lpfn_LoadImageToRsw(hmap, inname, outname, &parm);
    mapFreeLibrary(libinst);
    libinst = 0;
  }

  return ret;
}


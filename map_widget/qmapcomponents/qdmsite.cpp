/******* QDMSITE.CPP ***********************************************
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
*       ОПИСАНИЕ КЛАССОВ ДОСТУПА К ПОЛЬЗОВАТЕЛЬСКОЙ КАРТЕ          *
*                                                                  *
*******************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>

#include "qdmsite.h"
#include "qdmviewa.h"
#include "qdmpoint.h"
#include "qdmlayr.h"
#include "mapapi.h"

//++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДОСТУПА К ПОЛЬЗОВАТЕЛЬСКОЙ КАРТЕ +++
//++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSite::QDMapSite(QDMapViewAcces* mapview)
{
  VarMapView    = mapview;
  VarSite       = 0;
  VarMapPoint   = new QDMapPointAcces(VarMapView);
  VarSiteLayers = new QDMapLayers(VarMapView, this);
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapSite::~QDMapSite()
{
  delete VarMapPoint;
  delete VarSiteLayers;
}

//------------------------------------------------------------------
// Номенклатура пользовательской карты обстановки
//------------------------------------------------------------------
const char* QDMapSite::GetSiteListName(int listnumber)
{
  char* ret = 0;

  if (VarSite == 0)
    return ret;
  if (VarMapView == NULL)
    return ret;
  if (VarMapView->GetMapHandle() == 0)
    return ret;
  if (listnumber < 1)
    return ret;

  return (ret = (char *)mapGetSiteListName(VarMapView->GetMapHandle(), VarSite,
                                           listnumber));
}

//------------------------------------------------------------------
// Имя пользовательской карты обстановки
//------------------------------------------------------------------
char *QDMapSite::GetSiteName()
{
  char *ret = 0;

  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0)  return ret;
  return (ret = (char *)mapGetSiteName(VarMapView->GetMapHandle(), VarSite));
}

//------------------------------------------------------------------
// Номер пользовательской карты обстановки в цепочке
//------------------------------------------------------------------
int QDMapSite::GetNumber()
{
  int ret = 0;
  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0)  return ret;

  return (ret = mapGetSiteNumber(VarMapView->GetMapHandle(), VarSite));
}

//------------------------------------------------------------------
// Габариты пользовательской карты обстановки
//------------------------------------------------------------------
MAPDFRAME QDMapSite::GetSiteBorder(int place)
{
  MAPDFRAME ret;

  memset((char *)&ret, 0, sizeof(MAPDFRAME));
  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0)  return ret;

  ret.X1 = mapGetSiteX1(VarMapView->GetMapHandle(), VarSite);
  ret.Y1 = mapGetSiteY1(VarMapView->GetMapHandle(), VarSite);
  ret.X2 = mapGetSiteX2(VarMapView->GetMapHandle(), VarSite);
  ret.Y2 = mapGetSiteY2(VarMapView->GetMapHandle(), VarSite);

  switch (place)
  {
    case PP_PICTURE :
        mapPlaneToPicture(VarMapView->GetMapHandle(), &ret.X1, &ret.Y1);
        mapPlaneToPicture(VarMapView->GetMapHandle(), &ret.X2, &ret.Y2);
    break;
    case PP_GEO     :
        mapPlaneToGeo(VarMapView->GetMapHandle(), &ret.X1, &ret.Y1);
        mapPlaneToGeo(VarMapView->GetMapHandle(), &ret.X2, &ret.Y2);
    break;
    case PP_PLANE   : break;
  }

  return ret;
}

//------------------------------------------------------------------
// Имя файла пользовательской карты обстановки
//------------------------------------------------------------------
char *QDMapSite::GetSiteFileName()
{
  char *ret = 0;

  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0)  return ret;

  return (ret = (char *)mapGetSiteFileName(VarMapView->GetMapHandle(), VarSite));
}

//------------------------------------------------------------------
// Базовый масштаб пользовательской карты обстановки
//------------------------------------------------------------------
int QDMapSite::GetSiteScale()
{
  int ret = 0;

  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0)  return ret;

  return (ret = mapGetSiteScale(VarMapView->GetMapHandle(), VarSite));
}

//------------------------------------------------------------------
// Тип пользовательской карты обстановки
//------------------------------------------------------------------
int QDMapSite::GetSiteType()
{
  int ret = 0;

  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0)  return ret;

  return (ret = mapGetSiteType(VarMapView->GetMapHandle(), VarSite));
}

//------------------------------------------------------------------
// Получить идентификатор пользовательской карты
//------------------------------------------------------------------
HSITE QDMapSite::GetSiteHandle()
{
  return VarSite;
}

//------------------------------------------------------------------
// Установить новые габариты пользовательской карты с изменением параметров скролинга
// Функция обновляет габариты пользовательской карты и габариты всего района
// если одновременно открыто несколько карт
// При ошибке возвращает 0
//------------------------------------------------------------------
int QDMapSite::SetSiteBorder(MAPDFRAME *frame, int place)
{
  int       ret = 0;
  QDMapView *vmap;

  if (VarSite == 0) return ret;
  if (VarMapView == NULL) return ret;
  if (VarMapView->GetMapHandle() == 0) return ret;
  if ((frame->X1 - frame->X2) == 0)return ret;

  ret = mapSetSiteBorder(VarMapView->GetMapHandle(), VarSite, frame, place);
  if (ret == 0) return ret;

  vmap = (QDMapView *)VarMapView;
  vmap->UpdatePictureBorderForAll();

  if (vmap->GetMapVisible() == FALSE) return ret; // карта не отображается

//  vmap->repaint();  // 15/11/07 делается в UpdatePictureBorderForAll()
  return ret;
}

//------------------------------------------------------------------
// Отображение карты включить/выключить
//------------------------------------------------------------------
void QDMapSite::SetSiteVisible(bool visible)
{
 int ii;

 if ((VarMapView->GetMapHandle() == 0) || (VarSite == 0)) return;

 if (visible) ii = 1;
 else ii = 0;
 mapSetSiteViewFlag(VarMapView->GetMapHandle(), VarSite, ii);
}

bool QDMapSite::GetSiteVisible()
{
 bool ret = FALSE;

 if ((VarMapView->GetMapHandle() == 0) || (VarSite == 0)) return ret;

 if (mapGetSiteViewFlag(VarMapView->GetMapHandle(), VarSite)) ret = TRUE;

 return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДОСТУПА К ПОЛЬЗОВАТЕЛЬСКОЙ КАРТЕ - КОНТЕЙНЕР +++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSites::QDMapSites(QDMapViewAcces *mapview)
{
  VarMapSite = new QDMapSite(mapview);
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapSites::~QDMapSites()
{
  delete VarMapSite;
}

// Количество пользовательских карт обстановки в списке
int QDMapSites::GetSiteCount()
{
  int ret = 0;

  if (VarMapSite->VarMapView == NULL) return ret;

  if (VarMapSite->VarMapView->GetMapHandle() == 0) return ret;

  ret = mapGetSiteCount(VarMapSite->VarMapView->GetMapHandle());    // 22/03/12

  return ret;
}

// Доступ к пользовательской карте обстановки по порядковому номеру
QDMapSite *QDMapSites::GetMapSites(int number)
{
  VarMapSite->VarSite = mapGetSiteIdent(VarMapSite->VarMapView->GetMapHandle(), number);
  return VarMapSite;
}

// Получить указатель на MapSite по имени сайта
QDMapSite *QDMapSites::GetSitesForName(char *sitename)
{
  int  i;
  char *st;
  QDMapSite *ret = VarMapSite;

  for (i = 1; i <= GetSiteCount(); i++)
  {
    VarMapSite->VarSite = mapGetSiteIdent(VarMapSite->VarMapView->GetMapHandle(), i);
    st = VarMapSite->GetSiteName();
    if (strcmp(sitename, st) == 0) return ret;
  }
  VarMapSite->VarSite  = 0;

  return ret;
}

// Получить указатель на MapSite по полному имени файла
QDMapSite *QDMapSites::GetSitesForFileName(char *sitefilename)
{
  int  i;
  char *st;

  QDMapSite *ret = VarMapSite;

  for (i = 1; i <= GetSiteCount(); i++)
  {
    VarMapSite->VarSite = mapGetSiteIdent(VarMapSite->VarMapView->GetMapHandle(), i);
    st = VarMapSite->GetSiteFileName();
    if (strcmp(sitefilename, st) == 0) return ret;
  }
  VarMapSite->VarSite  = 0;

  return ret;
}

// Добавить пользовательскую карту обстановки
int QDMapSites::Append(char *sitefilename)
{
  int ret = 0;

  if (VarMapSite->VarMapView == NULL) return ret;
  if (VarMapSite->VarMapView->GetMapHandle() == 0) return ret;

  HSITE openedSite = mapOpenSiteForMap(VarMapSite->VarMapView->GetMapHandle(), sitefilename, 0);
  if (openedSite)
  {
     VarMapSite->VarSite = openedSite;
     ((QDMapView *)VarMapSite->VarMapView)->AllMapRef(ACTAFTERAPPENDSITE);
     ret = VarMapSite->GetNumber();
     ((QDMapView *)VarMapSite->VarMapView)->AllMapRef(ACTDOREPAINTFORCHANGEDATA);
  }

  return ret;
}


// Удалить карту из списка по ее номеру в списке
void QDMapSites::Delete(int number)
{

  HMAP oMap;

  if (VarMapSite->VarMapView == NULL) return ;
  if (VarMapSite->VarMapView->GetMapHandle() == 0) return;

  oMap = VarMapSite->VarMapView->GetMapHandle();
  VarMapSite->VarSite = mapGetSiteIdent(oMap, number);
  ((QDMapView *)VarMapSite->VarMapView)->AllMapRef(ACTBEFOREDELETESITE);
  mapCloseSiteForMap(oMap, VarMapSite->VarSite);
  ((QDMapView *)VarMapSite->VarMapView)->AllMapRef(ACTDOREPAINTFORCHANGEDATA);
}

// Удалить пользовательскую карту обстановки по полному имени файла
void QDMapSites::DeleteFileName(char *sitefilename)
{
  int  ii;
  char *st;
  HMAP oMap;

  if (VarMapSite->VarMapView == NULL) return ;
  if (VarMapSite->VarMapView->GetMapHandle() == 0) return;

  oMap = VarMapSite->VarMapView->GetMapHandle();

  for (ii = 1; ii <= GetSiteCount(); ii++)
  {
    VarMapSite->VarSite = mapGetSiteIdent(oMap, ii);
    st = VarMapSite->GetSiteFileName();
    if (!st || (strcmp(sitefilename, st) == 0))                 // 18/10/05
    {
       Delete(ii);
       break;
    }
    else VarMapSite->VarSite = 0;
  }
}

// Удалить пользовательскую карту обстановки по наименованию
void QDMapSites::DeleteSiteName(char *sitename)
{
  int  ii;
  char *st;
  HMAP oMap;

  if (VarMapSite->VarMapView == NULL) return ;
  if (VarMapSite->VarMapView->GetMapHandle() == 0) return;

  oMap = VarMapSite->VarMapView->GetMapHandle();

  for (ii = 1; ii <= GetSiteCount(); ii++)
  {
    VarMapSite->VarSite = mapGetSiteIdent(oMap, ii);
    st = VarMapSite->GetSiteName();
    if (!st || (strcmp(sitename, st) == 0))                      // 18/10/05
    {
       Delete(ii);
       break;
    }
    else VarMapSite->VarSite = 0;
  }
}

/******** QDMSELT.CPP **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                        FOR Qt-DESIGNER                           *
*                                                                  *
********************************************************************
*                                                                  *
*           КЛАСС КОНТЕКСТА УСЛОВИЙ ПОИСКА/ОТОБРАЖЕНИЯ             *
*                                                                  *
*******************************************************************/

#include <assert.h>

#include "qdmpoina.h"
#include "qdmobja.h"
#include "qdmsite.h"
#include "qdmselt.h"
#include "qdmcmp.h"
#include "mapapi.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС КОНТЕКСТА УСЛОВИЙ ПОИСКА/ОТОБРАЖЕНИЯ +++
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSelect::QDMapSelect(QDMapViewAcces* mapview)
{
  CountSelect = 0;
  memset(VarListSelect, 0, sizeof(VarListSelect));
  memset(VarListUses, 0, sizeof(VarListUses));

  VarEdit    = false;
  VarMapView = NULL;
  memset((char *)&VarMapRect, 0, sizeof(VarMapRect));
  VarSite = 0;
  VarMapView =  0;
  VarMapPoint = new QDMapPointAcces(0);
  SetMapView(mapview);

  memset(StringOut, 0, sizeof(StringOut));
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapSelect::~QDMapSelect()
{
  for (int ii = 0; ii < CountSelect; ii++)
    if (VarListSelect[ii] != 0)
    {
      mapDeleteSelectContext(VarListSelect[ii]);
      VarListSelect[ii] = 0;
    }

  CountSelect = 0;
  memset(VarListSelect, 0, sizeof(VarListSelect));
  memset(VarListUses, 0, sizeof(VarListUses));
  VarSite = 0;
  delete VarMapPoint;
}

//------------------------------------------------------------------
// Переустанавливаем MapView
//------------------------------------------------------------------
void QDMapSelect::SetMapView(QDMapViewAcces *value)
{
  if (VarMapView == value)
    return;

  for (int ii = CountSelect; ii >= 0; ii--)
  {
    if (VarListSelect[ii] != 0)
    {
      mapDeleteSelectContext((HSELECT)(VarListSelect[ii]));
      VarListSelect[ii] = 0;
    }
  }

  CountSelect = 0;
  VarSite = 0;
  memset(VarListSelect, 0, sizeof(VarListSelect));
  memset(VarListUses, 0, sizeof(VarListUses));

  VarMapView = value;
  VarMapPoint->SetMapView(VarMapView);

  if ((VarMapView != NULL) && (VarMapView->GetMapHandle() != 0))
    SetCountListSelect();
}

//------------------------------------------------------------------
// Запросить Hselect для карты number
// если такового нет то он будет создан
//------------------------------------------------------------------
HSELECT QDMapSelect::GetHSelect(int number)
{
  HSITE site1;
  int   maxsite;

  if (VarMapView == NULL) return 0;
  if (VarMapView->GetMapHandle() == 0) return 0;

  maxsite = ((QDMapView *)VarMapView)->VarMapSites->GetSiteCount();

  if ((number < 0) || (number > maxsite)) return 0;
  SetCountListSelect();

  assert(number < MAX_SELECT);

  if (VarListSelect[number] == 0)
  {
    if (number == 0) site1 = VarMapView->GetMapHandle();
    else site1 = mapGetSiteIdent(VarMapView->GetMapHandle(), number);

    VarListSelect[number] = mapCreateSiteSelectContext(VarMapView->GetMapHandle(), site1);
    VarListUses[number]   = true;
  }

  return VarListSelect[number];
}

//------------------------------------------------------------------
// Проверить видимость объектов
//------------------------------------------------------------------
bool QDMapSelect::GetCheckKey(int key)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0)
    return FALSE;
  return (bool)mapCheckKey(hs, key);
}

//------------------------------------------------------------------
// Включить/выключить объекты по внутреннему коду
//------------------------------------------------------------------
void QDMapSelect::SetIncode(int incode, bool value)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0) return;
  mapSelectObject(hs, incode, (int)value);
  VarEdit = TRUE;
}

bool QDMapSelect::GetIncode(int incode)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (hs == 0) return FALSE;
  return (bool)mapCheckObject(hs, incode);
}

//------------------------------------------------------------------
// Включить/выключить объекты по внешнему коду и локализации
//------------------------------------------------------------------
void QDMapSelect::SetExcode(int excode, int local, bool value)
{
  int   incode;
  HSITE site1;

  site1 = mapGetSiteIdent(VarMapView->GetMapHandle(), VarSite);
  if (!site1) return;

  if (excode == -1) incode = -1;
  else incode = mapSiteRscObjectCode(VarMapView->GetMapHandle(), site1, excode, local);

  SetIncode(incode, value);
}

bool QDMapSelect::GetExcode(int excode, int local)
{
  int   incode;
  HSITE site1;

  site1 = mapGetSiteIdent(VarMapView->GetMapHandle(), VarSite);
  if (!site1) return FALSE;

  incode = mapSiteRscObjectCode(VarMapView->GetMapHandle(),
                                site1, excode, local);
  return GetIncode(incode);
}

//------------------------------------------------------------------
// Включить/выключить объекты по ключу (уникальному названию объекта в классификаторе)
//------------------------------------------------------------------
void  QDMapSelect::SetKeyObject(char *keyobject, bool value)
{
  int   incode;
  HSITE site1;
  HRSC  rsc1;

  site1 = mapGetSiteIdent(VarMapView->GetMapHandle(), VarSite);
  if (!site1) return;

  rsc1 = mapGetRscIdent(VarMapView->GetMapHandle(), site1);
  if (!rsc1) return ;

  if (keyobject == 0 || strlen(keyobject) == 0)
    incode = -1;
  else
    incode = mapGetRscObjectKeyIncode(rsc1, keyobject);

  SetIncode(incode, value);
}

bool QDMapSelect::GetKeyObject(char *keyobject)
{
  int   incode;
  HSITE site1;
  HRSC  rsc1;

  site1 = mapGetSiteIdent(VarMapView->GetMapHandle(), VarSite);
  if (!site1) return FALSE;

  rsc1 = mapGetRscIdent(VarMapView->GetMapHandle(), site1);
  if (!rsc1) return  FALSE;

  incode = mapGetRscObjectKeyIncode(rsc1, keyobject);

  return GetIncode(incode);
}

//------------------------------------------------------------------
// Включить/выключить слой (сегмент)
//------------------------------------------------------------------
void QDMapSelect::SetLayers(int number, bool value)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return;
  mapSelectLayer(hs, number, (int)value);
  VarEdit = TRUE;
}

bool QDMapSelect::GetLayers(int number)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return FALSE;
  return (bool)mapCheckLayer(hs, number);
}

// Включить/выключить объекты по характеру локализации

void QDMapSelect::SetLocal(int objlocal, bool value)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return;
  mapSelectLocal(hs, objlocal, (int)value);
  VarEdit = TRUE;
}

bool QDMapSelect::GetLocal(int objlocal)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs)
    return FALSE;
  return (bool)mapCheckLayer(hs, objlocal);
}

//------------------------------------------------------------------
// Установить максимальную границу номеров объектов
//------------------------------------------------------------------
void QDMapSelect::SetMaxKey(DWORD value)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return;
  mapSelectKey(hs, mapGetMinKey(hs), value);
  VarEdit = TRUE;
}

DWORD QDMapSelect::GetMaxKey()
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return 0;
  return mapGetMaxKey(hs);
}

//------------------------------------------------------------------
// Установить минимальную границу номеров объектов
//------------------------------------------------------------------
void QDMapSelect::SetMinKey(DWORD value)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return;
  mapSelectKey(hs, value, mapGetMaxKey(hs));
  VarEdit = TRUE;
}

DWORD QDMapSelect::GetMinKey()
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return 0;
  return mapGetMinKey(hs);
}

//------------------------------------------------------------------
// Установить доступ к объектам с заданными номерами
// Если оба числа равны -1, то устанавливается доступ ко всем номерам.
//------------------------------------------------------------------
void QDMapSelect::SetSelectKey(int aMin, int aMax)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return;
  mapSelectKey(hs, aMin, aMax);
  VarEdit = TRUE;
}

//------------------------------------------------------------------
// Включить/выключить лист карты
//------------------------------------------------------------------
void QDMapSelect::SetLists(int number, bool value)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return;

  if (VarSite) return;
  mapSelectList(hs, number, (int)value);
  VarEdit = TRUE;
}

bool QDMapSelect::GetLists(int number)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (!hs) return FALSE;

  if (VarSite) return FALSE;
  return (bool)mapCheckList(hs, number);
}

//------------------------------------------------------------------
// Координаты района отображения/печати (в пикселах)
//------------------------------------------------------------------
void QDMapSelect::SetLeftTop(double left, double top)
{
  HSELECT hs;
  int     oldplaceout = VarMapPoint->VarPlaceOut;

  hs = GetHSelect(VarSite);
  if (!hs) return;

  VarMapPoint->VarPlaceOut = PP_PICTURE;
  if (VarMapPoint->VarPlaceIn == PP_PICTURE)
  VarMapPoint->SetPoint(left, top);
  else VarMapPoint->SetPoint(top, left);
  VarMapRect.Y1 = VarMapPoint->VarY;
  VarMapRect.X2 = VarMapPoint->VarX;
  VarEdit = TRUE;
  VarMapPoint->VarPlaceOut = oldplaceout;
}

void QDMapSelect::SetRightBottom(double right, double bottom)
{
  HSELECT hs;
  int oldplaceout = VarMapPoint->VarPlaceOut;

  hs = GetHSelect(VarSite);
  if (!hs) return;

  VarMapPoint->VarPlaceOut = PP_PICTURE;
  if (VarMapPoint->VarPlaceIn == PP_PICTURE)
  VarMapPoint->SetPoint(right, bottom);
  else VarMapPoint->SetPoint(bottom, right);
  VarMapRect.Y2 = VarMapPoint->VarY;
  VarMapRect.X1 = VarMapPoint->VarX;
  VarEdit = TRUE;
  VarMapPoint->VarPlaceOut = oldplaceout;
}

void QDMapSelect::GetLeftTop(double *left, double *top)
{
  HSELECT hs;
  double  value;

  hs = GetHSelect(VarSite);
  if (!hs) return;

  VarMapPoint->VarX = VarMapRect.X1;
  VarMapPoint->VarY = VarMapRect.Y1;

  if (VarMapPoint->VarPlaceOut == PP_PICTURE)
    VarMapPoint->GetPoint(left, &value);
  else VarMapPoint->GetPoint(&value, left);

  VarMapPoint->VarX = VarMapRect.X2;
  VarMapPoint->VarY = VarMapRect.Y2;

  if (VarMapPoint->VarPlaceOut == PP_PICTURE)
    VarMapPoint->GetPoint(&value, top);
  else VarMapPoint->GetPoint(top, &value);
}

void QDMapSelect::GetRightBottom(double *right, double *bottom)
{
  HSELECT hs;
  double  value;

  hs = GetHSelect(VarSite);
  if (!hs) return;

  VarMapPoint->VarX = VarMapRect.X2;
  VarMapPoint->VarY = VarMapRect.Y2;

  if (VarMapPoint->VarPlaceOut == PP_PICTURE)
    VarMapPoint->GetPoint(right, &value);
  else VarMapPoint->GetPoint(&value, right);

  VarMapPoint->VarX = VarMapRect.X1;
  VarMapPoint->VarY = VarMapRect.Y1;

  if (VarMapPoint->VarPlaceOut == PP_PICTURE)
    VarMapPoint->GetPoint(&value, bottom);
  else VarMapPoint->GetPoint(bottom, &value);
}

//------------------------------------------------------------------
// Система координат на запись
//------------------------------------------------------------------
void QDMapSelect::SetPlaceIn(int value)
{
  VarMapPoint->SetPlaceIn(value);
}

int QDMapSelect::GetPlaceIn()
{
  return VarMapPoint->GetPlaceIn();
}

//------------------------------------------------------------------
// Система координат на чтение
//------------------------------------------------------------------
void QDMapSelect::SetPlaceOut(int value)
{
  VarMapPoint->SetPlaceOut(value);
}

int QDMapSelect::GetPlaceOut()
{
  return VarMapPoint->GetPlaceOut();
}

//------------------------------------------------------------------
// Включить/Выключить из операции отбора указанную карту или все карты
//------------------------------------------------------------------
void QDMapSelect::SetMapSites(int number, bool value)
{
  int jj, ii,all;

  if ((number < -1) || (number > ((QDMapView *)VarMapView)->VarMapSites->GetSiteCount()))
    return;

  if (number == -1)
  {
     ii = 0; all = ((QDMapView *)VarMapView)->VarMapSites->GetSiteCount();
  }
  else
  {
    ii = number; all = number;
  }

  for (jj = ii; jj <= all; jj++)
  {
     GetHSelect(jj);
     VarListUses[jj] = value;
  }
}

bool QDMapSelect::GetMapSites(int number)
{
  HSELECT hs;

  hs = GetHSelect(number);
  if (!hs) return FALSE;
  return (bool)VarListUses[number];
}

//------------------------------------------------------------------
// Получить область селекции в пикселах
// Вход notnull = TRUE - если нет селекции то вернуть габариты списка карт
//------------------------------------------------------------------
RECT QDMapSelect::GetRect(bool notnull)
{
  int    OldPlace;
  RECT   FRect;
  double x, y;

  memset((char *)&FRect, 0, sizeof(FRect));
  if ((VarMapView == NULL) || (VarMapView->GetMapHandle() == 0)) return FRect;

  if ((notnull == TRUE) && (VarMapRect.X1 == VarMapRect.X2) && (VarMapRect.X1 == 0))
  {   // нет селекции
     FRect = ((QDMapView *)VarMapView)->GetRegionRect();
     return FRect;
  }

  if ((VarMapRect.X1 == VarMapRect.X2) || (VarMapRect.Y1 == VarMapRect.Y2))
  {
    VarMapRect.X1 = mapGetMapX1(VarMapView->GetMapHandle());
    VarMapRect.Y1 = mapGetMapY1(VarMapView->GetMapHandle());
    VarMapRect.X2 = mapGetMapX2(VarMapView->GetMapHandle());
    VarMapRect.Y2 = mapGetMapY2(VarMapView->GetMapHandle());
  }

  VarMapPoint->SetMapView(VarMapView);
  OldPlace = VarMapPoint->VarPlaceOut;
  VarMapPoint->VarPlaceOut = PP_PICTURE;
  VarMapPoint->VarX = VarMapRect.X1;
  VarMapPoint->VarY = VarMapRect.Y1;
  VarMapPoint->GetPoint(&x, &y);
  FRect.left        = x;
  FRect.bottom      = y;

  VarMapPoint->VarX = VarMapRect.X2;
  VarMapPoint->VarY = VarMapRect.Y2;
  VarMapPoint->GetPoint(&x, &y);
  FRect.right       = x;
  FRect.top         = y;
  VarMapPoint->VarPlaceOut = OldPlace;
  return FRect;
}

//------------------------------------------------------------------
// Вернуть установленный в Select габариты селекции в системе PlaceOut
// Вход NotNull = TRUE - если нет селекции то вернуть габариты списка карт
//------------------------------------------------------------------
MAPDFRAME QDMapSelect::GetDFrame(bool notnull)
{
  MAPDFRAME dframe;

  memset((char *)&dframe, 0, sizeof(dframe));
  if ((VarMapView == NULL) || (VarMapView->GetMapHandle() == 0)) return dframe;
  if ((notnull == TRUE) && (VarMapRect.X1 == VarMapRect.X2) && (VarMapRect.X1 == 0))
  {  // нет селекции
     dframe = ((QDMapView *)VarMapView)->GetRegionDFrame(GetPlaceOut());
     return dframe;
  }

  dframe = VarMapPoint->ConvertDFrame(VarMapRect, PP_PLANE, GetPlaceOut());
  return dframe;
}

//------------------------------------------------------------------
// Считать текущий контекст отображения  // Устаревшая
//------------------------------------------------------------------
void QDMapSelect::SetSelect()
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (hs == 0) return;
  mapGetSiteViewSelect(VarMapView->GetMapHandle(),
                       mapGetSiteIdent(VarMapView->GetMapHandle(), VarSite), hs);
  VarEdit = TRUE;
}

//------------------------------------------------------------------
// Сбросить все условия поиска/отображения для текущего
//------------------------------------------------------------------
void QDMapSelect::Clear()
{
  HSELECT hs;

  hs =  GetHSelect(VarSite);
  if (hs != 0)
  {
     mapDeleteSelectContext(hs);
     VarListSelect[VarSite] = 0;
  }

  GetHSelect(VarSite);
  VarListUses[VarSite] = true;
  VarEdit = TRUE;
  memset((char *)&VarMapRect, 0, sizeof(VarMapRect));
}

//------------------------------------------------------------------
// Сбросить все условия поиска/отображения для всех
//------------------------------------------------------------------
void QDMapSelect::ClearAll()
{
  for (int ii = 0; ii <= CountSelect; ii++)
  {
    VarSite = ii;
    Clear();
  }
}

//------------------------------------------------------------------
// Сбросить все условия поиска по семантике
//------------------------------------------------------------------
void QDMapSelect::ClearSemantic()
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (hs == 0) return;

  mapSelectSemanticClear(hs);
  VarEdit = TRUE;
}

//------------------------------------------------------------------
// Значение семантики
//------------------------------------------------------------------
char *QDMapSelect::GetSemanticValue(int number)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);
  if (hs == 0) return 0;

  memset(StringOut, 0, sizeof(StringOut));
  if ((number < 1 ) || (number > mapSelectSemanticCount(hs)))
    return 0;
  mapSelectSemanticValue(hs, number, StringOut, MAX_STRING);

  return StringOut;
}

//------------------------------------------------------------------
// Код семантики
//------------------------------------------------------------------
int QDMapSelect::GetSemanticCode(int number)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0) return 0;
  if ((number < 1 ) || (number > mapSelectSemanticCount(hs))) return 0;
  return mapSelectSemanticCode(hs, number);
}

//------------------------------------------------------------------
// Условие поиска по семантике
//------------------------------------------------------------------
int QDMapSelect::GetSemanticCondition(int number)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0) return 0;
  if ((number < 1 ) || (number > mapSelectSemanticCount(hs))) return 0;
  return mapSelectSemanticCondition(hs, number);
}

int QDMapSelect::GetSemanticCount()
{
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0) return 0;
  return mapSelectSemanticCount(hs);
}

//------------------------------------------------------------------
// Добавить условие по семантике
//------------------------------------------------------------------
int QDMapSelect::AddSemantic(int code, char *value, int condition)
{
  int ret = 0;
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0) return ret;
  ret = mapSelectSemanticAppend(hs, condition, code, value);
  VarEdit = TRUE;
  return ret;
}

//------------------------------------------------------------------
// Удалить условие по семантике
//------------------------------------------------------------------
void QDMapSelect::DeleteSemantic(int number)
{
  HSELECT hs;

  hs = GetHSelect(VarSite);

  if (hs == 0) return;
  if ((number < 1 ) || (number > mapSelectSemanticCount(hs))) return;

  mapSelectSemanticDelete(hs, number);
  VarEdit = TRUE;
}

//------------------------------------------------------------------
// Установить для текущей обработки номер карты
//------------------------------------------------------------------
void QDMapSelect::SetSiteNumber(int value)
{
  VarSite = -1;
  if (value > ((QDMapView *)VarMapView)->VarMapSites->GetSiteCount()) return;
  VarSite = value;
}

//------------------------------------------------------------------
// Обработка событий карты
//------------------------------------------------------------------
void QDMapSelect::MapAction(int action)
{
 QDMapView *FMV;
 int       ii;

 FMV = (QDMapView *)VarMapView;

//-------------------------
 if ((action == ACTAFTERAPPENDSITE) && (FMV->VarMapSites->VarMapSite->GetNumber() > 0))
 // если добавили VarSite в карту
 {
   SetCountListSelect();
   return;
 }

//-------------------------
 if ((action == ACTBEFOREDELETESITE) && (FMV->VarMapSites->VarMapSite->GetNumber() > 0))
  // если удалили VarSite из карты
 {
   DeleteSiteFromListSelect(FMV->VarMapSites->VarMapSite->GetNumber());
   if (VarSite == FMV->VarMapSites->VarMapSite->GetNumber()) VarSite = 0;
   return;
 } // если удалили VarSite из карты

//-------------------------
 if ((action == ACTBEFORECLOSE) && (FMV->GetMapHandle() != 0))
 // если закрыли карту
 {
   for (ii = 0; ii <= CountSelect; ii++)
     if (VarListSelect[ii] != 0)
     {
       mapDeleteSelectContext(VarListSelect[ii]);
       VarListSelect[ii] = 0;
     }

   CountSelect = 0;
   VarSite = 0;
   memset(VarListSelect, 0, sizeof(VarListSelect));
   memset(VarListUses, 0, sizeof(VarListUses));
   return;
 } // если закрыли карту

//-------------------------
 if ((action == ACTAFTEROPEN) && (FMV->GetMapHandle() != 0))
 // если открыли карту
 {
    SetCountListSelect();
    VarSite = 0;
    GetHSelect(VarSite);
 } // если открыли карту
}

//------------------------------------------------------------------
// Удаление select по номеру сайта
//------------------------------------------------------------------
void QDMapSelect::DeleteSiteFromListSelect(int numsite)
{
  if (VarListSelect[numsite] != 0)
    mapDeleteSelectContext(VarListSelect[numsite]);

  for (int ii = numsite; ii < CountSelect; ii++)
  {
    VarListSelect[ii] = VarListSelect[ii + 1];
    VarListUses[ii]   = VarListUses[ii + 1];
  }

  VarListSelect[CountSelect] = 0;
  CountSelect--;
  if (CountSelect < 0) CountSelect = 0;
}

//------------------------------------------------------------------
// Для копирования объектов QDMapSelect
//------------------------------------------------------------------
void QDMapSelect::Assign(QDMapSelect *src)
{
  signed long int ii, countsel;

  if (src == NULL) return;

  // очистим поля в текущем MapSelect
  for (ii = 0; ii < CountSelect; ii++)
    if (VarListSelect[ii] != 0)
    {
      mapDeleteSelectContext(VarListSelect[ii]);
      VarListSelect[ii] = 0;
    }

  CountSelect = 0;
  memset(VarListSelect, 0, sizeof(VarListSelect));
  memset(VarListUses, 0, sizeof(VarListUses));

  VarMapView = src->VarMapView;
  VarMapPoint->SetMapView(VarMapView);
  VarMapPoint->VarPlaceOut = src->VarMapPoint->VarPlaceOut;
  VarMapPoint->VarPlaceIn = src->VarMapPoint->VarPlaceIn;
  VarMapRect = src->VarMapRect;

  VarSite = src->VarSite;
  if ((VarMapView == NULL) || (VarMapView->GetMapHandle() == 0)) return;

  CountSelect = src->CountSelect;

  // Перепишем копии контекстов поиска для карт
  for (ii = 0; ii < CountSelect; ii++)
  {
    VarListUses[ii]   = src->VarListUses[ii];
    VarListSelect[ii] = 0;
    if (src->VarListSelect[ii] == 0) continue;
    VarListSelect[ii] = mapCreateCopySelectContext(src->VarListSelect[ii]);
  }

  if (CountSelect > 1)
  {
    VarListUses[1]   = src->VarListUses[1];
    if (VarListSelect[1] != 0) mapDeleteSelectContext(VarListSelect[1]);
    VarListSelect[1] = 0;
    if (src->VarListSelect[1] != 0) VarListSelect[1] = mapCreateCopySelectContext(src->VarListSelect[1]);
  }

  VarEdit = TRUE; // Изменили TMapSelect
}

//------------------------------------------------------------------
// Все ли Sites включены в операции отбора
//------------------------------------------------------------------
bool QDMapSelect::GetAllFindSites()
{
 int ii;
 bool ret = TRUE;

 for (ii = 0; ii <= CountSelect; ii++)
   ret = (ret && (bool)VarListUses[ii]);

 return ret;
}

void QDMapSelect::SetAllFindSites(bool value)
{
 for (int ii = 0; ii <= CountSelect; ii++)
   VarListUses[ii] = value;
}

//------------------------------------------------------------------
// установить размерность списка на текущее количество SiteCount
//  меньше в списке быть не может
//------------------------------------------------------------------
void QDMapSelect::SetCountListSelect()
{
  int CountSite,ii;

  CountSite = mapGetSiteCount(VarMapView->GetMapHandle());
  for (ii = CountSelect; ii <= CountSite; ii++)
  {
//    if (CountSite <= CountSelect) return;
    VarListSelect[ii] = 0;
    VarListUses[ii]   = true;
    CountSelect++;
  }
}

//------------------------------------------------------------------
// Очистить список объектов во всех контекстах условий поиска/отображения
//------------------------------------------------------------------
void QDMapSelect::ClearSampleAll()
{
  int     ii;
  HSELECT SelH;

  VarEdit = TRUE;

  for (ii = 0; ii <= CountSelect; ii++)
    mapClearSample(VarListSelect[ii]);
}

//------------------------------------------------------------------
// Установить/Снять доступ к заданному объекту заданного листа карты.
// seting = TRUE - установить доступ к объекту иначе снять
// listname - имя листа карты,
// key - уникальный номер объекта в карте.
// При ошибке возвращает ноль.
//------------------------------------------------------------------
int  QDMapSelect::SelectSample(bool seting, char *listname, int key)
{
  int     iSite, ret = 0;
  HSELECT SelH;

  // определим номер карты где есть лист listname
  iSite = mapWhatListLayoutIs(VarMapView->GetMapHandle(), listname);
  if (iSite == -1) return ret;
  SelH  = GetHSelect(iSite);
  if (seting == TRUE)
    ret = mapSelectSample(SelH, listname, key);
  else ret = mapUnselectSample(SelH, listname, key);
  VarEdit = TRUE;
  return ret;
}

//------------------------------------------------------------------
// Установить доступность тех карт для участия в поиске/выделения, для которых доступен
// хотя бы один объект (была выполнена функция SelectSample)
//------------------------------------------------------------------
void QDMapSelect::SetSiteEnabledForSample()
{
  int     ii;
  HSELECT SelH;

  SetMapSites(-1, false);

  for (ii = 0; ii <= CountSelect; ii++)
  {
     if (VarListUses[ii] == true) continue; // он уже и так доступен
     SelH = VarListSelect[ii];
     if ((SelH != 0) && (mapIsSample(SelH) != 0))
     {
       VarListUses[ii] = true;
     }
  }
}

//------------------------------------------------------------------
// Проверить Есть ли в списке объектов хотя бя один
// Вход   sitenumber  - номер карты в цепочке,
//                  для которой определяется условие
//                  =-1 - определить есть ли хотя бы в одной карте из всего списка
// Выход = TRUE - если список установлен
//------------------------------------------------------------------
bool QDMapSelect::GetIsSample(int sitenumber)
{
  int     ii, i1, iAll;
  HSELECT SelH;
  bool    ret = FALSE;

  if (sitenumber == -1)
  {
    i1   = 0;
    iAll = CountSelect;
  }
  else
  {
    i1   = sitenumber;
    iAll = sitenumber;
  }

  for (ii = i1; ii <= iAll; ii++)
  {
     SelH = VarListSelect[ii];
     if ((SelH != 0) && (mapIsSample(SelH) != 0))
     {
        ret = TRUE;
        return ret;
     }
  }

  return ret;
}

//------------------------------------------------------------------
// Сбросить в контексте параметры поиска объектов по области
// Вход   sitenumber  - номер карты в цепочке, для которой сбрасывается ограничение поиска
//                     =-1 - ограничение сбрасывается для всех доступных условий поиска
//------------------------------------------------------------------
void QDMapSelect::UnselectSeekArea(int sitenumber)
{
  HSELECT SelH;
  int     ii, iAll;

  if (sitenumber == -1)
  {
    sitenumber = 0;
    iAll       = CountSelect;
  }
  else iAll = sitenumber;
  for (ii = sitenumber; ii <= iAll; ii++)
  {
     SelH = VarListSelect[ii];
     if (SelH != 0) mapUnselectSeekArea(SelH);
  }
}

//------------------------------------------------------------------
// Установить в контексте параметры поиска объектов по области.
// Вход   sitenumber - номер карты в цепочке, для которой устанавливается ограничение поиска
//                     =-1 - ограничение установиттся для всех доступных условий поиска
//        object     - объект карты - область поиска
//                     (не обязан реально существовать в карте, достаточно наличие метрики)
//                     (после вызова функции можно удалить aObject)
//                     (объект может находиться на любой карте из списка)
//        distance   - расстояние поиска в метрах
//        action     - предстоящая операция по области: выделение/поиск (1/0)
//        filter     - учитывать/не учитывать(1/0) фильтр объектов в HSELECT
//                    (параметры фильтра должны быть установлены заранее в контексте поиска)
//        inside     - 1 - внутри области, 2 - целиком внутри области, 0 - по расстоянию.
//        visible    - с учетом/без учета(1/0) видимости объектов на карте
// При ошибке возвращает ноль иначе количество условий поиска в которых успешно установлено ограничение
//------------------------------------------------------------------
int QDMapSelect::SelectSeekArea(int sitenumber, QDMapObjAcces *object,
                                double distance, int filter,
                                int inside, int visible, int action)
{
  if (object == 0) return 0;

  HSELECT SelH;
  int ii, iAll, ret = 0;

  if (sitenumber == -1)
  {
    sitenumber = 0;
    iAll = CountSelect;
  }
  else iAll = sitenumber;
  for (ii = sitenumber; ii <= iAll; ii++)
  {
     SelH = GetHSelect(ii);
     if (SelH == 0) continue;
     if (mapSelectSeekArea(SelH, object->VarObjHandle, distance, filter, inside,
                           visible, action) != 0)
     ret = ret + 1;
  }

  return ret;
}

//------------------------------------------------------------------
// Считать текущий контекст поиска карты   // 09/12/09
//------------------------------------------------------------------
void QDMapSelect::GetSeekSelect(int aSite)
{
  if (VarMapView == 0 || VarMapView->GetMapHandle() == 0) return;

  HSELECT hs;
  int ii, all, jj;

  all = mapGetSiteCount(VarMapView->GetMapHandle());

  if (aSite < -1 || aSite > all) return;

  if (aSite == -1) ii = 0; // для всех карт (all := mapGetSiteCount)
  else
  {
    ii  = aSite;
    all = aSite; // для конкретной карты
  }

  for (jj = ii; jj <= all; jj++)
  {
    hs = GetHSelect(jj);
    if (hs == 0) return;
    mapGetSiteSeekSelect(VarMapView->GetMapHandle(),
                         mapGetSiteIdent(VarMapView->GetMapHandle(), jj), hs);
  }

  VarEdit = true;
}

//------------------------------------------------------------------
// Установить текущий контекст поиска карты
//------------------------------------------------------------------
void QDMapSelect::SetSeekSelect(int aSite)
{
  if (VarMapView == 0 || VarMapView->GetMapHandle() == 0) return;

  HSELECT hs;
  int ii, all, jj;

  all = mapGetSiteCount(VarMapView->GetMapHandle());

  if (aSite < -1 || aSite > all) return;

  if (aSite == -1) ii = 0; // для всех карт (all := mapGetSiteCount)
  else
  {
    ii  = aSite;
    all = aSite; // для конкретной карты
  }

  for (jj = ii; jj <= all; jj++)
  {
    hs = GetHSelect(jj);
    if (hs == 0) return;
    mapSetSiteSeekSelect(VarMapView->GetMapHandle(),
                         mapGetSiteIdent(VarMapView->GetMapHandle(), jj), hs);
  }

  VarEdit = false;
}

//------------------------------------------------------------------
// Считать текущий контекст поиска карты   // 09/12/09
//------------------------------------------------------------------
void QDMapSelect::GetViewSelect(int aSite)
{
  if (VarMapView == 0 || VarMapView->GetMapHandle() == 0) return;

  HSELECT hs;
  int ii, all, jj;

  all = mapGetSiteCount(VarMapView->GetMapHandle());

  if (aSite < -1 || aSite > all) return;

  if (aSite == -1) ii = 0; // для всех карт (all := mapGetSiteCount)
  else
  {
    ii  = aSite;
    all = aSite; // для конкретной карты
  }

  for (jj = ii; jj <= all; jj++)
  {
    hs = GetHSelect(jj);
    if (hs == 0) return;
    mapGetSiteViewSelect(VarMapView->GetMapHandle(),
                     mapGetSiteIdent(VarMapView->GetMapHandle(), jj), hs);
  }

  VarEdit = true;
}

//------------------------------------------------------------------
// Установить текущий контекст поиска карты
//------------------------------------------------------------------
void QDMapSelect::SetViewSelect(int aSite)
{
  if (VarMapView == 0 || VarMapView->GetMapHandle() == 0) return;

  HSELECT hs;
  int ii, all, jj;

  all = mapGetSiteCount(VarMapView->GetMapHandle());

  if (aSite < -1 || aSite > all) return;

  if (aSite == -1) ii = 0; // для всех карт (all := mapGetSiteCount)
  else
  {
    ii  = aSite;
    all = aSite; // для конкретной карты
  }

  for (jj = ii; jj <= all; jj++)
  {
    hs = GetHSelect(jj);
    if (hs == 0) return;
    mapSetSiteViewSelect(VarMapView->GetMapHandle(),
                         mapGetSiteIdent(VarMapView->GetMapHandle(), jj), hs);
  }

  VarEdit = false;
}

//------------------------------------------------------------------
// Запрос числа объектов на карте, удовлетворяющих условиям поиска   // 09/12/09
// Если aSiteNumber = -1 то подсчет для всех карт
//------------------------------------------------------------------
int QDMapSelect::GetSelectObjectCount(int aSiteNumber)
{
  if (VarMapView == 0 || VarMapView->GetMapHandle() == 0) return 0;

  HSELECT SelH;
  int     ii, iAll, Result = 0;

  if (aSiteNumber == -1)
  {
     aSiteNumber = 0;
     iAll        = CountSelect;
  }
  else iAll = aSiteNumber;

  for (ii = aSiteNumber; ii <= iAll; ii++)
  {
     if (VarListUses[ii] == false) continue;
     SelH = GetHSelect(ii);
     if (SelH == 0) continue;
     Result = Result + mapSeekSelectObjectCount(VarMapView->GetMapHandle(), SelH);
  }
  return Result;
}

/******** QDMDSEM.CPP **********************************************
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
*                  КЛАССЫ ДОСТУПА К СЕМАНТИКЕ                      *
*                                                                  *
*                                                                  *
*******************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>

#include "qdmsem.h"
#include "qdmobja.h"
#include "qdmviewa.h"
#include "mapapi.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К СЕМАНТИКЕ ОБЪЕКТА КАРТЫ     *
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSemantic::QDMapSemantic(QDMapObjAcces *mapobj)
{
  VarMapObj = mapobj;
  VarSemanticStyle = SS_NORMALDECODE;
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapSemantic::~QDMapSemantic()
{
}

//------------------------------------------------------------------
// Количество семантических характеристик
//------------------------------------------------------------------
int QDMapSemantic::GetSemanticCount()
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;

  return mapSemanticAmount(VarMapObj->GetObjHandle());
}

//------------------------------------------------------------------
// Название семантической характеристики
//------------------------------------------------------------------
char *QDMapSemantic::GetSemanticName(int number)
{
  if (VarMapObj == NULL) return NULL;
  if (VarMapObj->GetObjHandle() == 0) return NULL;

  return (char *)mapSemanticName(VarMapObj->GetObjHandle(), number);
}

//------------------------------------------------------------------
// Код семантической характеристики
//------------------------------------------------------------------
int QDMapSemantic::GetSemanticCode(int number)
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;

  return mapSemanticCode(VarMapObj->GetObjHandle(), number);
}

//------------------------------------------------------------------
// Запросить/установить значение семантической характеристики
//------------------------------------------------------------------
char *QDMapSemantic::GetSemanticValue(int number)
{
  char *ret = Value;

  memset(Value, 0, sizeof(Value));
  if (VarMapObj == NULL) return ret;
  if (VarMapObj->GetObjHandle() == 0) return ret;
  switch (VarSemanticStyle)
  {
    case SS_NODECODE    :
       mapSemanticValue(VarMapObj->GetObjHandle(), number, Value, 256);
       ret = Value;
       break;
    case SS_NORMALDECODE:
       mapSemanticValueName(VarMapObj->GetObjHandle(), number, Value, 256);
       ret = Value;
       break;
    case SS_FULLDECODE  :
       mapSemanticValueFullName(VarMapObj->GetObjHandle(), number, Value, 256);
       ret = Value;
       break;
  }

  return ret;
}

void QDMapSemantic::SetSemanticValue(int number, char *value)
{
  int i, Col;
  int Code;
  char StrVal[256];

  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0) return;

  memset(StrVal, 0, 256);
  strcpy(StrVal, value);
  Code = mapSemanticCode(VarMapObj->GetObjHandle(), number);
  if (Code == 0) return;
  Col = mapSemanticClassificatorCount(VarMapObj->GetObjHandle(), Code);
  if (Col != 0)
    for (i = 1; i <= Col; i++)
      if (strcmp(mapSemanticClassificatorName(VarMapObj->GetObjHandle(), Code, i), value) == 0)
      {
         sprintf(StrVal, "%i",
                 mapSemanticClassificatorCode(VarMapObj->GetObjHandle(), Code, i));
         break;
      }

  mapSetSemanticValue(VarMapObj->GetObjHandle(), number, StrVal, strlen(StrVal));
}

//------------------------------------------------------------------
// Количество допустимых кодов семантики
//------------------------------------------------------------------
int QDMapSemantic::GetAvailabelSemanticCount()
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;

  return mapAvailableSemanticCount(VarMapObj->GetObjHandle());
}

//------------------------------------------------------------------
// Допустимые внешние кода семантики
//------------------------------------------------------------------
int QDMapSemantic::GetAvailabelSemanticCode(int number)
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;
  if (number < 1) return 0;
  if (number > mapAvailableSemanticCount(VarMapObj->GetObjHandle())) return 0;

  return mapAvailableSemanticCode(VarMapObj->GetObjHandle(), number);
}

//------------------------------------------------------------------
// Наименования допустимых семантик
//------------------------------------------------------------------
char * QDMapSemantic::GetAvailabelSemanticName(int code)
{
  char *ret = Value;
  RSCSEMANTICEX rscsem;

  memset(Value, 0, sizeof(Value));
  memset((char *)&rscsem, 0, sizeof(rscsem));
  if (VarMapObj == NULL) return ret;
  if (VarMapObj->GetObjHandle() == 0) return ret;
  if (code != VarSemanticType.Code)
    if (mapSemanticDescribeEx(VarMapObj->GetObjHandle(), &rscsem, code) == 0) return ret;

  strcpy(Value, rscsem.Name);
  return ret;
}

//------------------------------------------------------------------
// Количество значений в классификаторе значений семантик
//------------------------------------------------------------------
int QDMapSemantic::GetSemanticValueCount(int code)
{
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;

  return mapSemanticClassificatorCount(VarMapObj->GetObjHandle(), code);
}

//------------------------------------------------------------------
// Значения допустимых семантик
//------------------------------------------------------------------
char *QDMapSemantic::GetSemanticValueName(int code, int number)
{
  char *ret = Value;

  memset(Value, 0, sizeof(Value));
  if (VarMapObj == NULL) return ret;
  if (VarMapObj->GetObjHandle() == 0) return ret;
  if (number < 1) return ret;
  if (number > mapSemanticClassificatorCount(VarMapObj->GetObjHandle(), code)) return ret;

  strcpy(Value, mapSemanticClassificatorName(VarMapObj->GetObjHandle(), code, number));
  return ret;
}

//------------------------------------------------------------------
// Добавить новую характеристику
//------------------------------------------------------------------
int QDMapSemantic::Add(int code)
{
  RSCSEMANTICEX rscsem;

  memset(Value, 0, sizeof(Value));
  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;
  memset((char *)&rscsem, 0, sizeof(rscsem));
  if (code != VarSemanticType.Code)
    if (mapSemanticDescribeEx(VarMapObj->GetObjHandle(), &rscsem, code) == 0) return 0;
  switch (rscsem.Type)
  {
    case TSTRING   :
    case TBMPFILE  :
    case TOLEFILE  :
    case TMAPFILE  :
    case TTXTFILE  :
    case TPCXFILE  : break;
    case TNUMBER   : sprintf(Value, "%fl", rscsem.Default); break;
    case TCODE     :
      if (mapSemanticClassificatorCount(VarMapObj->GetObjHandle(), code))
        sprintf(Value, "%d", mapSemanticClassificatorCode(VarMapObj->GetObjHandle(), code, 1));
      break;
    default       : return 0;
  }

  return mapAppendSemantic(VarMapObj->GetObjHandle(), code, Value, strlen(Value)+1);
}

//------------------------------------------------------------------
// Удалить характеристику
//------------------------------------------------------------------
void QDMapSemantic::Delete(int number)
{
  if (VarMapObj == NULL) return;
  if (VarMapObj->GetObjHandle() == 0) return;
  if (number < 1) return;
  if (number > mapSemanticAmount(VarMapObj->GetObjHandle())) return;
  mapDeleteSemantic(VarMapObj->GetObjHandle(), number);
}

//------------------------------------------------------------------
// Найти номер семантики в объекте по ее коду
//  при отсутсвии возвращает 0
//------------------------------------------------------------------
int QDMapSemantic::FindByCode(int code)
{
  int ii;

  for (ii = 1; ii <= GetSemanticCount(); ii++)
     if (GetSemanticCode(ii) == code)
       return ii;

  return 0;
}

//------------------------------------------------------------------
// Удалить семантику в объекте по ее коду одну или все
//------------------------------------------------------------------
void QDMapSemantic::DeleteByCode(int code, bool all)
{
  int ii;

  while (true)
  {
     ii = FindByCode(code);
     if (ii == 0) return;
     Delete(ii);
     if (all == false) return;
  }
}

//------------------------------------------------------------------
// Заполнить структуру описания семантической характеристики
// по номеру семантики в объекте
//------------------------------------------------------------------
RSCSEMANTICEX QDMapSemantic::GetDescribe(int number)
{
  RSCSEMANTICEX Desc;
  int           code;

  memset((char *)&Desc, 0, sizeof(Desc));
  if (VarMapObj == NULL) return Desc;
  if (VarMapObj->GetObjHandle() == 0) return Desc;
  code = mapSemanticCode(VarMapObj->GetObjHandle(), number);
  mapSemanticDescribeEx(VarMapObj->GetObjHandle(), &Desc, code);
  return Desc;
}

//------------------------------------------------------------------
//  Запросить количество записей в классификаторе
//  семантики по номеру семантики в объекте
//------------------------------------------------------------------
int QDMapSemantic::GetClassificatorCount(int number)
{
  int code;

  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;
  code = mapSemanticCode(VarMapObj->GetObjHandle(), number);
  return mapSemanticClassificatorCount(VarMapObj->GetObjHandle(), code);
}

//------------------------------------------------------------------
//  Запросить название значения характеристики из
//  классификатора семантики по номеру семантики в объекте и
//  последовательному номеру в классификаторе значений
//------------------------------------------------------------------
char *QDMapSemantic::GetClassificatorName(int number1, int numbervalue)
{
  int code;

  memset(Value, 0, sizeof(Value));                                           // 18/10/05
  if (VarMapObj == NULL) return NULL;
  if (VarMapObj->GetObjHandle() == 0) return NULL;
  code = mapSemanticCode(VarMapObj->GetObjHandle(), number1);

  strcpy(Value, mapSemanticClassificatorName(VarMapObj->GetObjHandle(), code, numbervalue));
  return Value;
}

//------------------------------------------------------------------
//  Запросить код значения характеристики из
//  классификатора семантики по номеру семантики в объекте
//  последовательному номеру в классификаторе значений
//------------------------------------------------------------------
int QDMapSemantic::GetClassificatorCode(int number1, int numbervalue)
{
  int code;

  if (VarMapObj == NULL) return 0;
  if (VarMapObj->GetObjHandle() == 0) return 0;
  code = mapSemanticCode(VarMapObj->GetObjHandle(), number1);
  return mapSemanticClassificatorCode(VarMapObj->GetObjHandle(), code, numbervalue);
}

//-----------------------------------------------------------
// Задать/запросить способ раскодирования значений семантики
//-----------------------------------------------------------
void QDMapSemantic::SetSemanticStyle(int value)
{
  VarSemanticStyle = value;
}

int QDMapSemantic::GetSemanticStyle()
{
  return VarSemanticStyle;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К ОПИСАНИЮ СЕМАНТИК В КЛАССИФИКАТОРЕ +
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSemantics::QDMapSemantics(QDMapViewAcces *mapview)
{
  VarMapView    = mapview;
  VarSiteNumber = 0;
  VarHRsc       = 0;
  memset(Value, 0, sizeof(Value));                                           // 18/10/05
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapSemantics::~QDMapSemantics()
{
}

//------------------------------------------------------------------
// Получить/установить VarHRsc для текущей комбинации параметров
//------------------------------------------------------------------
HRSC QDMapSemantics::GetRscHandle()
{
  HSITE site1;
  HMAP  maph1;

  // если есть VarMapView значит создали для карты
  if (VarMapView != NULL)
  {
      maph1 = VarMapView->GetMapHandle();
      if (maph1 == 0) return 0;

      site1 = mapGetSiteIdent(maph1, VarSiteNumber);
      if (site1 == 0)
      {
         VarSiteNumber = 0;
         site1 = mapGetSiteIdent(maph1, 0);
      }
      if (site1 == 0) return 0;

      return mapGetRscIdent(maph1, site1);
  } //if (VarMapView != NULL)

  // если есть rsc значит создали для отдельно открытого RSC
  return VarHRsc;
}

void QDMapSemantics::SetRscHandle(HRSC rsch)
{
 if (VarMapView != NULL) return;

 VarHRsc = rsch;
}

//------------------------------------------------------------------
// Количество кодов семантики в класификаторе
//------------------------------------------------------------------
int QDMapSemantics::GetSemanticCount()
{
  return mapGetRscSemanticCount(GetRscHandle());
}

//------------------------------------------------------------------
// Код семантики
//------------------------------------------------------------------
int QDMapSemantics::GetSemanticCode(int number)
{
  return mapGetRscSemanticCodeByNumber(GetRscHandle(), number);
}

//------------------------------------------------------------------
// Наименование семантики
//------------------------------------------------------------------
char *QDMapSemantics::GetSemanticName(int number)
{
  return (char *)mapGetRscSemanticNameByNumber(VarMapView->GetMapHandle(),
                                       number);
}

//------------------------------------------------------------------
// Наименование семантики по коду
//------------------------------------------------------------------
char *QDMapSemantics::GetSemanticNameCode(int code)
{
  RSCSEMANTICEX tmp1;
  char          *ret = Value;

  memset(Value, 0, sizeof(Value));                                           // 18/10/05
  if (mapGetRscSemanticExByCode(GetRscHandle(), &tmp1, code) == 0) return ret;

  strcpy(Value, tmp1.Name);
  return Value;
}

//------------------------------------------------------------------
// Код семантики по ее наименованию
//------------------------------------------------------------------
int QDMapSemantics::GetSemanticCodeName(char *name)
{
  HRSC rscH;
  int  ii;
  char *pStr;

  if (!name || !strlen(name)) return 0;
  rscH = GetRscHandle();
  if (rscH == 0) return 0;
  for (ii = 1; ii <= mapGetRscSemanticCount(rscH); ii++)
  {
       pStr = (char *)mapGetRscSemanticNameByNumber(rscH, ii);
       if (strcmp(name, pStr) != 0) continue;
       return mapGetRscSemanticCodeByNumber(rscH, ii);
  }

  return 0;
}

//------------------------------------------------------------------
// Порядковый номер семантики по ключу
//------------------------------------------------------------------
int QDMapSemantics::GetNumberByKey(char *key)
{
  return mapGetRscSemanticByKey(GetRscHandle(), key);
}

//------------------------------------------------------------------
// Тип семантики по ее коду
//------------------------------------------------------------------
int QDMapSemantics::GetTypeByCode(int code)
{
  return mapGetRscSemanticTypeByCode(GetRscHandle(), code);
}

//------------------------------------------------------------------
// Уникальное название-ключ семантики по ее коду
//------------------------------------------------------------------
char *QDMapSemantics::GetKeyByCode(int code)
{
  return (char *)mapGetRscSemanticKey(GetRscHandle(), code);
}

//------------------------------------------------------------------
// Получить описание семантики по ее коду
//------------------------------------------------------------------
RSCSEMANTICEX QDMapSemantics::GetDescribeByCode(int code)
{
  RSCSEMANTICEX ret;

  memset((char *)&ret, 0, sizeof(RSCSEMANTICEX));
  mapGetRscSemanticExByCode(GetRscHandle(), &ret, code);
  return ret;
}

//------------------------------------------------------------------
// Количество значений классификатора семантической характеристики по коду семантики
// (если ее тип TCODE) При ошибке возвращает ноль
//------------------------------------------------------------------
int QDMapSemantics::GetClassificatorValueCount(int code)
{
  return mapGetRscSemanticClassificatorCount(GetRscHandle(), code);
}

//------------------------------------------------------------------
// Название значения характеристики из классификатора семантики по коду семантики и
// последовательному номеру в классификаторе
//------------------------------------------------------------------
char *QDMapSemantics::GetClassificatorValueName(int code, int valuenumber)
{
  return (char *)mapGetRscSemanticClassificatorName(GetRscHandle(), code, valuenumber);
}

//------------------------------------------------------------------
// Код значения характеристики из классификатора семантики по коду семантики и
// последовательному номеру в классификаторе
//------------------------------------------------------------------
int QDMapSemantics::GetClassificatorValueCode(int code, int valuenumber)
{
  return mapGetRscSemanticClassificatorCode(GetRscHandle(), code, valuenumber);
}


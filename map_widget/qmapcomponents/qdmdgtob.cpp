/******** QDMDGTOB.CPP *********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR Qt-DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*                     КОМПОНЕНТ - ДИАЛОГ                           *
*          "ВЫБОРА ТИПА ОБЪЕКТА ИЗ КЛАССИФИКАТОРА"                 *
*                                                                  *
*******************************************************************/

#include "qdmdgtob.h"
#include "qdmcmp.h"
#include "qdmobj.h"
#include "mapapi.h"
#include "medapi.h"

#define MAPEDFRMLIB  "libqdmapedfrm.so"

// Главный обработчик сообщений
long int MessageHandler(long int context, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//========================================================
// Вызов формы для выбора объекта из классификатора
// Вход mv        - объект - карта
//      objinfo   -  начальные параметры диалога
//                   NumbSite = -1 - для выбора доступны все карты
//                            = >= 0 - доступна только конкретная карта
//                   Incode, либо Excode и Local, либо Name и Local - определяют
//                   с какого объекта начать выбор
//      locals    - перечень доступных типов локализаций объекта (должен
//                  указывать на массив, размерностью 6(количество локализаций))
// Выход - = 0 - выбор сделан и aObjInfo - заполненая структура
//         = 1 - отказ
//====================================================
bool SelectObjectFromRsc(QDMapView *mv, int *locals,
                         TMAPOBJECTINFO *objinfo)
{
  QDMapSelectObjectRsc *selobjdlg = NULL;
  bool ret = false;

  if ((mv == NULL) || (mv->GetMapHandle() == 0) || (locals == 0))
     return ret;

  selobjdlg = new QDMapSelectObjectRsc(NULL);

  selobjdlg->SetMapView(mv);
  selobjdlg->FSiteNumber = objinfo->NumbSite;

  if (objinfo->Incode != 0)
    selobjdlg->FRetIncode = objinfo->Incode;
  else
  {
    if (objinfo->ExCode != 0)
      selobjdlg->SetBeginObjectForExcode(objinfo->NumbSite,
                                    objinfo->ExCode, objinfo->Local);
    else
    {
      if ((objinfo->Name != NULL) && (strlen(objinfo->Name) != 0))
        selobjdlg->SetBeginObjectForName(objinfo->NumbSite,
                                   objinfo->Name, objinfo->Local);
    }
  }
  for (int i = 0; i < 6; i++)
    selobjdlg->FLocals[i] = locals[i];
  ret = selobjdlg->Execute(objinfo);

  delete selobjdlg;
  return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГА                                +
//+++ "ВЫБОРА ТИПА ОБЪЕКТА ИЗ КЛАССИФИКАТОРА"      +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSelectObjectRsc::QDMapSelectObjectRsc(QWidget *parent, const char *name)
                     :QDMapComponent(parent, name)
{
   FHelpEnabled    = false;
   FRetIncode = 0;
   FRetSite   = 0;
   for (int i = 0; i < 6; i++)
     FLocals[i] = 1;
   FMapEditEnabled = false;
   FSiteNumber     = -1;
   FRestoreEnabled = true;
   ObjH            = 0;
   strcpy(FCaption, "Создание объекта");
   hide();
}

//------------------------------------------------------------------
// Указатель на карту
//------------------------------------------------------------------
QDMapView *QDMapSelectObjectRsc::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapSelectObjectRsc::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//------------------------------------------------------------------
// Выполнить диалог
// Выход = true  - выбрали и заполнили objinfo
//         false - отказались
//------------------------------------------------------------------
bool QDMapSelectObjectRsc::Execute(TMAPOBJECTINFO *objinfo)
{
 // Диалог выбора вида объекта из файла ресурсов
 // hmap - идентификатор открытой векторной карты
 // hobj - идентификатор объекта
 // choiceparm - параметры для диалога выбора вида объекта
 // name - заголовок диалога
 // возвращает внутренний код объекта, hobj соответствует
 // выбранной карте и коду объекта
 // при ошибке или отказе возвращает 0
 long int (WINAPI *ChoiceTypeObject)(HMAP hmap, TASKPARM *parm,
                                     HOBJ hobj, CHOICEOBJECTPARM *choiceparm,
                                     char *name);

 long int          RetFunc;
 QDMapView         *MapView;
 TASKPARM          taskparm;
 HSELECT           SelectH;
 HMAP              MapH;
 HSITE             SiteH;
 int               SiteCount;
 CHOICEOBJECTPARM  Choice;
 HRSC              RscH;
 bool              Result = false;
 HINSTANCE         libinst;

 MapView = (QDMapView *)VarMapView;
 if (MapView == NULL) return Result;
 MapH = MapView->GetMapHandle();
 if (MapH == 0) return Result;

 // Заполнение параметров прикладной задачи
 memset((char *)&taskparm, 0, sizeof(TASKPARM));
 taskparm.Language  = ML_RUSSIAN;
 taskparm.Handle = &MessageHandler;

 if (ObjH != 0) mapFreeObject(ObjH);
 ObjH      = 0;
 SiteCount = mapGetSiteCount(MapH);

 if ((FRestoreEnabled == false) || (FRetIncode == 0)
     || (FRetSite > SiteCount))
 { // не работаем с тем же объектом , что и последний раз
        FRetSite   = 0;
        FRetIncode = 0;
        SiteH      = MapH;
 }
 else SiteH  =  mapGetSiteIdent(MapH, FRetSite);
 if (FSiteNumber != -1)
   SiteH   =  mapGetSiteIdent(MapH, FSiteNumber);

 if (SiteH == 0) return Result;

 ObjH = mapCreateSiteObject(MapH, SiteH, IDDOUBLE2, 0);
 if (ObjH == 0) return Result;
 if (FRetIncode != 0) // установим тот с которого надо начать
   mapDescribeObject(ObjH, FRetIncode);

 SelectH = mapCreateSiteSelectContext(MapH, SiteH);
 if (SelectH == 0) return Result;
 mapSelectLocal(SelectH, -1, 1);
 if (FLocals[LOCAL_LINE] == 0)
   mapSelectLocal(SelectH, LOCAL_LINE, 0);
 if (FLocals[LOCAL_SQUARE] == 0)
   mapSelectLocal(SelectH, LOCAL_SQUARE, 0);
 if (FLocals[LOCAL_POINT] == 0)
   mapSelectLocal(SelectH, LOCAL_POINT, 0);
 if (FLocals[LOCAL_TITLE] == 0)
   mapSelectLocal(SelectH, LOCAL_TITLE, 0);
 if (FLocals[LOCAL_VECTOR] == 0)
   mapSelectLocal(SelectH, LOCAL_VECTOR, 0);
 if (FLocals[LOCAL_MIXED] == 0)
   mapSelectLocal(SelectH, LOCAL_MIXED, 0);

 Choice.hSelect  = SelectH;
 if (FSiteNumber == -1)
   Choice.MapSelect = 0;
 else Choice.MapSelect = 1;
 if (FMapEditEnabled == true)
   Choice.MapEditSelect = 1;
 else Choice.MapEditSelect = 0;

 // вызовем диалог
 ChoiceTypeObject = (long int (WINAPI *)(HMAP HMap, TASKPARM *taskparm, HOBJ hobj,
           CHOICEOBJECTPARM *choiceparm, char *name))
                    mapLoadLibrary(MAPEDFRMLIB , &libinst,
                                   "medChoiceTypeObjectParm");
 if (ChoiceTypeObject == NULL) return Result;

 RetFunc = ChoiceTypeObject(MapView->GetMapHandle(),
                            &taskparm, ObjH, &Choice, FCaption);

 // Выгрузка библиотеки
 if (libinst)  mapFreeLibrary(libinst);

 if (RetFunc != 0) // выбрали
 {
   if (mapObjectLocal(ObjH) == LOCAL_TITLE)
   {
     // Установка/ изменение текста многострочной подписи
     // hobj - идентификатор объекта типа ПОДПИСЬ
     // subject - номер подобъекта
     // При отказе возвращает 0, иначе 1
     long int (WINAPI *medSetPolyTitle)(TASKPARM *taskparm,
                                        HOBJ hobj, long int subject);

     // вызовем диалог
     medSetPolyTitle = (long int (WINAPI *)(TASKPARM *taskparm, HOBJ hobj,
              long int subject))
                          mapLoadLibrary(MAPEDFRMLIB , &libinst,
                                   "medSetPolyTitle");
     if (medSetPolyTitle == NULL) return Result;

     RetFunc = medSetPolyTitle(&taskparm, ObjH, 0);

     // Выгрузка библиотеки
     if (libinst)  mapFreeLibrary(libinst);

     if (RetFunc == 0) return Result;
   }

   if (mapObjectLocal(ObjH) == LOCAL_MIXED)
   {
     // Создание подписи шаблона
     // info - объект типа ШАБЛОН
     // При отказе возвращает 0, иначе 1
     long int (WINAPI *medCreateMixedLabel)(HMAP hmap, TASKPARM *taskparm,
                                            HOBJ info);

     // вызовем диалог
     medCreateMixedLabel = (long int (WINAPI *)(HMAP hmap, TASKPARM *taskparm,
                                                HOBJ hobj))
                          mapLoadLibrary(MAPEDFRMLIB , &libinst,
                                   "medCreateMixedLabel");
     if (medCreateMixedLabel == NULL) return Result;

     RetFunc = medCreateMixedLabel(MapH, &taskparm, ObjH);

     // Выгрузка библиотеки
     if (libinst)  mapFreeLibrary(libinst);

     if (RetFunc == 0) return Result;
   }

   FRetIncode = mapObjectCode(ObjH);
   SiteH      = mapGetObjectSiteIdent(MapH, ObjH);
   FRetSite   = mapGetSiteNumber(MapH, SiteH);
   RscH       = mapGetRscIdent(MapH, SiteH);
   objinfo->NumbSite = FRetSite;
   objinfo->Incode   = FRetIncode;
   objinfo->ExCode   = mapObjectExcode(ObjH);
   objinfo->Local    = mapObjectLocal(ObjH);
   objinfo->Segment  = mapSegmentNumber(ObjH);
   objinfo->Name     = (char *)mapObjectName(ObjH);
   objinfo->KeyName  = (char *)mapObjectRscKey(ObjH);
   objinfo->Key      = (char *)mapGetRscObjectKey(RscH, FRetIncode);
   Result = true;
 }

 if (SelectH != 0) mapDeleteSelectContext(SelectH);

 return Result;
}

//------------------------------------------------------------------
// выполнить диалог для компонента QDMapObj
// Вход mapobj  - компонент QDMapObj - если он настроен на какой либо вид объекта
//                из классификатора, то при старте диалог будет настроен на него
//   createflag - надо ли после успешного выполнения диалога сформировать описание
//                QDMapObj (вид объекта, метрика, семантика) Заново или же установить
//                только вид
//------------------------------------------------------------------
bool QDMapSelectObjectRsc::ExecuteForMapObj(QDMapObj *mapobj, bool createflag)
{
 TMAPOBJECTINFO objinfo;
 bool           Result = false;

 // это важно
 if (mapobj->GetMapView() != GetMapView())
   return Result;

 if (mapobj->GetInCode() != 0)
 {
   FRetSite   = mapobj->GetSiteNumber();
   FRetIncode = mapobj->GetInCode();
 }
 else FRetSite = 0;
 Result = Execute(&objinfo);

 if (Result == true)
 {
   if (createflag == true)
   {
     mapobj->SetObjHandle(ObjH);
     Result = (mapobj->GetObjHandle() != 0);
   }
   else
   {
     HSITE hsite =
       mapobj->GetMapView()->VarMapSites->GetMapSites(objinfo.NumbSite)->GetSiteHandle();

     mapSetObjectMap(mapobj->GetObjHandle(), hsite);
     Result = (mapDescribeObject(mapobj->GetObjHandle(), objinfo.Incode) != 0);
   }
 }

 return Result;
}

//------------------------------------------------------------------
//  Установить  текущий объект карты в диалоге  по внешнему коду и локализации
//  Вход - sitenumber - номер карты в списке (для -1 - ищется во всех классификаторах)
//       - excode     - клас код объекта
//       - local      - локализация (для -1 - локализация любая)
//------------------------------------------------------------------
bool QDMapSelectObjectRsc::SetBeginObjectForExcode(int sitenumber,
                                            int excode, int local)
{
 HRSC       RscH;
 HMAP       MapH;
 HSITE      SiteH;
 int        SiteCount, iLocal, iSiteN;
 bool       Result = false;

 if (VarMapView == NULL)
   return Result;
 MapH = VarMapView->GetMapHandle();
 if (MapH == 0)
   return Result;

 if (sitenumber == -1)
 {
   iSiteN = 0;
   SiteCount = mapGetSiteCount(MapH);
 }
 else
 {
   iSiteN    = sitenumber;
   SiteCount = sitenumber;
 }

 while (iSiteN <= SiteCount)
 {
     SiteH = mapGetSiteIdent(MapH, iSiteN);
     if (SiteH == 0) break;
     RscH = mapGetRscIdent(MapH, SiteH);
     if (RscH == 0) break;
     if (local != -1)
       FRetIncode = mapGetRscObjectCodeByNumber(RscH, excode, local, 1);
     else // переберем все локализации
     {
       for (iLocal = 0; iLocal < 6; iLocal++)
       {
          FRetIncode = mapGetRscObjectCodeByNumber(RscH, excode, iLocal, 1);
          if (FRetIncode != 0) break;
       }
     }

     if (FRetIncode != 0)
     {
       FRetSite = iSiteN;
       Result   = true;
       break;
     }
     iSiteN++;
  }  // iSiteN <= SiteCount

  return Result;
}

//------------------------------------------------------------------
//  Установить  текущий объект карты в диалоге по названию и локализации
//  Вход - sitenumber - номер карты в списке (для -1 - ищется во всех классификаторах)
//       - nameobj      - название объекта
//       - local      - локалаизация (для -1 - локализация любая)
//------------------------------------------------------------------
bool QDMapSelectObjectRsc::SetBeginObjectForName(int sitenumber,
                                                 char *nameobj,
                                                 int  local)
{
 HRSC      RscH;
 HMAP      MapH;
 HSITE     SiteH;
 int       SiteCount, iSiteN, ObjCount, jj;
 RSCOBJECT RscObject;
 bool      Result = false;

 if (VarMapView == NULL) return Result;
 MapH = VarMapView->GetMapHandle();
 if (MapH == 0) return Result;

 if (sitenumber == -1)
 {
   iSiteN = 0;
   SiteCount = mapGetSiteCount(MapH);
 }
 else
 {
   iSiteN    = sitenumber;
   SiteCount = sitenumber;
 }

 FRetIncode = 0;
 while (iSiteN <= SiteCount)
 {
     SiteH = mapGetSiteIdent(MapH, iSiteN);
     if (SiteH == 0) break;
     RscH = mapGetRscIdent(MapH, SiteH);
     if (RscH == 0)  break;
     ObjCount = mapGetRscObjectCount(RscH);
     for (jj = 1; jj <= ObjCount; jj++)
     {
         if (mapGetRscObject(RscH, jj, &RscObject) == 0)
           break;
         if ((local != -1) && (RscObject.Local != local))
           continue;
         if (stricmp((char *)RscObject.Name, nameobj) == 0)
         {
              FRetIncode = jj;
              break;
         }
     }

     if (FRetIncode != 0)
     {
          FRetSite = iSiteN;
          Result   = true;
          break;
     }
     iSiteN++;
 }  // iSiteN <= SiteCount

 return Result;
}

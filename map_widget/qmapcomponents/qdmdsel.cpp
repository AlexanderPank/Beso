/******** QDMDSEL.CPP **********************************************
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
*             КОМПОНЕНТ - ДИАЛОГ ВЫБОРА СОСТАВА                    *
*              ОБЪЕКТОВ ДЛЯ ПОИСКА/ОТОБРАЖЕНИЯ                     *
*                                                                  *
*******************************************************************/

#include "qdmdsel.h"
#include "qdmcmp.h"
#include "qdmselt.h"
#include "mapapi.h"
#include "maplib.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГ ВЫБОРА СОСТАВА                  *
//+++ ОБЪЕКТОВ ДЛЯ ПОИСКА/ОТОБРАЖЕНИЯ              +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapSelectDialog::QDMapSelectDialog(QWidget *parent, const char *name)
    :QDMapComponent(parent, name)
{
    VarNote           = 0;
    VarModalResult    = 0;
    VarSeekMapAll     = TRUE;//FALSE;
    VarSeekMapNumber  = 0;
    VarSeekVisual     = TRUE;//FALSE;
    hide();
}

//------------------------------------------------------------------
// Указатель на карту
//------------------------------------------------------------------
QDMapView *QDMapSelectDialog::GetMapView() const
{
    return (QDMapView *)VarMapView;
}

void QDMapSelectDialog::SetMapView(QDMapView *value)
{
    SetMapViewAcces(value);
}

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void InitTaskParmDSel(TASKPARM *taskparm)
{
    // Заполнение параметров прикладной задачи
    memset((char *)taskparm, 0, sizeof(TASKPARM));
    taskparm->Language  = ML_RUSSIAN;
}


//------------------------------------------------------------------
// Выполнить диалог для mapselect
// Если viewoffind = true  то для режима Поиска объектов
//                         иначе для режима Отображения объектов
// Выход = true  - установили (для отображения необходимо QDMapView->repaint())
//         false - отказались
//------------------------------------------------------------------
bool QDMapSelectDialog::Execute(QDMapSelect *mapselect, bool viewoffind)
{
    const char *NameFuncView  = "selSetViewStaffEx",
            *NameFuncFind  = "selSetObjectsSearch";

    long int   (WINAPI * selSetViewStaffEx)(HMAP , TASKPARM* , long int* , int );
    long int   (WINAPI * selSetObjectsSearch)(HMAP , TASKPARM* , long int* );
    HINSTANCE  libinst;
    int        ii, RetFunc;
    QDMapView  *MapView;
    TASKPARM   TaskParm;
    bool       ret = false;

    MapView = GetMapView();
    if (MapView          == NULL) return ret;
    if (MapView->GetMapHandle() == 0) return ret;
    if (mapselect         == NULL) return ret;

    if (viewoffind == false)
    {
        selSetViewStaffEx = (long int (WINAPI *)(HMAP , TASKPARM* , long int* , int ))
                mapLoadLibrary(MAPSELECLIB , &libinst, NameFuncView);
        if (selSetViewStaffEx == NULL) return ret;
    }
    else
    {
        selSetObjectsSearch = (long int (WINAPI *)(HMAP , TASKPARM* , long int*))
                mapLoadLibrary(MAPSELECLIB , &libinst, NameFuncFind);
        if (selSetObjectsSearch == NULL) return ret;
    }

    // Заполнение параметров прикладной задачи
    InitTaskParmDSel(&TaskParm);
    TaskParm.HelpName = NULL;

    if (viewoffind == false) // работаем для целей селекции состава отображения
        //----------------------------------------------------------------------
    {
        // Установим системный ViewSelect
        if ((MapView->VarViewSelect != mapselect) || (MapView->VarSelecting == false))
        {
            for (ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
                mapSetSiteViewSelect(MapView->GetMapHandle(),
                                     MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(),
                                     mapselect->GetHSelect(ii));
        }

        // Вызовем диалог
        RetFunc = selSetViewStaffEx(MapView->GetMapHandle(), &TaskParm, &VarNote, 0);

        if (RetFunc != 0) // фильтр изменился и его перенесем в mapselect
        {
            for (ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
                mapGetSiteViewSelect(MapView->GetMapHandle(),
                                     MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(),
                                     mapselect->GetHSelect(ii));
            ret = true;
        }

        // Восстановим системный ViewSelect из QDMapView->VarViewSelect
        if ((MapView->VarViewSelect != mapselect) || (MapView->VarSelecting == FALSE))
            for (ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
                mapSetSiteViewSelect(MapView->GetMapHandle(),
                                     MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(),
                                     mapselect->GetHSelect(ii));

        GetMapView()->Repaint();   // 26/09/07
    } // ViewOrFind = false работаем для целей селекции состава отображения
    //----------------------------------------------------------------------
    else
    {
        // Работаем для целей селекции состава поиска
        // Установим системный SeekSelect
        for (ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
            mapSetSiteSeekSelect(MapView->GetMapHandle(),
                                 MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(),
                                 mapselect->GetHSelect(ii));

        if (VarSeekMapAll == TRUE)
        {
            mapSetTotalSeekMapRule(MapView->GetMapHandle(), -1);
        }
        else
            // Установить правило обобщенного поиска по картам
            // number - номер карты, по которой выполняется поиск,
            // если number == -1, поиск будет выполняться по всем картам
            // (0 - карта местности, 1...n - пользовательские карты)
            mapSetTotalSeekMapRule(MapView->GetMapHandle(), VarSeekMapNumber);
        if (VarSeekVisual)
            // Установить правило обобщенного поиска для
            // отображаемых объектов карты
            // Если view == 0, поиск выполняется среди всех объектов
            // карты, иначе - среди отображаемых (SeekViewObject())
            mapSetTotalSeekViewRule(MapView->GetMapHandle(), 1);
        else
            mapSetTotalSeekViewRule(MapView->GetMapHandle(), 0);

        // Вызовем диалог
        RetFunc = selSetObjectsSearch(MapView->GetMapHandle(), &TaskParm, &VarNote);
        if (RetFunc != 0) // фильтр изменился и его перенесем в mapselect
        {
          for (ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
          {
            mapGetSiteSeekSelect(MapView->GetMapHandle(),
                                 MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(),
                                 mapselect->GetHSelect(ii));

            if (MapView->IsViewFrameActive())
            {
              HSELECT selectForFrameView = mapselect->GetHSelect(ii);
              DFRAME viewFrame;
              MapView->GetViewFrame(&viewFrame);

              // Ограиничить область поиска рамкой
              mapSelectSeekAreaFrame(selectForFrameView, &viewFrame, 0.0, 0, 1, 0, 0);
              mapSetSiteSeekSelect(MapView->GetMapHandle(), MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(), selectForFrameView);
            }
          }
            ret = true;
        }

        if ((VarSeekMapNumber = mapGetTotalSeekMapRule(MapView->GetMapHandle())) == -1)
            VarSeekMapAll = TRUE;
        else VarSeekMapAll = FALSE;
        if (mapGetTotalSeekViewRule(MapView->GetMapHandle()))
            VarSeekVisual = TRUE;
        else VarSeekVisual = FALSE;

        if (VarSeekMapAll == FALSE)
        {
            mapselect->SetAllFindSites(false);
            mapselect->SetMapSites(VarSeekMapNumber, true);
        }
        else
        {
            mapselect->SetAllFindSites(true);
        }
    } // ViewOrFind = true работаем для целей селекции состава отображения

    if (ret == TRUE)
    {
        mapselect->VarEdit = TRUE;
    }

    switch (RetFunc)
    {
        case 1  : VarModalResult = MROK;break;
        case 2  : VarModalResult = MRALLMARK;break;
        default : VarModalResult = MRCANCEL;break;
    }

    // Выгрузка библиотеки
    if (libinst)    mapFreeLibrary(libinst);

    return ret;
}

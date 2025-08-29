/******** QDMMTR.CPP ***********************************************
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
*               КЛАССЫ ДОСТУПА К МАТРИЦЕ ВЫСОТ                     *
*                                                                  *
*******************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>

#include "qdmmtr.h"
#include "qdmcmp.h"
#include "mapapi.h"

//++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К РАСТРУ          +++
//++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapMtr::QDMapMtr(QDMapViewAcces* mapview)
{
    VarMapView  = mapview;
    VarMapPoint = new QDMapPointAcces(mapview);
    VarNumber   = 0;
}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapMtr::~QDMapMtr()
{
    delete VarMapPoint;
}

bool QDMapMtr::MapIsOK()
{
    if ((VarNumber <= 0) || (VarMapView == NULL) || (VarMapView->GetMapHandle() == 0))
        return false;
    else return true;
}

//------------------------------------------------------------------
// Имя матричной карты
//------------------------------------------------------------------
char *QDMapMtr::GetMtrFileName()
{
    if (!MapIsOK()) return 0;

    return (char *)mapGetMtrName(VarMapView->GetMapHandle(), VarNumber);
}

//------------------------------------------------------------------
// Получить параметры матрицы
//------------------------------------------------------------------
bool QDMapMtr::GetMtrDesc(MTRDESCRIBE *desc)
{
    if (!MapIsOK())
      return false;

    if (mapGetMtrDescribe(VarMapView->GetMapHandle(), VarNumber, desc))
      return true;
    else
      return false;
}

//------------------------------------------------------------------
// Разрешить/запретить отображение матрицы
//------------------------------------------------------------------
bool QDMapMtr::GetMtrView()
{
    if (!MapIsOK()) return false;

    if (mapGetMtrView(VarMapView->GetMapHandle(), VarNumber)) return true;
    else return false;
}

void QDMapMtr::SetMtrView(bool value)
{
    int view;

    if (!MapIsOK()) return;
    if (value) view = 1;
    else view = 0;
    mapSetMtrView(VarMapView->GetMapHandle(), VarNumber, view);
}

//------------------------------------------------------------------
// Запросить/Установить порядок отображения матрицы
//  (0 - под картой, 1 - над картой)
//------------------------------------------------------------------
int QDMapMtr::GetMtrViewOrder()
{
    if (!MapIsOK()) return -1;
    return mapGetMtrViewOrder(VarMapView->GetMapHandle(), VarNumber);
}

void QDMapMtr::SetMtrViewOrder(int value)
{
    int ii;

    if (!MapIsOK()) return;

    ii = mapGetMtrViewOrder(VarMapView->GetMapHandle(), VarNumber);

    if (ii == value) return;
    mapSetMtrViewOrder(VarMapView->GetMapHandle(), VarNumber, value);
    ((QDMapView *)VarMapView)->Repaint();
}

//------------------------------------------------------------------
// Указатель на компонент карты
//------------------------------------------------------------------
QDMapView  *QDMapMtr::GetMapView()
{
    return (QDMapView *)VarMapView;
}

//++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДОСТУПА К МАТРИЦЕ - КОНТЕЙНЕР ++
//++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapMtrs::QDMapMtrs(QDMapViewAcces* mapview)
{
    VarMapMtr = new QDMapMtr(mapview);
}

//------------------------------------------------------------------
//  Деструктор
//------------------------------------------------------------------
QDMapMtrs::~QDMapMtrs()
{
    delete VarMapMtr;
}

//------------------------------------------------------------------
// Количество файлов матричной информации в списке
//------------------------------------------------------------------
int QDMapMtrs::GetMtrCount()
{
    if (VarMapMtr->VarMapView == NULL)
      return 0;
    if (VarMapMtr->VarMapView->GetMapHandle() == 0)
      return 0;

    return mapGetMtrCount(VarMapMtr->VarMapView->GetMapHandle());
}

//------------------------------------------------------------------
// Доступ к файлу матричной информации по порядковому номеру
//------------------------------------------------------------------
QDMapMtr *QDMapMtrs::GetMapMtrs(int number)
{
    VarMapMtr->VarNumber = number;
    return VarMapMtr;
}

//------------------------------------------------------------------
// Доступ к файлам матричной информации по наименованию
//------------------------------------------------------------------
QDMapMtr *QDMapMtrs::GetNameMtrs(char *mtrname)
{
    int  ii;
    char *pc;

    VarMapMtr->VarNumber = 0;

    if (VarMapMtr->VarMapView == NULL) return VarMapMtr;
    if (VarMapMtr->VarMapView->GetMapHandle() == 0) return VarMapMtr;

    for (ii = 1;ii <= GetMtrCount();ii++)
    {
        pc = (char *)mapGetMtrName(VarMapMtr->VarMapView->GetMapHandle(), ii);
        if (strcmp(pc, mtrname) == 0)
        {
            VarMapMtr->VarNumber = ii;
            return VarMapMtr;
        }
    }

    return VarMapMtr;
}

//------------------------------------------------------------------
// Доступ к файлам матричной информации по координатам в метрах
// Number - номер файла матричной информации в перекрытии
//------------------------------------------------------------------
QDMapMtr *QDMapMtrs::GetPointMtrs(QDMapPointAcces *mappoint, int number)
{
    VarMapMtr->VarNumber = 0;

    if (VarMapMtr->VarMapView == NULL) return VarMapMtr;
    if (VarMapMtr->VarMapView->GetMapHandle() == 0) return VarMapMtr;

    VarMapMtr->VarNumber = mapGetMtrNumberInPoint(VarMapMtr->VarMapView->GetMapHandle(),
                                                  mappoint->VarX, mappoint->VarY, number);

    return VarMapMtr;
}

//------------------------------------------------------------------
// Открыть файл матричной информации
//------------------------------------------------------------------
int QDMapMtrs::Append(char *mtrfilename)
{
    int ret;

    if (VarMapMtr->VarMapView == NULL)
      return 0;
    if (VarMapMtr->VarMapView->GetMapHandle() == 0)
      return 0;

    ret = mapOpenMtrForMap(VarMapMtr->VarMapView->GetMapHandle(), mtrfilename, 0);
    if (ret != 0)
    {
        VarMapMtr->VarNumber = ret;
    }

    return ret;
}

//------------------------------------------------------------------
// Закрыть файл матричной информации по наименованию
//------------------------------------------------------------------
void QDMapMtrs::DeleteName(char *mtrfilename)
{
    int ii;

    if (VarMapMtr->VarMapView == NULL) return;
    if (VarMapMtr->VarMapView->GetMapHandle() == 0) return;

    GetNameMtrs(mtrfilename);
    ii = VarMapMtr->VarNumber;
    if (!ii) return;
    Delete(ii);
}

//------------------------------------------------------------------
// Закрыть файл матричной информации по порядковому номеру
// Внимание! При закрытии файла матричной информации с младшим номером
// старшие номера смещаются.
//------------------------------------------------------------------
void QDMapMtrs::Delete(int number)
{
    if (VarMapMtr->VarMapView == NULL) return;
    if (VarMapMtr->VarMapView->GetMapHandle() == 0) return;

    VarMapMtr->VarNumber = number;

    mapCloseMtrForMap(((QDMapView*)VarMapMtr->VarMapView)->GetMapHandle(), number);
    VarMapMtr->VarNumber = 0;
}


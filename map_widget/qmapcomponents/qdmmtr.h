/******** QDMMTR.H *************************************************
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

#ifndef QDMMTR_H
#define QDMMTR_H

#include "maptype.h"
#include "qdmpoina.h"

class QDMapViewAcces;
class QDMapView;

//++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К МАТРИЦЕ         +++
//++++++++++++++++++++++++++++++++++++++++

class QDMapMtr
{
  public :
    QDMapMtr(QDMapViewAcces *mapview);
    ~QDMapMtr();
    // Указатель на компонент карты
    QDMapView  *GetMapView();
    // Имя матричной карты
    char *GetMtrFileName();
    bool MapIsOK();
    // Разрешить/запретить отображение матрицы
    bool GetMtrView();
    void SetMtrView(bool value);
    // Порядок отображения матрицы относительно карты
    int  GetMtrViewOrder();
    void SetMtrViewOrder(int value);
    // Получить параметры матрицы
    bool GetMtrDesc(MTRDESCRIBE *desc);

  public :
    // Пользователю только на чтение
    QDMapViewAcces  *VarMapView;
    QDMapPointAcces *VarMapPoint;
    int             VarNumber;
};

//++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДОСТУПА К МАТРИЦЕ - КОНТЕЙНЕР ++
//++++++++++++++++++++++++++++++++++++++++++

class  QDMapMtrs
{

public :
    QDMapMtrs(QDMapViewAcces *mapview);
    ~QDMapMtrs();

    // Количество файлов матричной информации в списке
    int      GetMtrCount();
    // Доступ к файлу матричной информации по порядковому номеру
    QDMapMtr *GetMapMtrs(int number);
    // Доступ к файлам матричной информации по наименованию
    QDMapMtr *GetNameMtrs(char *mtrname);
    // Доступ к файлам матричной информации по координатам в метрах
    // Number - номер файла матричной информации в перекрытии
    QDMapMtr *GetPointMtrs(QDMapPointAcces *mappoint, int number);
    // Открыть файл матричной информации
    int      Append(char *mtrfilename);
    // Закрыть файл матричной информации по наименованию
    void     DeleteName(char *mtrfilename);
    // Закрыть файл матричной информации по порядковому номеру
    // Внимание! При закрытии файла матричной информации с младшим номером
    // старшие номера смещаются.
    void     Delete(int number);

public :
    // Для служебного пользования !!!!!
    QDMapMtr *VarMapMtr;
};

#endif // QDMMTR_H

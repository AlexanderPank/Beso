/******** QDMRST.H *************************************************
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
*                 КЛАССЫ ДОСТУПА К РАСТРУ                          *
*                                                                  *
*******************************************************************/

#ifndef QDMRST_H
#define QDMRST_H

#include "maptype.h"

class QDMapViewAcces;

//++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К РАСТРУ          +++
//++++++++++++++++++++++++++++++++++++++++

class QDMapRst
{
  public :
    QDMapRst(QDMapViewAcces *mapview);
    ~QDMapRst();
    // Имя растровой карты
    char *GetRstFileName();
  public :
// Для служебного пользования !!!!!
    QDMapViewAcces *VarMapView;
    int            VarNumber;
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К РАСТРУ - КОНТЕЙНЕР         +++
//+++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapRsts
{
  public :
    QDMapRsts(QDMapViewAcces *mapview);
    ~QDMapRsts();
    // Количество файлов растровой информации в списке
    int      GetRstCount();
    // Доступ к файлам растровой информации по порядковому номеру
    QDMapRst *GetMapRsts(int number);
    // Доступ к файлам растровой информации по наименованию
    QDMapRst *GetNameRsts(char *rstname);
    // Добавить растровую карту
    int      Append(char *rstfilename);
    // Удалить растровую карту по наименованию
    void     DeleteName(char *rstfilename);
    // Удалить растровую карту по порядковому номеру
    // Внимание! При удалениии растровой карты с младшим номером
    // старшие номера смещаются.
    void Delete(int number);
    // Сконвертировать растр из изображения
    int ImageToRstDialog(char *inname, char *outname);

  public :
 // Для служебного пользования !!!!!
    QDMapRst *VarMapRst;
};

#endif  // QDMRST_H

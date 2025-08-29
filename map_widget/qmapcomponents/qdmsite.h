/******* QDMSITE.H *************************************************
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

#ifndef QDMSITE_H
#define QDMSITE_H

#include "maptype.h"
#include "qdmlayr.h"

class QDMapPointAcces;
class QDMapViewAcces;

//++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДОСТУПА К ПОЛЬЗОВАТЕЛЬСКОЙ КАРТЕ +++
//++++++++++++++++++++++++++++++++++++++++++++++

class  QDMapSite
{
public :
    QDMapSite(QDMapViewAcces *mapview);
    ~QDMapSite();

    HSITE  GetSiteHandle();
    // Номенклатура пользовательской карты обстановки
    const char *GetSiteListName(int listnumber = 1);
    // Имя пользовательской карты обстановки
    char   *GetSiteName();
    // Номер пользовательской карты обстановки в цепочке
    int    GetNumber();
    // Имя файла пользовательской карты обстановки
    char   *GetSiteFileName();
    // Базовый масштаб пользовательской карты обстановки
    int    GetSiteScale();
    // Тип пользовательской карты обстановки
    int    GetSiteType();
    // Отображение карты
    void   SetSiteVisible(bool visible);
    bool   GetSiteVisible();
    // Установить/запосить новые габариты пользовательской карты с изменением параметров скролинга
    int    SetSiteBorder(MAPDFRAME *frame, int place);
    MAPDFRAME GetSiteBorder(int place);

public :
// Для служебного пользования !!!!!
    QDMapViewAcces  *VarMapView;
    HSITE           VarSite;
    QDMapPointAcces *VarMapPoint;
    QDMapLayers     *VarSiteLayers;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДОСТУПА К ПОЛЬЗОВАТЕЛЬСКОЙ КАРТЕ - КОНТЕЙНЕР +++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSites
{
public :
    QDMapSites(QDMapViewAcces *mapview);
    ~QDMapSites();
    // Количество пользовательских карт обстановки в списке
    int       GetSiteCount();
    // Доступ к пользовательской карте обстановки по порядковому номеру
    QDMapSite *GetMapSites(int number);
    // Доступ к пользовательской карте обстановки по наименованию
    QDMapSite *GetSitesForName(char *sitename);
    // Доступ к пользовательской карте обстановки полному имени файла
    QDMapSite *GetSitesForFileName(char *sitefilename);
    // Добавить пользовательскую карту обстановки
    int       Append(char *sitefilename);
    // Удалить пользовательскую карту обстановки по наименованию
    void      DeleteSiteName(char *sitename);
    // Удалить пользовательскую карту обстановки по полному имени файла
    void      DeleteFileName(char *sitefilename);
    // Удалить пользовательскую карту обстановки по порядковому номеру
    // Внимание! При удалениии растровой карты с младшим номером
    // старшие номера смещаются.
    void      Delete(int number);
public :
// Для служебного пользования !!!!!
    QDMapSite *VarMapSite;
};

#endif  //  QDMSITE_H


/******** QDMDSEL.H ************************************************
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
*             КОМПОНЕНТ - ДИАЛОГ ВЫБОРА СОСТАВА                    *
*              ОБЪЕКТОВ ДЛЯ ПОИСКА/ОТОБРАЖЕНИЯ                     *
*                                                                  *
*******************************************************************/

#ifndef QDMDSEL_H
#define QDMDSEL_H

#include "qdmcmp.h"
#include "qdmtype.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГ ВЫБОРА СОСТАВА                  *
//+++ ОБЪЕКТОВ ДЛЯ ПОИСКА/ОТОБРАЖЕНИЯ              +
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSelect;

class QDMapSelectDialog : public QDMapComponent
{
    Q_OBJECT

public :
    QDMapSelectDialog(QWidget *parent = 0, const char *name = 0);
    ~QDMapSelectDialog() {};
    // Указатель на карту
    virtual void SetMapView(QDMapView *value);
    QDMapView    *GetMapView() const;
    bool         Execute(QDMapSelect *mapselect, bool viewoffind);

public :
    long int  VarNote;           // Номер активной закладки в диалоге
    int       VarModalResult;
    int       VarSeekMapNumber;  // Номер карты для поиска (имеет смысл в случае VarSeekMapAll = false)
    bool      VarSeekMapAll;     // Признак необходимости поиска по всем доступным картам
    bool      VarSeekVisual;     // Флаг поиска среди видимых на карте объектов
};


#endif // QDMDSEL_H

/******** QDMDGTOB.H ***********************************************
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
*                     КОМПОНЕНТ - ДИАЛОГ                           *
*          "ВЫБОРА ТИПА ОБЪЕКТА ИЗ КЛАССИФИКАТОРА"                 *
*                                                                  *
*******************************************************************/

#ifndef QDMDGTOB_H
#define QDMDGTOB_H

#include "qdmcompa.h"

class QDMapView;
class QDMapObj;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГА                                +
//+++ "ВЫБОР ТИПА ОБЪЕКТА ИЗ КЛАССИФИКАТОРА"       +
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSelectObjectRsc : public QDMapComponent
{
    Q_OBJECT

public :
    // Для пользователя
    //------------------------------
    QDMapSelectObjectRsc(QWidget *parent = 0, const char *name = 0);
    ~QDMapSelectObjectRsc() {};
    // Указатель на карту
    virtual void SetMapView(QDMapView *value);
    QDMapView    *GetMapView() const;
    // Выполнить диалог для компонента QDMapObj
    // Вход mapobj  - компонент QDMapObj - если он настроен на какой либо вид объекта
    //                из классификатора, то при старте диалог будет настроен на него
    //   createflag - надо ли после успешного выполнения диалога сформировать описание
    //                QDMapObj (вид объекта, метрика, семантика) Заново или же установить
    //                только вид
    bool         ExecuteForMapObj(QDMapObj *mapobj, bool createflag);

    // Для служебного пользования !!!!!
    //------------------------------
    // Выполнить диалог и заполнить структуру описания объекта
    // Выход = true  - выбрали и заполнили objinfo
    //         false - отказались
    bool         Execute(TMAPOBJECTINFO *objinfo);
    //  Установить  текущий объект карты в диалоге  по внешнему коду и локализации
    //  Вход - sitenumber - номер карты в списке (для -1 - ищется во всех классификаторах)
    //       - excode     - клас код объекта
    //       - local      - локалаизация (для -1 - локализация любая)
    bool         SetBeginObjectForExcode(int sitenumber, int excode, int local);
    //  Установить  текущий объект карты в диалоге по названию и локализации
    //  Вход - sitenumber - номер карты в списке (для -1 - ищется во всех классификаторах)
    //       - nameobj      - название объекта
    //       - local      - локалаизация (для -1 - локализация любая)
    bool         SetBeginObjectForName(int sitenumber, char *nameobj, int local);

public :
    // Для пользователя
    //------------------------------
    HOBJ    ObjH;                      //
    int     FSiteNumber;               // номер карты в которой выбирается тип объекта
    int     FRetIncode;                // внутренний код выбранного объекта
    int     FLocals[6];                // локализации доступные в диалоге
    int     FRetSite;                  //
    char    FCaption[256];             // Заголовок диалога
    bool    FMapEditEnabled;           // флаг доступности карт, которые возможно редактировать
    bool    FRestoreEnabled;           // нужно ли восстанавливать состояние диалога
    bool    FHelpEnabled;              //
};

#endif // QDMDGTOB_H


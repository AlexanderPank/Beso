/******** QDMDSEM.H ************************************************
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
*                  КЛАССЫ ДОСТУПА К СЕМАНТИКЕ                      *
*                                                                  *
*                                                                  *
*******************************************************************/

#ifndef QDMDSEM_H
#define QDMDSEM_H

#include "qdmtype.h"

class QDMapObjAcces;
class QDMapViewAcces;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К СЕМАНТИКЕ ОБЪЕКТА КАРТЫ     *
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSemantic
{
  public :

    QDMapSemantic(QDMapObjAcces *mapobj);
    ~QDMapSemantic();

    // Описание семантики по ее номеру в объекте
    RSCSEMANTICEX GetDescribe(int number);
    // Количество записей в классификаторе значений семантики по ее номеру в объекте
    int          GetClassificatorCount(int number);
    // Название значения семантики из классификатора значений по номеру семантики в объекте
    // и номеру значения
    char         *GetClassificatorName(int number1, int numbervalue);
    // Код значения семантики из классификатора значений по номеру семантики в объекте
    // и номеру значения
    int          GetClassificatorCode(int number1, int numbervalue);
    // Количество семантических характеристик
    int          GetSemanticCount();
    // Название семантической характеристики
    char         *GetSemanticName(int number);
    // Код семантической характеристики
    int          GetSemanticCode(int number);
    // Запросить/установить значение семантической характеристики
    char         *GetSemanticValue(int number);
    void         SetSemanticValue(int number, char *value);
    // Количество допустимых кодов семантики
    int          GetAvailabelSemanticCount();
    // Допустимые внешние кода семантики
    int          GetAvailabelSemanticCode(int number);
    // Наименования допустимых семантик
    char        *GetAvailabelSemanticName(int code);
    // Количество значений в классификаторе значений семантик
    int         GetSemanticValueCount(int code);
    // Значения допустимых семантик
    char        *GetSemanticValueName(int code, int number);
    // Добавить новую характеристику
    int         Add(int code);
    // Удалить характеристику
    void        Delete(int number);
    // Найти номер семантики в объекте по ее коду
    int         FindByCode(int code);
    // Удалить семантику в объекте по ее коду одну или все
    void        DeleteByCode(int code, bool all);
    // Задать/запросить способ раскодирования значений семантики
    void           SetSemanticStyle(int value);
    int            GetSemanticStyle();

public :
    // Только на чтение !!!!!!!!!
    QDMapObjAcces  *VarMapObj;
    RSCSEMANTICEX  VarSemanticType;
    int            VarSemanticStyle;
    char           Value[256];
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++  КЛАСС ДОСТУПА К ОПИСАНИЮ СЕМАНТИК В КЛАССИФИКАТОРЕ +
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class  QDMapSemantics
{
  public :
    QDMapSemantics(QDMapViewAcces *mapview);
    ~QDMapSemantics();

    // Количество кодов семантики в класификаторе
    int           GetSemanticCount();
    // Код семантики
    int           GetSemanticCode(int number);
    // Наименование семантики
    char          *GetSemanticName(int number);
    // Наименование семантики по коду
    char          *GetSemanticNameCode(int code);
    // Код семантики по ее наименованию
    int           GetSemanticCodeName(char *name);
    // Порядковый номер семантики по ключу
    int           GetNumberByKey(char *key);
    // Тип семантики по ее коду
    int           GetTypeByCode(int code);
    // Уникальное название-ключ семантики по ее коду
    char          *GetKeyByCode(int code);
    // Количество значений классификатора семантической характеристики по коду семантики
    // (если ее тип TCODE) При ошибке возвращает ноль
    int           GetClassificatorValueCount(int code);
    // Название значения характеристики из классификатора семантики по коду семантики и
    // последовательному номеру в классификаторе
    char          *GetClassificatorValueName(int code, int valuenumber);
    // Код значения характеристики из классификатора семантики по коду семантики и
    // последовательному номеру в классификаторе
    int           GetClassificatorValueCode(int code, int valuenumber);
    // Получить описание семантики по ее коду
    RSCSEMANTICEX GetDescribeByCode(int code);

    // Для служебного пользования  !!!!
    void          SetRscHandle(HRSC rsch);
    HRSC          GetRscHandle();

public :
    QDMapViewAcces *VarMapView;
    HRSC           VarHRsc;
    // ---------------------------------
    // Для пользователя
    int            VarSiteNumber;      // Номер пользовательской карты с которой работает
                                       // в данный момент компонент
    char           Value[256];
};

#endif   // QDMDSEM_H


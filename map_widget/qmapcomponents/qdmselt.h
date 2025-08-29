/******** QDMSELT.H ************************************************
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
*        КЛАСС КОНТЕКСТА УСЛОВИЙ ПОИСКА/ОТОБРАЖЕНИЯ                *
*                                                                  *
*******************************************************************/

#ifndef QDMSELT_H
#define QDMSELT_H

#include "maptype.h"

#define MAX_STRING 256
#define MAX_SELECT 10240

class QDMapViewAcces;
class QDMapPointAcces;
class QDMapObjAcces;

class QDMapSelect
{

public :
    QDMapSelect(QDMapViewAcces *mapview);
    ~QDMapSelect();

    void SetCountListSelect();
    // Проверить видимость объектов
    bool GetCheckKey(int key);
    // Включить/выключить объекты по внутреннему коду
    bool GetIncode(int incode);
    void SetIncode(int incode, bool value);
    // Включить/выключить объекты по внешнему коду и локализации
    bool GetExcode(int excode, int local);
    void SetExcode(int excode, int local, bool value);
    // Включить/выключить объекты по ключу (уникальному названию объекта в классификаторе)
    bool GetKeyObject(char *keyobject);
    void SetKeyObject(char *keyobject, bool value);
    // Включить/выключить слой (сегмент)
    bool GetLayers(int number);
    void SetLayers(int number, bool value);
    // Включить/выключить объекты по характеру локализации
    bool GetLocal(int objlocal);
    void SetLocal(int objlocal, bool value);
    // Установить максимальную границу номеров объектов
    DWORD GetMaxKey();
    void  SetMaxKey(DWORD value);
    // Установить минимальную границу номеров объектов
    DWORD GetMinKey();
    void  SetMinKey(DWORD value);
    // Установить доступ к объектам с заданными номерами
    // Если оба числа равны -1, то устанавливается доступ ко всем номерам.
    void SetSelectKey(int aMin, int aMax);
    // Координаты района отображения/печати (в пикселах)
    void GetLeftTop(double *left, double *top);
    void SetLeftTop(double left, double top);
    void GetRightBottom(double *right, double *bottom);
    void SetRightBottom(double right, double bottom);
    // Система координат на запись
    int  GetPlaceIn();
    void SetPlaceIn(int value);
    // Система координат на чтение
    int  GetPlaceOut();
    void SetPlaceOut(int value);
    // Включить/выключить лист карты
    bool GetLists(int number);
    void SetLists(int number, bool value);
    // Значение семантики
    char *GetSemanticValue(int number);
    // Код семантики
    int GetSemanticCode(int number);

    int GetSemanticCondition(int number);
    int GetSemanticCount();
    // Включить/выключить в карту обстановки
    bool GetMapSites(int number);
    void SetMapSites(int number, bool value);

    // Номер карты, для которй производится редактирование контекста
    void SetSiteNumber(int value);
    void SetMapView(QDMapViewAcces *value);
    // Для всех ли карт используются установки отбора
    bool GetAllFindSites();       // все ли Sites используются
    void SetAllFindSites(bool value);
    // Проверить Есть ли в списке объектов хотя бя один
    bool GetIsSample(int sitenumber);
    // Вернуть установленный в Select габариты селекции в пикселах
    RECT GetRect(bool notnull);
    // Вернуть установленный в Select габариты селекции в системе PlaceOut
    MAPDFRAME GetDFrame(bool notnull);
    // Запросить Hselect для текущей карты
    HSELECT GetHSelect(int number);
    // Обработка событий карты
    void MapAction(int action);

    // Для копирования объектов QDMapSelect
    void Assign(QDMapSelect *source);
    // Сбросить все условия поиска/отображения для текущей карты
    void Clear();
    // Сбросить все условия поиска/отображения для всех карт
    void ClearAll();
    // Сбросить все условия поиска по семантике
    void ClearSemantic();
    // Добавить условие по семантике
    int AddSemantic(int Code, char *value, int condition);
    // Удалить условие по семантике
    void DeleteSemantic(int number);
    // Считать текущий контекст отображения // Устаревшая
    void  SetSelect();
    // Очистить список объектов во всех контекстах условий поиска/отображения
    void ClearSampleAll();
    // Установить/Снять доступ к заданному объекту заданного листа карты.
    int  SelectSample(bool seting, char *listname, int key);
    // Установить доступность для участия в поиске/выделения тех карт, для которых доступен
    // хотя бы один объект в списке(была выполнена функция SelectSample)
    void SetSiteEnabledForSample();
    // Установить в контексте параметры поиска объектов по области.
    int  SelectSeekArea(int siten, QDMapObjAcces *object, double distance,
                        int filter, int inside, int visible, int action);
    // Сбросить в контексте параметры поиска объектов по области
    void UnselectSeekArea(int sitenumber);
    // Считать текущий контекст поиска карты   // 09/12/09
    void GetSeekSelect(int aSite);
    // Установить текущий контекст поиска карты
    void SetSeekSelect(int aSite);
    // Считать текущий контекст отображения карты   // 09/12/09
    void GetViewSelect(int aSite);
    // Установить текущий контекст отображения карты
    void SetViewSelect(int aSite);
    // Запрос числа объектов на карте, удовлетворяющих условиям поиска   // 09/12/09
    // Если aSiteNumber = -1 то подсчет для всех карт
    int GetSelectObjectCount(int aSiteNumber);
    // Удаление select по номеру сайта                    // 16/03/10
    void DeleteSiteFromListSelect(int numsite);

public :
    // Для внутреннего пользования   !!!!!!!!
    QDMapViewAcces  *VarMapView;
    QDMapPointAcces *VarMapPoint;
    HSELECT         VarListSelect[MAX_SELECT];
    MAPDFRAME       VarMapRect;
    int             VarSite;
    int             CountSelect;
    bool            VarEdit;
    bool            VarListUses[MAX_SELECT];
    char            StringOut[MAX_STRING];
};

#endif // QDMSELT_H

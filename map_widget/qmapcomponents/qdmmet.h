/******** QDMDMET.H ************************************************
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
*           КЛАСС МЕТРИЧЕСКОГО ОПИСАНИЯ ОБЪЕКТА КАРТЫ              *
*                                                                  *
*                                                                  *
*******************************************************************/

#ifndef QDMDMET_H
#define QDMDMET_H

class QDMapObjAcces;
class QDMapPointAcces;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС МЕТРИЧЕСКОГО ОПИСАНИЯ ОБЪЕКТА КАРТЫ    *
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapMetric
{
  public :

    QDMapMetric(QDMapObjAcces *mapobj);
    ~QDMapMetric();

    // Добавить точку в объект
    void             Append(int subobj, QDMapPointAcces *mappoint);
    // Удалить точку объекта
    void             Delete(int subobj, int number);
    // Вставить точку
    void             Insert(int subobj, int number, QDMapPointAcces *mappoint);
    // Обновить точку объекта
    void             Update(int subobj, int number, QDMapPointAcces *mappoint);
    // Запросить/установить количество подобъектов
    int              GetSubObjCount();
    void             SetSubObjCount(int value);
    // Запросить/установить количество точек в подобъекте
    int              GetPointCount(int subobj);
    void             SetPointCount(int subobj, int value);
    // Запросить/установить точки объекта
    QDMapPointAcces  *GetPoints(int subobj, int number);
    void             SetPoints(int subobj, int number, QDMapPointAcces *mappoint);

public :
    QDMapObjAcces   *VarMapObj;
    QDMapPointAcces *VarMapPoint;
};

#endif  // QDMDMET_H

/******* QDMNET.H **************************************************
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
*     ОПИСАНИЕ КЛАССА "КОМПОНЕНТ СЕТЕВОЙ ЗАДАЧИ"                   *
*                                                                  *
*******************************************************************/

#ifndef QDMNET_H
#define QDMNET_H

#include "qdmcmp.h"
#include "netapi.h"

class QDMapSelect;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ СЕТЕВОЙ ЗАДАЧИ"                            ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapNet : public QDMapComponent
{
    Q_OBJECT

public  :
    QDMapNet(QWidget * parent = 0, const char * name = 0);
    ~QDMapNet();

    virtual void SetMapView(QDMapView *value);
    QDMapView    *GetMapView() const;

    void         SetRscName(char *rscname);
    void         SetNetName(char *netname);
    void         SetSelect(QDMapSelect *mapselect);
    int          GetNetSitNumber();
    // Заполнить селект по результатам и получить кол-во ребер
    int          GetSelectResult();
    // построить сеть - вернуть handle Sit-а с сетью
    HSITE        BuildNet(bool ShowDialog);
    // найти наименьший путь между узлами с диалогом
    int          GetShotWayDlg(HOBJ NodeStart, HOBJ NodeFinish);
    // найти наименьший путь между узлами без диалогa по времени
    int          GetFastWay(HOBJ NodeStart, HOBJ NodeFinish, double speed);
    // найти наименьший путь между узлами без диалогa по растоянию
    int          GetShotWay(HOBJ NodeStart, HOBJ NodeFinish);
    // найти ребра на заданном удалении от узла
    int          GetEdgesFromNode(HOBJ Node, double Meters);
    // найти ближайший узел к точке
    int          GetNodeByPoint(HOBJ ObjHandle,  DOUBLEPOINT *PlanePoint);
    //-------  HGRAPH ---------------------
    // Открытие или создание массивов графа
    HGRAPH       OpenGraph(HMAP Map, HSITE Site);
    //   Закрыть
    int          CloseGraph(HGRAPH Graph);
    // найти наименьший путь между узлами без диалогa по времени c созданием объекта , содержаший результат
    int          GetFastWayAndCreateObject(HOBJ Info, HOBJ NodeStart, HOBJ NodeFinish, double speed);
    // найти наименьший путь между узлами без диалогa по растоянию c созданием объекта , содержаший результат
    int          GetShotWayAndCreateObject(HOBJ Info, HOBJ NodeStart, HOBJ NodeFinish);
    // найти наименьший путь между узлами с диалогом по времени c созданием объекта , содержаший результат
    int          GetShotWayDlgAndCreateObject(HOBJ Info, DOUBLEPOINT *PointStart, DOUBLEPOINT *PointFinish);
     // найти наименьший путь между узлами без диалогa по времени c созданием объекта , содержаший результат
    int          GetShotWayByPointAndCreateObject(HOBJ Info, DOUBLEPOINT *PointStart,
                                                  DOUBLEPOINT *PointFinish);
    // Определение кратчайшего расстояния между двумя точками с учетом одностороннего
    // движения и исключением ребер (аварийных участков)
    int          GetShotWayByPointExAndCreateObject(HOBJ Info, DOUBLEPOINT *PointStart,
                                                    DOUBLEPOINT *PointFinish, HSELECT select);
    // Вычисление минимального пути между точками (узлами сети)
    int          GetMinimalPath(DOUBLEPOINT *arraypoint, int count);
    // Вычисление минимального пути между точками (узлами сети)
    int          GetMinimalPathSite(HSITE tempsite, DOUBLEPOINT *arraypoint, int count);
    // Вычисление минимального пути между точками (узлами сети)
    double       GetMinimalPathEx(DOUBLEPOINT *arraypoint, long int *arraynumber, int count,
                                  int flagreturn, int flagcalc);
    // Получить идентификатор карты
    virtual HGRAPH GetGraphHandle();
    // Заполнить структуру для вызова задачи
    void         InitTaskParmEx(TASKPARMEX *taskparm);
    // Обработка событий карты
    void         MapAction(int action);

public:
   QDMapSelect *FSelect;
   HSITE       FHSite;
   HGRAPH      FGraph;
   char        FNetName[MAX_PATH];
   char        FRscName[MAX_PATH];
   int         FColor;
};

#endif // QDMAPNET_H

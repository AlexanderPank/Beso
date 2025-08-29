/******* QDMNET.CPP   **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                        FOR Qt-DESIGNER                           *
*                                                                  *
********************************************************************
*                                                                  *
*            ОПИСАНИЕ КЛАССА "КОМПОНЕНТ СЕТЕВОЙ ЗАДАЧИ"            *
*                                                                  *
*******************************************************************/

#include "qdmnet.h"
#include "qdmselt.h"
#include "mapapi.h"

#define KNOTINCODE      5558   // Excode узла
#define RIBINCODE       5557   // Excode ребра
#define RIBINCODE2      5562   // Excode ребра одностороннего

// Шаблоны сообщений (по коду языка)
const char *msg_BADRSC = "Классификатор \"%s\" не может быть использован для создания сети!";

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ СЕТЕВОЙ ЗАДАЧИ"                            ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapNet::QDMapNet(QWidget *parent, const char *name)
  :QDMapComponent(parent, name)
{
  FHSite   = 0;
  FGraph   = 0;
  strcpy(FRscName, "service.rsc");
  FNetName[0] = 0;
  FColor   = 0;
  FSelect  = new QDMapSelect(0);

  hide();
}

QDMapNet::~QDMapNet()
{
  if (FSelect != 0) delete FSelect;
  FSelect = 0;
}

QDMapView *QDMapNet::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapNet::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
  FSelect->SetMapView(value);
}

//-------------------------------------------
// Установить FSelect
//-------------------------------------------
void QDMapNet::SetSelect(QDMapSelect *value)
{
  if (value != 0)
  {
    if (GetMapView() != 0)
    {
      FSelect->Assign(value);
      FSelect->SetMapView(GetMapView());
    }
  }
  else FSelect->ClearAll();
}

//-------------------------------------------
// Построить сеть - вернуть handle Sit-а с сетью
//-------------------------------------------
HSITE QDMapNet::BuildNet(bool ShowDialog)
{
  TASKPARMEX  parm;
  QDMapSelect *OldMapSelect;     //что было установлено в карте (ядре)
  QDMapSelect *OldMapViewSelect; //что пользователь установил в MapView
  char        arrsit[MAX_PATH], arrrsc[MAX_PATH];
  int         ii;
  HSITE       Result = 0;

  if (GetMapView() == 0) return 0;
  if (FSelect == 0) return 0;

  OldMapSelect     = new QDMapSelect(GetMapView());
  OldMapViewSelect = new QDMapSelect(GetMapView());
  // запомнить условия выборки установленные во всех картах
  OldMapViewSelect->Assign(GetMapView()->VarViewSelect); //из карты
  OldMapSelect->GetSeekSelect(-1); // из MapView

  if (GetMapView()->VarViewSelect == 0) GetMapView()->SetViewSelect(FSelect);
  else GetMapView()->VarViewSelect->Assign(FSelect);
  FSelect->SetSeekSelect(-1);
  if (FColor != 0) GetMapView()->SetObjSelected(FSelect, FColor, true);

  InitTaskParmEx(&parm);
  parm.Resource = (HINSTANCE) GetMapView();
  memset(arrsit, 0, sizeof(arrsit));
  memset(arrrsc, 0, sizeof(arrrsc));
  strcpy(arrsit, FNetName);
  strcpy(arrrsc, FRscName);

  if (ShowDialog == true) ii = 1;
  else ii = 0;


  Result = onCreateNetDlgEx(GetMapView()->GetMapHandle(), &parm, arrsit, arrrsc, ii); // 26/10/2009

  if (Result != 0)
  {
    GetMapView()->SetObjSelected(0, 0, true);
    strcpy(FNetName, arrsit);
    strcpy(FRscName, arrrsc);
  }

  // записать обратно все условия выборки
  GetMapView()->VarViewSelect->Assign(OldMapViewSelect);// в MapView

  OldMapSelect->SetSeekSelect(-1); // в карту

  if (OldMapSelect != 0) delete OldMapSelect;         // удалить временные селекты
  if (OldMapViewSelect != 0) delete OldMapViewSelect; // удалить временные селекты

  FHSite = Result;

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ КРАТЧАЙШЕЕ РАССТОЯНИЕ МЕЖДУ УЗЛАМИ С ДИАЛОГОМ
//-------------------------------------------
int QDMapNet::GetShotWayDlg(HOBJ NodeStart, HOBJ NodeFinish)
{
  TASKPARMEX parm;
  int        Result = 0;

  if (GetMapView() == 0) return 0;

  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  if (onSeekPathDlgGraph(GetMapView()->GetMapHandle(), FGraph, &parm, NodeStart, NodeFinish) != 0)
    Result = 1;                 // 17/03/10

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ КРАТЧАЙШЕЕ РАССТОЯНИЕ МЕЖДУ УЗЛАМИ БЕЗ ДИАЛОГА ПО ВРЕМЕНИ
//-------------------------------------------
int QDMapNet::GetFastWay(HOBJ NodeStart, HOBJ NodeFinish, double speed)
{
  TASKPARMEX parm;
  int        Result = 0;

  if (GetMapView() == 0) return 0;

  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  if (onSeekPathGraph(GetMapView()->GetMapHandle(), FGraph, &parm, NodeStart, NodeFinish, 1, speed) != 0)
    Result = GetSelectResult();

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ КРАТЧАЙШЕЕ РАССТОЯНИЕ МЕЖДУ УЗЛАМИ БЕЗ ДИАЛОГА ПО РАСТОЯНИЮ
//-------------------------------------------
int QDMapNet::GetShotWay(HOBJ NodeStart, HOBJ NodeFinish)
{
  TASKPARMEX parm;
  int        Result = 0;

  if (GetMapView() == 0) return 0;

  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  if (onSeekPathGraph(GetMapView()->GetMapHandle(), FGraph, &parm, NodeStart, NodeFinish, 0, 0) != 0)
    Result = GetSelectResult();

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ РЕБРА НА ЗАДАННОМ РАССТОЯНИИ ОТ УЗЛА
//-------------------------------------------
int QDMapNet::GetEdgesFromNode(HOBJ Node, double Meters)
{
  TASKPARMEX parm;
  int        Result = 0;
  int        ret = 0;

  if (GetMapView() == 0) return 0;

  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);

  if (onDistFromKnotGraph(GetMapView()->GetMapHandle(), FGraph, &parm, Node, Meters) != 0)
    Result = GetSelectResult();

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ БЛИЖАЙШИЙ УЗЕЛ К ЗАДАННОЙ ТОЧКЕ
//-------------------------------------------
int QDMapNet::GetNodeByPoint(HOBJ ObjHandle, DOUBLEPOINT *PlanePoint)
{
  TASKPARMEX parm;
  int        Result = 0;

  if (GetMapView() == 0) return 0;
  if (FHSite   == 0) return 0;
  if (ObjHandle == 0) return 0;

  InitTaskParmEx(&parm);
  Result = onGetNodeByPoint(GetMapView()->GetMapHandle(),  FHSite, ObjHandle, PlanePoint);

  return Result;
}

//-------------------------------------------
// Установить имя классификатора для sit-а с сетью
//-------------------------------------------
void QDMapNet::SetRscName(char *value)
{
  if (!value || !value[0] || strcmp(value, FRscName) == 0)
    return;

  HRSC hRsc = mapOpenRsc(value);
  if (hRsc)
  {
    RSCSEMANTICEX rscsem;
    memset(&rscsem, 0, sizeof(rscsem));
    mapGetRscSemanticExByCode(hRsc, &rscsem, SEMNETNUMBER);

    int number = mapGetRscSemanticByShortName(hRsc, rscsem.ShortName);
    const char *namesem = NULL;
    namesem = mapGetRscSemanticNameByNumber(hRsc, number);

    if (namesem)
      strcpy(FRscName, value);
    else
    {
      char mess[300];
      sprintf(mess, msg_BADRSC, value);
      MessageBox(0, mess, "Ошибка!", MB_OK | MB_ICONERROR);
    }

    mapCloseRsc(hRsc);
  }
}

//-------------------------------------------
// Установить имя sit-а с сетью
//-------------------------------------------
void QDMapNet::SetNetName(char *value)
{
  char strchar1[MAX_PATH];

  if (value == 0 || strlen(value) == 0 || strcmp(value, FNetName) == 0) return;

  strcpy(FNetName, value);
  memset(strchar1, 0, MAX_PATH);

  FHSite = mapGetSiteIdentByName(GetMapView()->GetMapHandle(), FNetName);
  if (FHSite != 0)
  {
    mapGetRscFileName(mapGetRscIdent(GetMapView()->GetMapHandle(), FHSite),
                      strchar1, MAX_PATH);
    SetRscName(strchar1);
  }
}

//-------------------------------------------
// Узнать номер sit-а с сетью
//-------------------------------------------
int QDMapNet::GetNetSitNumber()
{
  int Result = 0;

  if (GetMapView()->GetMapHandle() == 0 || FHSite == 0) return Result;
  Result = mapGetSiteNumber(GetMapView()->GetMapHandle(), FHSite);

  return Result;
}

//-------------------------------------------
// Заполнить селект по результатам и получить кол-во ребер
//-------------------------------------------
int QDMapNet::GetSelectResult()
{
  int Result = 0;

  FSelect->ClearAll();
  FSelect->SetMapSites(-1, false);
  FSelect->SetMapSites(GetNetSitNumber(), true);
  FSelect->GetSeekSelect(GetNetSitNumber());
  if (FSelect->GetIsSample(GetNetSitNumber()) == true)
  {
    Result = mapTotalSeekObjectCount(GetMapView()->GetMapHandle());
    if (FColor != 0) GetMapView()->SetObjSelected(FSelect, FColor, true);
  }

  return Result;
}

//------GRAPH-------------------------------------------------------------------
//-------------------------------------------
//   Открытие или создание массивов графа
//-------------------------------------------
HGRAPH QDMapNet::OpenGraph(HMAP Map, HSITE Site)
{
  TASKPARMEX parm;

  if (Map == 0 || Site == 0) return 0;
  if (GetMapView() == 0) return 0;
  if (FGraph != 0) CloseGraph(FGraph);

  InitTaskParmEx(&parm);
  FGraph = onOpenGraphEx(Map, Site);

  return FGraph;
}

//-------------------------------------------
//   Закрыть  граф
//-------------------------------------------
int QDMapNet::CloseGraph(HGRAPH Graph)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  Result = onCloseGraph(FGraph);

  FGraph = 0;
  return Result;
}

//-------------------------------------------
// Получить идентификатор графа
//-------------------------------------------
HGRAPH QDMapNet::GetGraphHandle()
{
  return FGraph;
}

//-------------------------------------------
// ПОЛУЧИТЬ КРАТЧАЙШЕЕ РАССТОЯНИЕ МЕЖДУ УЗЛАМИ БЕЗ ДИАЛОГА ПО ВРЕМЕНИ С СОЗДАНИЕМ
// ОБЪЕКТА В КОТОРОМ БУДЕТ РАЗМЕЩЕН РЕЗУЛЬТАТ
// info - идентификатор объекта в памяти,
//        предварительно созданного функцией mapCreateObject()
//        или mapCreateSiteObject(),
//        в котором будет размещен результат - минимальный путь.
//-------------------------------------------
int QDMapNet::GetFastWayAndCreateObject(HOBJ Info, HOBJ NodeStart, HOBJ NodeFinish, double speed)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  if (onSeekPathCreateObjectGraph(GetMapView()->GetMapHandle(), FGraph, &parm, Info,
                                  NodeStart, NodeFinish, 1, speed) != 0)
    Result = mapLength(Info);

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ КРАТЧАЙШЕЕ РАССТОЯНИЕ МЕЖДУ УЗЛАМИ БЕЗ ДИАЛОГА ПО РАСCТОЯНИЮ С СОЗДАНИЕМ
// ОБЪЕКТА В КОТОРОМ БУДЕТ РАЗМЕЩЕН РЕЗУЛЬТАТ
// info - идентификатор объекта в памяти,
//        предварительно созданного функцией mapCreateObject()
//        или mapCreateSiteObject(),
//        в котором будет размещен результат - минимальный путь.
//-------------------------------------------
int QDMapNet::GetShotWayAndCreateObject(HOBJ Info, HOBJ NodeStart, HOBJ NodeFinish)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);

  if (onSeekPathCreateObjectGraph(GetMapView()->GetMapHandle(), FGraph, &parm,
                                  Info, NodeStart, NodeFinish, 0, 0)  != 0)
  {
    Result = mapLength(Info);           // 09/03/10
  }

  return Result;
}

//-------------------------------------------
// ПОЛУЧИТЬ КРАТЧАЙШЕЕ РАССТОЯНИЕ МЕЖДУ УЗЛАМИ С ДИАЛОГОМ С СОЗДАНИЕМ
// ОБЪЕКТА В КОТОРОМ БУДЕТ РАЗМЕЩЕН РЕЗУЛЬТАТ
// info - идентификатор объекта в памяти,
//        предварительно созданного функцией mapCreateObject()
//        или mapCreateSiteObject(),
//        в котором будет размещен результат - минимальный путь.
//-------------------------------------------
int QDMapNet::GetShotWayDlgAndCreateObject(HOBJ Info, DOUBLEPOINT *PointStart,DOUBLEPOINT *PointFinish)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  if (onSeekPathDlgByPointGraph(GetMapView()->GetMapHandle(), FGraph,
                                Info, &parm, PointStart, PointFinish) != 0)
    Result = mapLength(Info);           // 09/03/10

  return Result;
}

//-------------------------------------------
// Определение кратчайшего расстояния между двумя точками
// hmap - идентификатор открытой векторной карты
// hgraph - идентификатор графа сети, предварительно должен быть открыт функцией
// onOpenGraph(...)
// info - идентификатор объекта в памяти,
//        предварительно созданного функцией mapCreateObject()
//        или mapCreateSiteObject(),
//        в котором будет размещен результат - минимальный путь
// point1 - начальная точка в метрах на местности
// point2 - конечная точка в метрах на местности
// Путь определяется от точки на ближайшем ребре к point1 до точки на ближайшем
// ребре к point2
// Возвращает: -1, если путь найден
//              0, при ошибке
//-------------------------------------------
int QDMapNet::GetShotWayByPointAndCreateObject(HOBJ Info, DOUBLEPOINT *PointStart,DOUBLEPOINT *PointFinish)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  Result = onSeekPathByPointGraph(GetMapView()->GetMapHandle(), FGraph, Info, &parm, PointStart,PointFinish);
  GetSelectResult();

  return Result;
}

//-------------------------------------------
// Определение кратчайшего расстояния между двумя точками с учетом одностороннего
// движения и исключением ребер (аварийных участков)
// hmap - идентификатор открытой векторной карты
// hgraph - идентификатор графа сети, предварительно должен быть открыт функцией
// onOpenGraph(...)
// info - идентификатор объекта в памяти,
//        предварительно созданного функцией mapCreateObject()
//        или mapCreateSiteObject(),
//        в котором будет размещен результат - минимальный путь
// point1 - начальная точка в метрах на местности
// point2 - конечная точка в метрах на местности
// select - контекст условий поиска (должен содержать ребра, которые исключаются из поиска мин. пути)
// одностороннее движение учитывается всегда
// Путь определяется от точки на ближайшем ребре к point1 до точки на ближайшем
// ребре к point2
// Возвращает: -1, если путь найден
//              0, при ошибке
//-------------------------------------------
int QDMapNet::GetShotWayByPointExAndCreateObject(HOBJ Info, DOUBLEPOINT *PointStart,DOUBLEPOINT *PointFinish, HSELECT select)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  if (onSeekPathByPointExGraph(GetMapView()->GetMapHandle(), FGraph, Info,
                               &parm, PointStart, PointFinish, select) != 0)
    Result = mapLength(Info);           // 09/03/10

  return Result;
}

//-------------------------------------------
// Вычисление минимального пути между точками (узлами сети)
// hmap - идентификатор открытой векторной карты
// hgraph - идентификатор графа сети, предварительно должен быть открыт функцией
// onOpenGraph(...)
// parm - описание параметров задачи (см.maptype.h)
// array - адрес массива координат опорных точек,
//         размер массива равен count
// count - количество точек для построения минимального пути
// Расчеты производятся по ближайшим к точкам узлам сети
// В функции вызывается диалог "Параметры построения минимального пути", в котором
// устанавливаются метод построения (длина или время,в последнем случае ребра
// должны иметь семантику "Скорость") и фильтр по семантикам ребер
// По окончании работы функции выдается результирующий диалог "Порядок обхода точек",
// в который записываются номера точек в том порядке, в котором путь прохождения по ним минимален,
// протяженность маршрута между соседней парой точек, общая длина пути.
// При ошибке возвращает 0
//-------------------------------------------
int QDMapNet::GetMinimalPathSite(HSITE tempsite, DOUBLEPOINT *arraypoint, int count)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  Result = onMinimalPathSiteGraph(GetMapView()->GetMapHandle(), FGraph, &parm, tempsite, arraypoint, count);
  GetSelectResult();

  return Result;
}

//-------------------------------------------
// Вычисление минимального пути между точками (узлами сети)
// hmap - идентификатор открытой векторной карты
// hgraph - идентификатор графа сети, предварительно должен быть открыт функцией
// onOpenGraph(...)
// parm - описание параметров задачи (см.maptype.h)
// arraypoint - адрес массива координат (в метрах)  опорных точек,
//              размер массива равен count
// arraynumber - адрес массива с номерами точек (для результатов),
//               размер массива равен count
// count - количество точек для построения минимального пути
// одностороннее движение учитывается всегда
// flagreturn - флаг возврата в первую точку (0 - не возвращаться, 1 - возвращаться)
// flagcalc - метод расчета (0 - по длине, 1- по времени)
// Расчеты производятся по ближайшим к точкам узлам сети
// В массив arraynumber будут записаны номера точек, которые определяют порядок обхода
// Возвращает общую длину пути
// При ошибке возвращает 0
//-------------------------------------------
double QDMapNet::GetMinimalPathEx(DOUBLEPOINT *arraypoint, long int *arraynumber, int count,   // 21/09/15
                                  int flagreturn, int flagcalc)
{
  double     Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  Result = onMinimalPathExGraph(GetMapView()->GetMapHandle(), FGraph, &parm,
                                arraypoint,arraynumber, count, flagreturn, flagcalc);

  return Result;
}

//-------------------------------------------
// Вычисление минимального пути между точками (узлами сети)
// hmap - идентификатор открытой векторной карты
// site - пользовательская карта с построенной сетью
// parm - описание параметров задачи (см.maptype.h)
// array - адрес массива координат опорных точек,
//         размер массива равен count
// count - количество точек для построения минимального пути
// Расчеты производятся по ближайшим к точкам узлам сети
// В функции вызывается диалог "Параметры построения минимального пути", в котором
// устанавливаются метод построения (длина или время,в последнем случае ребра
// должны иметь семантику "Скорость") и фильтр по семантикам ребер
// По окончании работы функции выдается результирующий диалог "Порядок обхода точек",
// в который записываются номера точек в том порядке, в котором путь прохождения по ним минимален,
// протяженность маршрута между соседней парой точек, общая длина пути.
// При ошибке возвращает 0
//-------------------------------------------
int QDMapNet::GetMinimalPath(DOUBLEPOINT *arraypoint, int count)
{
  int        Result = 0;
  TASKPARMEX parm;

  if (GetMapView() == 0) return 0;
  if (FHSite != 0 && FGraph == 0) OpenGraph(GetMapView()->GetMapHandle(), FHSite);
  if (FGraph == 0) return 0;

  InitTaskParmEx(&parm);
  Result = onMinimalPathGraph(GetMapView()->GetMapHandle(), FGraph, &parm, arraypoint, count);

  return Result;
}

// Главный обработчик сообщений
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void QDMapNet::InitTaskParmEx(TASKPARMEX *taskparm)
{
  if (taskparm == 0) return;

  // Заполнение параметров прикладной задачи
  memset((char *)taskparm, 0, sizeof(TASKPARMEX));
  taskparm->Language  = mapGetMapAccessLanguage();
  taskparm->Handle    = MessageHandler;
  taskparm->DocHandle = (HWND)GetMapView();
}

//------------------------------------------------------------------
// Обработка событий карты    // 18/03/10
//------------------------------------------------------------------
void QDMapNet::MapAction(int action)
{
  QDMapView *FMV;
  int       ii;

  FMV = (QDMapView *)VarMapView;

  //-------------------------
  if ((action == ACTBEFOREDELETESITE) && (FMV->VarMapSites->VarMapSite->GetNumber() > 0))
    // если удалили VarSite из карты
  {
    if (FHSite != 0 &&
        FMV->VarMapSites->VarMapSite->GetNumber() == mapGetSiteNumber(GetMapView()->GetMapHandle(), FHSite))
    {
      CloseGraph(FGraph);
      FHSite = 0;
      FNetName[0] = 0;
      FColor   = 0;
    }

    return;
  } // если удалили VarSite из карты

  // если закрыли карту
  if ((action == ACTBEFORECLOSE) && (FMV->GetMapHandle() != 0))
  {
    FSelect->MapAction(ACTBEFORECLOSE);
    CloseGraph(FGraph);
    FHSite = 0;
    FNetName[0] = 0;
    FColor   = 0;

    return;
  } // если закрыли карту
}

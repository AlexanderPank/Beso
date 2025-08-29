/******* QDMSELOB.CPP **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                        FOR QT-DESIGNER                           *
*                                                                  *
********************************************************************
*                                                                  *
*    ОПИСАНИЕ КЛАССА "КОМПОНЕНТ - ДИАЛОГ ВЫБОРА ОБЪЕКТА КАРТЫ"     *
*                                                                  *
*                                                                  *
*******************************************************************/

#include "qdmselob.h"
#include "qdmcmp.h"
#include "qdmobj.h"
#include "qdmselt.h"

#include <qcursor.h>
#include "maplib.h"

// Главный обработчик сообщений
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

typedef struct OBJECTFORM      // ПАРАМЕТРЫ ОТОБРАЖЕНИЯ ИНФОРМАЦИИ ОБ ОБ'ЕКТЕ
{
  HWND        Window;          // Идентификатор окна документа
  POINT       Position;        // Положение курсора в точке выбора в координатах экрана
  POINT       LeftTopPosition; // Координаты левого верхнего угла карты в пикселах в районе
  long int    PlaceSystem;     // Текущая система отображения координат метрики объекта:
  // 1 - в метрах на местности
  // 2 - в условных единицах карты (дискретах)
  // 3 - в пикселах текущего полного изображения
  //     эллипсоид Красовского 1942г.
  // 4 - в радианах в соответствии с проекцией
  // 5 - в градусах ...
  // 6 - в градусах, минутах, секундах ...
  short int   FormMode;        // Вид окна отображения объекта
  // 0 - только общие сведения об объекта
  // 1 - общие сведения и семантика объекта
  // 2 - общие сведения и метрика объекта
  // 3 - общие сведения и масштаб отображения объекта
  // 4 - общие сведения и внешний вид объекта
  // 5 - общие сведения и принтерный вид объекта
  // 6 - общие сведения и графический вид объекта
  // -1 - -6 соответствующая информация в полноэкранном варианте
  short int   MarkPointMode;   // Вид окна отображения карты
  // Информация устанавливается побитно
  // Перечисление выполнено начиная с младших разрядов
  // Состояние мониторинга положения текущей точки метрики
  // 0 - не отслеживать положение текущей точки метрики
  // 1 - отслеживать положение текущей точки метрики
  // Состав отображаемых карт
  // 0 - отображается только карта, содержащая объект
  // 1 - отображаются все карты
  HMAP        Map;             // Идентификатор открытой векторной карты
  // (УКАЗАТЕЛЬ НА TMapAccess)
}
OBJECTFORM;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ ДИАЛОГ ВЫБОРА ОБ'ЕКТА ЭЛЕКТРОННОЙ КАРТА"   ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapDlgObj::QDMapDlgObj(QWidget *parent, const char *name)
  : QDMapComponent(parent, name)
{
  hide();
}

//-------------------------------------------
//      Деструктор
//-------------------------------------------
QDMapDlgObj::~QDMapDlgObj()
{

}

QDMapView *QDMapDlgObj::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapDlgObj::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void InitTaskParm (TASKPARM *taskparm)
{
  // Заполнение параметров прикладной задачи
  memset((char *)taskparm, 0, sizeof(TASKPARM));
  taskparm->Language  = ML_RUSSIAN;
}

//----------------------------------------------------
// Выбрать объект карты в точке, используя диалог
// x,y    - координаты точки в пикселах в клиентской области
//                 элемнта управления TMapView, на который настроен TMapObj
// radius - радиус поиска в пикселах (не более 10)
// Выход = true  - выбрали и заполнили mapobj
//         false - отказались
//----------------------------------------------------
bool QDMapDlgObj::SelectObjectInPoint(QDMapObj *mapobj, int x, int y, int radius)
{
  if (!mapobj)
    return 0;

  QDMapView *mapView = (QDMapView *)mapobj->GetMapView();
  if (mapView == NULL)
    return 0;

  QDMapView *mapViewOld = (QDMapView *)GetMapView();
  SetMapView(mapView);

  HMAP mapHandle = mapView->GetMapHandle();
  if (mapHandle == 0)
  {
    SetMapView(mapViewOld);
    return 0;
  }

  HSITE siteHandle = mapGetSiteIdent(mapHandle, mapobj->GetSiteNumber());
  if (siteHandle == 0)
  {
    SetMapView(mapViewOld);
    return 0;
  }

  // Прямоугольник поиска в пикселах изображения
  if (mapView->IsViewFrameActive())
  {
    int width, height;
    mapView->GetPictureSize(&width, &height);
    if (width < x || height < y)
    {
      SetMapView(mapViewOld);
      return 0;
    }
  }

  LONGPOINT leftTopPoint;
  mapView->GetMapLeftTop(&leftTopPoint.X, &leftTopPoint.Y);

  MAPDFRAME  dframe;
  dframe.X1 = leftTopPoint.X + x - radius;
  dframe.X2 = leftTopPoint.X + x + radius;
  dframe.Y1 = leftTopPoint.Y + y - radius;
  dframe.Y2 = leftTopPoint.Y + y + radius;

  RECT rect;
  rect.left = dframe.X1;
  rect.top = dframe.Y1;
  rect.right = dframe.X2;
  rect.bottom = dframe.Y2;
  mapView->TranslatePictureRectFromViewFrame(&rect);

  dframe.X1 = rect.left;
  dframe.Y1 = rect.top;
  dframe.X2 = rect.right;
  dframe.Y2 = rect.bottom;

  mapPictureToMap(mapHandle, &dframe.X1, &dframe.Y1);
  mapPictureToMap(mapHandle, &dframe.X2, &dframe.Y2);

  // Поиск первого объекта
  HOBJ objectHandle = mapCreateSiteObject(mapHandle, siteHandle, IDDOUBLE2, 0);
  if (objectHandle == 0)
  {
    SetMapView(mapViewOld);
    return 0;
  }

  if (mapWhatObject(mapHandle, objectHandle, &dframe, WO_LAST, PP_MAP) == 0)
  {
    if (objectHandle != 0) mapFreeObject(objectHandle);
    SetMapView(mapViewOld);
    return 0;
  }

  // Заполнение параметров прикладной задачи
  TASKPARM   taskparm;
  InitTaskParm(&taskparm);
  taskparm.Handle = &MessageHandler;

  OBJECTFORM objform;
  memset((char *)&objform, 0, sizeof(OBJECTFORM));
  objform.Map    = mapHandle;
  objform.Window = (HWND)mapView;

  QPoint qpoint(x, y);
  mapView->mapToGlobal(qpoint);
  objform.Position.x = qpoint.x();
  objform.Position.y = qpoint.y();

  HINSTANCE  libinst;
  long int(WINAPI * formSelectObject)(HOBJ, MAPDFRAME *, long int, OBJECTFORM *, TASKPARM *);

  formSelectObject = (long int (WINAPI *)(HOBJ , MAPDFRAME* , long int , OBJECTFORM* , TASKPARM* ))
      mapLoadLibrary(MAPOBJLIB , &libinst, "formSelectObject");
  if (formSelectObject == NULL)
  {
    if (objectHandle != 0) mapFreeObject(objectHandle);
    SetMapView(mapViewOld);
    return 0;
  }

  bool isObjectSelected = 0;
  // Вызовем диалог
  if (formSelectObject(objectHandle, &dframe, PP_PICTURE, &objform, &taskparm))
  {
    mapobj->SetObjHandle(objectHandle);
    isObjectSelected = 1;
  }

  if (objectHandle != 0)
    mapFreeObject(objectHandle);

  SetMapView(mapViewOld);

  // Выгрузка библиотеки
  if (libinst)
    mapFreeLibrary(libinst);

  return isObjectSelected;
}

//----------------------------------------------------
// Поиск объекта перебором в диалоге
// mapselect - критерий поиска :
//       если AllMaps не установлен, то поиск будет выполнен
//       для первой доступной для поиска в списке;
// next      - надо ли выполнить продолжение поиска с текущего;
// Выход = true  - выбрали и заполнили mapobj
//         false - отказались
//----------------------------------------------------
bool QDMapDlgObj::ExecuteDlgFind(QDMapObj *mapobj, QDMapSelect *mapselect, bool next)
{
  bool       ret = FALSE;
  QDMapView  *MapView, *MapViewOld;
  TASKPARM   taskparm;
  HOBJ       ObjH;
  HMAP       MapH;
  // Прямоугольник поиска в пикселах изображения
  int   valx, valy;
  OBJECTFORM objform;
  int   flag;
  HINSTANCE  libinst;
  long int (WINAPI * formSeekObject)(HOBJ , long int , HSELECT ,
                                     OBJECTFORM* , TASKPARM *);

  if (!mapobj) return ret;

  MapView = (QDMapView *)mapobj->GetMapView();
  if (MapView == NULL) return ret;
  MapViewOld = (QDMapView *)GetMapView();
  SetMapView(MapView);
  MapH = MapView->GetMapHandle();
  if (MapH == 0)
  {
    SetMapView(MapViewOld);
    return ret;
  }

  flag = WO_FIRST;
  if (next) if (mapobj->GetKey() != 0)// текущий объект есть
    flag = WO_NEXT;

  ObjH = mapCreateCopyObject(MapH, mapobj->GetObjHandle());

  if (ObjH == 0)
  {
    SetMapView(MapViewOld);
    return ret;
  }

  // Заполнение параметров прикладной задачи
  InitTaskParm(&taskparm);
  taskparm.Handle = &MessageHandler;
  memset((char *)&objform, 0, sizeof(OBJECTFORM));
  objform.Map = MapH;
  objform.Window = (HWND)MapView;
  MapView->GetMapLeftTop(&valx, &valy);
  objform.LeftTopPosition.x = valx;
  objform.LeftTopPosition.y = valy;
  objform.PlaceSystem = mapobj->GetPlaceOut();

  if (mapselect)
    for (int ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
    {
      mapSetSiteSeekSelect(MapH, MapView->VarMapSites->GetMapSites(ii)->GetSiteHandle(),
                           mapselect->GetHSelect(ii));
    }

  int jj = -1;
  if ((mapselect) && (mapselect->GetAllFindSites() == FALSE))
  {
    for (int ii = 0; ii <= MapView->VarMapSites->GetSiteCount(); ii++)
      if (mapselect->GetMapSites(ii) == TRUE)
      {
        jj = ii;
        break;
      }
  }

  mapSetTotalSeekMapRule(MapH, jj);

  if (!mapTotalSeekObject(MapH, ObjH, flag))
  {
    if (ObjH != 0) mapFreeObject(ObjH);
    SetMapView(MapViewOld);
    return ret;
  }

  formSeekObject = (long int (WINAPI *)(HOBJ , long int , HSELECT ,
                                        OBJECTFORM* , TASKPARM *))
      mapLoadLibrary(MAPOBJLIB , &libinst, "formSeekObject");
  if (formSeekObject == NULL)
  {
    if (ObjH != 0) mapFreeObject(ObjH);
    SetMapView(MapViewOld);
    return ret;
  }

  // Вызовем диалог
  if ( formSeekObject(ObjH, flag, 0, &objform, &taskparm))
  {
    mapobj->SetObjHandle(ObjH);
    ret = TRUE;
  }

  if (ObjH != 0) mapFreeObject(ObjH);

  SetMapView(MapViewOld);
  // Выгрузка библиотеки
  if (libinst)    mapFreeLibrary(libinst);

  return ret;
}

//----------------------------------------------------
// Показать информационный диалог об объекте
// Выход = false - отказ
//----------------------------------------------------
bool QDMapDlgObj::ShowDialogInfo(QDMapObj *mapobj)
{
  bool       ret = FALSE;
  QDMapView  *MapView, *MapViewOld;
  TASKPARM   taskparm;
  OBJECTFORM objform;
  HMAP       MapH;
  HINSTANCE  libinst;
  long int (WINAPI * formSelectAssignObject)(HOBJ , OBJECTFORM* , TASKPARM* );

  if (!mapobj) return ret;

  MapView = (QDMapView *)mapobj->GetMapView();
  if (MapView == NULL) return ret;
  MapViewOld = (QDMapView *)GetMapView();
  SetMapView(MapView);
  MapH = MapView->GetMapHandle();
  if (MapH == 0)
  {
    SetMapView(MapViewOld);
    return ret;
  }

  // Заполнение параметров прикладной задачи
  InitTaskParm(&taskparm);
  taskparm.Handle = &MessageHandler;
  memset((char *)&objform, 0, sizeof(OBJECTFORM));

  objform.Map    = MapH;
  objform.Window = (HWND)MapView;

  objform.Position.x = MapView->cursor().pos().x();
  objform.Position.y = MapView->cursor().pos().y();

  formSelectAssignObject = (long int (WINAPI *)(HOBJ , OBJECTFORM* , TASKPARM* ))
      mapLoadLibrary(MAPOBJLIB , &libinst, "formSelectAssignObject");
  if (formSelectAssignObject == NULL)
  {
    SetMapView(MapViewOld);
    return ret;
  }

  // Вызовем диалог
  if (formSelectAssignObject(mapobj->GetObjHandle(), &objform, &taskparm))
  {
    ret = TRUE;
  }

  SetMapView(MapViewOld);
  // Выгрузка библиотеки
  if (libinst)    mapFreeLibrary(libinst);

  return ret;
}

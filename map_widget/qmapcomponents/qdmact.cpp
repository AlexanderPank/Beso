/******** QDMACT.CPP ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR QT-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*    QDGtkAction -  базовый класс обработки событий от компонента  *
*                         отображения карты                        *
*                                                                  *
*******************************************************************/

#include "qdmact.h"
#include "qdmcmp.h"
#include "qdmwinw.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
// КЛАСС ОБРАБОТКИ СОБЫТИЙ ОТ КОМПОНЕНТА ОТОБРАЖЕНИЯ КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

QDGtkAction::QDGtkAction(QWidget *parent, const char *name) : QDMapComponent(parent, name)
{
  VarActive = false;
}

QDGtkAction::~QDGtkAction()
{
  StopAction();
}

QDMapView *QDGtkAction::GetMapView() const
{
  return (QDMapView *) VarMapView;
}

void QDGtkAction::SetMapView(QDMapView *value)
{
  StopAction();
  SetMapViewAcces(value);
}

QDMapWindow *QDGtkAction::GetMapWindow() const
{
  return (QDMapWindow *) VarMapWindow;
}

void QDGtkAction::SetMapWindow(QDMapWindow *value)
{
  StopAction();
  SetMapWindowAcces(value);
}

void QDGtkAction::StartAction()
{
  QDMapViewWindow *viewwindow;

  viewwindow = (QDMapViewWindow *) GetMapViewWindow();
  if (!viewwindow)
    return;

  VarActive = viewwindow->SetCurrentAction(this);
}

void QDGtkAction::StopAction()
{
  QDMapViewWindow *viewwindow;

  viewwindow = (QDMapViewWindow *) GetMapViewWindow();
  if (!viewwindow)
    return;

  viewwindow->DeleteAction(this, true);
}

bool QDGtkAction::DoStopActionQuery()
{
  bool result = true;
  return result;
}

void QDGtkAction::DoStopAction()
{
  VarActive = false;
}

void QDGtkAction::MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect)
{

}

void QDGtkAction::MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect)
{

}

void QDGtkAction::DblClick()
{
  Commit();
}

void QDGtkAction::KeyUp(int key, int count, int state)
{

}

void QDGtkAction::KeyDown(int key, int count, int state)
{

}

void QDGtkAction::MouseDown(int x, int y, int modkeys)
{

}

void QDGtkAction::MouseMove(int x, int y, int modkeys)
{

}

void QDGtkAction::MouseUp(int x, int y, int modkeys)
{

}

// --------------------------------------------------------
// Сообщение о завершении работы
// При ошибке возвращает ноль
// --------------------------------------------------------
int QDGtkAction::Commit()
{
  return 0;
}

// --------------------------------------------------------
// Активен ли в данный момент обработчик
// --------------------------------------------------------
bool QDGtkAction::GetActive()
{
  return VarActive;
}

//--------------------------------------------------------
// Запросить координаты текущей точки
//--------------------------------------------------------
DOUBLEPOINT QDGtkAction::GetMapPoint(int clientx, int clienty)
{
  DOUBLEPOINT mapPoint;
  memset(&mapPoint, 0, sizeof(mapPoint));

  QDMapViewWindow *mapWindow = (QDMapViewWindow *) GetMapViewWindow();
  if (!mapWindow)
    return mapPoint;

  int left, top;
  mapWindow->GetMapLeftTop(&left, &top);

  mapPoint.X = left + clientx;
  mapPoint.Y = top + clienty;
  mapWindow->ConvertMetric(&mapPoint.X, &mapPoint.Y, PP_PICTURE, PP_MAP);

  return mapPoint;
}

DOUBLEPOINT QDGtkAction::GetMapPoint()
{
  DOUBLEPOINT mapPoint;
  memset(&mapPoint, 0, sizeof(mapPoint));

  QDMapView *mapView = GetMapView();
  if (mapView)
    return mapView->GetMapPoint();

  return mapPoint;
}

POINT QDGtkAction::GetPicturePoint()
{
  POINT mapPoint;
  memset(&mapPoint, 0, sizeof(mapPoint));

  QDMapView *mapView = GetMapView();
  if (mapView)
    return mapView->GetPicturePoint();

  return mapPoint;
}

DOUBLEPOINT QDGtkAction::GetPlanePoint()
{
  DOUBLEPOINT mapPoint;
  memset(&mapPoint, 0, sizeof(mapPoint));

  QDMapView *mapView = GetMapView();
  if (mapView)
    return mapView->GetPlanePoint();

  return mapPoint;
}

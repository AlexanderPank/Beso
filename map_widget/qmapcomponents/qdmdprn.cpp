/******** QDMDPRN.CPP  *********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR QT DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*                      КОМПОНЕНТ ПЕЧАТИ                            *
*                                                                  *
*******************************************************************/

#include <qcursor.h>
#include <QPrinter>

#include "mapapi.h"
#include "maplib.h"
#include "prnapi.h"
#include "struct.h"

#include "qdmactprint.h"
#include "qdmdprn.h"

// Главный обработчик сообщений
long int MessageHandler(long int context, long int code,
                        long int wp, long int lp, long int typemsg = 0);


//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КОМПОНЕНТ ПЕЧАТИ
//++++++++++++++++++++++++++++++++++++++++++++++++++

QDMapPrint::QDMapPrint(QWidget *parent, const char *name) :
    QDMapComponent(parent, name),
    AllRectActive(true)
{
   hide();

   MapPrintAction = new QDMapPrintAction(parent, name);

   memset(&MapRect, 0, sizeof(MapRect));
}

QDMapView *QDMapPrint::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapPrint::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);

  // Проинициализировать обработчик
  if (MapPrintAction)
    MapPrintAction->SetMapView(value);
}

void QDMapPrint::Execute()
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return;

  // Проинициализировать начальный прямоугольник печати
  DFRAME initialFrame;
  memset(&initialFrame, 0, sizeof(initialFrame));

  if (!AllRectActive && IsMapRectValid())
  {
    // Перевести единицы измерения (пикселы -> метры)
    double x = MapRect.LT;
    double y = MapRect.UP;
    mapPictureToPlane(mapHandle, &x, &y);
    initialFrame.X2 = x;
    initialFrame.Y1 = y;

    x = MapRect.RT;
    y = MapRect.DN;
    mapPictureToPlane(mapHandle, &x, &y);
    initialFrame.X1 = x;
    initialFrame.Y2 = y;
  }
  else
  {
    QDMapView *MapView = GetMapView();
    if (MapView->IsViewFrameActive())
      MapView->GetViewFrame(&initialFrame);
    else
      mapGetTotalBorder(mapHandle, &initialFrame, PP_PLANE);
  }

  MapPrintAction->PlaneFrame = initialFrame;

  if(AllRectActive)
    MapPrintAction->SetRegime(MPR_PRINTALL);
  else
    MapPrintAction->SetRegime(MPR_SELECTFRAME);

  // Запустить обработчик печати
  MapPrintAction->StartAction();
}

void QDMapPrint::SetMapRect(int left, int top, int right, int bottom)
{
  MapRect.UP = top;
  MapRect.LT = left;
  MapRect.DN = bottom;
  MapRect.RT = right;

  GetMapView()->TranslatePictureRectFromViewFrame(&MapRect);
}

int QDMapPrint::IsMapRectValid() const
{
  if ((MapRect.DN - MapRect.UP) > 0 &&
      (MapRect.RT - MapRect.LT) > 0)
    return 1;

  return 0;
}

void QDMapPrint::SetAllRectActive(bool value)
{
  AllRectActive = value;
}

bool QDMapPrint::GetAllRectActive() const
{
  return AllRectActive;
}

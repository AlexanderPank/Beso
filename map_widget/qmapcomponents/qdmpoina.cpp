/******* QDMPOINA.CPP  *********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*     ОПИСАНИЕ БАЗОВОГО КЛАССА ДОСТУПА К КООРДИНАТАМ ТОЧКИ         *
*                     ЭЛЕКТРОННОЙ КАРТЫ                            *
*                                                                  *
*******************************************************************/

#include "qdmpoina.h"
#include "qdmcmp.h"
#include "mapapi.h"

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapPointAcces::QDMapPointAcces(QDMapViewAcces* mapview, QWidget *parent, const char *name)
                : QWidget(parent)
{
  VarX = 0;
  VarY = 0;
  VarMapView  = mapview;
  VarPlaceIn = PP_PICTURE;
  VarPlaceOut = PP_PLANE;
  hide();
}

int QDMapPointAcces::GetPlaceIn() const
{
  return VarPlaceIn;
}

int QDMapPointAcces::GetPlaceOut() const
{
  return VarPlaceOut;
}

double QDMapPointAcces::GetX()
{
  double x;
  double y;

  if (VarMapView == NULL) return 0;
  if  (VarMapView->GetMapHandle() == 0) return 0;

  x = VarX;
  y = VarY;
  switch (VarPlaceOut)
  {
    case PP_PICTURE : TranslatePlaneToPicture(&x, &y); break;
    case PP_GEO     : mapPlaneToGeo(VarMapView->GetMapHandle(), &x, &y); break;
    case PP_GEOWGS84: mapPlaneToGeoWGS84(VarMapView->GetMapHandle(), &x, &y); break;
    case PP_PLANE   : break;
    default         : x = 0; break;
  }

  return x;
}

double QDMapPointAcces::GetY()
{
  double x;
  double y;

  if (VarMapView == NULL) return 0;
  if  (VarMapView->GetMapHandle() == 0) return 0;

  x = VarX;
  y = VarY;
  switch (VarPlaceOut)
  {
    case PP_PICTURE : TranslatePlaneToPicture(&x, &y); break;
    case PP_GEO     : mapPlaneToGeo(VarMapView->GetMapHandle(), &x, &y); break;
    case PP_GEOWGS84: mapPlaneToGeoWGS84(VarMapView->GetMapHandle(), &x, &y); break;
    case PP_PLANE   : break;
    default         : x = 0; y =0;break;
  }

  return y;
}

QDMapViewAcces *QDMapPointAcces::GetMapView() const
{
  return VarMapView;
}

void QDMapPointAcces::SetPlaceIn(int value)
{
  if (VarMapView == NULL)
    return;
  if (VarMapView->GetMapHandle() == 0)
    return;
  if (((value == PP_GEO) || (value == PP_GEOWGS84)) &&
      (mapIsGeoSupported(VarMapView->GetMapHandle()) == 0))
    return;

  VarPlaceIn = value;
}

void QDMapPointAcces::SetPlaceOut(int value)
{
  if (VarMapView == NULL)
    return;
  if (VarMapView->GetMapHandle() == 0)
    return;
  if (((value == PP_GEO) || (value == PP_GEOWGS84)) &&
      (mapIsGeoSupported(VarMapView->GetMapHandle()) == 0))
    return;

  VarPlaceOut = value;
}

void QDMapPointAcces::SetX(double x)
{
  if (VarMapView == NULL) return;
  if (VarMapView->GetMapHandle() == 0) return;

  switch (VarPlaceIn)
  {
    case PP_PICTURE : TranslatePlaneToPicture(&VarX, &VarY); break;
    case PP_PLANE   : break;
    case PP_GEO     : mapPlaneToGeo(VarMapView->GetMapHandle(), &VarX, &VarY); break;
    case PP_GEOWGS84: mapPlaneToGeoWGS84(VarMapView->GetMapHandle(), &VarX, &VarY); break;
    default         : return;
  }
  VarX = x;

  switch (VarPlaceIn)
  {
    case PP_PICTURE : TranslatePictureToPlane(&VarX, &VarY); break;
    case PP_PLANE   : break;
    case PP_GEO     : mapGeoToPlane(VarMapView->GetMapHandle(), &VarX, &VarY); break;
    case PP_GEOWGS84: mapGeoWGS84ToPlane3D(VarMapView->GetMapHandle(), &VarX, &VarY, 0); break;
  }
}

void QDMapPointAcces::SetY(double y)
{
  if (VarMapView == NULL) return;
  if (VarMapView->GetMapHandle() == 0) return;

  switch (VarPlaceIn)
  {
    case PP_PICTURE : TranslatePlaneToPicture(&VarX, &VarY); break;
    case PP_PLANE   : break;
    case PP_GEO     : mapPlaneToGeo(VarMapView->GetMapHandle(), &VarX, &VarY); break;
    case PP_GEOWGS84: mapPlaneToGeoWGS84(VarMapView->GetMapHandle(), &VarX, &VarY); break;
    default         : return;
  }
  VarY = y;

  switch (VarPlaceIn)
  {
    case PP_PICTURE : TranslatePictureToPlane(&VarX, &VarY); break;
    case PP_PLANE   : break;
    case PP_GEO     : mapGeoToPlane(VarMapView->GetMapHandle(), &VarX, &VarY); break;
    case PP_GEOWGS84: mapGeoWGS84ToPlane3D(VarMapView->GetMapHandle(), &VarX, &VarY, 0); break;
  }
}

void QDMapPointAcces::SetMapView(QDMapViewAcces *value)
{
  int placein;
  if (VarMapView == value) return;

  VarX = 0;
  VarY = 0;
  VarMapView = value;
  placein    = VarPlaceIn;
  VarPlaceIn = PP_PICTURE;
  SetX(0);
  SetY(0);
  VarPlaceIn = placein;
}

//-------------------------------------------------------------------
// Записать координату в системе PlaceIn
// Выход = 1 - успешно
//-------------------------------------------------------------------
bool QDMapPointAcces::SetPoint(double x, double y)
{
  if ((VarMapView == NULL) || (VarMapView->GetMapHandle() == 0)) return false;

  VarX = x;
  VarY = y;

  switch (VarPlaceIn)
  {
    case PP_PICTURE : TranslatePictureToPlane(&VarX, &VarY);break;
    case PP_PLANE   : break;
    case PP_GEO     : mapGeoToPlane(VarMapView->GetMapHandle(), &VarX, &VarY);break;
    case PP_GEOWGS84: mapGeoWGS84ToPlane3D(VarMapView->GetMapHandle(), &VarX, &VarY, 0); break;
    default         : return false;
  }

  return true;
}

//-------------------------------------------------------------------
// Считать  координату в системе PlaceOut
// Выход = 1 - успешно
//-------------------------------------------------------------------
bool QDMapPointAcces::GetPoint(double *x, double *y)
{
 if ((VarMapView == NULL) || (VarMapView->GetMapHandle() == 0)) return false;

 *x = VarX;
 *y = VarY;

 switch (VarPlaceOut)
 {
    case PP_PICTURE : TranslatePlaneToPicture(x, y);
                      break;
    case PP_PLANE   : break;
    case PP_GEO     : mapPlaneToGeo(VarMapView->GetMapHandle(), x, y);break;
    case PP_GEOWGS84: mapPlaneToGeoWGS84(VarMapView->GetMapHandle(), x, y); break;
    default         : return false;
 }
 return true;
}


//-------------------------------------------------------------------
// Конвертировать координаты из dframein система координат placein
// в систему координат placeout
//-------------------------------------------------------------------
MAPDFRAME QDMapPointAcces::ConvertDFrame(MAPDFRAME dframein, int placein, int placeout)   // 25/11/03
{
  if (VarMapView == NULL) return dframein;
  if (VarMapView->GetMapHandle() == 0) return dframein;

  ConvertMetric(&dframein.X1, &dframein.Y1,  placein, placeout);
  ConvertMetric(&dframein.X2, &dframein.Y2,  placein, placeout);

  return dframein;
}

//-------------------------------------------------------------------
// Преобразовать точку метрики с координатами x, y
// из системы placein в систему placeout
//-------------------------------------------------------------------
bool QDMapPointAcces::ConvertMetric(double *x, double *y, int placein, int placeout)
{
  double OldPlaceIn, OldPlaceOut;
  double VarXold, VarYold;

  if (VarMapView->GetMapHandle() == 0)
    return false;
  VarXold      = VarX;
  VarYold      = VarY;
  OldPlaceIn   = VarPlaceIn;
  OldPlaceOut  = VarPlaceOut;
  SetPlaceIn(placein);
  SetPlaceOut(placeout);

  double X = *x;
  double Y = *y;

  SetPoint(X, Y);
  GetPoint(x, y);

  VarX      = VarXold;
  VarY      = VarYold;
  VarPlaceIn  = OldPlaceIn;
  VarPlaceOut = OldPlaceOut;
  return true;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void QDMapPointAcces::Assign(QDMapPointAcces *mappoint)
{
  VarX        = mappoint->VarX;
  VarY        = mappoint->VarY;
  VarPlaceIn  = mappoint->VarPlaceIn;
  VarPlaceOut = mappoint->VarPlaceOut;
  VarMapView  = mappoint->VarMapView;
}

void QDMapPointAcces::TranslatePlaneToPicture(double *x, double *y)
{
  if (!x || !y)
    return;

  QDMapView *mapView = (QDMapView *)GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = VarMapView->GetMapHandle();

  if (mapView->IsViewFrameActive())
  {
    DFRAME viewFrame;
    mapView->GetViewFrame(&viewFrame);
    mapPlaneToPicture(mapHandle, &viewFrame.X1, &viewFrame.Y1);

    double * xValue = x;
    double * yValue = y;
    mapPlaneToPicture(mapHandle, xValue, yValue);

    *x = *xValue - viewFrame.X1;
    *y = *yValue - viewFrame.Y1;
  }
  else
    mapPlaneToPicture(mapHandle, x, y);
}

void QDMapPointAcces::TranslatePictureToPlane(double *x, double *y)
{
  if (!x || !y)
    return;

  QDMapView *mapView = (QDMapView *)GetMapView();
  if (!mapView)
    return;

  if (mapView->IsViewFrameActive())
  {
    POINT point;
    point.x = *x;
    point.y = *y;
    mapView->TranslatePicturePointFromViewFrame(&point);

    *x = point.x;
    *y = point.y;
  }

  mapPictureToPlane(VarMapView->GetMapHandle(), x, y);
}

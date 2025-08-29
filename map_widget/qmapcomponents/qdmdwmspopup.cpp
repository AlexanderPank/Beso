/******** QDMDWMSPOPUP.CPP *****************************************
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
*             ��������� - ������ ���������� ��������               *
*                       WMS-��������                               *
*                                                                  *
*******************************************************************/

#include "qdmdwmspopup.h"
#include "mapapi.h"
#include "wmsapi.h"
#include "maplib.h"


static HINSTANCE PortalMenuInst = 0;
// ������� ���������� ���������
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ ��������� - ������ ���������� ��������       +
//+++               WMS-��������                   +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   �����������
//------------------------------------------------------------------
QDMapWMSPopupDialog::QDMapWMSPopupDialog(QWidget *parent, const char *name)
  :QDMapComponent(parent, name)
{
  hide();
  FMapCenter.X = 0;
  FMapCenter.Y = 0;
  IsGeo = 0;
}

//-------------------------------------------
//      ����������
//-------------------------------------------
QDMapWMSPopupDialog::~QDMapWMSPopupDialog()
{
  if (PortalMenuInst)
    mapFreeLibrary(PortalMenuInst);
  PortalMenuInst = 0;
}

//------------------------------------------------------------------
// ��������� �� �����
//------------------------------------------------------------------
QDMapView *QDMapWMSPopupDialog::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapWMSPopupDialog::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//------------------------------------------------------------------
// ��������� ������
//------------------------------------------------------------------
void QDMapWMSPopupDialog::Execute()
{
  QDMapView   *mapView;
  TASKPARMEX  TaskParm;

  mapView = GetMapView();
  if (mapView == NULL)
    return;
  if (mapView->GetMapHandle() == 0) return;

  // ���������� ���������� ���������� ������
  // ���������� ���������� ���������� ������
  memset((void *)&TaskParm, 0, sizeof(TASKPARMEX));
  TaskParm.Language  = ML_RUSSIAN;
  TaskParm.Handle = &MessageHandler;
  TaskParm.DocHandle = (HWND)GetMapView();

  POINT leftTop;
  mapView->GetMapLeftTop(&leftTop.x, &leftTop.y);
  mapView->TranslatePicturePointFromViewFrame(&leftTop);

  FMapCenter.X = leftTop.x;
  FMapCenter.Y = leftTop.y;

  HMAP mapHandle = mapView->GetMapHandle();
  mapPictureToPlane(mapHandle, &FMapCenter.X, &FMapCenter.Y);

  IsGeo = mapIsGeoSupported(mapHandle);
  if (IsGeo)
    mapPlaneToGeoWGS84(mapHandle, &FMapCenter.X, &FMapCenter.Y);

  try
  {
    HMAP hmap = mapPortalMenu(mapHandle, &TaskParm, 50, 50);

    // ���������� ������
    if (mapHandle == 0 && hmap)
      mapCloseData(hmap);
  }
  catch (...)
  {
  }

  return;
}

//----------------------------------------------------------------------------//
// ����� - ���������� ��������� ���������� �������� ���� ���������
//----------------------------------------------------------------------------//
void QDMapWMSPopupDialog::MapAction(int acttype)
{
  QDMapView   *mv;
  TASKPARMEX  TaskParm;
  DOUBLEPOINT dp;
  double      H;

  mv = GetMapView();
  if (mv == NULL) return;

  switch (acttype)
  {
    // ��� ��������� �������� ��� ����� ���������� � ������� WMS
    // ����� ����������� ��� ��������� WMS
    // ���������� � �������� WMS ��������� ��� ������
    case ACTDOREPAINTFORCHANGEDATA:
    {
      mv->UpdatePictureBorderForAll();

      dp.X = FMapCenter.X;
      dp.Y = FMapCenter.Y;

      H = ERRORHEIGHT;
      if (IsGeo) mapGeoWGS84ToPlane3D(mv->GetMapHandle(), &dp.X, &dp.Y, &H);
      mapPlaneToPicture(mv->GetMapHandle(), &dp.X, &dp.Y);
      mv->SetMapLeftTop(dp.X, dp.Y);

      mv->Repaint();
    }
  }
}

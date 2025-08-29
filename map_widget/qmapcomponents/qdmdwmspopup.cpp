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
*             ÊÎÌÏÎÍÅÍÒ - ÄÈÀËÎÃ ÓÏĞÀÂËÅÍÈß ÑÎÑÒÀÂÎÌ               *
*                       WMS-ÑÅĞÂÈÑÎÂ                               *
*                                                                  *
*******************************************************************/

#include "qdmdwmspopup.h"
#include "mapapi.h"
#include "wmsapi.h"
#include "maplib.h"


static HINSTANCE PortalMenuInst = 0;
// Ãëàâíûé îáğàáîò÷èê ñîîáùåíèé
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ ÊÎÌÏÎÍÅÍÒ - ÄÈÀËÎÃ ÓÏĞÀÂËÅÍÈß ÑÎÑÒÀÂÎÌ       +
//+++               WMS-ÑÅĞÂÈÑÎÂ                   +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Êîíñòğóêòîğ
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
//      Äåñòğóêòîğ
//-------------------------------------------
QDMapWMSPopupDialog::~QDMapWMSPopupDialog()
{
  if (PortalMenuInst)
    mapFreeLibrary(PortalMenuInst);
  PortalMenuInst = 0;
}

//------------------------------------------------------------------
// Óêàçàòåëü íà êàğòó
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
// Âûïîëíèòü äèàëîã
//------------------------------------------------------------------
void QDMapWMSPopupDialog::Execute()
{
  QDMapView   *mapView;
  TASKPARMEX  TaskParm;

  mapView = GetMapView();
  if (mapView == NULL)
    return;
  if (mapView->GetMapHandle() == 0) return;

  // Çàïîëíåíèå ïàğàìåòğîâ ïğèêëàäíîé çàäà÷è
  // Çàïîëíåíèå ïàğàìåòğîâ ïğèêëàäíîé çàäà÷è
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

    // Îñâîáîäèòü ïàìÿòü
    if (mapHandle == 0 && hmap)
      mapCloseData(hmap);
  }
  catch (...)
  {
  }

  return;
}

//----------------------------------------------------------------------------//
// Ìåòîä - îáğàáîò÷èê ñîîáùåíèé ïîñûëàåìûõ äèàëîãîì îêíó äîêóìåíòà
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
    // İòî ñîîáùåíèå ïğèõîäèò ïğè ëşáûõ èçìåíåíèÿõ â ñîñòàâå WMS
    // êîãäà äîáàâëÿåòñÿ èëè óäàëÿåòñÿ WMS
    // äîáàâëåíèå è óäàëåíèå WMS âûïîëíÿåò ñàì äèàëîã
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

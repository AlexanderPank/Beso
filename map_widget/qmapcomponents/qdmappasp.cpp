/******** QDMAPPASP.cpp  *******************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2020              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR QT DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*                    Компонент Паспорт карты                       *
*                                                                  *
*******************************************************************/

#include <qcursor.h>

#include "mapapi.h"
#include "maplib.h"
#include "struct.h"

#include "qdmappasp.h"

long int MessageHandler(long int context, long int code,
                        long int wp, long int lp, long int typemsg = 0);


QDMapPasp::QDMapPasp(QWidget *parent, const char *name) :
    QDMapComponent(parent, name)
{
   hide();
}

QDMapView *QDMapPasp::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapPasp::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

int QDMapPasp::CreateMap(WCHAR *mapName, int mapNameSize, WCHAR *rscName, int rscNameSize,
                         WCHAR *password, int passwordSize, int epsg)
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return 0;

  TASKPARMEX taskParmEx;
  memset(&taskParmEx, 0, sizeof(taskParmEx));
  taskParmEx.Resource = (HINSTANCE)mapView;
  taskParmEx.Handle = MessageHandler;

  HINSTANCE libInst = 0;
  typedef long int(WINAPI * paspCreateMapProExType)(HMAP hmap, WCHAR *mapname, long int size,
                                                    TASKPARMEX * parm,
                                                    WCHAR * password, long int sizepassw,
                                                    WCHAR * rscname, long int sizersc,
                                                    long int epsgcode);

  paspCreateMapProExType paspCreateMapProExFunction =
      (paspCreateMapProExType)mapLoadLibrary(MAPPASPLIB, &libInst, "paspCreateMapProEx");

  int isMapCreated = 0;
  if (paspCreateMapProExFunction)
    isMapCreated = (*paspCreateMapProExFunction)(mapView->GetMapHandle(), mapName, mapNameSize, &taskParmEx, password, passwordSize, rscName, rscNameSize, epsg);

  if (libInst)
    mapFreeLibrary(libInst);

  return isMapCreated;
}

int QDMapPasp::ShowMapPasp()
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return 0;

  TASKPARMEX taskParmEx;
  memset(&taskParmEx, 0, sizeof(taskParmEx));
  taskParmEx.Resource = (HINSTANCE)mapView;
  taskParmEx.Handle = MessageHandler;

  HINSTANCE libInst = 0;

  typedef long int(WINAPI * paspViewPaspType)(HMAP hmap, HSITE hsite, TASKPARMEX * parm);
  paspViewPaspType paspViewPaspFunction = (paspViewPaspType)mapLoadLibrary(MAPPASPLIB, &libInst, "paspViewPasp");

  int isShown = 0;
  if (paspViewPaspFunction)
    isShown = (*paspViewPaspFunction)(mapView->GetMapHandle(), mapView->GetMapHandle(), &taskParmEx);

  if (libInst)
    mapFreeLibrary(libInst);

  return isShown;
}


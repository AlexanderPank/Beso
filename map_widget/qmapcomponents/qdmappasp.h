/******** QDMAPPASP_H **********************************************
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
*                      КОМПОНЕНТ Паспорт карты                     *
*                                                                  *
*******************************************************************/

#ifndef QDMAPPASP_H
#define QDMAPPASP_H

#include "qdmcmp.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КОМПОНЕНТ Паспорт карты
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapPasp : public QDMapComponent
{
  Q_OBJECT

  public :

  QDMapPasp(QWidget *parent = NULL, const char *name = NULL);

  virtual void SetMapView(QDMapView *value);
  QDMapView *GetMapView() const;

  int CreateMap(WCHAR *mapName, int mapNameSize, WCHAR *rscName = 0, int rscNameSize = 0,
                WCHAR *password = 0, int passwordSize = 0, int epsg = 0);

  int ShowMapPasp();
};

#endif // QDMAPPASP_H

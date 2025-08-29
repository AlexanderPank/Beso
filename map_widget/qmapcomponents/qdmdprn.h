/******** QDMDPRN.H ************************************************
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
*                      КОМПОНЕНТ ПЕЧАТИ                            *
*                                                                  *
*******************************************************************/

#ifndef QDMDPRN_H
#define QDMDPRN_H

#include "qdmcmp.h"

class QDMapPrintAction;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КОМПОНЕНТ ПЕЧАТИ
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapPrint : public QDMapComponent
{
  Q_OBJECT

  Q_PROPERTY(bool AllRectActive READ GetAllRectActive WRITE SetAllRectActive)

  public:
  QDMapPrint(QWidget *parent = NULL, const char *name = NULL);

  // Активировать компонент
  void Execute();

  virtual void SetMapView(QDMapView *value);
  QDMapView *GetMapView() const;

  void SetAllRectActive(bool value);
  bool GetAllRectActive() const;

  void SetMapRect(int left, int top, int right, int bottom);
  int IsMapRectValid() const;

  public:
  RECT MapRect;
  bool AllRectActive;

  QDMapPrintAction *MapPrintAction;  // Обработчик печати
};

typedef QDMapPrint QDMapBuildEps;  // для обратной совместимости

#endif // QDMDPRN_H

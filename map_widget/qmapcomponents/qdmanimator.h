/******** QDMANIMATOR.H ********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR QT-DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*  Компонент     : QDMapAnimator - компонент                       *
*                  изменения параметров отображения объектов       *
*                                                                  *
*******************************************************************/
#ifndef QDMAPANIMATOR_H
#define QDMAPANIMATOR_H

#include "qdmcompa.h"
#include "qdmselt.h"
#include "qdmcmp.h"
#include "QTimer"

class QDMapAnimator : public QDMapComponent
{
  Q_OBJECT

public:
  QDMapAnimator(QDMapView * parent, QDMapSelect* select);
  ~QDMapAnimator();

  void SetSelect(QDMapSelect * select);
  void SetColorSelection(int color);
  void SetTimeoutOn(int mseconds);
  void SetTimeoutOff(int mseconds);

  void MapAction(int acttype);

public slots:
  void TimerEvent();

private:
  void Paint();
  bool PaintToInternalBuffer(QDMapView*mapView);
  bool PaintToMapViewBuffer(QDMapView*mapView);
  void PaintRectToInternalBuffer(HMAP hMap, QRect paintRect, QPoint topLeftPoint);
  void StartTimer();
  void StopTimer();

  QDMapSelect * Select;
  QTimer * Timer;
  QImage Buffer;
  QRect BufferRect;
  HPAINT PaintControl;
  bool State;
  bool Active;
  int ColorSelection;
  int TimeoutOn;
  int TimeoutOff;
  long int OldShowScale;
};

#endif //QDMAPANIMATOR_H

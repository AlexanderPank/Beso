/******** QDMACT.H *************************************************
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
*    QDGtkAction -  базовый класс обработки событий от компонента  *
*                         отображения карты                        *
*                                                                  *
*******************************************************************/

#ifndef QDMACT_H
#define QDMACT_H

#include "qdmcompa.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
// КЛАСС ОБРАБОТКИ СОБЫТИЙ ОТ КОМПОНЕНТА ОТОБРАЖЕНИЯ КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapView;
class QDMapWindow;

class QDGtkAction : public QDMapComponent
{
  Q_OBJECT

  public:

  QDGtkAction(QWidget *parent = 0, const char *name = 0);
  ~QDGtkAction();

  // Активен ли в данный момент обработчик
  bool GetActive();

  virtual void SetMapView(QDMapView *value);
  QDMapView *GetMapView() const;

  virtual void SetMapWindow(QDMapWindow *value);
  QDMapWindow *GetMapWindow() const;

  virtual void StartAction();
  virtual void StopAction();

  virtual void MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect);
  virtual void MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect);

  bool DoStopActionQuery();
  virtual void DoStopAction();

  virtual void KeyUp(int key, int count, int state);
  virtual void KeyDown(int key, int count, int state);
  virtual void DblClick();

  virtual void MouseDown(int x, int y, int modkeys);
  virtual void MouseMove(int x, int y, int modkeys);
  virtual void MouseUp(int x, int y, int modkeys);

  // Сообщение о завершении работы
  // При ошибке возвращает ноль
  virtual int Commit();

  // Запросить координаты текущей точки
  POINT GetPicturePoint();     // пикселы
  DOUBLEPOINT GetPlanePoint(); // метры
  DOUBLEPOINT GetMapPoint();   // дискреты
  DOUBLEPOINT GetMapPoint(int clientx, int clienty);

  public:
  bool VarActive;
};

#endif

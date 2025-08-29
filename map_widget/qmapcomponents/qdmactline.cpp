/******** QDMACTLINE.CPP *******************************************
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
*  Компонент     : QDMapActionLine - компонент                     *
*                  интерактивного заполнения объекта-линии         *
*                                                                  *
*******************************************************************/

#include "QPainter"

#include "qdmwina.h"
#include "qdmobj.h"

#include "qdmactline.h"


QDMapActionLine::QDMapActionLine(QWidget *parent, const char *name)
  :QDGtkAction(parent, name)
{
  VarActive = false;
  FMapObj = 0;
}

QDMapActionLine::~QDMapActionLine()
{
  StopAction();
}

void QDMapActionLine::MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect)
{

}

void QDMapActionLine::MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect)
{
  Q_UNUSED(sender);
  Q_UNUSED(mappaintrect);

  LinePaint(mapImage);
}

void QDMapActionLine::MouseMove(int x, int y, int modkeys)
{
  CalcCoordsForMouse(x, y);
}

void QDMapActionLine::MouseUp(int x, int y, int modkeys)
{
  // Только левой кнопкой мыши
  if (modkeys == 1)
  {
    CalcCoordsForMouse(x, y);
    AppendMouseCoordsToObject();
  }
}

void QDMapActionLine::DblClick()
{
  emit SignalEndAction();
}

void QDMapActionLine::AppendMouseCoordsToObject()
{
  FMapObj->AppendPlanePoint(0, &MouseCoords);
}

void QDMapActionLine::CalcCoordsForMouse(int aMouseX, int aMouseY)
{
  if (! VarActive)
    return;

  // узнаем куда идет отображение
  QDMapViewWindow *mapwin;
  mapwin = (QDMapViewWindow *)GetMapViewWindow();
  if (mapwin == NULL)
    return;

  int left, top;
  mapwin->GetMapLeftTop(&left, &top);

  // определим координаты мыши в метрах
  MouseCoords.X = left + aMouseX;
  MouseCoords.Y = top + aMouseY;
  mapwin->ConvertMetric(&MouseCoords.X, &MouseCoords.Y, PP_PICTURE, PP_PLANE);
}

void QDMapActionLine::LinePaint(QImage *mapImage)
{
  // Указатель на главное окно
  QDMapViewWindow *mapwin;
  mapwin = (QDMapViewWindow *)GetMapViewWindow();
  if (mapwin == NULL)
    return;

  QPainter imagePainter( mapImage );
  imagePainter.setCompositionMode( QPainter::RasterOp_SourceXorDestination );
  QPen pen( Qt::DashLine );
  pen.setColor( Qt::white );
  pen.setWidth( 2 );
  imagePainter.setPen( pen );

  int left, top;
  mapwin->GetMapLeftTop(&left, &top);

  if ( PaintObject(mapwin, &imagePainter, left, top) == true )
  {
    PaintOneLine(mapwin, &imagePainter, &LastPointCoords, &MouseCoords, left, top);
  }
}

bool QDMapActionLine::PaintObject(QDMapViewWindow *mapwin, QPainter *painter, int left, int top)
{
  bool retval = false;
  if (FMapObj)
  {
    // Запросить число точек объекта
    long int pointCount = FMapObj->GetPointCount(0);
    if (pointCount == 1)
    {
      FMapObj->GetPlanePoint(0, 1, &LastPointCoords);
      retval = true;
    }
    else if (pointCount > 0)
    {
      DOUBLEPOINT fromPoint;
      DOUBLEPOINT toPoint;

      for (int pointNumber = 1; pointNumber < (pointCount); pointNumber++)
      {
        FMapObj->GetPlanePoint(0, pointNumber, &fromPoint);
        FMapObj->GetPlanePoint(0, pointNumber + 1, &toPoint);
        PaintOneLine(mapwin, painter, &fromPoint, &toPoint, left, top);
        LastPointCoords = toPoint;
      }
      retval = true;
    }
  }
  return retval;
}

bool QDMapActionLine::PaintOneLine(QDMapViewWindow *mapwin, QPainter *painter, DOUBLEPOINT *fromPoint, DOUBLEPOINT *toPoint, int left, int top)
{
  double pointStartX = fromPoint->X;
  double pointStartY = fromPoint->Y;
  double pointEndX = toPoint->X;
  double pointEndY = toPoint->Y;
  mapwin->ConvertMetric(&pointStartX, &pointStartY, PP_PLANE, PP_PICTURE);
  mapwin->ConvertMetric(&pointEndX, &pointEndY, PP_PLANE, PP_PICTURE);
  pointStartX -= left;
  pointStartY -= top;
  pointEndX -= left;
  pointEndY -= top;

  painter->drawLine(pointStartX, pointStartY, pointEndX, pointEndY);
  return 1;
}

void QDMapActionLine::StartAction()
{
  if (VarActive)
    return;
  // объект для которого выбираем точку не указан
  if (! FMapObj)
    return;

  QDGtkAction::StartAction();
}

void QDMapActionLine::StopAction()
{
  QDGtkAction::StopAction();
}

QDMapObj *QDMapActionLine::GetMapObj()
{
  return FMapObj;
}

void QDMapActionLine::SetMapObj(QDMapObj *mapobj)
{
  FMapObj = mapobj;
}

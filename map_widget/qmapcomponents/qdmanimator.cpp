/******** QDMANIMATOR.CPP ******************************************
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
#include "QPainter"
#include "QTimer"

#include "qdmanimator.h"
#include "qdmcompa.h"
#include "qdmcmp.h"


QDMapAnimator::QDMapAnimator(QDMapView *parent, QDMapSelect *select)
  : QDMapComponent(parent, NULL)
{
  Select = NULL;
  Timer = NULL;
  ColorSelection = 0x00FFFF;
  Active = false;
  State = false;
  PaintControl = 0;
  OldShowScale = -1;
  TimeoutOff = 500;
  TimeoutOn = 1000;
  SetMapViewAcces(parent);
  SetSelect(select);
  StartTimer();
}

QDMapAnimator::~QDMapAnimator()
{
  if (PaintControl)
    mapFreePaintControl(PaintControl);
  SetSelect(NULL);
}

void QDMapAnimator::SetSelect(QDMapSelect *select)
{
  delete Select;
    Select = select;
}

void QDMapAnimator::SetColorSelection(int color)
{
  ColorSelection = color;
  // Сбросим габариты отображенного, чтобы отрисовалось все заново
  BufferRect = QRect();
}

void QDMapAnimator::SetTimeoutOn(int mseconds)
{
  TimeoutOn = mseconds;
}

void QDMapAnimator::SetTimeoutOff(int mseconds)
{
  TimeoutOff = mseconds;
}

void QDMapAnimator::MapAction(int acttype)
{
  switch (acttype)
  {
    case ACTBEFORECLOSE:
      break;
    case ACTAFTEROPEN:
      break;
    case ACTAFTERPAINT:
      Paint();
      break;
    case  ACTBEFOREDELETESITE:
      break;
  }
}

void QDMapAnimator::TimerEvent()
{
  if (Active == true)
  {
    QDMapView * mapView = (QDMapView *)VarMapView;
    if (mapView == NULL)
    {
        StopTimer();
        return;
    }
    if (State)
      Timer->setInterval(TimeoutOff);
    else
      Timer->setInterval(TimeoutOn);

    State = (State == false);
    mapView->update();
    Timer->start();
  }
}

void QDMapAnimator::Paint()
{
  // Узнаем к кому привязаны
  QDMapView * mapView = (QDMapView *)VarMapView;
  if (mapView == NULL)
    return;
  if (Active == false)
      return;
  if (State == false)
      return;

  if(PaintToInternalBuffer(mapView) == true)
    PaintToMapViewBuffer(mapView);
}

bool QDMapAnimator::PaintToInternalBuffer(QDMapView * mapView)
{
  RECT paintRect = mapView->VarMapRec;
  HMAP hMap = mapView->GetMapHandle();

  // На изменение масштаба перерисовать всю карту
  long int showScale = mapGetShowScale(hMap);
  if (showScale != OldShowScale)
  {
    BufferRect = QRect();
    OldShowScale = showScale;
  }

  if (hMap == 0)
    return false;

  QPoint pointTopLeft(paintRect.left, paintRect.top);
  long int width = paintRect.right - paintRect.left;
  long int height = paintRect.bottom - paintRect.top;
  width = ((width/16) + 1) * 16;
  height = ((height/16) + 1) * 16;
  paintRect.right = paintRect.left + width;
  paintRect.bottom = paintRect.top + height;
  QPoint pointBottomRight(paintRect.right, paintRect.bottom);

  QRegion regionPaint(QRect(pointTopLeft, pointBottomRight));
  QRegion regionBuffer(BufferRect);
  // Область рисования не изменилась, поэтому оставляем буфер как был
  if (regionPaint == regionBuffer)
    return true;

  QRect qPaintRect = regionPaint.boundingRect();

  // Reallocate buffer
  QImage newBuffer = QImage(qPaintRect.width(), qPaintRect.height(), QImage::Format_ARGB32);
  if (newBuffer.isNull())
    return false;            // Не удалось выделить новый буфер
  if (Buffer.isNull() == false)
  {
    // Скопировать что можно из старого буфера в новый
    // Найти пересечение в координатах изображения
    QRect intersect = regionPaint.intersected(regionBuffer).boundingRect();
    if (intersect.isNull() == false)
    {
      // Перевести в координаты старого и нового QImage
      QRect intersectSource = intersect.translated(QPoint(0,0) - BufferRect.topLeft());
      QRect intersectDest = intersect.translated(QPoint(0,0) - pointTopLeft);

      newBuffer.fill(Qt::transparent);
      // Скопируем старое изображение в новое
      QPainter paintNew(&newBuffer);
      paintNew.drawImage(intersectDest, Buffer, intersectSource);
    }
  }
  Buffer = newBuffer;

  QVector<QRect> rectsPaint = regionPaint.subtracted(regionBuffer).rects();

  if (rectsPaint.isEmpty() == false)
  {
    for (int i = 0; i < rectsPaint.count(); i++)
    {
      QRect currentRect = rectsPaint.at(i);
      PaintRectToInternalBuffer(hMap, currentRect, currentRect.topLeft() - pointTopLeft);
    }
  }

  // Update buffer rect
  BufferRect = regionPaint.boundingRect();

  return true;
}

bool QDMapAnimator::PaintToMapViewBuffer(QDMapView *mapView)
{
  QPainter painter(mapView->GetScreenImageBuffer());
  painter.drawImage(0, 0, Buffer);
  return true;
}

void QDMapAnimator::PaintRectToInternalBuffer(HMAP hMap, QRect paintRect, QPoint topLeftPoint)
{
  if (PaintControl == 0)
    PaintControl = mapCreatePaintControl(hMap);
  XIMAGEDESC ximage;
  ximage.Height = Buffer.height();
  ximage.Width = Buffer.width();
  ximage.RowSize = Buffer.bytesPerLine();
  ximage.Depth = Buffer.depth();
  ximage.CellSize = Buffer.bytesPerLine() / Buffer.width();
  ximage.Point = (char*)Buffer.bits();

  RECT rect;
  rect.left = paintRect.left();
  rect.top = paintRect.top();
  rect.right = paintRect.right() + 1;
  rect.bottom = paintRect.bottom() + 1;

  mapPaintSelectToXImage(hMap, &ximage, topLeftPoint.x(), topLeftPoint.y(), &rect, Select->GetHSelect(0), ColorSelection, 1, PaintControl);
}

void QDMapAnimator::StartTimer()
{
  if ( Active )
    StopTimer();
  Active = true;
  State = true;  // Начать выделенным
  if (Timer == 0)
  {
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    Timer->setSingleShot(true);
    Timer->setInterval(TimeoutOn);
    Timer->start();
  }
  update();
}

void QDMapAnimator::StopTimer()
{
  Active = false;
  if (Timer != 0)
  {
    Timer->stop();
    delete Timer;
    Timer = 0;
  }
  update();
}


/******** QDMWINW.CPP **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*   ОПИСАНИЕ КЛАССА : "КОМПОНЕНТ ОТОБРАЖЕНИЯ ОТКРЫТЫХ ДАННЫХ       *
*                      ЭЛЕКТРОННОЙ КАРТЫ"                          *
*                                                                  *
*******************************************************************/


#include <QScrollBar>

#include "qdmwinw.h"
#include "qdmcmp.h"
#include "qdmvport.h"
#include "qdmselt.h"
#include "mapapi.h"


//-----------------------------------------------------------------------------
//  Конструктор
//-----------------------------------------------------------------------------
QDMapWindow::QDMapWindow(QWidget *parent)
            :QDMapViewWindow(parent)
{

  VarMapHandle     = 0;
  VarMapView       = NULL;
  VarMapBright     = 0;
  VarMapContrast   = 0;
  VarMapContour    = 0;
  SetClassName("QDMapWindow");
}

//-----------------------------------------------------------------------------
//  Деструктор
//-----------------------------------------------------------------------------
QDMapWindow::~QDMapWindow()
{
  SetMapView(NULL);
}

//-----------------------------------------------------------------------------
// Смена MapView
//-----------------------------------------------------------------------------
void QDMapWindow::SetMapView(QDMapView *value)
{
  if (VarMapView == value)
    return;

  bool oldRendering = VarRendering;

  if (VarMapView != NULL) // удалим ссылку на себя в старом
    VarMapView->DeleteMapWindow(this);

  if (GetMapHandle())
    MapAction(ACTBEFORECLOSE);

  VarMapView   = NULL;
  VarMapHandle = 0;

  if (value != NULL) // добавим ссылку на себя в новый
    value->AppendMapWindow(this);
  VarMapView = value;
  if (VarViewSelect == NULL)
    VarViewSelect = new QDMapSelect(VarMapView);
  else
    VarViewSelect->SetMapView(VarMapView);

  if (GetMapHandle())
  {
    MapAction(ACTAFTEROPEN);
    VarViewScaleInWindow = mapGetShowScale(GetMapHandle());
  }

  SetRendering(oldRendering);
  MapAction(ACTDOREPAINTFORCHANGEDATA);
}

//------------------------------------------------------------------
// Получить идентификатор карты
//------------------------------------------------------------------
HMAP QDMapWindow::GetMapHandle()
{
  if (VarMapView == NULL)
    return 0;
  return VarMapView->GetMapHandle();
}

//------------------------------------------------------------------
// Получить объект доступа к карте
//------------------------------------------------------------------
QDMapView *QDMapWindow::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

QDMapViewAcces *QDMapWindow::GetMapViewAcces()
{
  return (QDMapViewAcces *)VarMapView;
}

//------------------------------------------------------------------
// Обработка событий изменения состава карт на уровне MapWindow
//------------------------------------------------------------------
void QDMapWindow::MapAction(int action)
{
  QDMapViewWindow::MapAction(action);
  switch (action)
  {
    case ACTAFTEROPEN :
      VarViewScaleInWindow = VarMapView->GetMapScale();
      break;
    case ACTDOREPAINTFORCHANGEDATA:
      Repaint();
      break;
  }
}

//------------------------------------------------------------------
// Восстановить необходимый масштаб отображенеия для окна
// так как MapView может работать в другом масштабе
// Возврат - масштаб в котором работала VarMapView
//------------------------------------------------------------------
int QDMapWindow::RestoreViewScaleWin()
{
  long int  FX, FY;
  int       Result = 0;
  HMAP      oMap;

  oMap = GetMapHandle();
  Result = 0;
  if (oMap == 0)
    return Result;
  Result = mapGetShowScale(oMap);
  if (Result != VarViewScaleInWindow)
  {
     FX = 0; FY = 0;
     mapSetViewScale(oMap, &FX, &FY, VarViewScaleInWindow);
  }

  return Result;
}

//------------------------------------------------------------------
// Восстановить масштаб отображенеия для MapView
//------------------------------------------------------------------
void QDMapWindow::RestoreViewScale(int scale)
{
  long int  FX, FY;
  HMAP      oMap;

  oMap = GetMapHandle();
  if ((oMap == 0) || (scale == 0))
    return;
  if (scale != VarViewScaleInWindow)
  {
      FX = 0; FY = 0;
      mapSetViewScale(oMap, &FX, &FY, scale);
  }
}

//------------------------------------------------------------------
// Для данного метода надо сначала восстановить масштаб
//------------------------------------------------------------------
void QDMapWindow::drawContents(QPainter* p, int cx, int cy, int cw, int ch)
{
    XIMAGEDESC tempImage;
    QImage tempQImage(cw, ch, QImage::Format_ARGB32 );
    POINT point;
    RECT rect;

    point.x    = 0;
    point.y    = 0;
    rect.left   = cx;
    rect.right  = cx + cw;
    rect.top    = cy;
    rect.bottom = cy + ch;

    tempImage.RowSize = tempQImage.bytesPerLine();
    tempImage.Width = tempQImage.width();
    tempImage.Height = tempQImage.height();
    tempImage.CellSize = tempImage.RowSize / tempImage.Width;
    tempImage.Depth = tempImage.CellSize * 8;

    tempImage.Point = (char*)tempQImage.bits();

    mapPaintToXImage(GetMapHandle(), &tempImage, point.x, point.y, &rect);
    p->drawImage(cx, cy, tempQImage);

    return;

/*
 int  iScale;
 int  oldMapBright, oldMapContrast;
 HMAP oMap = 0;
 int  vtype;
 bool oldMapContour;

 oMap = GetMapHandle();
 oldMapBright   = 0;
 oldMapContrast = 0;
 iScale = 0;

 if (oMap != 0)
 {
   iScale = RestoreViewScaleWin();

    vtype = mapGetViewType(oMap);
    if ((vtype == 2) || (vtype == 4))
      oldMapContour = TRUE;
    else
      oldMapContour = FALSE;
    if (oldMapContour != VarMapContour)
    {
      if (VarMapContour)
        mapSetViewType(oMap, 2);
      else
        mapSetViewType(oMap, 1);
    }
    oldMapBright = mapGetBright(oMap);
    if (oldMapBright != VarMapBright)
      mapSetBright(oMap, VarMapBright);
    oldMapContrast = mapGetContrast(oMap);
    if (oldMapContrast != VarMapContrast)
      mapSetContrast(oMap, VarMapContrast);

    SetViewSelectNow(VarViewSelect, false);
  }

  if (oMap != 0)
  {
    if (VarMapVisible)
    {
      if (VarRendering)
      {
        QRect rect;

        HScrollBar->setValue(VarHorzPos);
        VScrollBar->setValue(VarVertPos);

        rect.setLeft(cx);
        rect.setTop(cy);
        rect.setRight(cx + cw);
        rect.setBottom(cy + ch);

        RectDraw = rect;
        PaintOfMapAcces(p);
      }
    }
  }

  VarPaintEnabled = 0;

  if (oMap != 0)
  {
    SetViewSelectNow(VarMapView->VarViewSelect, false);

    RestoreViewScale(iScale);

    if (oldMapContour != VarMapContour)
    {
      if (oldMapContour)
        mapSetViewType(oMap, 2);
      else
        mapSetViewType(oMap, 1);
    }
    if (oldMapBright != VarMapBright)
      mapSetBright(oMap, oldMapBright);
    if (oldMapContrast != VarMapContrast)
      mapSetContrast(oMap, oldMapContrast);
  }
  */
}

//------------------------------------------------------------------
// Текущий масштаб отображения
//------------------------------------------------------------------
int QDMapWindow::GetViewScale()
{
  return VarViewScaleInWindow;
}

int QDMapWindow::SetViewScale(int value)
{
  int  iScale;
  bool FRend;
  int  Result = 0;

  if (value == 0) return 0;
  if (VarViewScaleInWindow == value) return 0;
  FRend = VarRendering;
  VarRendering = false;
  iScale = RestoreViewScaleWin();
  VarViewScaleInWindow = value;
  Result = QDMapViewWindow::SetViewScale(value);
  RestoreViewScale(iScale);
  VarRendering = FRend;
  if (VarRendering) Repaint();

  return Result;
}

//------------------------------------------------------------------
// Получить габариты карты в пикселах для текущего масштаба
//------------------------------------------------------------------
void QDMapWindow::GetMapHW(long int *height, long int *width)
{
 int iScale;

 if (GetMapHandle() == 0) return;
 iScale  = RestoreViewScaleWin();
 mapGetPictureSize(GetMapHandle(), width, height);
 RestoreViewScale(iScale);
}

//------------------------------------------------------------------
// Определит габариты в пикселах всего набора карт
//------------------------------------------------------------------
RECT QDMapWindow::GetRegionRect()
{
  int  iScale;
  RECT result;

  iScale = 0;
  if (GetMapHandle() != 0)
    iScale = RestoreViewScaleWin();
  result = QDMapViewWindow::GetRegionRect();
  RestoreViewScale(iScale);
  return result;
}

//------------------------------------------------------------------
// определить габариты всего набора карт в указанной системе координат
//------------------------------------------------------------------
MAPDFRAME QDMapWindow::GetRegionDFrame(int pointplace)
{
  int       iScale;
  MAPDFRAME result;

  iScale = 0;
  if (GetMapHandle() != 0)
    iScale = RestoreViewScaleWin();
  result = QDMapViewWindow::GetRegionDFrame(pointplace);
  RestoreViewScale(iScale);
  return result;
}

//------------------------------------------------------------------
// Установить/cбросить отображение карты в контурном виде
//------------------------------------------------------------------
void QDMapWindow::SetMapContour(bool value)
{
  if (!GetMapHandle())
    return;

  VarMapContour = value;

  if (VarRendering)
    Repaint();
}

//------------------------------------------------------------------
// Отображена ли карта в контурном виде
//------------------------------------------------------------------
bool QDMapWindow::GetMapContour()
{
  return VarMapContour;
}

//------------------------------------------------------------------
// Яркость отображения карты (-4..+4)
//------------------------------------------------------------------
int QDMapWindow::GetMapBright()
{
  return VarMapBright;
}

void QDMapWindow::SetMapBright(int value)
{
  if ((value < -4) || (value > 4))
    return;

  VarMapBright = value;

  if (VarRendering)
    Repaint();
}

//------------------------------------------------------------------
// Контрастность отображения карты (-4..+4)
//------------------------------------------------------------------
int QDMapWindow::GetMapContrast()
{
  return VarMapContrast;
}

void QDMapWindow::SetMapContrast(int value)
{
  if ((value < -4) || (value > 4))
    return;

  VarMapContrast = value;

  if (VarRendering)
    Repaint();
}

//------------------------------------------------------------------
// Преобразовать точку метрики с координатами aX, aY из системы aPlaceIn в систему aPlaceOut
// При отказе возрващает <> 0
//------------------------------------------------------------------
int QDMapWindow::ConvertMetric(double *x, double *y, int placein, int placeout)
{
  int oldScale, result;


  if (GetMapHandle() == 0)
    return 0;
  oldScale = RestoreViewScaleWin();
  result = QDMapViewWindow::ConvertMetric(x, y, placein, placeout);
  RestoreViewScale(oldScale);

  return result;
}

//------------------------------------------------------------------
// Конвертировать координаты из dframein система координат placein
//------------------------------------------------------------------
MAPDFRAME QDMapWindow::ConvertDFrame(MAPDFRAME dframein, int placein, int placeout)
{
  int oldScale;
  MAPDFRAME result;

  oldScale = RestoreViewScaleWin();
  result = QDMapViewWindow::ConvertDFrame(dframein, placein, placeout);
  RestoreViewScale(oldScale);

  return result;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void QDMapWindow::Notification(QObject *component, int operation)
{
  if ((operation == OPREMOVE) && ((long int)VarMapView == (long int)component))
    SetMapView(NULL);
}

//------------------------------------------------------------------
// Перерисовка окна
//------------------------------------------------------------------
void QDMapWindow::Repaint()
{
  QDMapViewWindow::Repaint();
}

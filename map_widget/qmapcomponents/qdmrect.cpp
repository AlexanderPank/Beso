/******** QDMRECT.CPP **********************************************
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
*            Компонент выбора участка открытой карты               *
*                                                                  *
*******************************************************************/

#include <cmath>

#include "prnapi.h"

#include "qcursor.h"
#include "qdmcmp.h"
#include "qdmrect.h"
#include "qdmvport.h"
#include "qdmwina.h"

#define SENSITIVITY 3

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapSelectRect::QDMapSelectRect(QWidget * parent, const char * name):
  QDGtkAction(parent, name),
  Regime(0)
{
  VarActive = 0;
  SelectedPoints = 0;
  First  = 0;
  Second = 0;
  BuildFrame();

  VarPlace = PP_PICTURE;
  hide();
}

//-------------------------------------------
//      Деструктор
//-------------------------------------------
QDMapSelectRect::~QDMapSelectRect()
{
  StopAction();
}

void QDMapSelectRect::StopAction()
{
  if (!VarActive)
    return;

  SelectedPoints = 0;
  First  = 0;
  Second = 0;
  BuildFrame();

  QDGtkAction::StopAction();
}

// --------------------------------------------------------------
// Отрисовать обработчик до/после вывода карты в буфер
// --------------------------------------------------------------
void QDMapSelectRect::MapAfterPaint(QObject *sender, QImage * mapImage, RECT mappaintrect)
{
  if (!mapImage)
    return;

  if (SelectedPoints == 0)
    return;

  QDMapViewWindow *mapWindow = (QDMapViewWindow *) GetMapViewWindow();
  if (!mapWindow)
    return;

  // Настроить QPainter для рисования штриховой линией
  QPainter imagePainter(mapImage);
  imagePainter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  QPen pen(Qt::DashLine);
  pen.setColor(Qt::white);
  pen.setWidth(2);
  imagePainter.setPen(pen);

  BORDER drawBorder = Border;
  ConvertBorderToClient(drawBorder);

  if ((SelectedPoints == 1) && (Regime == MPR_SELECTTURNFRAME))  // нарисовать линию
  {
    imagePainter.drawLine(QPoint(drawBorder.UpLeft.X, drawBorder.UpLeft.Y), QPoint(drawBorder.DownRight.X, drawBorder.DownRight.Y));
  }
  else  // нарисовать прямоугольную или наклонную рамку
  {
    imagePainter.drawLine(QPoint(drawBorder.UpLeft.X, drawBorder.UpLeft.Y), QPoint(drawBorder.UpRight.X, drawBorder.UpRight.Y));
    imagePainter.drawLine(QPoint(drawBorder.UpRight.X, drawBorder.UpRight.Y), QPoint(drawBorder.DownRight.X, drawBorder.DownRight.Y));
    imagePainter.drawLine(QPoint(drawBorder.DownRight.X, drawBorder.DownRight.Y), QPoint(drawBorder.DownLeft.X, drawBorder.DownLeft.Y));
    imagePainter.drawLine(QPoint(drawBorder.DownLeft.X, drawBorder.DownLeft.Y), QPoint(drawBorder.UpLeft.X, drawBorder.UpLeft.Y));
  }

  QDGtkAction::MapAfterPaint(sender, mapImage, mappaintrect);
}

// --------------------------------------------------------------
// Обработка событий
// --------------------------------------------------------------
void QDMapSelectRect::MouseUp(int x, int y, int )
{
  if (VarActive == 0)
    return;

  if (SelectedPoints == 2)
    return;  // Фрагмент выбран

  if (SelectedPoints == 0)
  {
    SelectedPoints = 1;        // Первая точка выбранa
    First  = GetMapPoint(x, y);
    Second = First;
    BuildFrame();
  }
  else
  {
    SelectedPoints = 2;        // Вторая точка выбрана
    Second = GetMapPoint(x, y);
    BuildFrame();

    MAPDFRAME dframe;
    GetSelRect(&dframe, VarPlace);
    emit SignalAfterBuildRect(dframe.X1, dframe.Y1, dframe.X2, dframe.Y2);

    StartAction();
  }
}

void QDMapSelectRect::MouseMove(int x, int y, int )
{
  if (VarActive == 0)
    return;

  if (SelectedPoints != 1)
    return;

  Second = GetMapPoint(x, y);
  BuildFrame();
}

// --------------------------------------------------------------
//  Построить контур фрагмента по угловым точкам
// --------------------------------------------------------------
void QDMapSelectRect::BuildFrame()
{
  if (SelectedPoints == 0)
  {
    Border.UpLeft.X     = First.X;
    Border.UpLeft.Y     = First.Y;
    Border.UpRight.X    = First.X;
    Border.UpRight.Y    = First.Y;
    Border.DownRight.X  = First.X;
    Border.DownRight.Y  = First.Y;
    Border.DownLeft.X   = First.X;
    Border.DownLeft.Y   = First.Y;
    Border.UpLeftLast.X = First.X;
    Border.UpLeftLast.Y = First.Y;
    return;
  }

  // Построить точки по габаритам фрагмента
  if (First.X < Second.X)
  {
    Border.UpLeft.X    = Second.X;
    Border.UpRight.X   = Second.X;
    Border.DownLeft.X  = First.X;
    Border.DownRight.X = First.X;
  }
  else
  {
    Border.UpLeft.X    = First.X;
    Border.UpRight.X   = First.X;
    Border.DownLeft.X  = Second.X;
    Border.DownRight.X = Second.X;
  }

  if (First.Y < Second.Y)
  {
    Border.UpLeft.Y    = First.Y;
    Border.DownLeft.Y  = First.Y;
    Border.UpRight.Y   = Second.Y;
    Border.DownRight.Y = Second.Y;
  }
  else
  {
    Border.UpLeft.Y    = Second.Y;
    Border.DownLeft.Y  = Second.Y;
    Border.UpRight.Y   = First.Y;
    Border.DownRight.Y = First.Y;
  }

  Border.UpLeftLast.X = Border.UpLeft.X;
  Border.UpLeftLast.Y = Border.UpLeft.Y;
}

// --------------------------------------------------------------
// Запрос габаритов выбранного
// участка карты
// --------------------------------------------------------------
int QDMapSelectRect::GetSelRect(MAPDFRAME *dframe, int pp)
{
  QDMapViewWindow *mapwin1;
  DFRAME           dframe1;

  if (!dframe || SelectedPoints != 2) return 0;

  // узнаем куда идет отображение
  mapwin1 = (QDMapViewWindow *)GetMapViewWindow();
  if (mapwin1 == NULL) return 0;

  dframe1.X1 = Border.UpLeft.X;
  dframe1.Y1 = Border.UpLeft.Y;
  dframe1.X2 = Border.DownRight.X;
  dframe1.Y2 = Border.DownRight.Y;
  *dframe = mapwin1->ConvertDFrame(dframe1, PP_PLANE, pp);

  return 1;
}

// --------------------------------------------------------------
// Преобразовать рамку в пикселы клиентского окна
// При ошибке возвращает ноль
// --------------------------------------------------------------
int QDMapSelectRect::ConvertBorderToClient(QDMapSelectRect::BORDER &border)
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return 0;

  // UpLeft
  mapView->ConvertMetric(&(border.UpLeft.X), &(border.UpLeft.Y), PP_MAP, PP_PICTURE);
  mapView->ConvertFromPictureToClient(&(border.UpLeft.X), &(border.UpLeft.Y));

  // UpRight
  mapView->ConvertMetric(&(border.UpRight.X), &(border.UpRight.Y), PP_MAP, PP_PICTURE);
  mapView->ConvertFromPictureToClient(&(border.UpRight.X), &(border.UpRight.Y));

  // DownRight
  mapView->ConvertMetric(&(border.DownRight.X), &(border.DownRight.Y), PP_MAP, PP_PICTURE);
  mapView->ConvertFromPictureToClient(&(border.DownRight.X), &(border.DownRight.Y));

  // DownLeft
  mapView->ConvertMetric(&(border.DownLeft.X), &(border.DownLeft.Y), PP_MAP, PP_PICTURE);
  mapView->ConvertFromPictureToClient(&(border.DownLeft.X), &(border.DownLeft.Y));

  // UpLeftLast
  mapView->ConvertMetric(&(border.UpLeftLast.X), &(border.UpLeftLast.Y), PP_MAP, PP_PICTURE);
  mapView->ConvertFromPictureToClient(&(border.UpLeftLast.X), &(border.UpLeftLast.Y));

  return 1;
}

// --------------------------------------------------------------
// Запрос/установка системы координат
// --------------------------------------------------------------
void QDMapSelectRect::SetPlaceRect(int pp)
{
  VarPlace = pp;
}

int QDMapSelectRect::GetPlaceRect()
{
  return VarPlace;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КЛАСС ИЗМЕНЕНИЯ ВЫБРАННОГО УЧАСТКА КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

QDMapSelectRectAndChange::QDMapSelectRectAndChange(QWidget *parent, const char *name) :
  QDMapSelectRect(parent, name),
  ChangeFlag(0),
  ChangePos(0),
  ChangeUp(0),
  ChangeDown(0),
  ChangeRight(0),
  ChangeLeft(0)
{
  memset(&BeginPos, 0, sizeof(BeginPos));
  memset(&PlaneFrame, 0, sizeof(PlaneFrame));
}

// --------------------------------------------------------------
// Подключить обработчик
// --------------------------------------------------------------
void QDMapSelectRectAndChange::StartAction()
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return;

  if (SelectedPoints == 2)
  {
    // Перевести единицы измерения (дискреты -> метры)
    PlaneFrame.X2 = Border.UpLeft.X;
    PlaneFrame.Y1 = Border.UpLeft.Y;
    mapMapToPlane(mapHandle, &PlaneFrame.X2, &PlaneFrame.Y1);

    PlaneFrame.X1 = Border.DownRight.X;
    PlaneFrame.Y2 = Border.DownRight.Y;
    mapMapToPlane(mapHandle, &PlaneFrame.X1, &PlaneFrame.Y2);
  }

  // Вызвать диалог
  int ret = CallDialog();

  if (ret == 0)
  {
    StopAction();
    return;
  }

  DOUBLEPOINT p1, p2;
  switch (Regime)
  {
  case MPR_SELECTFRAME:  // выбрать фрагмент
    SelectedPoints = 0;

    ChangeFlag = 0;
    break;

  case MPR_CHANGEFRAME:  // изменить фрагмент
    p1.x = PlaneFrame.X2;
    p1.y = PlaneFrame.Y1;
    mapPlaneToMap(mapHandle, &p1.x, &p1.y);
    p2.x = PlaneFrame.X1;
    p2.y = PlaneFrame.Y2;
    mapPlaneToMap(mapHandle, &p2.x, &p2.y);

    SetFrame(&p1, &p2);

    ChangeFlag = 1;
    break;

  case MPR_MOVEFRAME:
    p1.x = PlaneFrame.X2;
    p1.y = PlaneFrame.Y1;
    mapPlaneToMap(mapHandle, &p1.x, &p1.y);
    p2.x = PlaneFrame.X1;
    p2.y = PlaneFrame.Y2;
    mapPlaneToMap(mapHandle, &p2.x, &p2.y);

    SetFrame(&p1, &p2);

    ChangeFlag = 2;
    break;

  case MPR_SELECTBYOBJECT:
    SelectedPoints = 0;

    ChangeFlag = 0;
    break;
  }

  QDGtkAction::StartAction();
}

// --------------------------------------------------------------
// Установить фрагмент
// --------------------------------------------------------------
void QDMapSelectRectAndChange::SetFrame(DOUBLEPOINT *first, DOUBLEPOINT *second)
{
  SelectedPoints = 2;
  First  = *first;
  Second = *second;
  BuildFrame();
}

// --------------------------------------------------------------
// Реакция на нажатие левой кнопки мыши
// --------------------------------------------------------------
void QDMapSelectRectAndChange::MouseDown(int x, int y, int)
{
  if (!GetMapView())
    return;

  if (Regime == MPR_SELECTBYOBJECT)
    return;

  if ((ChangeFlag == 0) && (SelectedPoints == 0))
    return QDMapSelectRect::MouseUp(x, y, 0);

  if (SelectedPoints < 2)
    return;

  // Определим положение мыши в пикселах
  POINT p = GetPicturePoint();

  // Определим размер фрагмента в пикселах
  DOUBLEPOINT temp = Border.UpLeft;
  POINT One;
  HMAP mapHandle = GetMapView()->GetMapHandle();
  mapMapToPicture(mapHandle, &temp.X, &temp.Y);
  One.x = temp.X;
  One.y = temp.Y;
  POINT Two;
  temp = Border.DownRight;
  mapMapToPicture(mapHandle, &temp.X, &temp.Y);
  Two.x = temp.X;
  Two.y = temp.Y;

  if (ChangeFlag == 1)
  {
    if (abs(p.x - One.x) < SENSITIVITY &&
        ((p.y > One.y-SENSITIVITY && p.y < Two.y+SENSITIVITY) ||
         (p.y > Two.y-SENSITIVITY && p.y < One.y+SENSITIVITY))) ChangeLeft  = 1;
    if (abs(p.y - One.y) < SENSITIVITY &&
        ((p.x > One.x-SENSITIVITY && p.x < Two.x+SENSITIVITY) ||
         (p.x > Two.x-SENSITIVITY && p.x < One.x+SENSITIVITY))) ChangeUp    = 1;
    if (abs(p.x - Two.x) < SENSITIVITY &&
        ((p.y > One.y-SENSITIVITY && p.y < Two.y+SENSITIVITY) ||
         (p.y > Two.y-SENSITIVITY && p.y < One.y+SENSITIVITY))) ChangeRight = 1;
    if (abs(p.y - Two.y) < SENSITIVITY &&
        ((p.x > One.x-SENSITIVITY && p.x < Two.x+SENSITIVITY) ||
         (p.x > Two.x-SENSITIVITY && p.x < One.x+SENSITIVITY))) ChangeDown  = 1;

    if (ChangeRight)
    {
      if (ChangeDown)
      {
        First  = Border.UpLeft;
        Second = Border.DownRight;
      }
      else
      {
        First  = Border.DownLeft;
        Second = Border.UpRight;
      }
    }
    else if (ChangeLeft)
    {
      if (ChangeDown)
      {
        First  = Border.UpRight;
        Second = Border.DownLeft;
      }
      else
      {
        First  = Border.DownRight;
        Second = Border.UpLeft;
      }
    }
    else if (ChangeDown)
    {
      First  = Border.UpLeft;
      Second = Border.DownRight;
    }
    else if (ChangeUp)
    {
      First  = Border.DownRight;
      Second = Border.UpLeft;
    }
    else if (((One.x < Two.x && p.x > One.x && p.x < Two.x) ||
              (One.x > Two.x && p.x < One.x && p.x > Two.x)) &&
             ((One.y < Two.y && p.y > One.y && p.y < Two.y) ||
              (One.y > Two.y && p.y < One.y && p.y > Two.y)))
    {
      ChangePos = 1;
      BeginPos = GetMapPoint();
    }
  }
  else
  {
    if (((One.x < Two.x && p.x > One.x && p.x < Two.x) ||
         (One.x > Two.x && p.x < One.x && p.x > Two.x)) &&
        ((One.y < Two.y && p.y > One.y && p.y < Two.y) ||
         (One.y > Two.y && p.y < One.y && p.y > Two.y)))
    {
      ChangePos = 1;
      BeginPos = GetMapPoint();
    }
  }
}

// --------------------------------------------------------------
// Реакция на отпуск левой кнопки мыши
// --------------------------------------------------------------
void QDMapSelectRectAndChange::MouseUp(int x, int y, int)
{
  if (Regime == MPR_SELECTBYOBJECT)
    return;

  if (ChangeFlag == 0)
    return QDMapSelectRect::MouseUp(x, y, 0);

  if (SelectedPoints == 1)
  {
    QDMapSelectRect::MouseMove(x, y, 0);
    SelectedPoints = 2;
  }

  ChangeLeft  = 0;
  ChangeUp    = 0;
  ChangeRight = 0;
  ChangeDown  = 0;
  ChangePos   = 0;

  return;
}

// --------------------------------------------------------------
// Перемещение мыши
// --------------------------------------------------------------
void QDMapSelectRectAndChange::MouseMove(int x, int y, int)
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return;

  if (Regime == MPR_SELECTBYOBJECT)
    return;

  if (ChangeFlag == 0 || SelectedPoints < 2)
  {
    return QDMapSelectRect::MouseMove(x, y, 0);
  }

  if (ChangeLeft || ChangeUp || ChangeRight || ChangeDown)
  {
    DOUBLEPOINT mp = GetMapPoint();

    if (ChangeLeft || ChangeRight)
    {
      if (ChangeUp || ChangeDown) Second = mp;
      else Second.Y = mp.Y;
    }
    else
    {
      if (ChangeUp || ChangeDown) Second.X = mp.X;
    }

    SelectedPoints = 1;
    BuildFrame();
    SelectedPoints = 2;
  }
  else if (ChangePos)
  {
    MoveFrame();
  }
  else
  {
    // Установка курсора
    int changeLeft = 0;
    int changeUp = 0;
    int changeRight = 0;
    int changeDown = 0;
    Qt::CursorShape cursorShape = Qt::ArrowCursor;

    // Определим положение мыши в пикселах
    POINT p = GetPicturePoint();

    // Определим размер фрагмента в пикселах
    DOUBLEPOINT temp = Border.UpLeft;
    POINT One;
    mapMapToPicture(mapHandle, &temp.X, &temp.Y);
    One.x = temp.X;
    One.y = temp.Y;
    POINT Two;
    temp = Border.DownRight;
    mapMapToPicture(mapHandle, &temp.X, &temp.Y);
    Two.x = temp.X;
    Two.y = temp.Y;

    if (ChangeFlag == 1)
    {
      if (abs(p.x - One.x) < SENSITIVITY && ((p.y > One.y - SENSITIVITY && p.y < Two.y + SENSITIVITY) || (p.y > Two.y - SENSITIVITY && p.y < One.y + SENSITIVITY)))
        changeLeft = 1;
      if (abs(p.y - One.y) < SENSITIVITY && ((p.x > One.x - SENSITIVITY && p.x < Two.x + SENSITIVITY) || (p.x > Two.x - SENSITIVITY && p.x < One.x + SENSITIVITY)))
        changeUp = 1;
      if (abs(p.x - Two.x) < SENSITIVITY && ((p.y > One.y - SENSITIVITY && p.y < Two.y + SENSITIVITY) || (p.y > Two.y - SENSITIVITY && p.y < One.y + SENSITIVITY)))
        changeRight = 1;
      if (abs(p.y - Two.y) < SENSITIVITY && ((p.x > One.x - SENSITIVITY && p.x < Two.x + SENSITIVITY) || (p.x > Two.x - SENSITIVITY && p.x < One.x + SENSITIVITY)))
        changeDown = 1;

      if ((changeLeft && changeUp) || (changeRight && changeDown))
      {
        cursorShape = Qt::SizeFDiagCursor;
      }
      else if ((changeLeft && changeDown) || (changeRight && changeUp))
      {
        cursorShape = Qt::SizeBDiagCursor;
      }
      else if (changeUp || changeDown)
      {
        cursorShape = Qt::SizeVerCursor;
      }
      else if (changeLeft || changeRight)
      {
        cursorShape = Qt::SizeHorCursor;
      }
      else if (((One.x < Two.x && p.x > One.x && p.x < Two.x) || (One.x > Two.x && p.x < One.x && p.x > Two.x))
               && ((One.y < Two.y && p.y > One.y && p.y < Two.y) || (One.y > Two.y && p.y < One.y && p.y > Two.y)))
      {
        cursorShape = Qt::SizeAllCursor;
      }
    }
    else
    {
      if (((One.x < Two.x && p.x > One.x && p.x < Two.x) || (One.x > Two.x && p.x < One.x && p.x > Two.x))
          && ((One.y < Two.y && p.y > One.y && p.y < Two.y) || (One.y > Two.y && p.y < One.y && p.y > Two.y)))
      {
        cursorShape = Qt::SizeAllCursor;
      }
    }

    mapView->setCursor(cursorShape);
  }
}

// --------------------------------------------------------------
// Сообщение о завершении работы
// При ошибке возвращает ноль
// --------------------------------------------------------------
int QDMapSelectRectAndChange::Commit()
{
  if (Regime == MPR_SELECTBYOBJECT)
  {
    StartAction();
    return 1;
  }

  if (ChangeFlag == 0)
    return 1;

  SelectedPoints = 2;

  ChangeLeft  = 0;
  ChangeUp    = 0;
  ChangeRight = 0;
  ChangeDown  = 0;
  ChangePos   = 0;

  StartAction();

  return 1;
}

// --------------------------------------------------------------
// Перемещение рамки по окну
// --------------------------------------------------------------
void QDMapSelectRectAndChange::MoveFrame()
{
  DOUBLEPOINT point = GetMapPoint();

  double dx = BeginPos.x - point.x;
  double dy = BeginPos.y - point.y;

  Border.UpLeft.X    -= dx;  Border.UpLeft.Y    -= dy;
  Border.DownLeft.X  -= dx;  Border.DownLeft.Y  -= dy;
  Border.UpRight.X   -= dx;  Border.UpRight.Y   -= dy;
  Border.DownRight.X -= dx;  Border.DownRight.Y -= dy;

  Border.UpLeftLast.X = Border.UpLeft.X;
  Border.UpLeftLast.Y = Border.UpLeft.Y;

  BeginPos = point;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КЛАСС ВЫБОРА НАКЛОННОГО УЧАСТКА КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

QDMapSelectRectTurn::QDMapSelectRectTurn(QWidget *parent, const char *name):
    QDMapSelectRectAndChange(parent, name),
    Angle(0.0),
    PageWidth(0.0),
    PageHeight(0.0)
{
  memset(&Third, 0, sizeof(Third));
  memset(PageLine, 0, sizeof(PageLine));
}

// --------------------------------------------------------------
// Подключить обработчик
// --------------------------------------------------------------
void QDMapSelectRectTurn::StartAction()
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return;

  if (SelectedPoints == 2 && Regime != MPR_SELECTTURNFRAME)
  {
    // Перевести единицы измерения (дискреты -> метры)
    PlaneFrame.X2 = Border.UpLeft.X;
    PlaneFrame.Y1 = Border.UpLeft.Y;
    mapMapToPlane(mapHandle, &PlaneFrame.X2, &PlaneFrame.Y1);

    PlaneFrame.X1 = Border.DownRight.X;
    PlaneFrame.Y2 = Border.DownRight.Y;
    mapMapToPlane(mapHandle, &PlaneFrame.X1, &PlaneFrame.Y2);
  }

  // Вызвать диалог
  Regime = CallDialog();
  if (Regime == 0)
  {
    StopAction();
    return;
  }

  Angle = 0.0;

  DOUBLEPOINT p1, p2;
  switch (Regime)
  {
  case MPR_SELECTFRAME:  // выбрать фрагмент
    SelectedPoints = 0;

    ChangeFlag = 0;
    break;

  case MPR_CHANGEFRAME:  // изменить фрагмент
    p1.x = PlaneFrame.X2;
    p1.y = PlaneFrame.Y1;
    mapPlaneToMap(mapHandle, &p1.x, &p1.y);
    p2.x = PlaneFrame.X1;
    p2.y = PlaneFrame.Y2;
    mapPlaneToMap(mapHandle, &p2.x, &p2.y);

    SetFrame(&p1, &p2);

    ChangeFlag = 1;
    break;

  case MPR_MOVEFRAME:
    p1.x = PlaneFrame.X2;
    p1.y = PlaneFrame.Y1;
    mapPlaneToMap(mapHandle, &p1.x, &p1.y);
    p2.x = PlaneFrame.X1;
    p2.y = PlaneFrame.Y2;
    mapPlaneToMap(mapHandle, &p2.x, &p2.y);

    SetFrame(&p1, &p2);

    ChangeFlag = 2;
    break;

  case MPR_SELECTTURNFRAME:
  case MPR_SELECTBYOBJECT:
    SelectedPoints = 0;

    ChangeFlag = 0;
    break;
  }

  QDGtkAction::StartAction();
}

// --------------------------------------------------------------
// Отрисовать обработчик до/после вывода карты в буфер
// --------------------------------------------------------------
void QDMapSelectRectTurn::MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect)
{
  if (SelectedPoints == 0)
    return;

  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return;

  QDMapSelectRect::MapAfterPaint(sender, mapImage, mappaintrect);

  // Для наклонной рамки не рисовать деление на листы
  if (Regime == MPR_SELECTTURNFRAME)
    return;

  // Размеры страницы меньше 10 мм
  if (PageWidth < 10.0 || PageHeight < 10.0)
    return;

  // Пересчет из миллиметров в дискреты
  double scale = PrintScale / 1000.0;
  double sizeX = PageHeight*scale;
  double sizeY = PageWidth*scale;
  mapPlaneToMap(mapHandle, &sizeX, &sizeY);

  double x = 0.0;
  double y = 0.0;
  mapPlaneToMap(mapHandle, &x, &y);
  sizeX -= x;
  sizeY -= y;

  x = Border.UpLeft.X - sizeX;
  y = Border.UpLeft.Y + sizeY;

  // Настроить QPainter для рисования штриховой линией
  QPainter imagePainter(mapImage);
  imagePainter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
  QPen pen(Qt::DashLine);
  pen.setColor(Qt::white);
  pen.setWidth(2);
  imagePainter.setPen(pen);

  // Горизонтальные линии деления на страницы
  while (x > Border.DownRight.X)
  {
    PageLine[0].X = x;
    PageLine[0].Y = Border.UpLeft.Y;

    PageLine[1].X = x;
    PageLine[1].Y = Border.DownRight.Y;

    // Перевести из дискретов в пикселы клиентского окна (для рисования QPainter)
    mapView->ConvertMetric(&(PageLine[0].X), &(PageLine[0].Y), PP_MAP, PP_PICTURE);
    mapView->ConvertFromPictureToClient(&(PageLine[0].X), &(PageLine[0].Y));
    mapView->ConvertMetric(&(PageLine[1].X), &(PageLine[1].Y), PP_MAP, PP_PICTURE);
    mapView->ConvertFromPictureToClient(&(PageLine[1].X), &(PageLine[1].Y));

    imagePainter.drawLine(PageLine[0].X, PageLine[0].Y, PageLine[1].X, PageLine[1].Y);

    x -= sizeX;
  }

  // Вертикальные линии деления на страницы
  while (y < Border.DownRight.Y)
  {
    PageLine[0].X = Border.UpLeft.X;
    PageLine[0].Y = y;

    PageLine[1].X = Border.DownRight.X;
    PageLine[1].Y = y;

    // Перевести из дискретов в пикселы клиентского окна (для рисования QPainter)
    mapView->ConvertMetric(&(PageLine[0].X), &(PageLine[0].Y), PP_MAP, PP_PICTURE);
    mapView->ConvertFromPictureToClient(&(PageLine[0].X), &(PageLine[0].Y));
    mapView->ConvertMetric(&(PageLine[1].X), &(PageLine[1].Y), PP_MAP, PP_PICTURE);
    mapView->ConvertFromPictureToClient(&(PageLine[1].X), &(PageLine[1].Y));

    imagePainter.drawLine(PageLine[0].X, PageLine[0].Y, PageLine[1].X, PageLine[1].Y);

    y += sizeY;
  }
}

// --------------------------------------------------------------
// Реакция на отпуск левой кнопки мыши
// --------------------------------------------------------------
void QDMapSelectRectTurn::MouseUp(int x, int y, int)
{
  if (Regime != MPR_SELECTTURNFRAME)
    return QDMapSelectRectAndChange::MouseUp(x, y, 0);

  if (SelectedPoints == 3)
    return;  // Фрагмент выбран

  if (SelectedPoints == 0)
  {
    SelectedPoints = 1;        // Первая точка выбрана
    First = GetMapPoint();
    Second = First;
    BuildFrame();
  }
  else if (SelectedPoints == 1)
  {
    SelectedPoints = 2;        // Вторая точка выбрана
    Second = GetMapPoint();
    BuildFrame();
  }
  else
  {
    SelectedPoints = 3;        // Третья точка выбрана
    Third = GetMapPoint();
    BuildFrame();

    StartAction();
  }
}

// --------------------------------------------------------------
// Перемещение мыши
// --------------------------------------------------------------
void QDMapSelectRectTurn::MouseMove(int x, int y, int)
{
  if (Regime != MPR_SELECTTURNFRAME)
    return QDMapSelectRectAndChange::MouseMove(x, y, 0);

  if (SelectedPoints == 0 || SelectedPoints == 3)
    return;

  if (SelectedPoints == 1)
    Second = GetMapPoint();
  else
    Third = GetMapPoint();

  BuildFrame();
}

// --------------------------------------------------------------
// Построить контур фрагмента по угловым точкам
// --------------------------------------------------------------
void QDMapSelectRectTurn::BuildFrame()
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return;

  if (Regime != MPR_SELECTTURNFRAME)
  {
    QDMapSelectRectAndChange::BuildFrame();
    return;
  }

  Border.UpLeftLast = First;

  if (SelectedPoints == 0)
  {
    Border.UpLeft     = First;
    Border.UpRight    = First;
    Border.DownRight  = First;
    Border.DownLeft   = First;
    Border.UpLeftLast = First;
    return;
  }

  if (SelectedPoints == 1)
  {
    Border.UpLeft     = First;
    Border.UpRight    = Second;
    Border.DownRight  = Second;
    Border.DownLeft   = Second;
    Border.UpLeftLast = Second;
    return;
  }

  double k,b2,b3,b4,xc,yc;
  double x[4],y[4];

  x[0] = Border.UpLeft.X;
  y[0] = Border.UpLeft.Y;
  x[1] = Border.UpRight.X;
  y[1] = Border.UpRight.Y;

  if (x[1] == x[0])
  {
    Border.DownRight.X  = Third.X;
    Border.DownRight.Y  = y[1];
    Border.DownLeft.X   = Third.X;
    Border.DownLeft.Y   = y[0];
  }
  else
  {
    if (y[1] == y[0])
    {
      Border.DownRight.X  = x[1];
      Border.DownRight.Y  = Third.Y;
      Border.DownLeft.X   = x[0];
      Border.DownLeft.Y   = Third.Y;
    }
    else
    {
      xc = Third.X;
      yc = Third.Y;
      k  = (y[1] - y[0])/(x[1] - x[0]);
      b2 = yc - k*xc;
      b3 = y[0]  + x[0]/k;
      b4 = y[1]  + x[1]/k;
      x[2] = ((b4 - b2)*k)/(k*k + 1);
      x[3] = ((b3 - b2)*k)/(k*k + 1);
      y[2] = (b2 + k*k*b4)/(k*k + 1);
      y[3] = (b2 + k*k*b3)/(k*k + 1);

      Border.DownRight.X  = x[2];
      Border.DownRight.Y  = y[2];
      Border.DownLeft.X   = x[3];
      Border.DownLeft.Y   = y[3];
    }
  }

  if (First.X == Second.X && First.Y == Second.Y)
    Angle = 0;
  else
    Angle = atan2(Second.Y-First.Y, Second.X-First.X);

  // Направление выбора по часовой стрелке ?
  if (( First.X + Second.X) * ( First.Y - Second.Y) +
        (Second.X +  Third.X) * (Second.Y -  Third.Y) +
        ( Third.X +  First.X) * ( Third.Y -  First.Y) < 0)
  {
    PlaneFrame.X1 = Border.UpLeft.X;
    PlaneFrame.Y1 = Border.UpLeft.Y;
    PlaneFrame.X2 = Border.DownRight.X;
    PlaneFrame.Y2 = Border.DownRight.Y;
  }
  else  // Против часовой стрелки
  {
    PlaneFrame.X1 = Border.DownLeft.X;
    PlaneFrame.Y1 = Border.DownLeft.Y;
    PlaneFrame.X2 = Border.UpRight.X;
    PlaneFrame.Y2 = Border.UpRight.Y;
  }

  // Перевести в метры
  mapMapToPlane(mapHandle, &PlaneFrame.X1, &PlaneFrame.Y1);
  mapMapToPlane(mapHandle, &PlaneFrame.X2, &PlaneFrame.Y2);
}



















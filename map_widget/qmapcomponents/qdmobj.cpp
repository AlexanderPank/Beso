/******* QDMOBJ.CPP ************************************************
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
*     ОПИСАНИЕ КЛАССА : КОМПОНЕНТА ОБЪЕКТА ЭЛЕКТРОННОЙ КАРТЫ       *
*                                                                  *
*******************************************************************/

#include <QApplication>
#include <QScrollBar>

#include "qdmvport.h"
#include "qdmobj.h"
#include "qdmsem.h"
#include "qdmmet.h"
#include "qdmdgtob.h"

// Максимальное значение координат(для функций семейства mapPaint)
extern int MaxValueCoordXY; //(qdmwina.cpp)

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ БАЗОВЫЙ КЛАСС ОБЪЕКТА ЭЛЕКТРОННОЙ КАРТЫ                  +++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QDMapObj::QDMapObj(QWidget * parent, const char * name)
  :QDMapObjAcces(parent, name)
{
  VarStyle         = OS_NOVIEW;
  VarStyleSelect   = SEL_LINE;
  VarImage.Image   = IMG_OBJECT;
  VarImage.Mode    = 3; // GXcopy для XWindow
  VarColorImage    = RGB(255, 0, 0);
  VarColorImageLin = VarColorImage;
  VarInCodeImage   = 0;
  VarImage.Parm    = (char *)&VarColorImage;
  memset((char *)&VarObjFrame, 0, sizeof(VarObjFrame));
  VarSemantic      = new QDMapSemantic(this);
  VarMetric        = new QDMapMetric(this);
  VarOldFrame      = true;
  VarPlace         = PP_PLANE;
  PaintControl     = 0;

  memset(StrVal, 0, sizeof(StrVal));
  hide();
}

QDMapObj::~QDMapObj()
{
  if (VarObjHandle != 0)
  {
    if ((GetMapView() != NULL) && (VarStyle != OS_NOVIEW))
    {
      VarStyle = OS_NOVIEW;
      Repaint();
    }

    // удалить описание объекта
    // векторной карты из памяти
    mapFreeObject(VarObjHandle);
    VarObjHandle = 0;
  }

  delete VarSemantic;
  VarSemantic = NULL;
  delete VarMetric;
  VarMetric = NULL;

  if (PaintControl)
  {
    mapFreePaintControl(PaintControl);
    PaintControl = 0;
  }
}

QDMapView *QDMapObj::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

long int QDMapObj::GetPointCount(long int subject)
{
  return mapPointCount(VarObjHandle, subject);
}

long QDMapObj::GetPlanePoint(long subject, long number, DOUBLEPOINT *outPoint)
{
  return mapGetPlanePoint(VarObjHandle, outPoint, number, subject);
}

long QDMapObj::AppendPlanePoint(long subject, DOUBLEPOINT *inPoint)
{
  return mapAppendPointPlane(VarObjHandle, inPoint->x, inPoint->y, subject);
}

void QDMapObj::SetMapView(QDMapView *value)
{
  int i;
  QDMapView *mapview;

  mapview = (QDMapView *)VarMapView;
  SetMapViewAcces(value);
  if (VarMetric != NULL) VarMetric->VarMapPoint->SetMapView(VarMapView);

  if (mapview == VarMapView) return;

  if (VarObjHandle != 0)
  {
    if (VarStyle != OS_NOVIEW)
    {
      VarStyle = OS_NOVIEW;
      RepaintPrivate(mapview);
    }
    mapFreeObject(VarObjHandle);
    VarObjHandle = 0;
  }

  if ((VarMapView != NULL) && (VarMapView->GetMapHandle() != 0))
  {
    VarObjHandle = mapCreateObject(VarMapView->GetMapHandle(), 1, IDDOUBLE2, 0);
  }

  VarOldFrame = true;
}

int QDMapObj::GetKey()
{
  if (VarObjHandle == 0) return 0;
  else return mapObjectKey(VarObjHandle);
}

int QDMapObj::GetExCode()
{
  if (VarObjHandle == 0) return 0;
  else return mapObjectExcode(VarObjHandle);
}

int QDMapObj::GetInCode()
{
  if (VarObjHandle == 0) return 0;
  else return mapObjectCode(VarObjHandle);
}

int QDMapObj::GetLocal()
{
  if (VarObjHandle == 0)
    return LOCAL_LINE;
  else
    return mapObjectLocal(VarObjHandle);
}

const char * QDMapObj::GetObjName()
{
  memset(StrVal, 0, sizeof(StrVal));

  if (VarObjHandle == 0)
    return StrVal;
  else
    return (char *)mapObjectName(VarObjHandle);
}

int QDMapObj::GetImage()
{
  return VarImage.Image;
}

int QDMapObj::GetMode()
{
  return VarImage.Mode;
}

const char *QDMapObj::GetParm()
{
  return VarImage.Parm;
}

char *QDMapObj::GetListName()
{
  memset(StrVal, 0, sizeof(StrVal));

  if (VarObjHandle == 0) return StrVal;
  else return (char *)mapListName(VarObjHandle);
}

char *QDMapObj::GetText(int subobj)
{
  memset(StrVal, 0, sizeof(StrVal));

  if (VarObjHandle == 0) return StrVal;
  else if (((GetLocal() == LOCAL_TITLE) || (GetLocal() == LOCAL_MIXED)) &&
           ((subobj >= 0) && (subobj <= VarMetric->GetSubObjCount())))
    mapGetText(VarObjHandle, StrVal, sizeof(StrVal), subobj);
  return (char *)StrVal;
}

bool QDMapObj::GetTextType()
{
  if (VarObjHandle == 0) return false;
  else if (mapObjectLocal(VarObjHandle) != 3) return false;
  return true;
}

double QDMapObj::GetSquare()
{
  if (VarObjHandle == 0) return  0;
  else return mapSquare(VarObjHandle);
}

double QDMapObj::GetPerimeter()
{
  if (VarObjHandle == 0) return  0;
  else return mapPerimeter(VarObjHandle);
}

double QDMapObj::GetLength()
{
  if (VarObjHandle == 0) return  0;
  else return mapLength(VarObjHandle);
}

int QDMapObj::GetSiteNumber()
{
  if (VarObjHandle == 0) return 0;
  else
    return mapGetSiteNumber(VarMapView->GetMapHandle(),
                            mapGetObjectSiteIdent(VarMapView->GetMapHandle(), VarObjHandle));
}

int QDMapObj::GetListNumber()
{
  int   i, ret = 0;
  char *st;

  if (VarObjHandle != 0)
  {
    st = (char *)mapListName(VarObjHandle);
    for (i = 0; i < mapGetListCount(VarMapView->GetMapHandle())-1; i++)
    {
      if (strcmp(mapGetListName(VarMapView->GetMapHandle(), i),st) == 0)
      {
        ret = i; break;
      }
    }
  }
  return ret;
}

int QDMapObj::GetPlaceOut()
{
  if (VarObjHandle == 0) return PP_PLANE;
  else return VarPlace;
}

//-----------------------------------------------------
// Определение габаритов существующего объекта
//-----------------------------------------------------
MAPDFRAME QDMapObj::GetObjFrame(int place)
{
  int  OldPlace;

  if ((VarObjHandle == 0) || (GetKey() == 0))
  {
    memset(&VarObjFrame, 0, sizeof(VarObjFrame));
    return VarObjFrame;
  }
  if (VarOldFrame)
  {
    double x1;

    mapObjectViewFrame(VarObjHandle, &VarObjFrame);
    switch (place)
    {
      case PP_PLANE: break;
      case PP_PICTURE :
        mapPlaneToPicture(VarMapView->GetMapHandle(),
                          &VarObjFrame.X2, &VarObjFrame.Y1);
        mapPlaneToPicture(VarMapView->GetMapHandle(),
                          &VarObjFrame.X1, &VarObjFrame.Y2);

        x1 = VarObjFrame.X1;
        VarObjFrame.X1 = VarObjFrame.X2;
        VarObjFrame.X2 = x1;
        break;
    }
  }
  return VarObjFrame;
}

int QDMapObj::GetLayerNumber()
{
  if (VarObjHandle == 0) return 0;
  else return mapSegmentNumber(VarObjHandle);
}

void QDMapObj::SetText(int subobj, char * value)
{
  if (VarObjHandle != 0)
    if ((GetLocal() == LOCAL_TITLE) || (GetLocal() == LOCAL_MIXED))
      if ((subobj >= 0) && (subobj <= VarMetric->GetSubObjCount()))
      {
        mapPutText(VarObjHandle, value, subobj);
        VarOldFrame = true;
      }
}

//-----------------------------------------------------
// Установить идентификатор HObj объекта векторной карты
//-----------------------------------------------------
void QDMapObj::SetObjHandle(HOBJ value)
{
  HOBJ obj;
  int  style;

  // Проверить условия допустимости операции установки
  if ((VarObjHandle == value) || (VarMapView == NULL) ||
      (VarMapView->GetMapHandle() == 0)) return;

  style  =  VarStyle;

  VarOldFrame  =  true;

  // удалить настройку на прежний объект
  if (VarObjHandle != 0)
  {
    if (VarStyle != OS_NOVIEW)
      VarStyle = OS_NORMAL;
    mapFreeObject(VarObjHandle);
    VarObjHandle  =  0;
  }

  if (value == 0) // настроим на пустой объект
    obj = mapCreateObject(VarMapView->GetMapHandle(), 1, IDDOUBLE2, 0);
  else obj = mapCreateCopyObject(VarMapView->GetMapHandle(), value);
  if (obj == 0) return; // плохо все

  // настроим на новый объект
  VarObjHandle = obj;
  if (obj == 0) return; // плохо все

  if (VarStyle != style) SetStyle(style);
}

//======================================================
// Создание объекта по его Внешнему коду и локализации (старая версия)
//======================================================
void QDMapObj::CreateObject(int site, int kind, bool text, int list, int excode, int local)
{
  int   T;
  HSITE siteh;

  if (local == LOCAL_TITLE) T = 1; else T = 0;
  if (VarObjHandle != 0)
  {
    if (VarStyle != OS_NOVIEW) SetStyle(OS_NOVIEW);
    mapFreeObject(VarObjHandle);
    VarObjHandle = 0;
  }

  if ((VarMapView != NULL) && (VarMapView->GetMapHandle() != 0))
  {
    if (site != 0)
    {
      siteh  =  mapGetSiteIdent(VarMapView->GetMapHandle(), site);
      VarObjHandle = mapCreateSiteObject(VarMapView->GetMapHandle(), siteh, kind, T);
    } else
      VarObjHandle = mapCreateObject(VarMapView->GetMapHandle(), 1, kind, T);
    if (VarObjHandle != 0) mapRegisterObject(VarObjHandle, excode, local);
  }
  VarOldFrame = true;
}

void QDMapObj::CreateNullObj(int sitenumber)
{
  HSITE siteh;

  if (VarObjHandle != 0)
  {
    if (VarStyle != OS_NOVIEW) SetStyle(OS_NOVIEW);
    mapFreeObject(VarObjHandle);
    VarObjHandle = 0;
  }

  if ((VarMapView != NULL) && (VarMapView->GetMapHandle() != 0))
  {
    if (sitenumber != 0)
    {
      siteh  =  mapGetSiteIdent(VarMapView->GetMapHandle(),sitenumber);
      VarObjHandle = mapCreateSiteObject(VarMapView->GetMapHandle(), siteh, IDDOUBLE2, 0);
    }
    else VarObjHandle = mapCreateObject(VarMapView->GetMapHandle(), 1, IDDOUBLE2, 0);
  }
}

void QDMapObj::Commit()
{
  if (VarMapView != NULL)
    if ((VarMapView->GetMapHandle() != 0) && (VarObjHandle!=0))
    {
      mapCommitWithPlace(VarObjHandle);
      Repaint();
    }
  VarOldFrame  =  true;
}

void QDMapObj::Delete()
{
  if (VarMapView != NULL)
    if ((VarMapView->GetMapHandle() != 0) && (VarObjHandle != 0))
    {
      VarStyle = OS_NOVIEW;
      mapDeleteObject(VarObjHandle);
      Repaint();
    }
  VarOldFrame  =  true;
}

void QDMapObj::MapAction(int acttype)
{
  switch (acttype)
  {
    case ACTBEFORECLOSE :
      if (VarObjHandle != 0)
      {
        if (VarStyle != OS_NOVIEW) SetStyle(OS_NOVIEW);
        mapFreeObject(VarObjHandle);
        VarObjHandle = 0;
      }
      VarOldFrame = true;
      break;
    case ACTAFTEROPEN :
      if (VarMapView != NULL)
        if (VarMapView->GetMapHandle() != 0)
          VarObjHandle = mapCreateObject(VarMapView->GetMapHandle(), 1, IDDOUBLE2, 0);
      VarOldFrame = true;
      break;
    case ACTAFTERPAINT :
      Paint(0, ((QDMapView *)VarMapView)->VarMapRec);
      break;
    case  ACTBEFOREDELETESITE :
      if (VarObjHandle != 0)
      {
        if (((QDMapView *)VarMapView)->VarMapSites->VarMapSite->VarSite ==
            mapGetObjectSiteIdent(VarMapView->GetMapHandle(), VarObjHandle))
        {
          if (VarStyle != OS_NOVIEW) SetStyle(OS_NOVIEW);
          mapFreeObject(VarObjHandle);
          VarObjHandle = 0;
          if (VarMapView != NULL) if (VarMapView->GetMapHandle() != 0)
          {
            VarObjHandle = mapCreateObject(VarMapView->GetMapHandle(), 1, IDDOUBLE2, 0);
          }
          VarOldFrame = true;
        }
      }
      break;
  }
}

void QDMapObj::Paint(HDC dc, RECT updateRect)
{
  if (VarStyle != OS_SELECT)
    return;

  QDMapView *mapView = GetMapView();

  if ((VarObjHandle == 0) || (mapView == NULL)) return;
  if (mapObjectKey(VarObjHandle) == 0) return;

  HMAP mapHandle = mapGetObjectDocIdent(VarObjHandle);
  HSITE sitHandle = mapGetObjectSiteIdent(mapHandle, VarObjHandle);

  if (mapHandle != mapView->GetMapHandle())
    return;

  if (!PaintControl)
    PaintControl = mapCreatePaintControl(mapHandle);

  HSELECT select = mapCreateSiteSelectContext(mapHandle, sitHandle);

  RECT paintRect = updateRect;

  QPoint pointTopLeft(paintRect.left, paintRect.top);

  long int width = paintRect.right - paintRect.left;
  long int height = paintRect.bottom - paintRect.top;
  width = ((width/16) + 1) * 16;
  height = ((height/16) + 1) * 16;

  paintRect.right = paintRect.left + width + 1;
  paintRect.bottom = paintRect.top + height + 1;

  QPoint pointBottomRight(paintRect.right, paintRect.bottom);

  QRegion regionPaint(QRect(pointTopLeft, pointBottomRight));
  QRect qPaintRect = regionPaint.boundingRect();

  mapClearSelectContext(select);
  mapClearSample(select);
  long int objectList = mapGetListNumber(VarObjHandle);
  long int objectNumber = mapGetObjectNumber(VarObjHandle);
  mapSelectSampleByNumber(select, objectList, objectNumber);

  QImage newBuffer = QImage(qPaintRect.width(), qPaintRect.height(), QImage::Format_ARGB32);
  if (newBuffer.isNull())
      return;

  newBuffer.fill(Qt::transparent);

  XIMAGEDESC ximage;
  memset(&ximage, 0, sizeof(ximage));
  ximage.CellSize = newBuffer.bytesPerLine() / newBuffer.width();
  ximage.Depth = newBuffer.depth();
  ximage.Height = newBuffer.height();
  ximage.Width = newBuffer.width();
  ximage.RowSize = newBuffer.bytesPerLine();
  ximage.Point = (char*)newBuffer.bits();

  mapPaintSelectToXImage(mapHandle, &ximage, 0, 0, &paintRect, select, VarColorImageLin, 1, PaintControl);

  QPainter painter(mapView->GetScreenImageBuffer());
  painter.drawImage( 0, 0, newBuffer);

  mapDeleteSelectContext(select);
}


//===================================================
// Установить тип выделенного отображения объекта карты
//===================================================
void QDMapObj::SetStyleSelect(int value)
{
  VarStyleSelect  =  value;

  if (VarObjHandle != 0) Repaint();
}

//===================================================
// Установить вид отображения объекта карты
//===================================================
void QDMapObj::SetStyle(int value)
{
  if (VarStyle == value) return;

  VarStyle = value;
  if (VarStyle != OS_SELECT)
  {
    if (VarObjHandle != 0) Repaint();
  }
  else SetStyleSelect(VarStyleSelect);
}

void QDMapObj::SetColorImage(int value)
{
  if (VarColorImage == value) return;

  VarColorImage = value;
  VarColorImageLin = VarColorImage;

  if ((VarObjHandle != 0) && (VarStyle == OS_SELECT)) Repaint();
}

void QDMapObj::Repaint()
{
  RepaintPrivate((QDMapView *)VarMapView);
}

//-----------------------------------------------------------
// Найти пересечение областей rect1 и rect2
// Результат поместить в result
//-----------------------------------------------------------
int IntersectRect(RECT *result, RECT rect1, RECT rect2)   // 05/09/07
{
  if (!result) return 0;

  if (rect1.left < rect2.left) result->left = rect2.left;
  else result->left = rect1.left;
  if (rect1.right > rect2.right) result->right = rect2.right;
  else result->right = rect1.right;
  if (result->right < result->left) return 0;

  if (rect1.top < rect2.top) result->top = rect2.top;
  else result->top = rect1.top;
  if (rect1.bottom > rect2.bottom) result->bottom = rect2.bottom;
  else result->bottom = rect1.bottom;
  if (result->bottom < result->top) return 0;
  else return 1;
}

void QDMapObj::RepaintPrivate(QDMapViewAcces *mapview)
{
  QDMapView      *mv;
  RECT           RObj, RMap, RView;
  MAPDFRAME      Frame;
  double         x, y;

  if ((VarObjHandle == 0) || (mapview == NULL)) return;
  mv  =  (QDMapView *)mapview;
  if ((mv->GetMapHandle() == 0) || (!mv->VarMapVisible)) return;
  if (!mv->VarRendering) return;

  if (mapObjectKey(VarObjHandle) == 0) return;

  RMap.left    =  0;
  RMap.right   =  mv->MapViewPort->width();
  RMap.top     =  0;
  RMap.bottom  =  mv->MapViewPort->height();

  // стало после  08/04/99 берем всю клиентскую часть окна для надежности
  if (VarStyle == OS_SELECT)
  {
    Paint(0, RMap); return;
  }

  // определим область перерисовки карты
  mapObjectViewFrame(VarObjHandle, &Frame);       // 08/06/07
  x = Frame.X2;
  y = Frame.Y1;
  mapPlaneToPicture(mv->GetMapHandle(), &x, &y);
  RObj.left  =  x;// - 40;
  RObj.top   =  y;// - 40;
  x = Frame.X1;
  y = Frame.Y2;
  mapPlaneToPicture(mv->GetMapHandle(), &x, &y);
  RObj.right   =  x;// + 40;
  RObj.bottom  =  y;// + 40;

  RObj.left    =  RObj.left   - mv->HScrollBar->value();
  RObj.right   =  RObj.right  - mv->HScrollBar->value();
  RObj.top     =  RObj.top    - mv->VScrollBar->value();
  RObj.bottom  =  RObj.bottom - mv->VScrollBar->value();

  if (IntersectRect(&RView, RObj, RMap) == 0)
  {
    return;
  }

  RView.left    =  RView.left   + mv->HScrollBar->value();
  RView.right   =  RView.right  + mv->HScrollBar->value();
  RView.top     =  RView.top    + mv->VScrollBar->value();
  RView.bottom  =  RView.bottom + mv->VScrollBar->value();

  mv->Repaint(RView.left, RView.top,
              RView.right - RView.left, RView.bottom - RView.top);
}

//-----------------------------------------------------------
// Освободить текущий объект
//-----------------------------------------------------------
void QDMapObj::FreeCurrentObj()
{
  if (VarObjHandle == 0) return;
  if (VarStyle != OS_NOVIEW) SetStyle(OS_NOVIEW);
  mapFreeObject(VarObjHandle);
  VarObjHandle = 0;
  VarOldFrame  =  true;
}

//-----------------------------------------------------------
// Найти объект карты по его уникальным параметрам
// Вход  - listname - название листа (или карты для Site)
//         Key      - номер объекта
// Выход - При успешном поиске возвращает 1 иначе 0
//-----------------------------------------------------------
int QDMapObj::SeekObject(char *listname, int key)
{
  HOBJ obj1;
  HOBJ seek;
  int  ret =0;

  if ((VarMapView == NULL) || (VarMapView->GetMapHandle() == 0))
    return 0;
  obj1 = mapCreateObject(VarMapView->GetMapHandle(), 1, IDDOUBLE2, 0);
  if (obj1 == 0)
    return ret;
  seek = mapSeekObject(VarMapView->GetMapHandle(), obj1, listname,key);

  if (seek == 0)
    return ret;

  if (obj1 != 0)
  {
    if (seek  != 0)
    {
      SetObjHandle(obj1);
      ret  =  1;
    }
    mapFreeObject(obj1);
  }

  VarOldFrame  =  true;
  return ret;
}

//-----------------------------------------------------------
// Переместить отображение объекта карты в центр окна
//-----------------------------------------------------------
void QDMapObj::Center()
{
  QDMapPointAcces *MapPoint;
  double          DX,DY;
  MAPDFRAME       dframe = GetObjFrame(PP_PLANE);
  bool            OldRendering;                      // 13/02/03

  DX = (VarObjFrame.X1 + VarObjFrame.X2)/2;
  DY = (VarObjFrame.Y1 + VarObjFrame.Y2)/2;

  MapPoint = new QDMapPointAcces(GetMapView());

  MapPoint->SetPlaceIn(PP_PLANE);
  MapPoint->SetPlaceOut(PP_PICTURE);
  MapPoint->SetPoint(DX, DY);
  MapPoint->GetPoint(&DX, &DY);

  OldRendering = GetMapView()->VarRendering;
  GetMapView()->VarRendering = false;
  ((QDMapViewWindow *)VarMapView)
      ->HScrollBar->setValue(DX - (((QDMapViewWindow *)VarMapView)->MapViewPort->width() / 2));
  ((QDMapViewWindow *)VarMapView)
      ->VScrollBar->setValue(DY - (((QDMapViewWindow *)VarMapView)->MapViewPort->height() / 2));

  GetMapView()->VarRendering = OldRendering;
  GetMapView()->Repaint();
  delete MapPoint;
}

//-----------------------------------------------------------
// Возвращает идентификатор пользовательской карты
//-----------------------------------------------------------
HSITE QDMapObj::GetSiteHandle()
{
  HSITE SiteH = mapGetSiteIdent(VarMapView->GetMapHandle(),
                                GetSiteNumber());

  return SiteH;
}

//-----------------------------------------------------------
// Установить/запросить верхнюю и нижнюю границу масштаба
//-----------------------------------------------------------
void  QDMapObj::SetBotTopScale(int bot, int top)
{
  if (GetObjHandle() != 0)
  {
    mapSetObjectBotScale(GetObjHandle(), bot);
    mapSetObjectTopScale(GetObjHandle(), top);
    if (VarStyle != OS_NOVIEW) Repaint();
  }
}

void QDMapObj::GetBotTopScale(int *bot, int *top)
{
  if (GetObjHandle() == 0) return;
  *bot = mapObjectBotScale(GetObjHandle());
  *top = mapObjectTopScale(GetObjHandle());
}

//-----------------------------------------------------------
// Установить описание объекта используя диалог
// Вход    acreate - надо ли после успешного выполнения диалога сформировать описание
//                QDMapObj (вид объекта, метрика, семантика) Заново или же установить
//                только вид
// Выход = true - вид объекта и карта установлены
//-----------------------------------------------------------
bool QDMapObj::SetObjectByDLg(bool acreate)
{
  QDMapSelectObjectRsc *objDlg;

  objDlg = new QDMapSelectObjectRsc(NULL, "objDlg");

  objDlg->FRestoreEnabled = true;
  objDlg->SetMapView(GetMapView());
  // Выбор вида(классификационного кода из файла rsc) для создаваемого объекта
  bool ret = objDlg->ExecuteForMapObj(this, acreate);
  if (objDlg != NULL) delete objDlg;

  return ret;
}


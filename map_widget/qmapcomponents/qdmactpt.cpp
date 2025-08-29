/******** QDMACTPT.CPP *********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                         FOR QT-DESIGNER                          *
*                                                                  *
********************************************************************
*                                                                  *
*  Компонент      : QDGetMapPoint -  компонент интерактивного      *
*                   выбора точки карты                             *
*                                                                  *
*******************************************************************/

#include <QPainter>
#include <QPen>

#include "qdmactpt.h"
#include "qdmobj.h"
#include "qdmwina.h"

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDGetMapPoint::QDGetMapPoint(QWidget * parent, const char * name)
    :QDGtkAction(parent, name)
{
    VarActive    = 0;
    FMapObj      = NULL;

    FColorDot    = 0x0F0F0F0;
    FLineWidth   = 1;
    FDashLenght  = 5;
    FBlankLenght = 3;

    FImageParm.Image = IMG_DOT;
    FImageParm.Mode  = R2_NOT;
    FImageParm.Parm  = (char *)&FImageObj;
    FImageObj.Color  = FColorDot;
    FImageObj.Thick  = PIX2MKM(FLineWidth);
    FImageObj.Dash   = PIX2MKM(FDashLenght);
    FImageObj.Blank  = PIX2MKM(FBlankLenght);
    FPlaceData.Count      = 1;
    FPlaceData.PolyText   = NULL;
    FPlaceData.Points     = &FPlacePoints[0];
    FPlaceData.PolyCounts = &FPlaceCount;
    FPlaceCount = 2;
    FIsLine = false;

    FTypeGetPoint = GETEXISTPOINT;
    FNumPoint     = 0;
    FNumSubj      = 0;
    hide();
}

//-------------------------------------------
//      Деструктор
//-------------------------------------------
QDGetMapPoint::~QDGetMapPoint()
{
    StopAction();
}

//-------------------------------------------
// Подсоеденить обработчик к компоненту отображения карты
//-------------------------------------------
void QDGetMapPoint::StartAction()
{
    QDMapViewWindow *mapwin1;

    if (VarActive)
      return;

    // объект для которого выбираем точку не указан
    if ((!FMapObj) || (!FMapObj->GetKey()))
      return;

    // узнаем куда идет отображение
    mapwin1 = (QDMapViewWindow *)GetMapViewWindow();
    if (mapwin1 == NULL)
        return;

    FImageObj.Color  = FColorDot;
    FImageObj.Thick  = PIX2MKM(FLineWidth);
    FImageObj.Dash   = PIX2MKM(FDashLenght);
    FImageObj.Blank  = PIX2MKM(FBlankLenght);

    FIsLine = false;

    QDGtkAction::StartAction();
}

void QDGetMapPoint::StopAction()
{
    QDGtkAction::StopAction();
}

//-------------------------------------------
// ОТрисуем резинку
// aMouseX, aMouseY - в системе координат окна
// aPointX, aPointY - в системе координат района в пикселах
//-------------------------------------------
void QDGetMapPoint::PaintDot(QImage * mapImage)
{
    // линия уже нарисована и нам надо ее погасить путем повторного отображения
    if (!VarActive) return;

    // нарисуем на новом месте
    FPlacePoints[0].X = FPointMouse.X;
    FPlacePoints[0].Y = FPointMouse.Y;

    FPlacePoints[1].X = FPointObj.X;
    FPlacePoints[1].Y = FPointObj.Y;

    LinePaint(mapImage);
}

//-------------------------------------------
// надо нарисовать линию
//-------------------------------------------
void QDGetMapPoint::LinePaint(QImage *mapImage)
{
    QDMapViewWindow *mapwin1;

    // узнаем куда идет отображение
    mapwin1 = (QDMapViewWindow *)GetMapViewWindow();
    if (mapwin1 == NULL)
        return;

    QPainter imagePainter( mapImage );
    imagePainter.setCompositionMode( QPainter::RasterOp_SourceXorDestination );
    QPen pen( Qt::DashLine );
    pen.setColor( Qt::white );
    pen.setWidth( 2 );
    imagePainter.setPen( pen );

    int left, top;
    mapwin1->GetMapLeftTop(&left, &top);
    double pointStartX = FPlaceData.Points[0].X;
    double pointStartY = FPlaceData.Points[0].Y;
    double pointEndX = FPlaceData.Points[1].X;
    double pointEndY = FPlaceData.Points[1].Y;
    mapwin1->ConvertMetric(&pointStartX, &pointStartY, PP_PLANE, PP_PICTURE);
    mapwin1->ConvertMetric(&pointEndX, &pointEndY, PP_PLANE, PP_PICTURE);
    pointStartX -= left;
    pointStartY -= top;
    pointEndX -= left;
    pointEndY -= top;

    imagePainter.drawLine( pointStartX, pointStartY, pointEndX, pointEndY );
}

void QDGetMapPoint::MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect)
{
}


void QDGetMapPoint::MapAfterPaint(QObject *sender, QImage * mapImage, RECT mappaintrect)
{
    PaintDot(mapImage);
}

//-------------------------------------------
// Вычислим координату точки объекта
// вход - положение мышки
// выход - расчитанные координаты точки объекта,
// номер ближайшей точки и номер подобъекта
// FPointMouse, FPointObj
//-------------------------------------------
void QDGetMapPoint::CalcCoordsForObjAndMouse(int aMouseX, int aMouseY)
{
    HOBJ hobjsel;
    QDMapViewWindow *mapwin1;
    int left, top;

    if (!VarActive) return;
    // объект для которого выбираем точку не указан
    if ((!FMapObj) || (!FMapObj->GetKey())) return;

    hobjsel = FMapObj->GetObjHandle();

    // узнаем куда идет отображение
    mapwin1 = (QDMapViewWindow *)GetMapViewWindow();
    if (mapwin1 == NULL) return;

    mapwin1->GetMapLeftTop(&left, &top);
    // определим координаты мыши в метрах
    FPointMouse.X = left + aMouseX;
    FPointMouse.Y = top + aMouseY;
    mapwin1->ConvertMetric(&FPointMouse.X, &FPointMouse.Y, PP_PICTURE, PP_PLANE);

    // определим координаты существующей точки
    if (FTypeGetPoint == GETEXISTPOINT)
    {
        // Определение номера ближайшей существующей точки и координаты виртуальной
        FNumPoint = mapSeekNearPoint(hobjsel, &FPointMouse, -1);

        if (!FNumPoint) return;

        // Определение номера подобъекта
        FNumSubj = mapGetCurrentSubject(hobjsel);

        FPointObj.X = mapXPlane(hobjsel, FNumPoint, FNumSubj);
        FPointObj.Y = mapYPlane(hobjsel, FNumPoint, FNumSubj);
    }

    // определим координаты виртуальной
    if (FTypeGetPoint == GETVIRTUALPOINT)
    {
        FNumPoint = mapSeekNearVirtualPointSubject(mapwin1->GetMapHandle(), hobjsel, -1,
                                                   &FPointMouse, &FPointObj);
        if (!FNumPoint) return;

        // Определение номера подобъекта
        FNumSubj = mapGetCurrentSubject(hobjsel);
    }
}

void QDGetMapPoint::MouseUp(int x, int y, int modkeys)
{
    if (VarActive)
    {
        emit SignalSelectPoint(FPointObj.X, FPointObj.Y, FNumPoint, FNumSubj);
    }
}

void QDGetMapPoint::MouseMove(int x, int y, int modkeys)
{
    if (!VarActive) return;
    CalcCoordsForObjAndMouse(x, y);
}

//-------------------------------------------
// Объект для которого выбирается точка
//-------------------------------------------
QDMapObj *QDGetMapPoint::GetMapObj()
{
    return FMapObj;
}

void QDGetMapPoint::SetMapObj(QDMapObj *mapobj)
{
    FMapObj = mapobj;
}

//-------------------------------------------
// Тип точки(TSELECTIONMODE) // 14/11/05
//-------------------------------------------
int QDGetMapPoint::GetTypePoint()
{
    return FTypeGetPoint;
}

void QDGetMapPoint::SetTypePoint(int type)
{
    FTypeGetPoint = (TSELECTIONMODE)type;
}

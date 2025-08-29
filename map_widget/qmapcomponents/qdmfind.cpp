/******* QDMFIND.CPP ***********************************************
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
*  ОПИСАНИЕ КЛАССА "КОМПОНЕНТ ПОИСКА ОБЪЕКТА ЭЛЕКТРОННОЙ КАРТА"    *
*                                                                  *
*******************************************************************/

#include <math.h>
#include "qdmfind.h"
#include "qdmpoina.h"
#include "qdmobj.h"
#include "qdmselt.h"
#include "qdmcmp.h"
#include "qdmvport.h"
#include "mapapi.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ ПОИСКА ОБЪЕКТА ЭЛЕКТРОННОЙ КАРТА"          ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapFind::QDMapFind(QWidget * parent, const char * name)
    :QDMapComponent(parent, name)
{
    VarMapObj    = NULL;
    VarMapView   = NULL;
    VarMapSelect = new QDMapSelect((QDMapView *)VarMapView);
    VarMapPoint  = new QDMapPointAcces((QDMapView *)VarMapView);
    VarRadius    = 0;
    VarFindPoint = false;
    VarActive    = false;
    VarBOF       = true;
    VarEOF       = true;
    hide();
}

//-------------------------------------------
//      Деструктор
//-------------------------------------------
QDMapFind::~QDMapFind()
{
    delete VarMapSelect;
    delete VarMapPoint;
}

QDMapView *QDMapFind::GetMapView() const
{
    return (QDMapView *)VarMapView;
}

void QDMapFind::SetMapView(QDMapView *value)
{
    if ((QDMapView *)VarMapView != value) SetActive(false);
    SetMapViewAcces(value);

    if (VarMapSelect != NULL) VarMapSelect->SetMapView((QDMapView *)VarMapView);
    if (VarMapPoint != NULL) VarMapPoint->SetMapView((QDMapView *)VarMapView);
    if (VarMapView == NULL)
    {
        VarMapObj = NULL;
        SetActive(false);
    }
}

QDMapPointAcces *QDMapFind::GetPoint()
{
    if (VarActive && VarFindPoint) return VarMapPoint;
    VarMapPoint->VarX = VarFrame.X1 + VarRadius;
    VarMapPoint->VarY = VarFrame.Y1 + VarRadius;
    return VarMapPoint;
}

double QDMapFind::GetRadius()
{
    double RX, RY, ret, x, y;

    VarMapPoint->GetPoint(&RX, &RY);
    VarMapPoint->VarY = VarMapPoint->VarY + VarRadius;
    VarMapPoint->GetPoint(&x, &y);
    ret = sqrt((RX - x)*(RX - x) + (RY - y)*(RY - y));
    VarMapPoint->VarY = VarMapPoint->VarY-VarRadius;
    return ret;
}

int QDMapFind::GetPlaceIn()
{
    return VarMapPoint->GetPlaceIn();
}

int QDMapFind::GetPlaceOut()
{
    return VarMapPoint->GetPlaceOut();
}

void QDMapFind::GetCoordPoint(double *x, double *y)
{
    VarMapPoint->GetPoint(x, y);
}

void QDMapFind::SetPlaceIn(int value)
{
    VarMapPoint->SetPlaceIn(value);
}

void QDMapFind::SetPlaceOut(int value)
{
    VarMapPoint->SetPlaceOut(value);
}

void QDMapFind::SetCoordPoint(double x, double y)
{
    if (VarActive) return;
    VarMapPoint->SetPoint(x, y);
}

//-------------------------------------------------------------------
// Включить/выключить режим поиска в заданной точке
//-------------------------------------------------------------------
void QDMapFind::SetFindPoint(bool value)
{
    if (VarActive) return;
    if (value == VarFindPoint) return;
    VarFindPoint = value;
}

bool QDMapFind::GetFindPoint()
{
    return VarFindPoint;
}

void QDMapFind::SetRadius(double value)
{
    double DX,DY;
    int    OldPlace;

    if (VarActive) return;
    DX = VarMapPoint->VarX;
    DY = VarMapPoint->VarY;
    OldPlace = VarMapPoint->VarPlaceOut;
    VarMapPoint->VarPlaceOut = VarMapPoint->VarPlaceIn;
    VarMapPoint->SetY(VarMapPoint->GetY() + value);
    VarMapPoint->VarPlaceOut = OldPlace;
    VarRadius = sqrt((DX - VarMapPoint->VarX)*(DX - VarMapPoint->VarX)
                     + (DY - VarMapPoint->VarY)*(DY - VarMapPoint->VarY));
    VarMapPoint->VarX = DX;
    VarMapPoint->VarY = DY;
}

//-------------------------------------------------------------------
// Указатель на объект карты
//-------------------------------------------------------------------
void QDMapFind::SetMapObj(QDMapObj *value)
{
    int  i;
    bool OldActive;

    if (VarMapObj == value) return;
    if ((VarMapView == NULL) && (value != NULL)) return;
    OldActive = VarActive;

    SetActive(false);
    if (value != NULL)
    {
        if (value->VarMapView == NULL)
          value->SetMapView((QDMapView *)VarMapView);
        if (value->VarMapView != VarMapView)
          return;
        // проверим что Obj не присоеденен к другому MapFind
    }

    VarMapObj = value;
    SetActive(OldActive);
}

QDMapObj *QDMapFind::GetMapObj() const
{
    return VarMapObj;
}

//-------------------------------------------------------------------
// Внутренняя функция поиска объекта в точке
//-------------------------------------------------------------------
HOBJ QDMapFind::SeekInPoint(HOBJ obj, int flag)
{
    int     aflag, ii, iall;
    HSELECT aSel;
    HOBJ    NewObj = 0;
    HSITE   asite;

    aflag  =  0;

    // Будем искать по каждой карте в отдельности
    // те для каждой карты свой Select
    if ((flag & WO_INMAP) == 0)
      aflag  =  WO_INMAP; // ?????????

    // Учтем что ищем в точке сверху вниз
    if (WO_FIRST == flag)
      aflag  =  WO_LAST  + aflag;
    else if ((WO_NEXT & flag) > 0)
      aflag  =  WO_BACK  + aflag;
    else  if ((WO_LAST & flag) > 0)
      aflag  =  WO_FIRST + aflag;
    else if ((WO_BACK & flag) > 0)
      aflag  =  WO_NEXT  + aflag;

    iall = ((QDMapView *)VarMapView)->VarMapSites->GetSiteCount();
    // Если ищем от объекта то определим с какой он карты
    if (((WO_BACK  & flag) > 0) || ((WO_NEXT & flag) > 0))
    {
        asite = mapGetObjectSiteIdent(VarMapView->GetMapHandle(), obj);
        ii    = mapGetSiteNumber(VarMapView->GetMapHandle(), asite);
        if (ii <= iall)  iall = ii;
    }

    for (ii = iall; ii>= 0; ii--)
    { // цикл по картам с конца
        if (GetMapSelect()->GetMapSites(ii) == FALSE)
          continue; // данная карта не используется
        aSel = GetMapSelect()->GetHSelect(ii);
        if (aSel == 0)
          continue;
        NewObj = mapWhatObjectBySelect(VarMapView->GetMapHandle(), obj, &VarFrame,
                                       aSel, aflag, PP_PLANE);
        if (NewObj != 0)  // нашли
        {
            return NewObj;
        }
        if (ii < iall) // когда не нашли первый раз
        {
            if ((WO_BACK & aflag) > 0)
              aflag = WO_FIRST + WO_INMAP;
            if ((WO_NEXT & aflag) > 0)
              aflag = WO_LAST  + WO_INMAP;
        }
    } // цикл по картам

    return NewObj;
}

//-------------------------------------------------------------------
// Внутренняя функция поиска объекта в карте
//-------------------------------------------------------------------
HOBJ QDMapFind::SeekInMap(HOBJ obj, int flag)
{
    int     ii, iall;
    HSELECT asel;
    HOBJ    NewObj = 0;
    HSITE   asite;

    // будем искать по каждой карте в отдельности
    // те для каждой карты свой Select
    if ((flag & WO_INMAP) == 0) flag  =  flag + WO_INMAP;

    iall = ((QDMapView *)VarMapView)->VarMapSites->GetSiteCount();
    if ((WO_NEXT  & flag) > 0) ii = 0;
    else if (((WO_BACK  & flag) > 0) || ((WO_LAST & flag) > 0)) ii = iall;
    else ii = 0; // WO_FIRST

    // если ищем от объекта то определим с какой он карты
    if (((WO_BACK  & flag) > 0) || ((WO_NEXT & flag) > 0))
    {
        asite = mapGetObjectSiteIdent(VarMapView->GetMapHandle(), obj);
        ii    = mapGetSiteNumber(VarMapView->GetMapHandle(), asite);
    }

    // Если ищем предыдущий или последний
    if (((WO_BACK  & flag) > 0) || ((WO_LAST & flag) > 0))
    {
        for (ii = ii; ii >= 0; ii--)
        { // цикл по картам
            if (GetMapSelect()->GetMapSites(ii) == FALSE) continue; // данная карта не используется
            asel = GetMapSelect()->GetHSelect(ii);
            if (asel == 0) continue;
            NewObj = mapSeekSiteSelectObject(VarMapView->GetMapHandle(),
                                             mapGetSiteIdent(VarMapView->GetMapHandle(), ii),
                                             obj, asel, flag);
            if (NewObj != 0) return NewObj;// нашли
            // когда не нашли в этой карте
            flag = WO_LAST + WO_INMAP;
        }  // цикл по картам
    }
    else
    {  // Если ищем следующий или первый
        for (ii = ii; ii <= iall; ii++)
        {  // цикл по картам
            if (GetMapSelect()->GetMapSites(ii) == FALSE) continue; // данная карта не используется
            asel = GetMapSelect()->GetHSelect(ii);
            if (asel == 0) continue;
            NewObj = mapSeekSiteSelectObject(VarMapView->GetMapHandle(),
                                             mapGetSiteIdent(VarMapView->GetMapHandle(), ii),
                                             obj, asel, flag);
            if (NewObj != 0) return NewObj;// нашли
            // когда не нашли в этой карте
            flag = WO_FIRST  + WO_INMAP;
        } // цикл по картам
    }

    return NewObj;
}

bool QDMapFind::GetActive() const
{
  return VarActive;
}

void QDMapFind::SetActive(bool value)
{
    if (VarActive == value) return;

    if (value)
    {
        if (VarMapView == NULL) return;
        if (VarMapObj == NULL) return;
        if (VarMapView->GetMapHandle() == 0) return;
        if (VarMapObj->VarObjHandle == 0) return;
        if (VarFindPoint)
        {
            VarFrame.X1 = VarMapPoint->VarX - VarRadius;
            VarFrame.X2 = VarMapPoint->VarX + VarRadius;
            VarFrame.Y1 = VarMapPoint->VarY - VarRadius;
            VarFrame.Y2 = VarMapPoint->VarY + VarRadius;
        }
    }

    VarActive = value;
    // First();
}

bool QDMapFind::First()
{
    HOBJ AObj;
    bool ret = false;

    VarEOF = true;
    VarBOF = true;

    if (!VarActive) return ret;

    AObj = mapCreateCopyObject(VarMapView->GetMapHandle(), VarMapObj->VarObjHandle);
    if (AObj == 0) return ret;

    if (VarFindPoint) // если поиск в точке
        VarEOF  =  (SeekInPoint(AObj,WO_FIRST) == 0);
    else VarEOF  =  (SeekInMap(AObj,WO_FIRST) == 0);

    if (VarEOF) VarMapObj->SetObjHandle(0);
    else
    {
        VarMapObj->SetObjHandle(AObj);
        ret = true;
    }

    if (AObj != 0) mapFreeObject(AObj);

    return ret;    // 18/02/03
}

bool QDMapFind::Prior()
{
    HOBJ AObj;
    bool ret = false;

    if (!VarActive) return ret;
    if (VarBOF) return ret;

    AObj = mapCreateCopyObject(VarMapView->GetMapHandle(), VarMapObj->VarObjHandle);
    if (AObj == 0) return ret;

    if (VarFindPoint) // если поиск в точке
        VarBOF  =  (SeekInPoint(AObj,WO_BACK) == 0);
    else VarBOF  =  (SeekInMap(AObj,WO_BACK) == 0);

    if (VarBOF == false)
    {
        VarMapObj->SetObjHandle(AObj);
        VarEOF  =  false;
        ret = true;
    }

    if (AObj  !=  0) mapFreeObject(AObj);

    return ret;
}


bool QDMapFind::Next()
{
    HOBJ AObj;
    bool ret = false;

    if (!VarActive) return ret;
    if (VarEOF) return ret;

    AObj = mapCreateCopyObject(VarMapView->GetMapHandle(), VarMapObj->VarObjHandle);
    if (AObj == 0) return ret;

    if (VarFindPoint) // если поиск в точке
        VarEOF  =  (SeekInPoint(AObj,WO_NEXT) == 0);
    else (VarEOF  =  SeekInMap(AObj,WO_NEXT) == 0);

    if (VarEOF == false)
    {
        VarMapObj->SetObjHandle(AObj);
        VarBOF  =  false;
        ret = true;
    }

    if (AObj != 0) mapFreeObject(AObj);

    return ret;
}

bool QDMapFind::Last()
{
    HOBJ AObj;
    bool ret = false;

    if (!VarActive) return ret;

    VarBOF  =  true;
    VarEOF  =  true;

    AObj = mapCreateCopyObject(VarMapView->GetMapHandle(), VarMapObj->VarObjHandle);
    if (AObj == 0) return ret;

    if (VarFindPoint) // если поиск в точке
        VarBOF  =  (SeekInPoint(AObj,WO_LAST) == 0);
    else VarBOF  =  (SeekInMap(AObj,WO_LAST) == 0);

    if (VarBOF) VarMapObj->SetObjHandle(0);
    else
    {
        VarMapObj->SetObjHandle(AObj);
        ret = true;
    }

    if (AObj != 0) mapFreeObject(AObj);

    return ret;
}

void QDMapFind::Center()
{
    MAPDFRAME      Frame;
    QDMapPointAcces *mapPoint;
    double         DX,DY;

    if (!VarActive) return;
    if (VarEOF && VarBOF) return;

    if (mapObjectFrame(VarMapObj->VarObjHandle, &Frame) == 0) return;

    DX = (Frame.X1+Frame.X2)/2;
    DY = (Frame.Y1+Frame.Y2)/2;
    mapPoint = new QDMapPointAcces(VarMapView);
    mapPoint->SetPlaceIn(PP_PLANE);
    mapPoint->SetPlaceOut(PP_PICTURE);
    mapPoint->SetPoint(DX, DY);

    mapPoint->GetPoint(&DX, &DY);
    ((QDMapViewWindow *)VarMapView)->HScrollBar->setValue(DX -
                                                          (((QDMapViewWindow *)VarMapView)->MapViewPort->width() / 2));
    ((QDMapViewWindow *)VarMapView)->VScrollBar->setValue(DY -
                                                          (((QDMapViewWindow *)VarMapView)->MapViewPort->height() / 2));
    delete mapPoint;
}

void QDMapFind::MapAction(int acttype)
{
    switch (acttype)
    {
        case ACTBEFORECLOSE:
            VarMapSelect->MapAction(acttype);
            SetActive(false);
            break;
        case ACTAFTEROPEN:
            VarMapSelect->MapAction(acttype);
            break;
        case ACTAFTERAPPENDSITE:
            VarMapSelect->MapAction(acttype);
            break;
        case ACTBEFOREDELETESITE:
            VarMapSelect->MapAction(acttype);
            break;
    }
}

//-------------------------------------------------------------------
// Определить - находится ли система поиска на первом элементе
//-------------------------------------------------------------------
bool QDMapFind::GetBOF()
{
    return VarBOF;
}

//-------------------------------------------------------------------
// Определить - вышла ли система за последний элемент
//-------------------------------------------------------------------
bool QDMapFind::GetEOF()
{
    return VarEOF;
}

//-------------------------------------------------------------------
// Запросить HSELECT
//-------------------------------------------------------------------
HSELECT QDMapFind::GetMapSelectHandle()
{
    return VarMapSelect->GetHSelect(0);
}

//-------------------------------------------------------------------
// Установить/запросить условия поиска
//-------------------------------------------------------------------
void QDMapFind::SetMapSelect(QDMapSelect *value)
{
    if (value == VarMapSelect)
      return;
    VarMapSelect->Assign(value);
}

QDMapSelect * QDMapFind::GetMapSelect()
{
    return VarMapSelect;
}

void QDMapFind::Notification(QObject *component, int operation)
{
    if (((long int)component == (long int)VarMapObj) && (operation == OPREMOVE))
    {
        SetMapObj(NULL);
        SetActive(false);
    }
    QDMapComponent::Notification(component, operation);
}

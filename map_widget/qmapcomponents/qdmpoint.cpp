/******* QDMVPOINT.H ***********************************************
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
*     �������� ������ "��������� - ����� ����������� �����"        *
*                                                                  *
*******************************************************************/

#include "qdmpoint.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ ����� "��������� - ����� ����������� �����"                 ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-------------------------------------------
//      �����������
//-------------------------------------------
QDMapPoint::QDMapPoint(QWidget *parent, const char *name)
           : QDMapComponent(parent, name)
{
   VarMapPoint = new QDMapPointAcces(VarMapView);
   ParentHandle = parent;
   hide();
}

QDMapPoint::~QDMapPoint()
{
  delete VarMapPoint;
}

QDMapView *QDMapPoint::GetMapView() const
{
 return (QDMapView *)VarMapView;
}

void QDMapPoint::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
  VarMapPoint->VarMapView = VarMapView;
}

int QDMapPoint::GetPlaceIn() const
{
  return VarMapPoint->GetPlaceIn();
}

int QDMapPoint::GetPlaceOut() const
{
  return VarMapPoint->GetPlaceOut();
}

void QDMapPoint::SetPlaceIn(int value)
{
  VarMapPoint->SetPlaceIn(value);
}

void QDMapPoint::SetPlaceOut(int value)
{
  VarMapPoint->SetPlaceOut(value);
}

//-------------------------------------------------------------------
// �������� ���������� � ������� PlaceIn
// ����� = 1 - �������
//-------------------------------------------------------------------
bool QDMapPoint::SetPoint(double x, double y)
{
  return VarMapPoint->SetPoint(x, y);
}

//-------------------------------------------------------------------
// �������  ���������� � ������� PlaceOut
// ����� = 1 - �������
//-------------------------------------------------------------------
bool QDMapPoint::GetPoint(double *x, double *y)
{
  return VarMapPoint->GetPoint(x, y);
}

void QDMapPoint::MapAction(int typeaction)
{
  int placein;

  switch (typeaction)
  {
    case ACTAFTEROPEN :
      placein = VarMapPoint->VarPlaceIn;
      VarMapPoint->VarPlaceIn = PP_PICTURE;
      VarMapPoint->SetX(0);
      VarMapPoint->SetY(0);
      VarMapPoint->VarPlaceIn = placein;
    break;
    case ACTBEFORECLOSE :
      VarMapPoint->VarX = 0;
      VarMapPoint->VarY = 0;
    break;
  }
}

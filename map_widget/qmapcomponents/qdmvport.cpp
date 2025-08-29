/******* QDMVPORT.CPP **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR QT-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*        �������� ������ ������� � �������� �����������            *
*                     ����������� �����                            *
*                                                                  *
*******************************************************************/

#include "qdmvport.h"
#include "mapapi.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ ����� "��������� ����������� ����������� ������" +++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QDMapViewPort::QDMapViewPort(QWidget *parent)
  :QWidget(parent)
{
  setMouseTracking(true);
  MapViewWindow = NULL;
}

QDMapViewPort::~QDMapViewPort()
{
}

void QDMapViewPort::SetMapViewWindow(QDMapViewWindow *mw)
{
  MapViewWindow = mw;
}

//------------------------------------------------------------------
//  ��������� ������� Qt :
//  ��������� ����
//------------------------------------------------------------------
void QDMapViewPort::paintEvent(QPaintEvent * e)
{
  QRect redrawRect = e->rect();
  int ch = redrawRect.bottom() - redrawRect.top() + 1;
  int cw = redrawRect.right() - redrawRect.left() + 1;
  int cy = redrawRect.top();
  int cx = redrawRect.left();

  if (MapViewWindow)
  {
    QPainter p(this);
    MapViewWindow->drawContents(&p, cx, cy, cw, ch);
  }
}


//------------------------------------------------------------------
//  ��������� ������� Qt :
//  ���������� ���������
//  ����������� ����
//------------------------------------------------------------------
void QDMapViewPort::mouseMoveEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMouseMoveEvent(e);
}

//------------------------------------------------------------------
//  ��������� ������� Qt :
//  ���������� ��������� ������� ������ ����
//------------------------------------------------------------------
void QDMapViewPort::mousePressEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMousePressEvent(e);
}

//------------------------------------------------------------------
//  ��������� ������� Qt :
//  ���������� ������� ������ ����
//------------------------------------------------------------------
void QDMapViewPort::mouseReleaseEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMouseReleaseEvent(e);
}

//------------------------------------------------------------------
//  ��������� ������� Qt :
//  ���������� ������� ������� ������ ����
//------------------------------------------------------------------
void QDMapViewPort::mouseDoubleClickEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMouseDoubleClickEvent(e);
}

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
*        ÎÏÈÑÀÍÈÅ ÊËÀÑÑÀ ÄÎÑÒÓÏÀ Ê ÔÓÍÊÖÈßÌ ÎÒÎÁÐÀÆÅÍÈß            *
*                     ÝËÅÊÒÐÎÍÍÎÉ ÊÀÐÒÛ                            *
*                                                                  *
*******************************************************************/

#include "qdmvport.h"
#include "mapapi.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ ÊËÀÑÑ "ÈÍÒÅÐÔÅÉÑ ÎÒÎÁÐÀÆÅÍÈß ÝËÅÊÒÐÎÍÍÎÉ ÊÀÐÒÎÉ" +++
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
//  Ñèñòåìíàÿ ôóíêöèÿ Qt :
//  Îòðèñîâêà îêíà
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
//  Ñèñòåìíàÿ ôóíêöèÿ Qt :
//  Îáðàáîòàòü ñîîáùåíèå
//  Ïåðåìåùåíèå ìûøè
//------------------------------------------------------------------
void QDMapViewPort::mouseMoveEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMouseMoveEvent(e);
}

//------------------------------------------------------------------
//  Ñèñòåìíàÿ ôóíêöèÿ Qt :
//  Îáðàáîòàòü ñîîáùåíèå íàæàòèå êíîïêè ìûøè
//------------------------------------------------------------------
void QDMapViewPort::mousePressEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMousePressEvent(e);
}

//------------------------------------------------------------------
//  Ñèñòåìíàÿ ôóíêöèÿ Qt :
//  Îáðàáîòàòü îòæàòèå êíîïêè ìûøè
//------------------------------------------------------------------
void QDMapViewPort::mouseReleaseEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMouseReleaseEvent(e);
}

//------------------------------------------------------------------
//  Ñèñòåìíàÿ ôóíêöèÿ Qt :
//  Îáðàáîòàòü äâîéíîå íàæàòèå êíîïêè ìûøè
//------------------------------------------------------------------
void QDMapViewPort::mouseDoubleClickEvent(QMouseEvent * e)
{
  if (MapViewWindow)
    MapViewWindow->viewportMouseDoubleClickEvent(e);
}

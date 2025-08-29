/******** QDMDWMSPOPUP.H *******************************************
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
*             КОМПОНЕНТ - ДИАЛОГ УПРАВЛЕНИЯ СОСТАВОМ               *
*                       WMS-СЕРВИСОВ                               *
*                                                                  *
*******************************************************************/

#ifndef QDMDWMSPOPUP_H
#define QDMDWMSPOPUP_H

#include "qdmcmp.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КОМПОНЕНТ - ДИАЛОГ УПРАВЛЕНИЯ СОСТАВОМ       +
//+++               WMS-СЕРВИСОВ                   +
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapWMSPopupDialog : public QDMapComponent
{
    Q_OBJECT

public:
  QDMapWMSPopupDialog(QWidget *parent = 0, const char *name = 0);
  ~QDMapWMSPopupDialog();
  QDMapView *GetMapView() const;

  void SetMapView(QDMapView *value);

  // Выполнить диалог
  void Execute();

  // Метод - обработчик сообщений посылаемых диалогом окну документа
  void MapAction(int acttype);

public:
  DOUBLEPOINT FMapCenter;
  int IsGeo;
};

#endif // QDMDWMSPOPUP_H

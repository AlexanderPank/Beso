/******** QDMCOMPA.H ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2020              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR QT DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*          ОПИСАНИЕ БАЗОВОГО КЛАССА КОМПОНЕНТА ДОСТУПА К           *
*                     ЭЛЕКТРОННОЙ КАРТЕ                            *
*                                                                  *
*******************************************************************/

#ifndef QDMCOMPA_H
#define QDMCOMPA_H

#include <QtGlobal>
#include <QWidget>

#include "qdmviewa.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// БАЗОВЫЙ КЛАСС КОМПОНЕНТ ДОСТУПА К ЭЛЕКТРОННОЙ КАРТЕ
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class QDMapComponent : public QWidget
{
  Q_OBJECT

public:
  QDMapComponent(QWidget *parent = NULL, const char *name = NULL);
  ~QDMapComponent();

  void SetMapViewAcces(QDMapViewAcces *value);
  void SetMapWindowAcces(QDMapViewAcces *value);  // установка VarMapWindow (для служебного пользования)
  // Получить объект типа QDMapViewWindow в котором отображается карта (QDMapView or QDMapWindow)
  QDMapViewAcces *GetMapViewWindow();

  // Обработка событий изменения состава карт на уровне MapViewWindow
  virtual void MapAction(int) {}
  virtual void Notification(QObject *component, int operation);

public:
  QDMapViewAcces *VarMapView;  // (для пользователя только на чтение)
  // Объект типа TMapWindow к которому подключен данный компонент и в который идет отображение
  QDMapViewAcces *VarMapWindow;  // (для служебного пользования)
};

#endif  // QDMCOMPA_H

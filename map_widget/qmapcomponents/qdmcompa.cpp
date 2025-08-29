/*******  QDMCOMPA.H ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR QT-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*     ОПИСАНИЕ БАЗОВОГО КЛАССА КОМПОНЕНТ ДОСТУПА К                 *
*                     ЭЛЕКТРОННОЙ КАРТЫ                            *
*                                                                  *
*******************************************************************/

#include "qdmcompa.h"
#include "qdmcmp.h"
#include "qdmwinw.h"

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapComponent::QDMapComponent(QWidget *parent, const char *name )
               : QWidget( parent )
{
  VarMapView   = NULL;
  VarMapWindow = NULL;
}

QDMapComponent::~QDMapComponent()
{
  SetMapViewAcces(NULL);
}

void QDMapComponent::SetMapViewAcces(QDMapViewAcces *value)
{
  if (VarMapView == value)
    return;

  if (VarMapView)
    ((QDMapView *)VarMapView)->DeleteMapObject(this);
  VarMapView = NULL;

  if (value)
    ((QDMapView *)value)->AppendMapObject(this);
  VarMapView = value;
}

//--------------------------------------------------------------------
// Установить вывод в окно связанное с value дб типа QDMapWindow
//--------------------------------------------------------------------
void QDMapComponent::SetMapWindowAcces(QDMapViewAcces *value)
{
  if (VarMapWindow == value)
    return;

  if (VarMapWindow)
    ((QDMapWindow *)VarMapWindow)->DeleteMapObject(this);
  VarMapWindow = NULL;

  if (value && (strcmp(value->GetClassName(), "QDMapWindow") == 0))
    ((QDMapWindow *)value)->AppendMapObject(this);
  VarMapWindow = value;
}

//--------------------------------------------------------------------
// Получить объект типа QDMapViewWindow, который
// реально управляет отображением
//--------------------------------------------------------------------
QDMapViewAcces *QDMapComponent::GetMapViewWindow()
{
 if (VarMapWindow != NULL)
   return VarMapWindow;
 else
   return VarMapView;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void QDMapComponent::Notification(QObject *component, int operation)
{
  if ((operation == OPREMOVE) && ((long int)VarMapView == (long int)component))
     SetMapViewAcces(NULL);
}

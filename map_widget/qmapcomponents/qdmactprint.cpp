/******** QDMACTPRN.CPP ********************************************
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
*                      ОБРАБОТЧИК ПЕЧАТИ                           *
*                                                                  *
*******************************************************************/

#include <QPrinter>

#include "maplib.h"
#include "prnapi.h"
#include "struct.h"

#include "qdmactprint.h"
#include "qdmcmp.h"
#include "qdmfind.h"
#include "qdmobj.h"

// Главный обработчик сообщений
long int MessageHandler(long int context, long int code,
                        long int wp, long int lp, long int typemsg = 0);

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  ОБРАБОТЧИК ПЕЧАТИ
//++++++++++++++++++++++++++++++++++++++++++++++++++

QDMapPrintAction::QDMapPrintAction(QWidget *parent, const char *name) :
  QDMapSelectRectTurn(parent, name)
{
  memset(&PrintParam, 0, sizeof(PrintParam));
  PrintParam.Length = sizeof(PrintParam);
  PrintParam.FieldLTmm = 20;
  PrintParam.FieldUPmm = 20;
  PrintParam.FieldRTmm = 10;
  PrintParam.FieldDNmm = 10;
  PrintParam.Restore = 1;      // флаг восстановления параметров печати из INI-файла
  PrintParam.Scale = PrintScale;
  PrintParam.ScaleSave = PrintScale;

  PlaneFrame = PrintParam.PlaneFrame;
  Regime = PrintParam.Regime;

  // Компоненты для выбора объекта на карте
  MapObject = new QDMapObj(parent, name);
  MapObject->SetStyle(QDMapObj::OS_NORMAL);

  MapFind = new QDMapFind(parent, name);
  MapFind->SetMapObj(MapObject);
  MapFind->SetFindPoint(true);
  MapFind->SetRadius(3);
  MapFind->SetActive(false);
}

void QDMapPrintAction::SetMapView(QDMapView *value)
{
  QDMapSelectRectTurn::SetMapView(value);

  if (value)
  {
    MapFind->SetMapView(value);
    MapObject->SetMapView(value);
  }
}

// --------------------------------------------------------------
// Реакция на отпуск левой кнопки мыши
// --------------------------------------------------------------
void QDMapPrintAction::MouseUp(int x, int y, int)
{
  if (Regime != MPR_SELECTBYOBJECT)
    return QDMapSelectRectTurn::MouseUp(x, y, 0);

  QDMapView *mapView = GetMapView();
  if (!mapView)
    return;

  int leftTopX = 0;
  int leftTopY = 0;
  mapView->GetMapLeftTop(&leftTopX, &leftTopY);  // TODO

  // Выбрать объект
  MapFind->SetCoordPoint((double)(leftTopX + x), (double)(leftTopY + y));
  MapFind->SetActive(true);
  MapFind->First();
}

// -----------------------------------------------------------------
// Вызвать диалог печати
// Возвращает режим работы обработчика (enum MAPPRINT_REGIME)
// При необходимости остановить обработчик возвращает ноль
// -----------------------------------------------------------------
int QDMapPrintAction::CallDialog()
{
  QDMapView *mapView = GetMapView();
  if (!mapView)
    return 0;

  HMAP mapHandle = mapView->GetMapHandle();
  if (!mapHandle)
    return 0;

  TASKPARM taskparm;
  memset((char *)&taskparm, 0, sizeof(taskparm));
  taskparm.Handle = MessageHandler;
  taskparm.Resource = (HINSTANCE)mapView;

  PrintParam.Handle = (HWND)mapView;
  PrintParam.PlaneFrame = PlaneFrame;
  PrintParam.Angle = Angle;
  if (PrintParam.Angle != 0.)
    PrintParam.TurnFrame = 1;

  // Установить масштаб печати
  if (PrintParam.Scale <= 0)
    PrintParam.Scale = mapGetMapScale(mapHandle);
  PrintParam.ScaleSave = PrintParam.Scale;

  HINSTANCE libinstPrint = 0;
  long int(WINAPI * prnPrintMapDialog)(HMAP, HPRINTER, MAPPRINTPARMEX *, TASKPARM *, const char*);

  prnPrintMapDialog =
      (long int(WINAPI *)(HMAP, HPRINTER, MAPPRINTPARMEX *, TASKPARM *, const char*))mapLoadLibrary(
              MAPPRINTLIB, &libinstPrint, "prnMapPrintEx");

  // Выполнить загрузку данных
  long int printDialogResult = 0;
  if (prnPrintMapDialog)
    printDialogResult = (*prnPrintMapDialog)(mapHandle, 0, &PrintParam, &taskparm, 0);

  // Выгрузка библиотеки
  if (libinstPrint)
    mapFreeLibrary(libinstPrint);

  QRectF pageRect;
  switch (printDialogResult)
  {
    case ACTION_RUN_NEEDED:  // запустить обработчик
    {
      if (PrintParam.Regime == MPR_SELECTFRAME || PrintParam.Regime == MPR_SELECTBYOBJECT ||
          PrintParam.Regime == MPR_SELECTTURNFRAME)
      {
        SelectedPoints = 0;
        PrintParam.Angle = 0;
        PrintParam.TurnFrame = 0;
      }

      // Передать обработчику размер рамки
      if (PrintParam.Regime == MPR_CHANGEFRAME)
        PlaneFrame = PrintParam.PlaneFrame;

      return PrintParam.Regime;
    }
    case CHOSED_PRINT_TO_FILE:
    case CHOSED_PRINT_TO_DEVICE:
    default:
      break;
  }

  return 0;
}

void QDMapPrintAction::SetRegime(char regime)
{
  PrintParam.Regime = regime;
}

/******** QDMACTPRN.H **********************************************
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

#ifndef QDMACTPRINT_H
#define QDMACTPRINT_H

#include "prnapi.h"
#include "struct.h"

#include "qdmrect.h"

class QDMapFind;
class QDMapObj;
class QDMapPrint;

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  ОБРАБОТЧИК ПЕЧАТИ
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapPrintAction : public QDMapSelectRectTurn
{
  Q_OBJECT

  public:
  enum PRINT_DIALOG_RESULT {
    DIALOG_EXITED = 0,
    CHOSED_PRINT_TO_FILE = 1,
    CHOSED_PRINT_TO_DEVICE = 2,
    ACTION_RUN_NEEDED = 3
  };

  QDMapPrintAction(QWidget *parent = NULL, const char *name = NULL);

  virtual void SetMapView(QDMapView *value);

  // Реакция на отпуск левой кнопки мыши
  virtual void MouseUp(int x, int y, int );

  // Вызвать диалог печати
  // Возвращает режим работы обработчика (enum MAPPRINT_REGIME)
  // При необходимости остановить обработчик возвращает ноль
  int CallDialog();

  void SetRegime(char regime);

  private:
  MAPPRINTPARMEX PrintParam;

  QDMapFind *MapFind;    // компоненты для выбора объекта на карте
  QDMapObj *MapObject;
};

#endif // QDMACTPRINT_H

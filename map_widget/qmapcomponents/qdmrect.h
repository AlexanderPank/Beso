/******** QDMRECT.H ************************************************
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
*                КЛАСС ВЫБОРА УЧАСТКА КАРТЫ                        *
*                                                                  *
*******************************************************************/

#ifndef QDMRECT_H
#define QDMRECT_H

#include "qdmact.h"
#include "mapapi.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КЛАСС ВЫБОРА УЧАСТКА КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSelectRect : public QDGtkAction
{
  Q_OBJECT

public:
  typedef struct BORDER
  {
    DOUBLEPOINT UpLeft;      // Координаты первой точки (Map)
    DOUBLEPOINT UpRight;     // Координаты второй точки
    DOUBLEPOINT DownRight;   // Координаты третьей точки
    DOUBLEPOINT DownLeft;    // Координаты четвертой точки
    DOUBLEPOINT UpLeftLast;  // Координаты последней точки (Map)
  }
  BORDER;

  QDMapSelectRect(QWidget * parent = NULL, const char * name = NULL);
  ~QDMapSelectRect();

  // Запуск/останов обработчика
  virtual void StopAction();

  // Отрисовать обработчик до/после вывода карты в буфер
  virtual void MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect);

  // Запрос/установка системы координат
  void SetPlaceRect(int pp);
  int GetPlaceRect();

  // Обработка событий
  virtual void MouseMove(int x, int y, int );
  virtual void MouseUp(int x, int y, int );

  //  Построить контур фрагмента по угловым точкам
  void BuildFrame();

  int GetSelRect(MAPDFRAME *dframe, int pp);

  // Преобразовать рамку в пикселы клиентского окна
  // При ошибке возвращает ноль
  int ConvertBorderToClient(BORDER &border);

signals:
  // Сигнал об окончании построения рамки
  // параметры - координаты верхнего левого и
  // правого нижнего угла рамки
  void SignalAfterBuildRect(double left, double top, double right, double bottom);

public:
  int Regime;      // текущий режим работы обработчика (enum MAPPRINT_REGIME)

  BORDER Border;

  DOUBLEPOINT First;    // Координаты первого угла (Map)
  DOUBLEPOINT Second;   // Координаты второго угла (Map)

  int SelectedPoints;

  int VarPlace;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КЛАСС ИЗМЕНЕНИЯ ВЫБРАННОГО УЧАСТКА КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSelectRectAndChange : public QDMapSelectRect
{
  Q_OBJECT

  public:
  QDMapSelectRectAndChange(QWidget *parent = NULL, const char *name = NULL);

  // Подключить обработчик
  virtual void StartAction();

  // Вызвать диалог
  virtual int CallDialog() { return 0; }

  // Установить фрагмент
  virtual void SetFrame(DOUBLEPOINT* first, DOUBLEPOINT* second);

  // Реакция на нажатие левой кнопки мыши
  virtual void MouseDown(int x, int y, int );

  // Реакция на отпуск левой кнопки мыши
  virtual void MouseUp(int x, int y, int );

  // Перемещение мыши
  virtual void MouseMove(int x, int y, int);

  // Сообщение о завершении работы
  // При ошибке возвращает ноль
  virtual int Commit();

  // Перемещение рамки по окну
  void MoveFrame();

  public:
  DOUBLEPOINT BeginPos;  // точка начальной позиции рамки при перемещении

  int ChangeFlag;  // флаг режима работы обработчика:
                   // 0 - режим выбора фрагмента
                   // 1 - режим изменения границ выбранной области
  int ChangePos;

  int ChangeUp;
  int ChangeDown;
  int ChangeRight;
  int ChangeLeft;

  DFRAME PlaneFrame;  // выбранная область в метрах
};

//++++++++++++++++++++++++++++++++++++++++++++++++++
//  КЛАСС ВЫБОРА НАКЛОННОГО УЧАСТКА КАРТЫ
//++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapSelectRectTurn: public QDMapSelectRectAndChange
{
  Q_OBJECT

  public:
  QDMapSelectRectTurn(QWidget *parent = NULL, const char *name = NULL);

  // Подключить обработчик
  virtual void StartAction();

  // Отрисовать обработчик до/после вывода карты в буфер
  virtual void MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect);

  // Реакция на отпуск левой кнопки мыши
  virtual void MouseUp(int x, int y, int );

  // Перемещение мыши
  virtual void MouseMove(int x, int y, int);

  // Построить контур фрагмента по угловым точкам
  virtual void BuildFrame();

  public:
  DOUBLEPOINT Third;  // третья точка
  double Angle;       // угол поворота фрагмента (в радианах)

  double PageWidth;  // размер страницы в миллиметрах
  double PageHeight;
  double PrintScale;  // масштаб печати

  DOUBLEPOINT PageLine[2];  // описание метрики линии сетки
};

#endif

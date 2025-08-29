/******* QDMWINA.H *************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR QT DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*        ОПИСАНИЕ КЛАССА ДОСТУПА К ФУНКЦИЯМ ОТОБРАЖЕНИЯ            *
*                     ЭЛЕКТРОННОЙ КАРТЫ                            *
*                                                                  *
*******************************************************************/

#ifndef QDMWINA_H
#define QDMWINA_H
#include <QtGlobal>

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QTimer>
#include <QtCore/QTimerEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QScrollBar>

#include "qdmviewa.h"
#include "qdmcompa.h"

class QDGtkAction;
class QDMapSelect;
class QDMapViewPort;
class TCallS57Setup;

class GlobLock
{
  public:
  void Lock();
  void UnLock();

  QMutex mymutex;
};

inline void GlobLock::Lock()
{
  mymutex.lock();
}

inline void GlobLock::UnLock()
{
  mymutex.unlock();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// КЛАСС ОКНА ОТОБРАЖЕНИЯ ЭЛЕКТРОННОЙ КАРТОЙ
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapViewWindow : public QDMapViewAcces
{
  Q_OBJECT

  // Разрешить/запретить отображать карты
  Q_PROPERTY(bool MapVisible READ GetMapVisible WRITE SetMapVisible)

  public:
  QDMapViewWindow(QWidget *parent = 0);
  ~QDMapViewWindow();

  // Запросить/установить верхнюю левую позицию начала отображения
  void GetMapLeftTop(int *left, int *top);
  void SetMapLeftTop(int left, int top);

  // Установить/получить текущую позицию карты в метрах
  bool GetMapLeftTopPlane(double *leftPlane, double *topPlane);
  bool SetMapLeftTopPlane(double *leftPlane, double *topPlane);
  // Восстановить положение карты
  bool RestoreMapPosition();

  // Преобразовать координаты точки
  // из пикселей на карте (PP_PICTURE) в пикселы клиентского окна
  POINT ConvertFromPictureToClient(POINT mapPoint);
  // При ошибке возвращает ноль
  int ConvertFromPictureToClient(int *mapPointX, int *mapPointY);
  int ConvertFromPictureToClient(double *mapPointX, double *mapPointY);

  // Переместить карту в заданную точку
  int MoveMap(long int wparam, long int lparam);
  // Установить координаты левого верхнего угла карты
  void SetPicturePoint(POINT &point);

  // Запросить координаты текущей точки
  POINT GetPicturePoint();      // пикселы
  DOUBLEPOINT GetMapPoint();    // дискреты
  DOUBLEPOINT GetPlanePoint();  // метры

  // Разрешить/запретить отображать карты
  bool GetMapVisible() const;

  // Запрос габаритов окна отображения карты
  int GetVisibleWidth();
  int GetVisibleHeight();

  // Разрешить/запретить перерисовку карты
  void SetRendering(bool value);
  bool GetRendering();

  void SetTypePaint(int type);
  int GetTypePaint();

  // Запросить/установить текущий масштаб отображения
  virtual int GetViewScale();
  virtual int SetViewScale(int value);
  // Установить масштаб относительно текущей точки
  // value - реальный масштаб
  int SetViewScaleInPoint(int value);
  // Установить масштаб для отображения всей карты в окне
  int SetScaleAllMapInWindow();

  // Определить габариты всего набора карт в пикселах
  virtual RECT GetRegionRect();
  // Определить габариты всего набора карт в указанной системе координат
  virtual MAPDFRAME GetRegionDFrame(int pointplace);

  // Выделить объекты карты, удовлетворяющие QDMapSelect
  void DoViewSelect(HIMAGE image, RECT *rect, QDMapSelect *select, COLORREF color);
  // Условия отображения
  void SetViewSelect(QDMapSelect *value);
  // Установить текущий Select для отображения
  void SetViewSelectNow(QDMapSelect *mapselect, bool forceUpdate);
  // Установить параметры группового выделения объетов, не используя VarViewSelect
  // objselect - параметры выделения объектов карты если надо выделить,
  //             NULL, если надо снять выделение;
  // color - цвет выделения;
  // repaint - надо ли вызвать перерисовку карты;
  void SetObjSelected(QDMapSelect *objselect, COLORREF color, bool repaint);
  // Установить параметры группового выделения объектов для ViewSelect
  void SetViewSelected(bool view);
  // Разрешить/запретить отображение карты в соответсвии с контекстом условий отображения
  void SetSelecting(bool value);

  // Установить/удалить текущий обработчик событий
  // actionquery - надо ли сначала запросить разрешение
  bool SetCurrentAction(QDGtkAction *gtkaction);
  bool DeleteAction(QDGtkAction *gtkaction, bool actionquery);

  // Добавить/удалить компонент в список компонентов
  void AppendMapObject(QDMapComponent *mapcomponent);
  void DeleteMapObject(QDMapComponent *mapcomponent);

  // Конвертировать координаты из dframein система координат placein
  // в систему координат placeout
  virtual MAPDFRAME ConvertDFrame(MAPDFRAME dframein, int placein, int placeout);
  // Конвертировать координаты из  системы координат placein
  // в систему координат placeout
  virtual int ConvertMetric(double *x, double *y, int placein, int placeout);

  // Выполнить настройку на новые габариты изображения района
  // Возвращает true, если габариты действительно изменились
  bool UpdatePictureBorder();

  // Перерисовать район
  virtual void Repaint();
  void Repaint(int x, int y, int width, int height);
  // Установить положение полос прокрутки и перерисовать карту
  bool SetScrollBarsPositionAndRepaint(int leftPicture, int topPicture);

  // Перерисовка окна средствами ядра mapacces
  void PaintOfMapAcces(QPainter *viewportPainter);

  // Запросить указатель на буфер отображения
  QImage *GetScreenImageBuffer();
  void UpdateScreenImageBuffer(HIMAGE hScreen);

  virtual void drawContents(QPainter *p, int cx, int cy, int cw, int ch);

  // Начать подсветку миганием объекта
  void StartLightObject(HOBJ);
  void EndLightObject();
  void PaintLightObject(HIMAGE);

  // Обработчик сообщений
  long int MessageHandler(long int context, long int code, long int wp, long int lp, long int typemsg = 0);
  // Обработать сообщение окну карты
  int DoMapWindowMsg(long int code, long int param);
  // Отправить сообщение окну карты
  int MapWindowPort(int command, long int parm);

  // Обработка событий изменения состава карт и их отображения на уровне MapViewWindow
  virtual void AllMapRef(int typeaction);
  // Обработка событий изменения состава карт на уровне MapViewWindow
  virtual void MapAction(int typeaction);

  // Вызывается перед отрисовкой карты
  void MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect);
  // Вызывается после отрисовки карты
  void MapAfterPaint(QObject *sender, QImage *mapImage, RECT mappaintrect);

  // Обработчики сообщений
  void viewportMouseMoveEvent(QMouseEvent *event);
  void viewportMousePressEvent(QMouseEvent *event);
  void viewportMouseReleaseEvent(QMouseEvent *event);
  void viewportMouseDoubleClickEvent(QMouseEvent *event);

  void keyPressEvent(QKeyEvent *k);
  void keyReleaseEvent(QKeyEvent *k);

  virtual void timerEvent(QTimerEvent *e);
  virtual bool event(QEvent *e);

  // Установить режим работы с частью карты
  void SetViewFrameMode(int frameModeActive);
  int IsViewFrameActive() const;

  // Установить рамку для отображения части карты
  void SetViewFrame(const DFRAME *frame);
  int GetViewFrame(DFRAME *frame) const;

  // Размер изображения карты в пикселах
  int GetPictureSize(int *pictureWidth, int *pictureHeight);

  int TranslatePictureRectFromViewFrame(RECT *rect);
  int TranslatePicturePointFromViewFrame(POINT *point);

  protected:
  void resizeEvent(QResizeEvent *e);

  public slots:
  // Разрешить/запретить отображать карты
  void SetMapVisible(bool visible);

  void SlotHBarChanged(int x);
  void SlotVBarChanged(int y);

  void HighLightTimerEvent();

  signals:
  // Сигналы от мыши
  // x, y - позиция курсора в системе координат окна отображения карты (пикселы)
  // modkeys - состояние клавиш модификаторов и кнопок мыши
  void SignalMouseMove(int x, int y, int modkeys);
  void SignalMousePress(int x, int y, int modkeys);
  void SignalMouseRelease(int x, int y, int modkeys);
  void SignalMouseDoubleClick(int x, int y, int modkeys);

  // Сигналы от клавиатуры
  // key     - код клавиши,
  // count   - количество нажатий клавиши,
  // modkeys - состояние клавиш модификаторов
  void SignalKeyPress(int key, int count, int state);
  void SignalKeyRelease(int key, int count, int state);

  // Сигналы от окна отображения карты
  void SignalAfterPaint(QPainter *p, int cx, int cy, int cw, int ch);

public:
  QDMapSelect *VarViewSelectLast;
  QDMapSelect *VarViewSelect;
  QDMapSelect *VarObjSelect;

  QDGtkAction *VarCurrentGtkAction;
  TCallS57Setup *CallS57Setup;

  QDMapViewPort *MapViewPort;
  HIMAGE hScreen;
  QScrollBar *HScrollBar;
  QScrollBar *VScrollBar;
  HDC VarHDC;
  QList<QDMapComponent *> VarMapObjects;
  QRect RectDraw;  // Область отрисовки карты
  DFRAME VarMapFrame;
  int VarPlace;
  RECT VarMapRec;
  int VarHorzPos;
  int VarVertPos;
  int VarColorSelected;  // Цвет группового выделения
  int VarPaintEnabled;   // Флаг отрисовки карты
  bool VarRendering;
  bool VarViewSelected;
  bool VarMapVisible;
  bool VarSelecting;
  DOUBLEPOINT MapLeftTopPlane;  // текущая позиция карты в метрах

  POINT PicturePoint;  // координаты в изображении карты (пикселы)
                       // от верхнего левого угла изображения

  int IsPainting;               // для работы геопорталов: индикатор отрисовки
  int IsPaintingFromMapacces;

  QTimer *HighLightTimer;       // Таймер мигания подсвечиваемого объекта
  HOBJ HighLightingObject;      // Подсвечиваемый объект
  bool VarHighLightingEnabled;  // Флаг включенной подсветки объекта
  bool VarHighLightingState;    // Флаг состояния подсветки объекта

  // Буфер, используемый для отображения действий активного Action и отображения на экран
  QImage ScreenImageBuffer;

private:
  int FrameModeActive;
  DFRAME ViewFrame;
};

#endif // QDMWINA_H

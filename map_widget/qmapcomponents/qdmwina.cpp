/******* QDMWINA.CPP ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*        ОПИСАНИЕ КЛАССА ДОСТУПА К ФУНКЦИЯМ ОТОБРАЖЕНИЯ            *
*                     ЭЛЕКТРОННОЙ КАРТЫ                            *
*                                                                  *
*******************************************************************/
#include <algorithm>
#include <math.h>

#include <QtCore/QtGlobal>

#if QT_VERSION >= 0x050000
 #include <QMessageBox>
 #include <QScrollBar>
 #include <QApplication>
 #include <QGridLayout>
 #include <QPainter>
 #include <QPainterPath>
#else
 #include <QtGui/QPainter>
 #include <QtGui/QtGui>
#endif

#include "qdmselt.h"
#include "qdmwina.h"
#include "qdmvport.h"
#include "qdmpoina.h"
#include "qdmact.h"
#include "qdmvport.h"
#include "qdmcmp.h"
#include "qdmtxt.h"

#include "mapgdi.h"
#include "mapview.rh"
#include "mwhandlr.h"
#include "m3dcom.h"
#include "visfuncs.h"

// Максимальное количество экземпляров класса QDMapViewWindow
#define MAXCOUNT_VIEWWINDOW 128

int MapViewWindowCount = 0;     // Счетчик экземпляров класса QDMapViewWindow

// Таблица указателей на экземпляры класса QDMapViewWindow
QDMapViewWindow *MapViewWindowArray[MAXCOUNT_VIEWWINDOW] = {NULL};

// Максимальное значение координат(для функций семейства mapPaint)
int MaxValueCoordXY = 32000;

typedef struct TParmPostMessage                   // Параметры Post сообщения
{
  long int klxHwnd;
  int Code;
  int wParam;
  int lParam;
}
TParmPostMessage;

const int          MaxCountPostMsg = 10;          // Максимально допустимое количество отложенных сообщений
int                CountPostMsg = 0;              // Текущее количество необработанных сообщений
TParmPostMessage   recPostMsg;                    // Параметры необработанного сообщения
TParmPostMessage   ArrayPostMsg[MaxCountPostMsg]; // Массив необработанных сообщений

//------------------------------------------------------------------
// Главный обработчик сообщений - определен ниже
//------------------------------------------------------------------
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);

//-------------------------------------------------------------
// Выполнить обработку отложенных
//-------------------------------------------------------------
void DoPostMessage()
{
  if (CountPostMsg < 1)    return;

  recPostMsg = ArrayPostMsg[0];

  for (int i = 1; i < CountPostMsg; i++)
    ArrayPostMsg[i - 1] = ArrayPostMsg[i];

  ArrayPostMsg[CountPostMsg - 1].Code = 0; // на всякий случай
  CountPostMsg = CountPostMsg - 1;

  MessageHandler(recPostMsg.klxHwnd, recPostMsg.Code, recPostMsg.wParam, recPostMsg.lParam, 0);
}

//-------------------------------------------------------------
// Добавить отложенное сообщение в очередь
//-------------------------------------------------------------
void AppendPostMsg(long int mapViewWindow, int aCode,int  aWP, int aLP)
{
  if (CountPostMsg >= MaxCountPostMsg)    return;    // уже перебор

  CountPostMsg = CountPostMsg + 1;

  ArrayPostMsg[CountPostMsg - 1].klxHwnd = mapViewWindow;
  ArrayPostMsg[CountPostMsg - 1].Code    = aCode;
  ArrayPostMsg[CountPostMsg - 1].wParam  = aWP;
  ArrayPostMsg[CountPostMsg - 1].lParam  = aLP;
}

//------------------------------------------------------------------
// Главный обработчик сообщений
//------------------------------------------------------------------
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg)
{
  int i = 0;

  // Выполнить обработку отложенных
  DoPostMessage();
  if (typemsg == 1) // Сообщение надо обработать потом
  {
    AppendPostMsg(mapViewWindow, code, wp, lp);
    return 1;
  }

  // Пока текущее окно карты - всегда первое
  if (code == AW_GETCURRENTDOC)
  {
    *((long int *)wp) = (long int)MapViewWindowArray[0];
    if (lp != 0)
      *((long int *)lp) = (long int)(MapViewWindowArray[0]->GetMapHandle());
    return AW_GETCURRENTDOC;
  }

  while ((i < MapViewWindowCount) &&
         ((long int)(MapViewWindowArray[i]) != mapViewWindow)) i++;
  if (i < MapViewWindowCount)
  {
    return MapViewWindowArray[i]->MessageHandler(mapViewWindow, code, wp, lp, typemsg);
  }

  return 0;
}

extern "C"
{
// Установить функцию MessageBox для LINUX
// При ошибке возвращает ноль
long int _MAPAPI mapSetMessageBox(long int value);

//-------------------------------------------------------------
// Функция обратного вызова MessageBox для libmapacces.so
//  hWnd     - идентификатор владельца окна(для LINUX не имеет значения)
//  message     - Message
//  caption - Caption
// Exit
//   такие коды возврата надо отдавать в libacces
//   IDOK      =  1;
//   IDCANCEL  =  2;
//   IDABORT   =  3;
//   IDRETRY   =  4;
//   IDIGNORE  =  5;
//   IDYES     =  6;
//   IDNO      =  7;
//   IDCLOSE   =  8; этого нет
//   IDHELP    =  9; этого нет
//-------------------------------------------------------------
int PanMessageBox(HWND hWnd, const WCHAR *message, const WCHAR *caption, int flag)
{
  int ident;
  int buttoncount = 1;
  int ret = 0;
  int retmessage = 0;
  const WCHAR * button_OK[] = {_qdmtxt(1)}; // Да
  const WCHAR * button_OKCANCEL[] = {_qdmtxt(4), _qdmtxt(3)};  // Продолжить, Отменить
  const WCHAR * button_ABORTRETRYIGNORE[] = {_qdmtxt(6), _qdmtxt(5), _qdmtxt(7)}; // Прервать, Повторить, Пропустить
  const WCHAR * button_YESNOCANCEL[] = {_qdmtxt(1), _qdmtxt(2), _qdmtxt(3)}; // Да, Нет, Отменить
  const WCHAR * button_YESNO[] =  { _qdmtxt(1), _qdmtxt(2)}; // Да, Нет
  const WCHAR * button_RETRYCANCEL[] = {_qdmtxt(5), _qdmtxt(3)}; // Повторить, Отменить
  const WCHAR ** name_button = button_OK;
  int  maxcountbut = 3;

  QString *qnamebut[maxcountbut];
  QString qcaption;
  QString qmessage;
  QTextCodec *codec = QTextCodec::codecForName("KOI8-R");

  if (caption)
    qcaption = QString::fromUtf16(caption);
  if (message)
    qmessage = QString::fromUtf16(message);

  for (int i = 0; i < maxcountbut; i++)
    qnamebut[i] = 0;

  // Получаем типы кнопок
  ident = flag & 0x7;

  switch (ident)
  {
    case MB_OK :
      name_button = button_OK;
      buttoncount = 1;
      break;
    case MB_OKCANCEL :
      name_button = button_OKCANCEL;
      buttoncount = 2;
      break;
    case MB_YESNOCANCEL :
      name_button = button_YESNOCANCEL;
      buttoncount = 3;
      break;
    case MB_YESNO :
      name_button = button_YESNO;
      buttoncount = 2;
      break;
    case MB_ABORTRETRYIGNORE :
      name_button = button_ABORTRETRYIGNORE;
      buttoncount = 3;
      break;
    case MB_RETRYCANCEL :
      name_button = button_RETRYCANCEL;
      buttoncount = 2;
      break;
  }

  for (int i = 0; i < buttoncount; i++)
    qnamebut[i] = new QString(QString::fromUtf16(name_button[i]));

  // Получаем типы иконок
  int identicon = flag & 0x70;
  int icon = QMessageBox::Warning;
  switch (identicon)
  {
    case MB_ICONERROR:
      icon = QMessageBox::Critical;
      break;
    case MB_ICONQUESTION:
      icon = QMessageBox::Question;
      break;
    case MB_ICONWARNING:
      icon = QMessageBox::Warning;
      break;
    case MB_ICONINFORMATION:
      icon = QMessageBox::Information;
      break;
    default:
      icon = QMessageBox::Warning;
      break;
  }



  QMessageBox *messbox = 0;
  switch (ident)
  {
    case MB_OK :
      messbox = new QMessageBox(qcaption, qmessage, (QMessageBox::Icon)icon,
                                QMessageBox::Ok | QMessageBox::Default,
                                QMessageBox::NoButton, QMessageBox::NoButton,
                                0);
      messbox->setButtonText(QMessageBox::Ok, *qnamebut[0]);
      retmessage = messbox->exec();
      break;
    case MB_OKCANCEL :
    case MB_YESNO :
    case MB_RETRYCANCEL :
      messbox = new QMessageBox(qcaption, qmessage, (QMessageBox::Icon)icon,
                                QMessageBox::Yes | QMessageBox::Default,
                                QMessageBox::No  | QMessageBox::Escape, QMessageBox::NoButton,
                                0);
      messbox->setButtonText(QMessageBox::Yes, *qnamebut[0]);
      messbox->setButtonText(QMessageBox::No, *qnamebut[1]);
      retmessage = messbox->exec();
      break;
    case MB_YESNOCANCEL :
    case MB_ABORTRETRYIGNORE :
      messbox = new QMessageBox(qcaption, qmessage, (QMessageBox::Icon)icon,
                                QMessageBox::Yes | QMessageBox::Default,
                                QMessageBox::No,
                                QMessageBox::Cancel  | QMessageBox::Escape,
                                0);
      messbox->setButtonText(QMessageBox::Yes, *qnamebut[0]);
      messbox->setButtonText(QMessageBox::No, *qnamebut[1]);
      messbox->setButtonText(QMessageBox::Cancel, *qnamebut[2]);
      retmessage = messbox->exec();
      break;
  }

  if (messbox != 0) delete messbox;

  for (int i = 0; i < buttoncount; i++)
    delete qnamebut[i];

  switch (ident)
  {
    case MB_OK :
      ret = IDOK;
      break;
    case MB_OKCANCEL :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDOK;
          break;
        case QMessageBox::No:
          ret = IDCANCEL;
          break;
      }
      break;
    case MB_YESNOCANCEL :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDYES;
          break;
        case QMessageBox::No:
          ret = IDNO;
          break;
        case QMessageBox::Cancel:
          ret = IDCANCEL;
          break;
      }
      break;
    case MB_YESNO :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDYES;
          break;
        case QMessageBox::No:
          ret = IDNO;
          break;
      }
      break;
    case MB_ABORTRETRYIGNORE :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDABORT;
          break;
        case QMessageBox::No:
          ret = IDRETRY;
          break;
        case QMessageBox::Cancel:
          ret = IDIGNORE;
          break;
      }
      break;
    case MB_RETRYCANCEL :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDRETRY;
          break;
        case QMessageBox::No:
          ret = IDCANCEL;
          break;
      }
      break;

  }

  return ret;
}

}  // "C"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ КЛАСС " НАСТРОЙКА ПАРАМЕТРОВ S57 "  ++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TCallS57Setup
{
public:

  TCallS57Setup()
  {
    // Вызов функции
    typedef long int (WINAPI *type_rscInitS57)(const char *mapviewini);
    type_rscInitS57 rscInitS57;
    char path[MAX_PATH], name[MAX_PATH];
    QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
#ifndef BUILD_DLL64
    const char * namelib = "s57navy.iml";
#else
    const char * namelib = "s57navy.iml64";
#endif

    memset(ProgramIniName, 0, sizeof(ProgramIniName));
    strcpy(ProgramIniName, codec->fromUnicode(qApp->applicationFilePath()).data());
    SplitThePath(ProgramIniName, NULL, path, name, NULL);
    MakeThePath(ProgramIniName, NULL, path, name, NULL);
    strcat(ProgramIniName, ".ini");

    rscInitS57 = (type_rscInitS57)mapLoadLibrary(namelib, &libInst, "imlInit");
    if (rscInitS57 != 0)
      (*rscInitS57)(ProgramIniName);
  }
  ~TCallS57Setup()
  {
    if (libInst != 0)
      mapFreeLibrary(libInst);
  }

  // Загрузка библиотеки
  HINSTANCE libInst;
  char      ProgramIniName[MAX_PATH];
};

#ifdef QT_DRAW

static int FlagStartQtDraw = 0;

//------------------------------------------------------------------
// Сплошная линия
//------------------------------------------------------------------
int DrawSolidLine(DRAWFUNCPARM& dfp, const POLYDATA4D *data, float thick)
{
  if (dfp.Begin == 0 || data == 0)
    return 0;

  DRAWPOINT * Points     = data->Points;      // Указатель на метрику
  int       * PolyCounts = data->PolyCounts;  // Число точек ломаной
  int Count              = data->Count;       // Число ломаных

  if (Points == 0 || *PolyCounts <= 0)
    return 0;

  QImage image(dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32);

  QPainter painter;
  painter.begin(&image);
  if (mapGetAntialiasedQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  QRegion clipRegion(0, 0, dfp.DrawWidth, dfp.DrawHeight);
  painter.setClipRegion(clipRegion);

  if(dfp.Color == 0xffffff)
    dfp.Color -= 1;

  if (thick < 1)
    thick = 1;

  // сформировать цвет
  QColor qColor(dfp.Color);
  qColor.setAlpha(dfp.Alpha);
  QPen pen(QBrush(qColor), thick, Qt::SolidLine);

  while (Count-- > 0)
  {
    if(*PolyCounts == 0)
    {
      PolyCounts++;
      continue;
    }

    // Признак замкнутости
    int lock = (Points[0].x == Points[*PolyCounts-1].x &&
                Points[0].y == Points[*PolyCounts-1].y);

    if(lock && *PolyCounts < 3)   // Отрисовка одной или двух совпадающих точек
    {
      painter.setPen(pen);

      painter.drawPoint(*((QPoint*)Points));
    }
    else
    {
      if (lock) pen.setCapStyle(Qt::RoundCap);
      else  pen.setCapStyle(Qt::FlatCap);
      pen.setJoinStyle(Qt::RoundJoin);
      painter.setPen(pen);

      painter.drawPolyline((QPoint*)Points, *PolyCounts);
    }

    Points += *PolyCounts;
    PolyCounts++;
  }

  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Штриховая линия
//------------------------------------------------------------------
int DrawDashLine(DRAWFUNCPARM& dfp, const POLYDATA4D *data, float thick, float dash, float space)
{
  if (dfp.Begin == 0 || data == 0)
    return 0;

  DRAWPOINT * Points     = data->Points;      // Указатель на метрику
  int       * PolyCounts = data->PolyCounts;  // Число точек ломаной
  int Count              = data->Count;       // Число ломаных

  if (Points == 0 || *PolyCounts <= 0)
    return 0;

  QImage image(dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32);

  QPainter painter;
  painter.begin(&image);
  if (mapGetAntialiasedQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  QRegion clipRegion(0, 0, dfp.DrawWidth, dfp.DrawHeight);
  painter.setClipRegion(clipRegion);

  if (thick < 1)
    thick = 1;

  // Сформировать цвет
  QColor qColor(dfp.Color);
  qColor.setAlpha(dfp.Alpha);
  QPen pen(QBrush(qColor), thick, Qt::CustomDashLine);

  QVector<qreal> dashes;
  dashes << dash << space;
  pen.setDashPattern(dashes);

  while (Count-- > 0)
  {
    // Признак замкнутости
    int lock = (Points[0].x == Points[*PolyCounts-1].x &&
                Points[0].y == Points[*PolyCounts-1].y);

    if (lock) pen.setCapStyle(Qt::RoundCap);
    else  pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    try
    {
      painter.drawPolyline((QPoint*)Points, *PolyCounts);
    }
    catch(...)
    {
      fprintf(stderr, "catch\n");
    }

    Points += *PolyCounts;
    PolyCounts++;
  }

  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Площадной объект
//------------------------------------------------------------------
int DrawPolygon(DRAWFUNCPARM& dfp, const POLYDATA4D *data)
{
  if (dfp.Begin == 0 || data == 0)
    return 0;

  DRAWPOINT * Points     = data->Points;      // Указатель на метрику
  int       * PolyCounts = data->PolyCounts;  // Число точек ломаной
  int Count              = data->Count;       // Число ломаных

  if (Points == 0 || *PolyCounts <= 0)
    return 0;

  QImage image( dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32 );

  QColor qColor( dfp.Color );
  qColor.setAlpha( dfp.Alpha );

  QPainter painter( &image );
  painter.setPen( Qt::NoPen );
  painter.setBrush( QBrush( qColor ) );

  if (Count > 100)
  {
    painter.setClipRect( 0, 0, dfp.DrawWidth, dfp.DrawHeight );
  }

  int numPoints = 0;

  while (Count-- > 0)
  {
    numPoints += *PolyCounts;
    PolyCounts++;
  }

  painter.drawPolygon( (QPoint*) data->Points, numPoints );

  return 1;
}

// Коэффициент уточнения высоты (подобрано)
#define FACTOR_HEIGHT  0.82

//------------------------------------------------------------------
// Запросить ширину строки
//
//------------------------------------------------------------------
int DrawTextWidth(DRAWFUNCPARM& dfp, const WCHAR *text,
                  float height, int italic, int weight, int wide, const char *fontName)
{
  if ( dfp.Begin == 0 )
    return 1;

  if ( weight < 401 )
    weight = QFont::Light;
  else if ( weight < 501 )
    weight = QFont::Normal;
  else if ( weight < 701 )
    weight = QFont::DemiBold;
  else
    weight = QFont::Bold;

  QFont font( fontName, height, weight, italic );
  font.setPixelSize( height );    // Заново установить размер, т.к. выше он задан в поинтах

  if ( wide == UNIW_NARROW )
    font.setStretch( 100.0 / 1.2 );
  else if ( wide == UNIW_WIDE )
    font.setStretch( 100.0 * 1.2 );

#if QT_VERSION > 0x040800
  // Отключение сглаживания шрифта, т.к. у Painter уже есть сглаживание
  font.setHintingPreference( QFont::PreferNoHinting );
#endif

  //  return QFontMetrics(font).width( MainCodec->toUnicode( text ) );
  QString textString = QString::fromUtf16( text );
  return QFontMetrics(font).width( textString );
}

//------------------------------------------------------------------
// Подпись
//------------------------------------------------------------------
int DrawText(DRAWFUNCPARM& dfp, const WCHAR *text,
             float height, int italic, int weight, int wide, const char *fontName)
{
  QString textString = QString::fromUtf16( text );

  QPainter painter;
  QImage image( dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32 );

  //- Восстановил
  //- чтобы работало выделение подписей -------------------------------
  QWidget *DrawWidget = (QWidget*)(dfp.hWidget);
  if(DrawWidget)
  {
    if (DrawWidget->paintingActive())
      return 0;
    painter.begin(DrawWidget);
  }
  else
  {
    if (dfp.Begin == 0)
      return 0;
    painter.begin(&image);
  }
  //- Восстановил
  if (mapGetTextQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  if ( weight < 401 )
    weight = QFont::Light;
  else  if ( weight < 501 )
    weight = QFont::Normal;
  else  if ( weight < 701 )
    weight = QFont::DemiBold;
  else
    weight = QFont::Bold;

  QFont font( fontName, height, weight, italic );
  font.setPixelSize( height );    // Заново установить размер, т.к. выше он задан в поинтах

  if ( wide == UNIW_NARROW )
    font.setStretch( 100.0 / 1.2 );
  else if ( wide == UNIW_WIDE )
    font.setStretch( 100.0 * 1.2 );

#if QT_VERSION > 0x040800
  // Для RedHat отключим сглаживания шрифта, т.к. у Painter уже есть сглаживание
  // выставив PreferNoHinting
  // Для Астры необходимо PreferFullHinting
  //  font.setStyleStrategy( QFont::PreferFullHinting );
  font.setHintingPreference( QFont::PreferNoHinting );
#endif

  painter.setFont( font );

  painter.translate(dfp.Dx + dfp.DrawPointX * 1., dfp.Dy + dfp.DrawPointY * 1.);
  painter.rotate( dfp.Angle * 180.0 / 3.14 );

  if ( dfp.MarkColor != -1 )
  {
    QPen pen2( QColor( dfp.MarkColor ), dfp.MarkThick*2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    painter.setPen( pen2 );

    if (dfp.MarkType == 0)
    {
      // Тень
      painter.drawText( dfp.MarkThick * 2, dfp.MarkThick * 2, textString );
    }
    else
    {
      // Контур
      QPainterPath path;
      path.addText( 0, 0, font, textString );
      painter.drawPath( path );
    }
  }

  // блок отображения текста
  if (dfp.Color != -1)
  {
    // сформировать цвет
    QColor qColor(dfp.Color);
    qColor.setAlpha(dfp.Alpha);
    QPen pen(qColor);

    painter.setPen(pen);
    painter.drawText(0, 0, textString);
  }

  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Эллипс
//------------------------------------------------------------------
int DrawEllipse(DRAWFUNCPARM& dfp, int x, int y, int width, int height,
                float thick, float dash, float space,
                int imagetype)
{
  if (dfp.Begin == 0)
    return 0;

  QImage image(dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32);
  QWidget *DrawWidget = (QWidget *) (dfp.hWidget);
  QPainter painter;

  if (DrawWidget)
  {
    if (DrawWidget->paintingActive())
      return 0;
    painter.begin(DrawWidget);
  }
  else
  {
    if (dfp.Begin == 0)
      return 0;
    painter.begin(&image);
  }
  if (mapGetAntialiasedQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  // Выбор границы области рисования
  QRegion clipRegion(0, 0, dfp.DrawWidth, dfp.DrawHeight);
  painter.setClipRegion(clipRegion);

  QColor qColor(dfp.Color);
  qColor.setAlpha(dfp.Alpha);

  // Создание пера
  QPen pen(qColor);

  painter.translate(dfp.Dx + x + width/2, dfp.Dy + y + height/2);
  painter.rotate(dfp.Angle * 180 / 3.14);
  painter.translate(- x - width/2, - y - height/2);

  // Обработка радиуса меньше пикселя
  if (width < 3 && height < 3)
  {
    painter.setPen(pen);
    painter.drawPoint(x, y);
    painter.end();

    return 1;
  }

  // Настройка пера в зависимости от типа объекта
  if (imagetype == IMG_SQUARE)
  {
    painter.setBrush(QBrush(qColor));
  }
  else
  {
    pen.setWidth(thick);
    if (imagetype == IMG_DOT)
    {
      pen.setStyle(Qt::CustomDashLine);
      // Описание штриха и пробела повторяемой части
      QVector<qreal> dashes;
      dashes << dash << space;
      pen.setDashPattern(dashes);
    }
  }
  painter.setPen(pen);

  // Непосредственно рисование
  painter.drawEllipse(x, y, width, height);
  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Дуга
//------------------------------------------------------------------
int DrawArc(DRAWFUNCPARM& dfp, int x, int y, int width, int height,
            int startAngle, int spanAngle,
            float thick, float dash, float space,
            int imagetype)
{
  if (dfp.Begin == 0)
    return 0;

  if(width == 0 || height == 0)
    return 0;

  QImage image(dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32);
  QPainter painter;

  painter.begin(&image);
  if (mapGetAntialiasedQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  // Выбор границы области рисования
  QRegion clipRegion(0, 0, dfp.DrawWidth, dfp.DrawHeight);
  painter.setClipRegion(clipRegion);

  QColor qColor(dfp.Color);
  qColor.setAlpha(dfp.Alpha);

  // Создание пера
  QPen pen(qColor);
  pen.setWidth(thick);

  // Настройка пера в зависимости от типа объекта
  if (imagetype == IMG_DOT)
  {
    pen.setStyle(Qt::CustomDashLine);
    // Описание штриха и пробела повторяемой части
    QVector<qreal> dashes;
    dashes << dash << space;
    pen.setDashPattern(dashes);
  }
  painter.setPen(pen);

  // Смещение области рисования
  painter.translate(dfp.Dx + x + width/2, dfp.Dy + y + height/2);
  painter.rotate(dfp.Angle * 180 / 3.14);
  painter.translate(- x - width/2, - y - height/2);

  // Непосредственно рисование
  painter.drawArc(x, y, width, height, (-startAngle) * 16, (-spanAngle) * 16);

  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Сектор
//------------------------------------------------------------------
int DrawPie(DRAWFUNCPARM &dfp, int x, int y, int width, int height, int startAngle, int spanAngle)
{
  if (dfp.Begin == 0)
    return 0;

  if (width == 0 || height == 0)
    return 0;

  QImage image(dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32);
  QPainter painter;

  painter.begin(&image);
  if (mapGetAntialiasedQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  // Выбор границы области рисования
  QRegion clipRegion(0, 0, dfp.DrawWidth, dfp.DrawHeight);
  painter.setClipRegion(clipRegion);

  QColor qColor(dfp.Color);
  qColor.setAlpha(dfp.Alpha);

  // Создание пера
  QPen pen(qColor);
  painter.setPen(pen);
  painter.setBrush(QBrush(qColor));

  // Смещение области рисования
  painter.translate(dfp.Dx + x + width/2, dfp.Dy + y + height/2);
  painter.rotate(dfp.Angle * 180 / 3.14);
  painter.translate(- x - width/2, - y - height/2);

  // Непосредственно рисование
  painter.drawPie(x, y, width, height, (-startAngle) * 16, (-spanAngle) * 16);

  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Запрос параметров шрифта для правильного расчета отображения подписей
//------------------------------------------------------------------
int DrawFontMetrics(const char *fontName, int height, int italic,
                    unsigned long* ascent, unsigned long* descent, unsigned long* resultHeight)
{
  Q_UNUSED(italic)

  QFont font( fontName, height );
  font.setPixelSize( height );

  QFontMetrics fm(font);
  *ascent = fm.ascent();
  *descent = fm.descent();
  *resultHeight = fm.ascent() - fm.leading();

  return 1;
}

//------------------------------------------------------------------
// Картинка формата:
// 0 - PNG, 1 - BMP, 2 - JPG,  3 - TIF
//------------------------------------------------------------------
int DrawPicture(DRAWFUNCPARM &dfp, IMGPICTURE *imgpict, const char *fileaddr, int picturesize)
{
  if (dfp.Begin == 0 || imgpict == 0 || fileaddr == 0 || picturesize == 0)
    return 0;

  const char *ext = "PNG";
  if (imgpict->Type == 0)
    ext = "PNG";
  else if (imgpict->Type == 1)
    ext = "BMP";
  else if (imgpict->Type == 2)
    ext = "JPG";
  else if (imgpict->Type == 3)
    ext = "TIFF";
  else
    return 0;

  // Загрузка картинки
  QImage drawImage;
  if (!drawImage.loadFromData((const uchar *)fileaddr, picturesize, ext))
    return 0;

  QImage image(dfp.Begin, dfp.Width, dfp.Height, QImage::Format_RGB32);

  QPainter painter;
  painter.begin(&image);

  painter.setOpacity(dfp.Alpha / 255.);
  if (mapGetAntialiasedQuality())
    painter.setRenderHint(QPainter::Antialiasing, true);

  // Поворот картинки
  if (fabs(dfp.Angle) >= 0.1)
  {
    painter.translate(dfp.Dx, dfp.Dy);
    painter.rotate(dfp.Angle);
    painter.translate(-dfp.Dx, -dfp.Dy);
  }
  else
  {
    QRegion clipRegion(0, 0, dfp.DrawWidth, dfp.DrawHeight);
    painter.setClipRegion(clipRegion);
  }

  QRect rectdst(dfp.DrawPointX, dfp.DrawPointY, imgpict->Width, imgpict->Height);
  QRect rectsrc(0, 0, drawImage.width(), drawImage.height());

  painter.drawImage(rectdst, drawImage, rectsrc);
  painter.end();

  return 1;
}

//------------------------------------------------------------------
// Сохранить изображение в файл
//------------------------------------------------------------------
int SaveImageToFile(const char *filename, const char *data, int w, int h)
{
  if (filename == 0 || data == 0 || w <= 0 || h <= 0)
    return 0;

  QImage image((uchar*)data, w, h, QImage::Format_RGB32);
  int isSaved = image.save(QString::fromUtf8(filename));

  return isSaved;
}

//------------------------------------------------------------------
// Сохранить изображение с прозрачностью в файл
//------------------------------------------------------------------
int SaveTranspImageToFile(const char *filename, const char *data, int w, int h)
{
  if (filename == 0 || data == 0 || w <= 0 || h <= 0)
    return 0;

  QImage image((uchar*)data, w, h, QImage::Format_ARGB32);
  int isSaved = image.save(QString::fromUtf8(filename));

  return isSaved;
}

//------------------------------------------------------------------
// Сохранение изображения в файл заполненного указанным цветом
//------------------------------------------------------------------
int SaveFillRectangleToFile(const char *filename, int w, int h, COLORREF color)
{
  if (filename == 0 || w <= 0 || h <= 0)
    return 0;

  QImage image( w, h, QImage::Format_RGB32 );
  QColor color1((color&0x000000FF), ((color&0x0000FF00)>>8), ((color&0x00FF0000)>>16));
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
  image.fill( color1 );
#else
  image.fill( color1.value() );
#endif

  int isSaved = image.save(QString::fromUtf8(filename));

  return isSaved;
}

#endif

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ КЛАСС "ИНТЕРФЕЙС ОТОБРАЖЕНИЯ ЭЛЕКТРОННОЙ КАРТОЙ" +++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapViewWindow::QDMapViewWindow(QWidget *parent) : QDMapViewAcces(parent)
{
  // Точка - разделитель плавающих чисел
  setlocale(LC_NUMERIC, "C");

  IsPainting = 0;

  MapLeftTopPlane.X = 0;
  MapLeftTopPlane.Y = 0;

  PicturePoint.x = 0;
  PicturePoint.y = 0;

  hScreen = 0;
  setMinimumWidth(50);
  setMinimumHeight(50);
  VarRendering  = true;
  CallS57Setup  = 0;

  QGridLayout *layout  = new QGridLayout;
  MapViewPort = new QDMapViewPort(this);
  HScrollBar  = new QScrollBar(Qt::Horizontal);
  VScrollBar  = new QScrollBar(Qt::Vertical);

  connect(HScrollBar, SIGNAL(valueChanged(int)), this, SLOT(SlotHBarChanged(int)));
  connect(VScrollBar, SIGNAL(valueChanged(int)), this, SLOT(SlotVBarChanged(int)));

  layout->addWidget(MapViewPort, 0, 0);
  layout->addWidget(VScrollBar, 0, 1);
  layout->addWidget(HScrollBar, 1, 0);

  layout->setMargin(0);
  layout->setSpacing(0);

  setLayout(layout);
  MapViewPort->SetMapViewWindow(this);
  MapViewPort->setMouseTracking(TRUE);
  MapViewPort->hide();

  VarMapHandle     = 0;
  VarMapVisible    = FALSE;
  memset((char *)&VarMapFrame, 0, sizeof(VarMapFrame));
  VarPlace         = PP_PLANE;
  VarHorzPos       = 0;
  VarVertPos       = 0;
  VarViewSelected  = FALSE;
  VarColorSelected = RGB(0xF0, 0x20, 0x90);   // Фиолетовый
  VarSelecting     = FALSE;
  VarViewSelect    = NULL;
  VarObjSelect     = NULL;
  VarPaintEnabled  = 1;
  VarCurrentGtkAction = NULL;
  setMouseTracking(TRUE);

  HighLightingObject = 0;
  HighLightTimer = 0;
  VarHighLightingEnabled = false;
  VarHighLightingState = false;

  MapViewWindowArray[MapViewWindowCount] = this;
  MapViewWindowCount = MapViewWindowCount + 1;
  if (MapViewWindowCount == 1)
    // Установить функцию MessageBox для LINUX
    // При ошибке возвращает ноль
    mapSetMessageBox((long int)&PanMessageBox);

  FrameModeActive = 0;

#ifdef QT_DRAW
  if (FlagStartQtDraw == 0)
  {
    FlagStartQtDraw = 1;
    // Установить функции рисования:
    mapDrawSolidLine(DrawSolidLine);                     // Сплошная линия
    mapDrawDashLine(DrawDashLine);                       // Штриховая линия
    mapDrawPolygon(DrawPolygon);                         // Площадной объект
    mapDrawText(DrawText);                               // Подпись
    mapDrawTextWidth(DrawTextWidth);                     // Ширина подписи
    mapDrawEllipse(DrawEllipse);                         // Эллипс
    mapDrawArc(DrawArc);                                 // Дуга
    mapDrawPie(DrawPie);                                 // Сектор
    mapDrawFontMetrics(DrawFontMetrics);                 // Метрика шрифта
    mapDrawPicture(DrawPicture);                         // Картинка
    mapSaveImageToFile(SaveImageToFile);                 // Изображение в файл
    mapSaveTranspImageToFile(SaveTranspImageToFile);     // Изображение с прозрачностью в файл
    mapSaveFillRectangleToFile(SaveFillRectangleToFile); // Сохранение изображения в файл заполненного указанным цветом
  }
#endif

}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapViewWindow::~QDMapViewWindow()
{
  DeleteAction(VarCurrentGtkAction, false);

  // Удалим MapView из всех TMapComponent
  while (VarMapObjects.size() > 0)
  {
    if (VarMapObjects.first())
    {
      (VarMapObjects.first())->SetMapViewAcces(0);
    }
  }
  VarRendering = false;

  delete VarViewSelect;
  VarViewSelect = NULL;

  delete VarObjSelect;
  VarObjSelect = NULL;

  SetMapVisible(FALSE);
  int i = 0;
  while ((i < MapViewWindowCount) && (MapViewWindowArray[i] != this)) i++;
  if (i < MapViewWindowCount)
  {
    memmove((char *)&MapViewWindowArray[i], (char *)&MapViewWindowArray[i+1], (MapViewWindowCount-i-1) * 4);
    MapViewWindowCount = MapViewWindowCount - 1;
    MapViewWindowArray[MapViewWindowCount] = NULL;
  }

  if (hScreen)
  {
    mapCloseImage(hScreen);
    hScreen = 0;
  }

}

// -----------------------------------------------------------------------------
// Обработчики сообщений
// -----------------------------------------------------------------------------
void QDMapViewWindow::viewportMousePressEvent(QMouseEvent * e)
{
  if (VarCurrentGtkAction)   // есть обработчик
  {
    VarCurrentGtkAction->MouseDown(e->x(), e->y(), e->button());
  }

  emit SignalMousePress(e->x(), e->y(), e->button());
  update();
}

void QDMapViewWindow::viewportMouseReleaseEvent ( QMouseEvent * e )
{
  if (VarCurrentGtkAction)   // есть обработчик
    VarCurrentGtkAction->MouseUp(e->x(), e->y(), e->button());

  emit SignalMouseRelease(e->x(), e->y(), e->button());
  update();
}

void QDMapViewWindow::viewportMouseMoveEvent(QMouseEvent * e)
{
  // Обновить координаты точки курсора в пикселах
  PicturePoint.x = VarHorzPos + e->x();
  PicturePoint.y = VarVertPos + e->y();

  if (VarCurrentGtkAction)   // есть обработчик
    VarCurrentGtkAction->MouseMove(e->x(), e->y(), e->buttons());

  emit SignalMouseMove(e->x(), e->y(), e->buttons());
  update();
}

void QDMapViewWindow::viewportMouseDoubleClickEvent ( QMouseEvent * e )
{
  if (VarCurrentGtkAction)   // есть обработчик
    VarCurrentGtkAction->DblClick();

  emit SignalMouseDoubleClick(e->x(), e->y(), e->button());
}

void QDMapViewWindow::keyPressEvent(QKeyEvent *k )
{
  QWidget::keyPressEvent(k);

  // есть обработчик
  if (VarCurrentGtkAction)   // есть обработчик
    VarCurrentGtkAction->KeyDown(k->key(), k->count(), k->modifiers());

  emit SignalKeyPress(k->key(), k->count(), k->modifiers());
}

void QDMapViewWindow::keyReleaseEvent(QKeyEvent *k )
{
  QWidget::keyReleaseEvent(k);

  if (VarCurrentGtkAction)   // есть обработчик
    VarCurrentGtkAction->KeyUp(k->key(), k->count(), k->modifiers());

  emit SignalKeyRelease(k->key(), k->count(), k->modifiers());
}

bool QDMapViewWindow::event(QEvent *e)
{
  DoPostMessage();
  bool ret = QWidget::event(e);

  return ret;
}

void QDMapViewWindow::resizeEvent(QResizeEvent *e)
{
  QDMapViewAcces::resizeEvent(e);
  VarPaintEnabled = 0;
  if (hScreen != 0)
  {
    POINT point1;
    RECT rect;
    int mapWidth, mapHeight;

    if(IsPainting == 0)
    {
      IsPainting = 1;

      mapChangeImageSizeEx(hScreen, 1, e->size().width(),
                           e->size().height());
      GetPictureSize(&mapWidth, &mapHeight);

      if (mapWidth - e->size().width() > 0)
        HScrollBar->setMaximum(mapWidth - e->size().width());
      else
        HScrollBar->setMaximum(0);
      if (VarHorzPos > HScrollBar->maximum())
      {
        HScrollBar->setValue(HScrollBar->maximum());
        VarHorzPos = HScrollBar->maximum();
      }
      else
        HScrollBar->setValue(VarHorzPos);

      if (mapHeight - e->size().height() > 0)
        VScrollBar->setMaximum(mapHeight - e->size().height());
      else
        VScrollBar->setMaximum(0);
      if (VarVertPos > VScrollBar->maximum())
      {
        VScrollBar->setValue(VScrollBar->maximum());
        VarVertPos = VScrollBar->maximum();
      }
      else
        VScrollBar->setValue(VarVertPos);

      HScrollBar->setPageStep(e->size().width());
      VScrollBar->setPageStep(e->size().height());
      HScrollBar->setSingleStep(32);
      VScrollBar->setSingleStep(32);

      point1.x = point1.y = 0;
      rect.left   = VarHorzPos;
      rect.top    = VarVertPos;
      rect.right  = VarHorzPos + e->size().width();
      rect.bottom = VarVertPos + e->size().height();
      TranslatePictureRectFromViewFrame(&rect);
      mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

      VarPaintEnabled = 1;
      IsPainting = 0;

      MapViewPort->update();
    }
  }
}

void QDMapViewWindow::timerEvent(QTimerEvent *)
{
  DoPostMessage();
}

int QDMapViewWindow::IsViewFrameActive() const
{
  return FrameModeActive;
}

void QDMapViewWindow::SetViewFrameMode(int frameModeActive)
{
  if (FrameModeActive == frameModeActive)
    return;

  FrameModeActive = frameModeActive;

  UpdatePictureBorder();
}

void QDMapViewWindow::SetViewFrame(const DFRAME *viewFrame)
{
  if (!viewFrame)
    return;

  ViewFrame.X1 = viewFrame->X1;
  ViewFrame.X2 = viewFrame->X2;
  ViewFrame.Y1 = viewFrame->Y1;
  ViewFrame.Y2 = viewFrame->Y2;
}

int QDMapViewWindow::GetViewFrame(DFRAME *viewFrame) const
{
  if (!viewFrame)
    return 0;

  viewFrame->X1 = ViewFrame.X1;
  viewFrame->X2 = ViewFrame.X2;
  viewFrame->Y1 = ViewFrame.Y1;
  viewFrame->Y2 = ViewFrame.Y2;

  return 1;
}

int QDMapViewWindow::GetPictureSize(int *pictureWidth,  int *pictureHeight)
{
  if (!pictureWidth || !pictureHeight)
    return 0;

  if (FrameModeActive)
  {
    double viewFrameInPictureX1 = ViewFrame.X1;
    double viewFrameInPictureY1 = ViewFrame.Y1;
    mapPlaneToPicture(GetMapHandle(), &viewFrameInPictureX1, &viewFrameInPictureY1);

    double viewFrameInPictureX2 = ViewFrame.X2;
    double viewFrameInPictureY2 = ViewFrame.Y2;
    mapPlaneToPicture(GetMapHandle(), &viewFrameInPictureX2, &viewFrameInPictureY2);

    *pictureWidth = abs(viewFrameInPictureX2 - viewFrameInPictureX1);
    *pictureHeight = abs(viewFrameInPictureY2 - viewFrameInPictureY1);
  }
  else
  {
    long int widht, height;
    mapGetPictureSize(GetMapHandle(), &widht, &height);
    *pictureWidth  = widht;
    *pictureHeight = height;
  }
  return 1;
}

int QDMapViewWindow::TranslatePictureRectFromViewFrame(RECT *paintRect)
{
  if (!paintRect)
    return 0;

  if (FrameModeActive)
  {
    double viewFrameInPictureX1 = ViewFrame.X1;
    double viewFrameInPictureY1 = ViewFrame.Y1;
    mapPlaneToPicture(GetMapHandle(), &viewFrameInPictureX1, &viewFrameInPictureY1);

    double viewFrameInPictureX2 = ViewFrame.X2;
    double viewFrameInPictureY2 = ViewFrame.Y2;
    mapPlaneToPicture(GetMapHandle(), &viewFrameInPictureX2, &viewFrameInPictureY2);

    paintRect->left += viewFrameInPictureX1;
    if (paintRect->bottom + viewFrameInPictureX1 < viewFrameInPictureX2)
      paintRect->right += viewFrameInPictureX1;
    else
      paintRect->right = viewFrameInPictureX2;
    paintRect->top += viewFrameInPictureY1;
    if (paintRect->bottom + viewFrameInPictureY1 < viewFrameInPictureY2)
      paintRect->bottom += viewFrameInPictureY1;
    else
      paintRect->bottom = viewFrameInPictureY2;
  }
  return 1;
}

int QDMapViewWindow::TranslatePicturePointFromViewFrame(POINT *point)
{
  if (!point)
    return 0;

  if (FrameModeActive)
  {
    double viewFrameInPictureX1 = ViewFrame.X1;
    double viewFrameInPictureY1 = ViewFrame.Y1;
    mapPlaneToPicture(GetMapHandle(), &viewFrameInPictureX1, &viewFrameInPictureY1);

    point->x += viewFrameInPictureX1;
    point->y += viewFrameInPictureY1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
// Перерисовать район
// -----------------------------------------------------------------------------
void QDMapViewWindow::Repaint()
{
  if (GetMapHandle() == 0 || hScreen == 0)
    return;

  POINT point1;
  RECT rect;

  point1.x    = 0;
  point1.y    = 0;
  rect.left   = VarHorzPos;
  rect.right  = VarHorzPos + MapViewPort->width();
  rect.top    = VarVertPos;
  rect.bottom = VarVertPos + MapViewPort->height();
  TranslatePictureRectFromViewFrame(&rect);

  mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

  MapViewPort->update();
}

void QDMapViewWindow::Repaint(int x, int y, int width, int height)
{
  if (GetMapHandle() == 0 || hScreen == 0) return;
  if (width < 0 || height < 0) return;

  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (abs(x - VarHorzPos) > MapViewPort->width()) return;
  if (abs(y - VarVertPos) > MapViewPort->height()) return;
  if (x < VarHorzPos) x = VarHorzPos;
  if (y < VarVertPos) y = VarVertPos;

  if (width > MapViewPort->width()) width = MapViewPort->width();
  if (height > MapViewPort->height()) height = MapViewPort->height();
  if (x + width > VarHorzPos + MapViewPort->width())
    width = width - ((x + width) - (VarHorzPos + MapViewPort->width()));
  if (y + height > VarVertPos + MapViewPort->height())
    height = height - ((y + height) - (VarVertPos + MapViewPort->height()));

  POINT point1;
  RECT rect;

  point1.x    = x - VarHorzPos;
  point1.y    = y - VarVertPos;
  rect.left   = x;
  rect.right  = rect.left + width;
  rect.top    = y;
  rect.bottom = rect.top + height;

  mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

  MapViewPort->update(x - VarHorzPos, y - VarVertPos, width, height);
}

// -----------------------------------------------------------------------------
// Перерисовка окна средствами ядра  mapacces
// -----------------------------------------------------------------------------
void QDMapViewWindow::PaintOfMapAcces(QPainter* viewportPainter)
{
  // Начинаем отрисовку - сделать скролл-бары неактивными,
  // чтобы не мешать
  VScrollBar->setEnabled(false);
  HScrollBar->setEnabled(false);
  // Проверить условие, что ничего не рисуется в данный момент
  if (GetMapHandle() && IsPainting == 0)
  {
    // Установить флаг отрисовки
    IsPainting = 1;

    if (VarRendering && VarMapVisible)
    {
      POINT point1;
      point1.x    = 0;
      point1.y    = 0;

      RECT rect;
      rect.left   = VarHorzPos;
      rect.right  = VarHorzPos + MapViewPort->width();
      rect.top    = VarVertPos;
      rect.bottom = VarVertPos + MapViewPort->height();
      TranslatePictureRectFromViewFrame(&rect);

      // Установим текущий контекст отображения заново
      SetViewSelectNow(VarViewSelect, FALSE);

      // Проверить размеры буфера изображения
      if (mapGetImageWidth(hScreen) < MapViewPort->width() || mapGetImageHeight(hScreen) < MapViewPort->height())
      {
        mapChangeImageSizeEx(hScreen, 1, MapViewPort->width(), MapViewPort->height());
        mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);
      }

      // Делаем дополнительную перерисовку в памяти -
      // так как  изменился контекст отображения
      if (VarViewSelect->VarEdit == true)
        mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

      RECT maprect;
      maprect.left     = RectDraw.left() + HScrollBar->value();
      maprect.top      = RectDraw.top() + VScrollBar->value();
      maprect.right    = RectDraw.right() + HScrollBar->value();
      maprect.bottom   = RectDraw.bottom() + VScrollBar->value();
      TranslatePictureRectFromViewFrame(&maprect);

      VarMapRec        = maprect;
      MapBeforePaint(this, 0, maprect);

      if (VarViewSelected)
        DoViewSelect(hScreen, &rect, VarViewSelect, VarColorSelected);
      if (VarObjSelect)
        DoViewSelect(hScreen, &rect, VarObjSelect, VarColorSelected);

      // Работа с HIMAGE 2-й буфер
      // Скопировать изображение во второй буфер
      mapClearImageObjects(hScreen, 0);

      // Подсветить объекты во второй буфер
      PaintLightObject(hScreen);

      // Переход на работу с QImage
      // Обновить буфер изображения, скопировав его из Screen
      UpdateScreenImageBuffer(hScreen);

      // Вызвать MapAction AFTER PAINT для QMapComponents
      AllMapRef(ACTAFTERPAINT);

      // Вызвать активный обработчик QDGtkAction с событием AfterPaint
      MapAfterPaint(this, &ScreenImageBuffer, maprect);

      // Отобразить принятый буфер
      viewportPainter->drawImage(0, 0, ScreenImageBuffer);

      // Сигнал для пользователя об окончании отрисовки карты
      emit SignalAfterPaint(viewportPainter, maprect.left, maprect.top,
                            maprect.right - maprect.left,
                            maprect.bottom - maprect.top);
    }
  }

  // Снять флаг отрисовки
  IsPainting = 0;
  // Восстановить активность скролл-баров
  VScrollBar->setEnabled(true);
  HScrollBar->setEnabled(true);
}

// -----------------------------------------------------------------------------
// Перерисовка окна
// -----------------------------------------------------------------------------
void QDMapViewWindow::drawContents(QPainter* p, int cx, int cy, int cw, int ch)
{
  if (VarMapVisible)
  {
    if (GetMapHandle() && VarRendering)
    {
      if (VarPaintEnabled)
      {
        RectDraw.setLeft(cx);
        RectDraw.setTop(cy);
        RectDraw.setRight(cx + cw);
        RectDraw.setBottom(cy + ch);

        PaintOfMapAcces(p);
        return;
      }
    }
  }
  else
  {
    // Очистить экран
    QBrush brush( Qt::gray );
    p->setBrush( brush );
    p->drawRect( cx, cy, cw, ch );
    VarPaintEnabled = 0;
  }
}

// -----------------------------------------------------------------------------
// Установить/запросить флаг отображения карты
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetMapVisible(bool visible)
{
  if (visible == VarMapVisible)
    return;

  VarMapVisible = visible;
  if (GetMapHandle() == 0)
    return;

  VarPaintEnabled = 0;
  if (VarMapVisible)
    MapViewPort->show();
  else
    MapViewPort->hide();

  VarPaintEnabled = 1;
  Repaint();
}

bool QDMapViewWindow::GetMapVisible()  const
{
  return VarMapVisible;
}

// -----------------------------------------------------------------------------
// Обработка событий изменения состава карт на уровне MapViewWindow
// -----------------------------------------------------------------------------
void QDMapViewWindow::MapAction(int typeaction)
{
  int  OldPlace;

  // Разберемся с начала с собой
  switch (typeaction)
  {
    case ACTBEFORECLOSE :
      if (VarObjSelect)
      {
        delete VarObjSelect;
        VarObjSelect = NULL;
      }

      if (VarViewSelect != NULL) VarViewSelect->MapAction(ACTBEFORECLOSE);
      memset((char *)&VarMapFrame, 0, sizeof(VarMapFrame));
      SetMapVisible(FALSE);
      break;

    case ACTAFTEROPEN :

      if (GetMapHandle() == 0) SetMapVisible(FALSE);
      else
      {
        POINT point;
        RECT  RectDraw;

        if (mapGetMapType(GetMapHandle()) == SEANAUTIC)
        {
          if (CallS57Setup != 0) delete CallS57Setup;
          CallS57Setup = new TCallS57Setup();
        }

        if (VarViewSelect != NULL) VarViewSelect->MapAction(ACTAFTEROPEN);

        VarPaintEnabled = 0;
        VarHorzPos = 0;
        VarVertPos = 0;
        HScrollBar->setValue(VarHorzPos);
        VScrollBar->setValue(VarVertPos);
        HScrollBar->setMaximum(mapGetPictureWidth(GetMapHandle()) - MapViewPort->width());
        VScrollBar->setMaximum(mapGetPictureHeight(GetMapHandle()) - MapViewPort->height());

        if (hScreen) mapCloseImage(hScreen);
        hScreen = mapCreateImageEx(MapViewPort->width(),
                                   MapViewPort->height());

        point.x = 0;
        point.y = 0;
        RectDraw.left   = 0;
        RectDraw.top    = 0;
        RectDraw.right  = MapViewPort->width();
        RectDraw.bottom = MapViewPort->height();

        mapDrawImageMap(hScreen, GetMapHandle(), &RectDraw, &point);
        VarPaintEnabled = 1;

        OldPlace = VarViewSelect->VarMapPoint->VarPlaceOut;
        VarViewSelect->VarMapPoint->VarPlaceOut = VarPlace;
        VarViewSelect->VarMapPoint->VarX = mapGetMapX1(GetMapHandle());
        VarViewSelect->VarMapPoint->VarY = mapGetMapY1(GetMapHandle());
        VarViewSelect->VarMapPoint->GetPoint(&VarMapFrame.X1, &VarMapFrame.Y1);
        VarViewSelect->VarMapPoint->VarX = mapGetMapX2(GetMapHandle());
        VarViewSelect->VarMapPoint->VarY = mapGetMapY2(GetMapHandle());
        VarViewSelect->VarMapPoint->GetPoint(&VarMapFrame.X2, &VarMapFrame.Y2);
        VarViewSelect->VarMapPoint->VarPlaceOut = OldPlace;
      } // if FMap = 0
      break;

    case ACTAFTERCHANGEBORDER :
      GetMapLeftTopPlane(&MapLeftTopPlane.X, &MapLeftTopPlane.Y);
      UpdatePictureBorder();
      RestoreMapPosition();
      break; // acAfterChangeBorder
  }
}

// -----------------------------------------------------------------------------
// Обработка событий изменения состава карт и их отображения
// на уровне MapViewWindow
// -----------------------------------------------------------------------------
void QDMapViewWindow::AllMapRef(int typeaction)
{
  //  Обработаем сами
  MapAction(typeaction);

  // Сообщим во все TMapComponent
  int count = VarMapObjects.count();

  for (int i = 0; i < count; i++)
  {
    if (VarMapObjects.at(i))
      (VarMapObjects.at(i))->MapAction(typeaction);
  }

  // Сообщим некоторым не TMapComponent но имеющим MapAction
  if (VarViewSelect  != NULL)
    VarViewSelect->MapAction(typeaction);
}

// -----------------------------------------------------------------------------
// Запросить/установить текущий масштаб отображения
// -----------------------------------------------------------------------------
int QDMapViewWindow::GetViewScale()
{
  if (!GetMapHandle())
    return 0;

  return mapGetShowScale(GetMapHandle());
}

int QDMapViewWindow::SetViewScale(int value)
{
  long int x, y;
  int      oldscale;
  bool     VarRend = VarRendering;
  POINT    point1;
  RECT     rect;
  int mapWidth, mapHeight;

  if (!GetMapHandle())
    return 0;

  oldscale = mapGetShowScale(GetMapHandle());
  if (oldscale == value)
    return value;

  // Проверить условие, что ничего не рисуется в данный момент
  if(IsPainting == 0)
  {
    VarRendering = false;
    // Установить флаг отрисовки
    IsPainting = 1;
    // Начинаем отрисовку - сделать скролл-бары неактивными,
    // чтобы не мешать
    HScrollBar->setEnabled(false);
    VScrollBar->setEnabled(false);

    if (VarMapVisible)
    {
      x = HScrollBar->value() + MapViewPort->width() / 2;
      y = VScrollBar->value() + MapViewPort->height() / 2;
    }
    else
    {
      x = VarHorzPos + MapViewPort->width() / 2;
      y = VarVertPos + MapViewPort->height() / 2;
    }

    mapSetViewScale(GetMapHandle(), &x, &y, value);

    VarHorzPos = x - MapViewPort->width() / 2;
    if (VarHorzPos < 0) VarHorzPos = 0;
    VarVertPos = y - MapViewPort->height() / 2;
    if (VarVertPos < 0) VarVertPos = 0;

    GetPictureSize(&mapWidth, &mapHeight);

    VarPaintEnabled = 0;
    if (mapWidth - MapViewPort->width() > 0)
      HScrollBar->setMaximum(mapWidth - MapViewPort->width());
    else
      HScrollBar->setMaximum(0);
    if (VarHorzPos > HScrollBar->maximum())
    {
      HScrollBar->setValue(HScrollBar->maximum());
      VarHorzPos = HScrollBar->maximum();
    }
    else
      HScrollBar->setValue(VarHorzPos);

    if (mapHeight - MapViewPort->height() > 0)
      VScrollBar->setMaximum(mapHeight - MapViewPort->height());
    else
      VScrollBar->setMaximum(0);
    if (VarVertPos > VScrollBar->maximum())
    {
      VScrollBar->setValue(VScrollBar->maximum());
      VarVertPos = VScrollBar->maximum();
    }
    else
      VScrollBar->setValue(VarVertPos);

    qApp->processEvents();

    VarPaintEnabled = 1;

    point1.x = point1.y = 0;
    rect.left   = VarHorzPos;
    rect.top    = VarVertPos;
    rect.right  = VarHorzPos + MapViewPort->width();
    rect.bottom = VarVertPos + MapViewPort->height();
    TranslatePictureRectFromViewFrame(&rect);
    mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

    VarRendering = VarRend;

    IsPainting = 0;

    if (VarRendering) MapViewPort->update();
    return mapGetShowScale(GetMapHandle());
  }
  return 0;
}

// -----------------------------------------------------------------
// Установить масштаб относительно текущей точки
// value - реальный масштаб
// -----------------------------------------------------------------
int QDMapViewWindow::SetViewScaleInPoint(int value)
{
  RECT  rClient;
  POINT MousePos;
  int   MOUSEMOVE = 1;     // Флаг перемещения мыши
  DOUBLEPOINT dis;         // Позиция мыши в районе в дискретах
  int      oldscale;
  bool     VarRend = VarRendering;
  POINT    point1;
  RECT     rect;
  int mapWidth, mapHeight;

  if (!GetMapHandle())
    return 0;

  oldscale = mapGetShowScale(GetMapHandle());
  if (oldscale == value)
    return value;

  // Проверить условие, что ничего не рисуется в данный момент
  if(IsPainting == 0)
  {
    VarRendering = false;            // 07/05/15 Савелов
    // Установить флаг отрисовки
    IsPainting = 1;
    //----------------------------------------------
    // Позиция мыши на экране
    MessageHandler(0, MH_GETCURSORPOS, (long int)&MousePos, 0, 0);

    // Преобразование к координатам клиентной области
    MessageHandler(0, MH_SCREENTOCLIENT, (long int)&MousePos, 0, 0);

    // Клиентная область
    MessageHandler(0, MT_MAPWINPORT, MWP_GETCLIENTRECT, (long int)&rClient, 0);

    // Если мышь не в клиентной области - мышь не смещать
    if (MousePos.x < 0 || MousePos.x >= rClient.RT ||
        MousePos.y < 0 || MousePos.y >= rClient.DN)
    {
      MOUSEMOVE = 0;

      // Установить координаты мыши в середину клиентной области окна
      MousePos.x = (rClient.RT - rClient.LT)/2;
      MousePos.y = (rClient.DN - rClient.UP)/2;
    }

    // Преобразование к координатам района
    MousePos.x = HScrollBar->value() + MousePos.x;
    MousePos.y = VScrollBar->value() + MousePos.y;

    if (MOUSEMOVE)
    {
      dis.x = MousePos.x; dis.y = MousePos.y;
      mapPictureToMap(GetMapHandle(), &(dis.x), &(dis.y));
      mapMapToPlane(GetMapHandle(), &(dis.x), &(dis.y));
    }

    long int mousex = MousePos.x;
    long int mousey = MousePos.y;

    int move = mapSetViewScale(GetMapHandle(), &mousex, &mousey, value);

    MousePos.x = mousex;
    MousePos.y = mousey;


    if (move == 0)
    {
      // Восстановить флаг отрисовки
      IsPainting = 0;
      return 0;
    }

    VarHorzPos = MousePos.x - MapViewPort->width() / 2;
    if (VarHorzPos < 0) VarHorzPos = 0;
    VarVertPos = MousePos.y - MapViewPort->height() / 2;
    if (VarVertPos < 0) VarVertPos = 0;

    GetPictureSize(&mapWidth, &mapHeight);

    VarPaintEnabled = 0;
    if (mapWidth - MapViewPort->width() > 0)
      HScrollBar->setMaximum(mapWidth - MapViewPort->width());
    else
      HScrollBar->setMaximum(0);

    if (VarHorzPos > HScrollBar->maximum())
    {
      HScrollBar->setValue(HScrollBar->maximum());
      VarHorzPos = HScrollBar->maximum();
    }
    else
      HScrollBar->setValue(VarHorzPos);

    if (mapHeight - MapViewPort->height() > 0)
      VScrollBar->setMaximum(mapHeight - MapViewPort->height());
    else
      VScrollBar->setMaximum(0);
    if (VarVertPos > VScrollBar->maximum())
    {
      VScrollBar->setValue(VScrollBar->maximum());
      VarVertPos = VScrollBar->maximum();
    }
    else
      VScrollBar->setValue(VarVertPos);

    qApp->processEvents();

    VarPaintEnabled = 1;

    point1.x = point1.y = 0;
    rect.left   = VarHorzPos;
    rect.top    = VarVertPos;
    rect.right  = VarHorzPos + MapViewPort->width();
    rect.bottom = VarVertPos + MapViewPort->height();
    TranslatePictureRectFromViewFrame(&rect);

    mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

    VarRendering = VarRend;
    // 15/09/14 Железняков В.: восстановить флаг отрисовки
    IsPainting = 0;
    if (VarRendering) MapViewPort->update();

    if (MOUSEMOVE && VarMapVisible && VarRend)
    {
      mapPlaneToMap(GetMapHandle(), &(dis.x),&(dis.y));
      mapMapToPicture(GetMapHandle(), &dis.x, &dis.y);
      MousePos.x = dis.x;
      MousePos.y = dis.y;

      // Преобразование к координатам клиентной области
      MousePos.x = MousePos.x - HScrollBar->value();
      MousePos.y = MousePos.y - VScrollBar->value();

      // Клиентная область (меняется при удалении линеек скроллинга
      MessageHandler(0, MT_MAPWINPORT, MWP_GETCLIENTRECT, (long int)&rClient, 0);

      // Поместить мышь в клиентную область
      MousePos.x = std::max(16L,
                       std::min((long)(MousePos.x),
                           (long)(rClient.RT-rClient.LT-16L)));
      MousePos.y = std::max(16L,
                       std::min((long)(MousePos.y),
                           (long)(rClient.DN-rClient.UP-16L)));

      // Преобразование к координатам экрана
      MessageHandler(0, MH_CLIENTTOSCREEN, (long int)&MousePos, 0, 0);

      // Установить мышь
      MessageHandler(0, MH_SETCURSORPOS, (long int)&MousePos, 0, 0);
    }

    return mapGetShowScale(GetMapHandle());
  }
  return 0;
}

//------------------------------------------------------------------
// Установить масштаб для отображения всей карты в окне
//------------------------------------------------------------------
int QDMapViewWindow::SetScaleAllMapInWindow()
{
  if (GetMapHandle())
  {
    // Запросить размеры карты
    int mapWidth = 0;
    int mapHeight = 0;
    GetPictureSize(&mapWidth, &mapHeight);

    // Вычислить отношение размеров карты к размерам экрана
    double heightRatio = (double)mapHeight/(double)MapViewPort->height();
    double widthRatio = (double)mapWidth/(double)MapViewPort->width();

    // Выбрать наибольший коэффициент
    double scaleMultiplier = widthRatio;
    if (heightRatio > widthRatio)
      scaleMultiplier = heightRatio;

    // Установить новое значение масштаба
    int isScaleSet = SetViewScale(GetViewScale()*scaleMultiplier);

    if (VarMapVisible && VarRendering)
      Repaint();

    return isScaleSet;
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Запросить/установить верхнюю левую позицию начала отображения
// -----------------------------------------------------------------------------
void QDMapViewWindow::GetMapLeftTop(int *left, int *top)
{
  *left = VarHorzPos;
  *top = VarVertPos;
}

void QDMapViewWindow::SetMapLeftTop(int left, int top)
{
  POINT    point1;
  RECT     rect;

  // Проверить условие, что ничего не рисуется в данный момент
  if(IsPainting == 0)
  {
    // Установить флаг отрисовки
    IsPainting = 1;
    // Начинаем отрисовку - сделать скролл-бары неактивными,
    // чтобы не мешать
    VScrollBar->setEnabled(false);
    HScrollBar->setEnabled(false);

    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (left > HScrollBar->maximum()) left = HScrollBar->maximum();
    if (top > VScrollBar->maximum()) top = VScrollBar->maximum();

    VarHorzPos = left;
    VarVertPos = top;

    HScrollBar->setValue(VarHorzPos);
    VScrollBar->setValue(VarVertPos);

    qApp->processEvents();
    VarHorzPos = HScrollBar->value();
    VarVertPos = VScrollBar->value();

    point1.x = point1.y = 0;
    rect.left   = VarHorzPos;
    rect.top    = VarVertPos;
    rect.right  = VarHorzPos + MapViewPort->width();
    rect.bottom = VarVertPos + MapViewPort->height();

    TranslatePictureRectFromViewFrame(&rect);
    mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

    // восстановить флаг отрисовки
    IsPainting = 0;
    MapViewPort->update();
  }
}

// -----------------------------------------------------------------------------
// Запросить координаты текущей точки
// -----------------------------------------------------------------------------
POINT QDMapViewWindow::GetPicturePoint()
{
  return PicturePoint;
}

DOUBLEPOINT QDMapViewWindow::GetMapPoint()
{
  DOUBLEPOINT mapPoint;
  mapPoint.X = PicturePoint.x;
  mapPoint.Y = PicturePoint.y;
  ConvertMetric(&mapPoint.X, &mapPoint.Y, PP_PICTURE, PP_MAP);

  return mapPoint;
}

DOUBLEPOINT QDMapViewWindow::GetPlanePoint()
{
  DOUBLEPOINT planePoint;
  planePoint.X = PicturePoint.x;
  planePoint.Y = PicturePoint.y;
  ConvertMetric(&planePoint.X, &planePoint.Y, PP_PICTURE, PP_PLANE);

  return planePoint;
}

// -----------------------------------------------------------------------------
// Добавить/удалить компонент в список компонентов
// -----------------------------------------------------------------------------
void QDMapViewWindow::AppendMapObject(QDMapComponent *mapcomponent)
{
  if (mapcomponent == NULL)
    return;

  if (VarMapObjects.contains(mapcomponent) == true)
    return;  // уже есть такой

  VarMapObjects.append(mapcomponent);
}

void QDMapViewWindow::DeleteMapObject(QDMapComponent *mapcomponent)
{
  for (int i = 0; i < VarMapObjects.size(); i++)
  {
    if (VarMapObjects.at(i) == mapcomponent)
    {
      VarMapObjects.removeAt(i);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
// Определить габариты всего набора карт в пикселах
// -----------------------------------------------------------------------------
RECT QDMapViewWindow::GetRegionRect()
{
  RECT      FRect;
  long int  iWidth, iHeight;

  memset((char *)&FRect, 0, sizeof(FRect));
  if (GetMapHandle() != 0)
  {
    // получим максимальные габариты всего набора карт
    mapGetPictureSize(GetMapHandle(), &iWidth, &iHeight);
    FRect.right  = iWidth;
    FRect.bottom = iHeight;
  }

  return FRect;
}

// -----------------------------------------------------------------------------
// Определить габариты всего набора карт в указанной системе координат
// -----------------------------------------------------------------------------
MAPDFRAME QDMapViewWindow::GetRegionDFrame(int pointplace)
{
  MAPDFRAME dframe;

  memset((char *)&dframe, 0, sizeof(dframe));
  if (GetMapHandle() == 0) return dframe;

  dframe.X1 = mapGetMapX1(GetMapHandle());
  dframe.X2 = mapGetMapX2(GetMapHandle());
  dframe.Y1 = mapGetMapY1(GetMapHandle());
  dframe.Y2 = mapGetMapY2(GetMapHandle());

  dframe    = VarViewSelect->VarMapPoint->ConvertDFrame(dframe, PP_PLANE, pointplace);
  return dframe;
}

// -----------------------------------------------------------------------------
// конвертировать координаты из dframein система координат placein
// в систему координат placeout
// -----------------------------------------------------------------------------
MAPDFRAME QDMapViewWindow::ConvertDFrame(MAPDFRAME dframein, int placein, int placeout)
{
  MAPDFRAME dframe;

  memset((char *)&dframe, 0, sizeof(dframe));
  if (!VarViewSelect) return dframe;
  if (GetMapHandle() == 0) return dframe;

  dframe = VarViewSelect->VarMapPoint->ConvertDFrame(dframein, placein, placeout);
  return dframe;
}

// -----------------------------------------------------------------------------
// Условия отображения
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetViewSelect(QDMapSelect *value)
{
  if (value == VarViewSelect)
    return;
  VarViewSelect->Assign(value);

  if (VarMapVisible && VarRendering)
    Repaint();
}

// -----------------------------------------------------------------------------
// Установить текщий Select для отображения
// always = true - не надо контролировать QDMapSelect.SelectNew
// mapselect = NULL очистка системного ViewSelect
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetViewSelectNow(QDMapSelect *mapselect, bool forceUpdate)
{
  HSELECT hSelectH;
  HSITE   hSiteH;

  if (GetMapHandle() == 0) return;

  if (mapselect == NULL) // надо очистить системный ViewSelect
  {
    for (int ii = 0; ii <= mapGetSiteCount(GetMapHandle()); ii++)
    {
      hSiteH   = mapGetSiteIdent(GetMapHandle(), ii);
      hSelectH = mapCreateSiteSelectContext(GetMapHandle(), hSiteH);
      mapSetSiteViewSelect(GetMapHandle(), hSiteH, hSelectH);
      mapDeleteSelectContext(hSelectH);
    }

    VarViewSelectLast = NULL;
    return;
  }

  if (!VarSelecting)
    return;

  if (VarViewSelectLast == mapselect)
  {
    if ((!forceUpdate) && (!mapselect->VarEdit))
      return;
  }

  mapselect->VarEdit = FALSE;
  VarViewSelectLast  = mapselect;

  for (int ii = 0; ii <= mapGetSiteCount(GetMapHandle()); ii++)
  {
    mapSetSiteViewSelect(GetMapHandle(), mapGetSiteIdent(GetMapHandle(), ii),
                         mapselect->GetHSelect(ii));
  }
}

// -----------------------------------------------------------------------------
// Выделить объекты карты, удовлетворяющие QDMapSelect
// -----------------------------------------------------------------------------
void QDMapViewWindow::DoViewSelect(HIMAGE image, RECT * rect, QDMapSelect *select, COLORREF color)
{
  POINT tempPoint;
  tempPoint.x = 0;
  tempPoint.y = 0;

  for (int ii = 0; ii <= GetSiteCount(); ii++)
  {
    if (select->GetMapSites(ii) == TRUE)
    {
      mapDrawImageSelect(image, GetMapHandle(), rect, &tempPoint,
                         select->GetHSelect(ii), color, 1 );
    }
  }
}

// -----------------------------------------------------------------------------
// Установить параметры группового выделения объектов для ViewSelect
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetViewSelected(bool view)
{
  if (view == VarViewSelected)
    return;
  VarViewSelected  = view;

  if (VarMapVisible && VarRendering)
    Repaint();
}

// -----------------------------------------------------------------------------
// Разрешить/запретить отображение карты в соответсвии с контекстом
// условий отображения
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetSelecting(bool value)
{
  if (value == VarSelecting)
    return;
  VarSelecting = value;
  if (GetMapHandle() == 0)
    return;
  SetViewSelectNow(VarViewSelect, true);

  if (!VarSelecting)
    SetViewSelectNow(NULL, true);
  else
    VarViewSelect->VarEdit = TRUE;
  if (VarMapVisible && VarRendering)
    Repaint();
}

// -----------------------------------------------------------------------------
// Установить параметры группового выделения объетов, не используя VarViewSelect
// objselect - параметры выделения объектов карты если надо выделить,
//             NULL, если надо снять выделение;
// color - цвет выделения;
// repaint - надо ли вызвать перерисовку карты;
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetObjSelected(QDMapSelect *objselect, COLORREF color,
                                     bool repainted)
{
  if ((objselect == NULL) && (VarObjSelect))
  {                                      // хотим снять выделение
    delete VarObjSelect;
    VarObjSelect = NULL;
  }

  if (objselect)
  { // хотим установить выделение
    if (!VarObjSelect)
      VarObjSelect = new QDMapSelect(GetMapViewAcces());
    VarObjSelect->Assign(objselect);
    VarColorSelected = color;
  }
  if (repainted && VarMapVisible && VarRendering) Repaint();
}

// -----------------------------------------------------------------------------
// Выполнить настройку на новые габариты изображения района
// Возвращает true, если габариты действительно изменились
// -----------------------------------------------------------------------------
bool QDMapViewWindow::UpdatePictureBorder()
{
  long int FX, FY;
  POINT    point1;
  RECT     rect;
  int mapWidth, mapHeight;

  if (GetMapHandle() == 0)
    return false;

  FX = VarHorzPos;
  FY = VarVertPos;

  mapChangeViewScale(GetMapHandle(), &FX, &FY, 1);

  GetPictureSize(&mapWidth, &mapHeight);

  VarPaintEnabled = 0;
  if (mapWidth - MapViewPort->width() > 0)
    HScrollBar->setMaximum(mapWidth - MapViewPort->width());
  else
    HScrollBar->setMaximum(0);
  if (FX > HScrollBar->maximum())
  {
    HScrollBar->setValue(HScrollBar->maximum());
    FX = HScrollBar->maximum();
  }
  else
    HScrollBar->setValue(FX);

  if (mapHeight - MapViewPort->height() > 0)
    VScrollBar->setMaximum(mapHeight - MapViewPort->height());
  else
    VScrollBar->setMaximum(0);
  if (FY > VScrollBar->maximum())
  {
    VScrollBar->setValue(VScrollBar->maximum());
    FY = VScrollBar->maximum();
  }
  else
    VScrollBar->setValue(FY);

  qApp->processEvents();

  VarPaintEnabled = 1;

  VarHorzPos = FX;
  VarVertPos = FY;

  point1.x = point1.y = 0;
  rect.left   = VarHorzPos;
  rect.top    = VarVertPos;
  rect.right  = VarHorzPos + MapViewPort->width();
  rect.bottom = VarVertPos + MapViewPort->height();
  TranslatePictureRectFromViewFrame(&rect);

  mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);
  MapViewPort->update();

  return true;
}

// -----------------------------------------------------------------------------
// Восстановить положение карты
// -----------------------------------------------------------------------------
bool QDMapViewWindow::RestoreMapPosition()
{
  if (!GetMapHandle())
    return false;

  SetMapLeftTopPlane(&MapLeftTopPlane.X, &MapLeftTopPlane.Y);

  return true;
}

// -----------------------------------------------------------------------------
// Преобразовать координаты точки
// из пикселей на карте (PP_PICTURE) в пикселы клиентского окна
// -----------------------------------------------------------------------------
POINT QDMapViewWindow::ConvertFromPictureToClient(POINT mapPoint)
{
  int viewportLeft, viewportTop;
  GetMapLeftTop(&viewportLeft, &viewportTop);

  mapPoint.x -= viewportLeft;
  mapPoint.y -= viewportTop;
  return mapPoint;
}

// -----------------------------------------------------------------------------
// Преобразовать координаты точки
// из пикселей на карте (PP_PICTURE) в пикселы клиентского окна
// При ошибке возвращает ноль
// -----------------------------------------------------------------------------
int QDMapViewWindow::ConvertFromPictureToClient(int *mapPointX, int *mapPointY)
{
  if (!mapPointX || !mapPointY)
    return 0;

  int viewportLeft, viewportTop;
  GetMapLeftTop(&viewportLeft, &viewportTop);

  *mapPointX -= viewportLeft;
  *mapPointY -= viewportTop;
  return 1;
}

int QDMapViewWindow::ConvertFromPictureToClient(double *mapPointX, double *mapPointY)
{
  if (!mapPointX || !mapPointY)
    return 0;

  int viewportLeft, viewportTop;
  GetMapLeftTop(&viewportLeft, &viewportTop);

  *mapPointX -= viewportLeft;
  *mapPointY -= viewportTop;
  return 1;
}

// -----------------------------------------------------------------------------
// Установить положение полос прокрутки и перерисовать карту
// -----------------------------------------------------------------------------
bool QDMapViewWindow::SetScrollBarsPositionAndRepaint(int leftPicture, int topPicture)
{
  if (!GetMapHandle())
    return false;

  VarPaintEnabled = 0;

  // Установить значение полос прокрутки
  HScrollBar->setValue(leftPicture);
  VScrollBar->setValue(topPicture);

  // Обновить горизонтальную и вертикальную позицию
  VarHorzPos = HScrollBar->value();
  VarVertPos = VScrollBar->value();

  qApp->processEvents();

  VarPaintEnabled = 1;

  Repaint();

  return true;
}

// -----------------------------------------------------------------------------
// Обработчик сообщений
// mapViewWindow - контекст окна
// code    - код сообщения
// wp      - WPARAM
// lp      - LPARAM
// typemsg - тип 0 - SendMessage/ 1 - PostMessage - у нас всегда 0
// -----------------------------------------------------------------------------
long int QDMapViewWindow::MessageHandler(long int mapViewWindow,
                                         long int code, long int wp,
                                         long int lp, long int typemsg)
{
  int        retcode = 0;
  QPoint     pos;
  QPoint     pos1;
  POINT    * point;

  switch(code)
  {
    case MT_MAPWINPORT:
      retcode = DoMapWindowMsg(wp,lp);
      break;
    case MT_GETMAPHANDLE:
      if (wp != 0)
      {
        *((long int *)wp) = (long int)GetMapHandle();
        retcode = 1;
      }
      break;
    case MT_CHANGEDATA:
      if (UpdatePictureBorder())
      {
        RestoreMapPosition();
        AllMapRef(ACTDOREPAINTFORCHANGEDATA);
        retcode = 1;
      }
      break;
    case WM_MOVEDOC  :
      retcode = MoveMap(wp,lp);
      break;
    case MH_SCREENTOCLIENT:
      if (wp == 0) return 0;
      point = (POINT *) wp;
      pos.setX(point->x);
      pos.setY(point->y);
      pos1 = MapViewPort->mapFromGlobal(pos);
      point->x = pos1.x();
      point->y = pos1.y();
      retcode = MH_SCREENTOCLIENT;
      break;
    case MH_CLIENTTOSCREEN:
      if (wp == 0) return 0;
      point = (POINT *) wp;
      pos.setX(point->x);
      pos.setY(point->y);
      pos1 = MapViewPort->mapToGlobal(pos);
      point->x = pos1.x();
      point->y = pos1.y();
      retcode = MH_CLIENTTOSCREEN;
      break;
    case MH_GETCURSORPOS:      // Запросить позицию курсора
      if (!wp) return 0;
      point = (POINT *)(wp);
      pos = MapViewPort->cursor().pos();
      point->x = pos.x();
      point->y = pos.y();
      retcode = MH_GETCURSORPOS;
      break;
    case MH_SETCURSORPOS:      // Установить позицию курсора
      if (!wp) return 0;
      point = (POINT *)(wp);
      MapViewPort->cursor().setPos(point->x, point->y);
      retcode = MH_SETCURSORPOS;
      break;
    case WM_COMMAND:
      if (wp == CM_T3DSTOP)
      {
        AllMapRef(CM_T3DSTOP);
        retcode = CM_T3DSTOP;
      }
      break;
    case AW_OPENDOC:
      if (wp != 0)
      {
        char *name = (char *)wp;

        // Установить новое имя файла карты
        ((QDMapView *)this)->SetMapFileName(MainCodec->toUnicode(name));
        ((QDMapView *)this)->SetMapActive(TRUE);
        ((QDMapView *)this)->SetMapVisible(TRUE);
      }
    case AW_OPENDOCUN:
      if (wp != 0)
      {
        WCHAR *name = (WCHAR *)wp;

        // Установить новое имя файла карты
        ((QDMapView *)this)->SetMapFileName(QString::fromUtf16(name));
        ((QDMapView *)this)->SetMapActive(TRUE);
        ((QDMapView *)this)->SetMapVisible(TRUE);
      }
      break;

  }

  return retcode;
}

// -----------------------------------------------------------------
//  Обработать сообщение окну карты
// -----------------------------------------------------------------
int QDMapViewWindow::DoMapWindowMsg(long int code, long int param)
{
  POINT        * point;
  RECT         * rect;
  QRect          QClientRect;
  long           x,y;

  switch(code)
  {
    default :
      return 0;

    case MWP_GETCLIENTRECT :

      if (param == 0) return 0;
      rect = (RECT *) param;
      QClientRect = MapViewPort->rect();
      rect->left = QClientRect.left();
      rect->right = QClientRect.right();
      rect->top = QClientRect.top();
      rect->bottom = QClientRect.bottom();
      break;

    case MWP_INVALIDATERECT :
      if (param)
      {
        rect = (RECT *) param;
        QClientRect.setLeft(rect->left);
        QClientRect.setRight(rect->right);
        QClientRect.setTop(rect->top);
        QClientRect.setBottom(rect->bottom);

        Repaint(QClientRect.left(), QClientRect.top(),
                QClientRect.right() - QClientRect.left(),
                QClientRect.bottom() - QClientRect.top());
        break;
      }

    case MWP_INVALIDATE :
      Repaint();
      break;

    case MWP_UPDATE :

      if (param)
      {
        rect = (RECT *) param;
        QClientRect.setLeft(rect->left);
        QClientRect.setRight(rect->right);
        QClientRect.setTop(rect->top);
        QClientRect.setBottom(rect->bottom);
      }
      else
      {
        QClientRect = MapViewPort->rect();
      }

      MapViewPort->update(QClientRect);
      break;

    case MWP_SCROLLBY :
      // Скроллинг окна
      if (param == 0) return 0;
      point = (POINT *) param;
      HScrollBar->setValue(VarHorzPos + point->x);
      VScrollBar->setValue(VarVertPos + point->y);
      break;
    case MWP_SETPICTUREPOINT :

      // Установить координаты верхнего левого окна карты
      // Если окно только создано
      if (param == 0) return 0;
      point = (POINT *) param;
      if (point->x < 0)    x = 0;
      else                 x = point->x;
      if (point->y < 0)    y = 0;
      else                 y = point->y;
      HScrollBar->setValue(x);
      VScrollBar->setValue(y);

      // нужно ли перемещать карту?
      break;

    case MWP_GETPICTUREPOINT :
    case WM_LEFTTOPPOINT :

      // Запросить координаты верхнего левого окна карты
      if (param == 0) return 0;
      point = (POINT *) param;
      point->x = HScrollBar->value();
      point->y = VScrollBar->value();
      break;
    case MWP_SETWINDOWTEXT :
      if (param == 0) return 0;
      else
      {
        QString qstring = QString::fromLocal8Bit((char *)param);
      }
      break;
    case MWP_CANSELECTOBJECT :
      return 1;

    case MWP_LIGHTOBJECT :
      StartLightObject((HOBJ)param);
      break;
    case MWP_UNLIGHTOBJECT :
      EndLightObject();
      break;
  }

  return MT_MAPWINPORT;
}

// -----------------------------------------------------------------------------
//  Переместить карту в заданную точку
// -----------------------------------------------------------------------------
int QDMapViewWindow::MoveMap(long int wparam, long int lparam)
{
  POINT PicturePoint;
  RECT rect = {0, 0};
  POINT * point = (POINT *)lparam;

  if (point == 0)
  {
    return 0;
  }

  MessageHandler(0, MT_MAPWINPORT, MWP_GETCLIENTRECT, (long int)&rect, 0);

  PicturePoint.x = point->x - (rect.RT + rect.LT)/2;
  PicturePoint.y = point->y - (rect.DN + rect.UP)/2;

  SetPicturePoint(PicturePoint);

  return WM_MOVEDOC;
}

// -----------------------------------------------------------------------------
// Установить координаты левого верхнего угла карты
// -----------------------------------------------------------------------------
void QDMapViewWindow::SetPicturePoint(POINT& point)
{
  MapWindowPort(MWP_SETPICTUREPOINT,(long int)&point);
}

// ---------------------------------------------------------------
// Отправить сообщение окну карты
// ---------------------------------------------------------------
int QDMapViewWindow::MapWindowPort(int command, long int parm)
{
  return MessageHandler(0, MT_MAPWINPORT, command, parm, 0);
}

// ---------------------------------------------------------------
// Разрешить/запретить перерисовку карты
// ---------------------------------------------------------------
void QDMapViewWindow::SetRendering(bool value)
{
  VarRendering = value;
}

bool QDMapViewWindow::GetRendering()
{
  return VarRendering;
}

// ---------------------------------------------------------------
// Конвертировать координаты из  системы координат placein
// в систему координат placeout
// ---------------------------------------------------------------
int QDMapViewWindow::ConvertMetric(double *x, double *y, int placein, int placeout)
{
  if (GetMapHandle() == 0) return 0;

  return VarViewSelect->VarMapPoint->ConvertMetric(x, y, placein, placeout);
}

// ---------------------------------------------------------------
// Установить текущий обработчик событий
// ---------------------------------------------------------------
bool QDMapViewWindow::SetCurrentAction(QDGtkAction *gtkaction)
{
  DeleteAction(VarCurrentGtkAction, true);

  VarCurrentGtkAction = gtkaction;

  return true;
}

// ---------------------------------------------------------------
// Удалить текущий обработчик событий
// actionquery - надо ли сначала запросить разрешение
// ---------------------------------------------------------------
bool QDMapViewWindow::DeleteAction(QDGtkAction *gtkaction, bool actionquery)
{
  bool result = false;

  if (VarCurrentGtkAction == NULL)
    return result;

  if (actionquery)
    if (!VarCurrentGtkAction->DoStopActionQuery())
      return result;

  VarCurrentGtkAction->DoStopAction();

  VarCurrentGtkAction = NULL;

  return result = true;
}

//------------------------------------------------------------
// Вызывается перед отрисовкой карты
//------------------------------------------------------------
void QDMapViewWindow::MapBeforePaint(QObject *sender, HWND canvas, RECT mappaintrect)
{
  if (VarCurrentGtkAction)         // есть обработчик
    VarCurrentGtkAction->MapBeforePaint(sender, canvas, mappaintrect);
}

//------------------------------------------------------------
// Вызывается после отрисовки карты
//------------------------------------------------------------
void QDMapViewWindow::MapAfterPaint(QObject *sender, QImage * mapImage, RECT mappaintrect)
{
  if (VarCurrentGtkAction)   // есть обработчик
    VarCurrentGtkAction->MapAfterPaint(sender, mapImage, mappaintrect);
}

QImage *QDMapViewWindow::GetScreenImageBuffer()
{
  return &ScreenImageBuffer;
}

void QDMapViewWindow::UpdateScreenImageBuffer(HIMAGE hScreen)
{
  long imageWidth = mapGetImageWidth(hScreen);
  long imageHeight = mapGetImageHeight(hScreen);
  if (ScreenImageBuffer.width() != imageWidth || ScreenImageBuffer.height() != imageHeight)
  {
    // Reallocate image
    ScreenImageBuffer = QImage(imageWidth, imageHeight, QImage::Format_RGB32);
  }

  if (ScreenImageBuffer.isNull() == false)
  {
    XIMAGEDESC ximage;
    ximage.Height = ScreenImageBuffer.height();
    ximage.Width = ScreenImageBuffer.width();
    ximage.RowSize = ScreenImageBuffer.bytesPerLine();
    ximage.Depth = ScreenImageBuffer.depth();
    ximage.CellSize = ScreenImageBuffer.bytesPerLine() / ScreenImageBuffer.width();
    ximage.Point = (char*)ScreenImageBuffer.bits();

    if (!IsViewFrameActive())
      mapViewImageToXImage(hScreen, &ximage, 0, 0);
    else
    {
      HMAP mapHandle = GetMapHandle();

      int backgroundColor = mapGetBackColor(mapHandle);
      if (mapGetViewType(mapHandle) == VT_PRINTRST)
        backgroundColor = mapGetBackPrintColor(mapHandle);
      ScreenImageBuffer.fill(backgroundColor);

      DFRAME viewFrame;
      GetViewFrame(&viewFrame);
      mapPlaneToPicture(mapHandle, &viewFrame.X1, &viewFrame.Y1);
      mapPlaneToPicture(mapHandle, &viewFrame.X2, &viewFrame.Y2);

      RECT paintRect;
      paintRect.left = 0;
      paintRect.top = 0;
      paintRect.right = abs(ceil(viewFrame.X1 - viewFrame.X2));
      paintRect.bottom = abs(ceil(viewFrame.Y1 - viewFrame.Y2));

      mapViewImageToXImage(hScreen, &ximage, &paintRect, 0);
    }
  }
}

//------------------------------------------------------------
// Слоты на изменение положения ползунка скролл-бара
//------------------------------------------------------------
void QDMapViewWindow::SlotHBarChanged(int x)
{
  POINT point1;
  RECT rect;

  if (GetMapHandle() == 0) return;
  if (VarPaintEnabled == 0 || VarHorzPos == x) return;

  // Проверить условие, что ничего не рисуется в данный момент
  if(IsPainting == 0)
  {
    // Установить флаг отрисовки
    IsPainting = 1;
    // Начинаем отрисовку - сделать скролл-бары неактивными,
    // чтобы не мешать
    VScrollBar->setEnabled(false);
    HScrollBar->setEnabled(false);
    //----------------------------------------------
    if (abs(x - VarHorzPos) < MapViewPort->width())
      mapScrollImage(hScreen, x - VarHorzPos, 0);

    if (x - VarHorzPos > 0)
    {
      if ((x - VarHorzPos) < MapViewPort->width())
      {
        point1.x   = MapViewPort->width() - (x - VarHorzPos);
        rect.left  = VarHorzPos + MapViewPort->width();
        rect.right = rect.left + (x - VarHorzPos);
      }
      else
      {
        point1.x   = 0;
        rect.left  = x;
        rect.right = rect.left + MapViewPort->width();
      }
    }
    else
    {
      if ((x - VarHorzPos) < MapViewPort->width())
      {
        point1.x   = 0;
        rect.left  = x;
        rect.right = VarHorzPos;
      }
      else
      {
        point1.x   = 0;
        rect.left  = x;
        rect.right = rect.left + MapViewPort->width();
      }
    }
    point1.y    = 0;
    rect.top    = VarVertPos;
    rect.bottom = VarVertPos + MapViewPort->height();

    TranslatePictureRectFromViewFrame(&rect);
    mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

    VarHorzPos = x;

    IsPainting = 0;
    MapViewPort->update();
  }
}

void QDMapViewWindow::SlotVBarChanged(int y)
{
  POINT point1;
  RECT rect;

  if (GetMapHandle() == 0) return;
  if (VarPaintEnabled == 0 || VarVertPos == y) return;

  if(IsPainting == 0)
  {
    // Установить флаг отрисовки
    IsPainting = 1;
    // Начинаем отрисовку - сделать скролл-бары неактивными,
    // чтобы не мешать
    VScrollBar->setEnabled(false);
    HScrollBar->setEnabled(false);
    //----------------------------------------------
    //
    if (abs(y - VarVertPos) < MapViewPort->height())
      mapScrollImage(hScreen, 0, y - VarVertPos);

    if (y - VarVertPos > 0)
    {
      if ((y - VarVertPos) < MapViewPort->height())
      {
        point1.y    = MapViewPort->height() - (y - VarVertPos);
        rect.top    = VarVertPos + MapViewPort->height();
        rect.bottom = rect.top  + (y - VarVertPos);
      }
      else
      {
        point1.y    = 0;
        rect.top    = y;
        rect.bottom = rect.top + MapViewPort->height();
      }
    }
    else
    {
      if ((VarVertPos - y) < MapViewPort->height())
      {
        point1.y    = 0;
        rect.top    = y;
        rect.bottom = VarVertPos;
      }
      else
      {
        point1.y    = 0;
        rect.top    = y;
        rect.bottom = rect.top + MapViewPort->height();
      }
    }
    point1.x   = 0;
    rect.left  = VarHorzPos;
    rect.right = VarHorzPos + MapViewPort->width();

    TranslatePictureRectFromViewFrame(&rect);
    mapDrawImageMap(hScreen, GetMapHandle(), &rect, &point1);

    VarVertPos = y;
    // Восстановить значение флага
    IsPainting = 0;
    // Отрисовать карту после масштабирования
    MapViewPort->update();
    //----------------------------------------------
  }
}



//------------------------------------------------------------
// Запрос габаритов окна отображения карты
//------------------------------------------------------------
int QDMapViewWindow::GetVisibleWidth()
{
  int w = MapViewPort->width();

  return w;
}

int QDMapViewWindow::GetVisibleHeight()
{
  int h = MapViewPort->height();

  return h;
}

//-------------------------------------------------------------------
// Установить/получить текущую позицию карты в метрах
//-------------------------------------------------------------------
bool QDMapViewWindow::SetMapLeftTopPlane(double *leftPlane, double *topPlane)
{
  if (!GetMapHandle() || !leftPlane || !topPlane )
    return false;

  // Установить  текущую позицию
  MapLeftTopPlane.X = *leftPlane;
  MapLeftTopPlane.Y = *topPlane;

  // Пересчитать координаты в пикселы
  double leftPicture = MapLeftTopPlane.X;
  double topPicture = MapLeftTopPlane.Y;
  mapPlaneToPicture(GetMapHandle(), &leftPicture, &topPicture);

  // Обновить положение полос прокрутки и перерисовать карту
  SetScrollBarsPositionAndRepaint(leftPicture, topPicture);

  return true;
}

bool QDMapViewWindow::GetMapLeftTopPlane(double *leftPlane, double *topPlane)
{
  if (!GetMapHandle() || !leftPlane || !topPlane )
   return false;

  *leftPlane = VarHorzPos;
  *topPlane = VarVertPos;

  mapPictureToPlane(GetMapHandle(), leftPlane, topPlane);

  return true;
}

//------------------------------------------------------------
// Начать подсвечивать объект карты
//------------------------------------------------------------
void QDMapViewWindow::StartLightObject(HOBJ info)
{
  if ( VarHighLightingEnabled )
    EndLightObject();

  HighLightingObject = info;
  VarHighLightingEnabled = true;
  VarHighLightingState = true;  // Начать выделенным

  if (HighLightTimer == 0)
  {
    HighLightTimer = new QTimer(this);
    connect(HighLightTimer, SIGNAL(timeout()), this, SLOT(HighLightTimerEvent()));
    HighLightTimer->setInterval(500);
    HighLightTimer->start();
  }

  update();
}

void QDMapViewWindow::EndLightObject()
{
  VarHighLightingEnabled = false;

  if (HighLightTimer != 0)
  {
    HighLightTimer->stop();
    delete HighLightTimer;
    HighLightTimer = 0;
  }

  update();
}

void QDMapViewWindow::PaintLightObject(HIMAGE hScreen)
{
  if (VarHighLightingEnabled == true)
  {
    if (VarHighLightingState == true)
    {
      IMGLINE objectImage;
      memset(&objectImage, 0, sizeof(objectImage));
      objectImage.Color = 0x000000;
      objectImage.Thick = PIX2MKM(2);

      PAINTPARM objectView;
      memset(&objectView, 0, sizeof(objectView));
      objectView.Image = IMG_LINE;
      objectView.Parm = (char*) &objectImage;
      objectView.Mode = R2_COPYPEN;

      mapDrawImageMapObject(hScreen, GetMapHandle(),
                            &objectView, HighLightingObject);
    }
  }
}

void QDMapViewWindow::HighLightTimerEvent()
{
  if (VarHighLightingEnabled == true)
  {
    VarHighLightingState = (VarHighLightingState == false);
    update();
  }
}

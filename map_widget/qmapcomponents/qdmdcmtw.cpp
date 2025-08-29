#include "mapapi.h"
#include "mtrexapi.h"
typedef struct MTRPARM
{
  BUILDMTW BuildMtw;

  PRIORMTRPARM PriorMtrParm;

  RECT   RectMetr;          // Прямоугольник вывода в районе
                            // (в целых метрах)

  DFRAME DFrameMetr;        // Прямоугольник вывода в районе
                            // (в плавающих метрах)

  char Select;              // Флаг выбора и вывода фрагмента
                            //  (см. MAP2MTROUTPUTAREA)

  char CoordinateSystem;    // Флаг системы координат при задании
                            // области вывода (см. MAP2MTRCOORDINATESYSTEM)

  char SelectSheetName;     // Флаг выбора имен матриц по листам

  char Compress;            // Флаг выполнения сжатия

  char BuildBySheets;       // Создание матриц по листам района

  char UseFilter;           // Флаг использования фильтра объектов (IMH-файл)

                            // Флаг получения "Абсолютного уровня моря" из семантики моря
  char GetAbsSeaLevelFromSemantic;

  char IsPriorMtrParm;      // Флаг выполнения предварительной оценки матрицы
                            //  0 - оценка не выполнялась;
                            //  1 - вычислена абсолютная величина разности высот объектов,
                            //      попадающих в элементы матрицы (PriorMtrParm.AbsHeightDifference)

  HWND MapWindowHandle;     // Идентификатор окна карты

  int AbsoluteHeightCode;   // Код х-ки Абс.высота
  int RelativeHeightCode;   // Код х-ки Отн.высота

  int AbsoluteHeightSign;   // Знак х-ки Абс.высота
  int DepthCode;            // Код х-ки Глубина

  int SeaCode;              // Код объекта "море"
  int IsoCode;              // Код объекта "изобата"
  int PntCode;              // Код объекта "отметки глубин"
  int DepthProcess;         // Флаг "Обработать глубины"

  int SaveUnitedMarine;     // Флаг "Сохранить объединённую (морскую) карту"  11/11/19
  int Free;

  double AbsSeaLevel;       // Абсолютный уровень моря

  WCHAR NameMTR[MAX_PATH_LONG];   // Имя MTW-файла
  WCHAR NameTXT[MAX_PATH_LONG];   // Имя IMH-файла
}
  MTRPARM;
#define ABSOLUTE_HEIGHT_CODE 4  // Код х-ки АБСОЛЮТНАЯ ВЫСОТА
#define RELATIVE_HEIGHT_CODE 1  // Код х-ки ОТНОСИТЕЛЬНАЯ ВЫСОТА
/******** QDMDCMTW.CPP *********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2021              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR Qt-DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*        КОМПОНЕНТ - ДИАЛОГ "СОЗДАНИЯ МАТРИЦЫ ВЫСОТ"               *
*                                                                  *
*******************************************************************/
#include <algorithm>
#include "qdmdcmtw.h"
#include "mapapi.h"

#include "maplib.h"

// Главный обработчик сообщений
long int MessageHandler(long int context, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ КЛАСС ДИАЛОГА                                +
//+++ "ДИАЛОГ СОЗДАНИЯ МАТРИЦЫ ВЫСОТ"              +
//++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapCreateMtw::QDMapCreateMtw(QWidget *parent, const char *name)
               :QDMapComponent(parent, name)
{
   hide();
   AllRectActive = true;
   MapRect.UP = 0;
   MapRect.LT = 0;
   MapRect.DN = 0;
   MapRect.RT = 0;
}

//------------------------------------------------------------------
// Указатель на карту
//------------------------------------------------------------------
QDMapView *QDMapCreateMtw::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapCreateMtw::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void QDMapCreateMtw::InitTaskParm(TASKPARM *taskparm, void *mtrparm)
{
 if (!taskparm || !mtrparm) return;

  QDMapView   *MapView = GetMapView();
 // Заполнение параметров прикладной задачи
 memset((char *)taskparm, 0, sizeof(TASKPARM));
 taskparm->Language  = ML_RUSSIAN;
 taskparm->Handle = &MessageHandler;

 MTRPARM* MtrParm = (MTRPARM*)mtrparm;
 memset(MtrParm,0,sizeof(MTRPARM));

 // Область вывода - весь район
 MtrParm->Select = 0;

 // Результирующий рельеф - абсолютный
 MtrParm->BuildMtw.ReliefType = 0;

 // Высота при наложении - максимальная
 MtrParm->BuildMtw.HeightSuper = 1;

 // Pазмер элемента в байтах
 MtrParm->BuildMtw.ElemSizeBytes = 4;

 // Eдиница измерения высоты (миллиметры)
 MtrParm->BuildMtw.Unit = 3;//TMtr::mm;

 // Кол-во метров на элемент в плане - 0.5 мм карты
 MtrParm->BuildMtw.ElemSizeMeters =
         std::max(0.01, (float)mapGetMapScale(MapView->GetMapHandle())/2000.0);

 // Имя выходного MTR-файла и фильтра объектов
 if (mapIsActive(MapView->GetMapHandle()))
 {
   WCHAR drive[MAX_PATH];
   WCHAR dir  [MAX_PATH];
   WCHAR file [MAX_PATH];
   WCHAR ext  [MAX_PATH];
   WCHAR mainNameUn[MAX_PATH_LONG];

   mapGetMainNameUn(MapView->GetMapHandle(), mainNameUn, sizeof(mainNameUn));
   ::SplitThePath(mainNameUn, drive, dir, file, ext);
   StringToUnicode(".mtw",ext, sizeof(ext));
   ::MakeThePath(MtrParm->NameMTR, drive, dir, file, ext);
   StringToUnicode(".imh",ext, sizeof(ext));
   ::MakeThePath(MtrParm->NameTXT, drive, dir, file, ext);
 }

 // Использовать фильтр объектов
 MtrParm->UseFilter = 1;

 // Режим быстрого построения
 MtrParm->BuildMtw.FastBuilding = 1;

 // Режим формирования экстремумов
 MtrParm->BuildMtw.Extremum = 0;

 // Режим ограничения области вывода
 // Габариты матрицы ограничиваются габаритами района
 MtrParm->BuildMtw.LimitMatrixFrame = 1;

 if (AllRectActive)
 {
   // Область вывода - весь район
   MtrParm->Select = 0;

   // Определение габаритов района в метрах
   long Xmax,Ymax; // Габариты района в пикселах
   mapGetPictureSize(MapView->GetMapHandle(), &Xmax, &Ymax);

   double x = 0, y = 0; // Габариты района в метрах
   mapPictureToPlane(MapView->GetMapHandle(), &x, &y);
   MtrParm->RectMetr.LT = y;
   MtrParm->RectMetr.UP = x;

   x = Xmax;
   y = Ymax;
   mapPictureToPlane(MapView->GetMapHandle(), &x, &y);
   MtrParm->RectMetr.RT = y;
   MtrParm->RectMetr.DN = x;
 }
 else
 {
   // Область вывода из Rect
   MtrParm->Select = 2;// AREA_SELECT_FRAME

   double x = 0, y = 0;

   x = MapRect.LT;
   y = MapRect.UP;
   mapPictureToPlane(MapView->GetMapHandle(), &x, &y);
   MtrParm->RectMetr.LT = y;
   MtrParm->RectMetr.UP = x;
   x = MapRect.RT;
   y = MapRect.DN;
   mapPictureToPlane(MapView->GetMapHandle(), &x, &y);
   MtrParm->RectMetr.RT = y;
   MtrParm->RectMetr.DN = x;
 }

 // Без запроса имен матриц по листам
 MtrParm->SelectSheetName = 0;

 // Система координат - прямоугольная (метры)
 MtrParm->CoordinateSystem = 0;

 // Коды характеристик :
 //   Абсолютная высота, Относительная высота
 MtrParm->AbsoluteHeightCode = ABSOLUTE_HEIGHT_CODE;
 MtrParm->RelativeHeightCode = RELATIVE_HEIGHT_CODE;

 // Метод построения поверхности - средневзвешенная интерполяция,поиск по 8 направлениям
 MtrParm->BuildMtw.Method = 2;

 // Не использовать трехмерную метрику объектов
 MtrParm->BuildMtw.NotUse3DMetric = 1;

 // Создать объект "рамка матрицы"
 if ((MtrParm->BuildMtw.Border =
      mapCreateObject(MapView->GetMapHandle(),1,IDDOUBLE2,0)) == 0)
 {
   return;
 }

 MtrParm->MapWindowHandle = (HWND)GetMapView();
}


//------------------------------------------------------------------
// Выполнить диалог
//------------------------------------------------------------------
void QDMapCreateMtw::Execute()
{
 const char *NameFunc  = "LoadMap2MtrDialog";

 long int   (WINAPI * LoadMap2MtrDialog)(HMAP hMap,TASKPARM* taskparm,MTRPARM* mtrparm);

 HINSTANCE   libinst;
 QDMapView   *MapView;
 TASKPARM    TaskParm;
 MTRPARM     MtrParm;

 MapView = GetMapView();
 if (MapView          == NULL) return;
 if (MapView->GetMapHandle() == 0) return;

 if (mapGetListCount(MapView->GetMapHandle()) == 0 &&
     mapGetSiteCount(MapView->GetMapHandle()) == 0)
 {
   return;
 }

 LoadMap2MtrDialog = (long int (WINAPI *)(HMAP hMap,
                                          TASKPARM* taskparm,MTRPARM* mtrparm))
               mapLoadLibrary(MAPQTFRMLIB , &libinst, NameFunc);
 if (LoadMap2MtrDialog == NULL) return;

 // Заполнение параметров прикладной задачи
 InitTaskParm(&TaskParm, (void *)&MtrParm);

 LoadMap2MtrDialog(MapView->GetMapHandle(), &TaskParm, &MtrParm);

 // Выгрузка библиотеки
 if (libinst)    mapFreeLibrary(libinst);

 // Удалить объект "рамка матрицы"
 if (MtrParm.BuildMtw.Border)
 {
    mapFreeObject(MtrParm.BuildMtw.Border);
    MtrParm.BuildMtw.Border = 0;
 }

 return;
}

//------------------------------------------------------------------
// Установить размеры области карты в пикселах
//------------------------------------------------------------------
void QDMapCreateMtw::SetMapRect(int left, int top, int right, int bottom)
{
  MapRect.UP = top;
  MapRect.LT = left;
  MapRect.DN = bottom;
  MapRect.RT = right;

  GetMapView()->TranslatePictureRectFromViewFrame(&MapRect);
}

//------------------------------------------------------------------
// Установить\Запросить значение флага выбора области карты s
//------------------------------------------------------------------
void QDMapCreateMtw::SetAllRectActive(bool value)
{
  AllRectActive = value;
}

bool QDMapCreateMtw::GetAllRectActive() const
{
 return AllRectActive;
}

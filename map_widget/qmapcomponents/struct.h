/****  STRUCT.H *************** Solovieva C.A.    **  12/04/04 *****
*****  STRUCT.H *************** Dishlenko S.G.    **  09/10/14 *****
*****  STRUCT.H *************** Derevyagina Zh.A. **  15/05/06 *****
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
*                  FOR  WINDOWS95 & WINDOWS NT                     *
*                                                                  *
********************************************************************
*                                                                  *
*               ОПИСАНИЕ БАЗОВЫХ СТРУКТУР                          *
*                                                                  *
*******************************************************************/
#if !defined(STRUCT_H)
#define STRUCT_H

#ifndef GIS_DESIGNER
  #ifndef MTACTION_H
    #include "mtaction.h"
  #endif
#else
  #include "mapapi.h"

  class TMapAccess;
  class TMapWindow;
#endif


// Данные
 // 08/04/04
 typedef struct
 {
  double      ResolutionGor;            // метр.эл гориз
  double      ResolutionVer;            // метр.эл вертик
  double      Scale;                    // масштаб
  long int    Length;                   // размер структуры
  char        NameInputPost[MAX_PATH];  // имя выходного файла
  char        NamePrinter[MAX_PATH];    // имя принтера                // 09/10/14
  char        PrintPage[MAX_PATH];      // номера страниц
  long int    CutListFlag;              // 1-5 - однолистовая печать   // 05/12/08
                                        //  1 - A0;...; 5 - A4
                                        // -1-(-5) печать по листам
                                        // -1 - A0;...;-5 - A4
  long int    Resolution;               // точки на дюйм
  long int    FormatPage;               // Формат страницы:
                                        // 0 - A0; 1 - A1; 2 - A2; 3 - A3; 4 - A4
  TMapWindow *MapWindow;                // окно обрабатываемой карты    // 15/05/06
 }
 SCRIPTPARAM;


 typedef struct
 {
  long int    Length;                   // размер структуры
  long int    Rezerv;
  char        NameInputPost[MAX_PATH];  // имя выходного файла
  char        NameMap[MAX_PATH];        // имя карты
  char        PrintPage[MAX_PATH];      // номера страниц
  char        PrintPageCopy[MAX_PATH];  //

 }
 NAMESCRIPT;
 // Общие параметры
  typedef struct
  {
   double     Scale;               //  базовый масштаб
   double     Resolution;          //  разрешение
   double     MeterInElementGor;   //  метр.эл гор
   double     MeterInElementVer;   //  метр.эл вер
   double     MeterGor;            //  ширина в метрах
   double     MeterVer;            //  высота в метрах
   long int   BitCount;            //  бит.пиксел
   long int   ColorCount;          //  кол-во цветов
   long int   SaveResolution;      //  разрешение экрана
   long int   SaveScale;           //  масштаб тек
   long int   MaxNumberPage;       //  максимальный номер для печати
   long int   MinNumberPage;       //  минимальный номер для печати
 }
  MYPARAM;

// Данные описание параметров
 typedef struct
 {
  double      SizePageMmWidth;    // размер страницы ширина  (мм)
  double      SizePageMmHeight;   // размер страницы высота  (мм)
  double      ResolutionMetGor;   // т/метр гориз
  double      ResolutionMetVer;   // т/метр вертик
  long int    Length;             // размер структуры
  long int    PagesAll;           // кол-во страниц всего
  long int    Left;               // смещение слева для участка
  long int    Top;                // смещение справа для участка
  long int    Weight;             // ширина участка
  long int    Height;             // высота участка
  //    добавлено 04/06/02
  long int    CutListFlag;              // 1-5 - однолистовая печать  // 05/12/08
                                        // 1 - A0;...;5 - A4
                                        // -1-(-5) печать по листам
                                        // -1 - A0;...;-5 - A4
  long int    PageLeft;           // отступ слева    (в пикселах устройства)
  long int    PageRight;          // отступ справа     - "" -
  long int    PageDown;           // отступ снизу      - "" -
  long int    PageUp;             // отступ сверху     - "" -
  long int    PageOverLeft;       // зона перекрытия слева (в пикселах устройства)
  long int    PageOverRight;      // зона перекрытия справа    - "" -
  long int    PageOverDown;       // зона перекрытия снизу     - "" -
  long int    PageOverUp;         // зона перекрытия сверху    - "" -
  long int    PageShiftLeft;      // Смещение слева  (в пикселах устройства)
  long int    PageShiftUp;        // Смещение справа       - "" -
  long int    ResolutionGor;      // разрешение по горизонтали т/д
  long int    ResolutionVer;      // разрешение по вертикали т/д
  long int    Rezerv;             // резерв
  char        Version [80];       // версмя
  char        TrueType[80];       // шрифт
  char        Title[80];          // титул/навание
  char        Date[80];           // дата
  char        Creator[80];        // автор
  char        Adresat[80];        // адресат
 }
 NAMEPARAM;
 // 08/04/04
 typedef struct
 {
  double      SizePageMmWidth;    // размер листа в мм по ширине
  double      SizePageMmHeight;   // размер листа в мм во высоте
  double      dPageCountW;        // истинное неокругленное кол-во страниц по ширине
  double      dPageCountH;        // истинное неокругленное кол-во страниц по высоте
  long int    PageCountW;         // округленное кол-во страниц по ширине
  long int    PageCountH;         // округленное кол-во страниц по высоте
  long int    PageCountWo;        // округленное кол-во страниц по ширине
  long int    PageCountHo;        // округленное кол-во страниц по высоте
  long int    DibHeight;          //
  long int    Weight;             // ширина участка выдачи
  long int    Height;             // высоте участка выдачи
  long int    CountW;             // остаток по ширине
  long int    CountH;             // остаток по высоте
  long int    CountPage;          // кол-во страниц
  long int    NumberPage;         // номер страниц
  long int    CountWDop;          // размер неполного блока по гор
  long int    CountHDop;          // размер неполного блока по вер
  long int    AllPage;
 }
  PARAMLIST;

//

  typedef struct
 {
  DFRAME        FrameTrap;
  DFRAME        ParFrameTrap;
  SCRIPTPARAM   ScriptParam;
  MYPARAM       Myparam;
  NAMESCRIPT    NameScript;
  NAMEPARAM     NameParam;
  PARAMLIST     ParamList;
  TMapAccess *  Doc ;
  HMAP          hMap;
  RECT          RectPixel;
  long int      FlagCancel;
  long int      FlagRun;
  char          Mesage[80];
  char*         lpImagebmp;
  char*         adrlpImagebmp;
  char*         PrintBlock;
  char*         AdrPrintBlock;
 }
 MASSTRUCTSCRIPT;


#endif



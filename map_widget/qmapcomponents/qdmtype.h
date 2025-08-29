/******** QDMTYPE.H ************* Dishlenko S.G.  *****  21/10/16 **
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2016              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR Qt4-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*               Объявления типов и переменных                      *
*                                                                  *
*******************************************************************/

#ifndef QDMTYPE_H
#define QDMTYPE_H

#include "maptype.h"

// Тип возвращаемого параметра для QDMapSelectDialog
enum MAPSELECTDIALOGRETCODE
{
   MRCANCEL  = 0,
   MROK      = 1,
   MRALLMARK = 2
};

// Типы событий для компонент карты
typedef enum MAPACTIONTYPE
{
    ACTBEFOREPAINT            = 1,
    ACTAFTERPAINT             = 2,
    ACTBEFORECLOSE            = 3,
    ACTAFTEROPEN              = 4,
    ACTAFTERAPPENDSITE        = 5,
    ACTBEFOREDELETESITE       = 6,
    ACTDOREPAINTFORCHANGEDATA = 7, // изменились данные и надо перевывести
    ACTAFTERCHANGEBORDER      = 8
} MAPACTIONTYPE;

// Типы операций для компонент карты
typedef enum MAPOPERATION
{
    OPREMOVE           = 1,
} MAPOPERATION;

enum  TSEMANTICSTYLE // Способ выдачи значений семантики объектов карты
{
   SS_NODECODE     = 0,     // Значения не раскодируются
   SS_NORMALDECODE = 1,     // Значения раскодируются
   SS_FULLDECODE   = 2      // Значения раскодируются с добавлением единицы измерения
};

// Типы конвертируемых файлов источников
enum TCONVERTERTYPESOURCE
{
  TS_VIEW      = 0,
  TS_MAP       = 1,
  TS_DIRSXFWIN = 2,
  TS_SXFWIN    = 3,
  TS_S57       = 4,         // 21/10/16
};

// Типы конвертируемых файлов приемников
enum TCONVERTERTYPEDEST
{
  TD_VIEW      = 0,
  TD_MAP       = 1,
  TD_DIRSXFWIN = 2,
  TD_SXFWIN    = 3,
};

// Описание выбранного объекта из классификатора карты
typedef struct
{
   int             NumbSite;  // Номер карты (для основной = 0)
   int             Incode;    // Внутренний код объекта
   int             ExCode;    // КЛАССИФИКАЦИОННЫЙ КОД
   int             Local;     // ХАРАКТЕР ЛОКАЛИЗАЦИИ
   int             Segment;   // НOМЕР СЛОЯ
   char            *Name;     // О©?О©?О©?О©?О©?О©?О©?О©? О©?О©?О©?О©?О©?О©?О©?
   char            *KeyName;  // О©?О©?О©?О©?О©?О©?О©?О©?О©?О©? О©?О©?О©? О©?О©?О©?О©?О©?О©?О©? О©? О©?О©?О©?О©?О©?-О©?О©?
   char            *Key;      // О©?О©?О©?О©?
}
  TMAPOBJECTINFO;


// Загрузка SXF, DIR, S57 в формат MAP
typedef long int (*QDLoadSxf2Map)(char *lpszsource,
                                  char *lpsztarget,
                                  char *lprsctarget,
                                  TASKPARM *parm);
typedef long int (* QDLoadDir2Map)(char* lpszsource,
                                   char* lpsztarget,
                                   TASKPARM* parm);
typedef long int (* QDLoadS57ToMap)(char* lpszsource,     // 21/10/16
                                   char* lpsztarget,
                                   TASKPARM* parm);

// Выгрузка DIR
typedef long int (* QDSaveMap2Dir)(HMAP hmap,
                                   const char* lpsztarget, TASKPARM* parm);
// Выгрузка SXF
typedef long int (* QDSaveMap2Sxf)(HMAP hmap,
                                   const char* lpsztarget, TASKPARM* parm);

#endif // QDMTYPE_H

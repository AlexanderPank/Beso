/****** qdmtxt_en.h  ***********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2020              *
*                     All Rights Reserved                          *
*                                                                  *
********************************************************************
*                                                                  *
*            КОМПЛЕКТ СТРОКОВЫХ ПЕРЕМЕННЫХ БИБЛИОТЕКИ              *
*                                                                  *
*******************************************************************/

// Формирование переменной константы
#define VCONST(_y) _y ## _En

// -----------------------------------------------------------------
// АНГЛИЙСКИЙ ИНТЕРФЕЙС (EN)
// -----------------------------------------------------------------

// qdmwina.cpp
// -------------

const WCHAR* VCONST(main1)    = WTEXT("Ok");
const WCHAR* VCONST(main2)    = WTEXT("No");
const WCHAR* VCONST(main3)    = WTEXT("Cancel");
const WCHAR* VCONST(main4)    = WTEXT("Continue");
const WCHAR* VCONST(main5)    = WTEXT("Retry");
const WCHAR* VCONST(main6)    = WTEXT("Abort");
const WCHAR* VCONST(main7)    = WTEXT("Ignore");


#include "qdmtxt.h"

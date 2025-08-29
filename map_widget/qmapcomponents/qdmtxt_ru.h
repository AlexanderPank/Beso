/****** qdmtxt_ru.h  ***********************************************
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
#define VCONST(_y) _y ## _Ru

// -----------------------------------------------------------------
// РУССКИЙ ИНТЕРФЕЙС (RU)
// -----------------------------------------------------------------

// qdmwina.cpp
// -------------

const WCHAR* VCONST(main1)    = WTEXT("Да");
const WCHAR* VCONST(main2)    = WTEXT("Нет");
const WCHAR* VCONST(main3)    = WTEXT("Отменить");
const WCHAR* VCONST(main4)    = WTEXT("Продолжить");
const WCHAR* VCONST(main5)    = WTEXT("Повторить");
const WCHAR* VCONST(main6)    = WTEXT("Прервать");
const WCHAR* VCONST(main7)    = WTEXT("Пропустить");


#include "qdmtxt.h"

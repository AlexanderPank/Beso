/****** qdmtxt.cpp *************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2020              *
*                     All Rights Reserved                          *
*                                                                  *
********************************************************************
*                                                                  *
*     ФУНКЦИИ ЗАПРОСА ЗНАЧЕНИЙ СТРОКОВЫХ ПЕРЕМЕННЫХ БИБЛИОТЕКИ     *
*                                                                  *
*******************************************************************/

#ifdef WIN32API
 #include <tchar.h>
#endif

#include "mapapi.h"

#ifndef HIDEEN
 #include "qdmtxt_en.h"
#endif
#ifndef PANENGL
#ifndef HIDERU
 #include "qdmtxt_ru.h"
#endif
#endif

// Функция запроса строки - WCHAR
const WCHAR * _qdmtxt(int number)
{
  int language = mapGetMapAccessLanguage();

#ifndef PANENGL
#ifndef HIDERU
  if (language == ML_RUSSIAN)   return _qdmtxt_Ru(number);
#endif
#endif

#ifndef HIDEEN
  return _qdmtxt_En(number);
#else
  return L" ? ";
#endif
}

// Функция запроса строки - char
int _qdmtxt(int number, char * name, int size)
{
  const WCHAR * text = _qdmtxt(number);
  return UnicodeToString(text, name, size);
}

// Функция запроса строки - WCHAR
int _qdmtxt(int number, WCHAR * name, int size)
{
  const WCHAR * text = _qdmtxt(number);
  return (WcsCopy(name, text, size) != 0);
}



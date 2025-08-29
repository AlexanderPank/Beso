/****** qdmtext.h ***************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2020              *
*                     All Rights Reserved                          *
*                                                                  *
********************************************************************
*                                                                  *
*                   ФУНКЦИЯ ЗАПРОСА СТРОКИ                         *
*                                                                  *
*******************************************************************/

#ifndef VCONST

const WCHAR * _qdmtxt(int number);
int _qdmtxt(int number, char * name, int size);
int _qdmtxt(int number, WCHAR * name, int size);

#else

// Функция запроса строки
const WCHAR * VCONST(_qdmtxt)(int number)
{
  switch(number)
  {
    case 1: return VCONST(main1);
    case 2: return VCONST(main2);
    case 3: return VCONST(main3);
    case 4: return VCONST(main4);
    case 5: return VCONST(main5);
    case 6: return VCONST(main6);
    case 7: return VCONST(main7);
  }

  return WTEXT(" ??? ");
}

#undef VCONST
#endif

/******* QDMVIEWA.H ************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                      FOR Qt-DESIGNER                             *
*                                                                  *
********************************************************************
*                                                                  *
*       ОПИСАНИЕ БАЗОВОГО КЛАССА ДОСТУПА К ОБ'ЕКТУ                 *
*                     "ЭЛЕКТРОННАЯ КАРТА"                          *
*                                                                  *
*******************************************************************/

#ifndef QDMVIEWA_H
#define QDMVIEWA_H

#include <QWidget>
#include <QTextCodec>
#include "qdmtype.h"

#include "mapapi.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ БАЗОВЫЙ КЛАСС "ИНТЕРФЕЙС УПРАВЛЕНИЯ ЭЛЕКТРОННОЙ КАРТОЙ" ++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class  QDMapViewAcces : public QWidget
{
    Q_OBJECT

public :
   QDMapViewAcces(QWidget *parent = 0);

   // Получить идентификатор карты
   virtual HMAP GetMapHandle();

   // Получить объект доступа к карте
   virtual QDMapViewAcces *GetMapViewAcces();

   // Получить название класса
   const char * GetClassName();

   // Установить название класса
   void SetClassName(const char *name);

   // Запросить количество открытых сайтов в карте
   long GetSiteCount();

public:
   // Для внутреннего использования
   HMAP VarMapHandle;         // Реальный идентификатор карты
   QTextCodec *MainCodec;
   QTextCodec *MainCodecCp1251;
   char ClassName[260];
};

#endif  // QDMVIEWA


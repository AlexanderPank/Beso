/****** MWHANDLR.H ************** Savelov A.S.  ******* 08/02/17 ***
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2017              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                           FOR  LINUX                             *
*                                                                  *
********************************************************************
*                                                                  *
*            ОБРАБОТКА СООБЩЕНИЙ ОКНА ЭЛЕКТРОННОЙ КАРТЫ            *
*                                                                  *
*******************************************************************/

#ifndef MWHANDLR_H
#define MWHANDLR_H

#define    DT_LOADFROMSXF         1  // загрузить из SXF в MAP
#define    DT_GETTYPEOBJECTCREATE 2  // выбрать тип объекта карты из 
                                     // классификатора для создания
#define    DT_OBJECTINFOBEGIN     3  // создать диалог информации об объекте карты
#define    DT_SITECREATEFORMAP    4  // создать карту пользователя для открытой карты
#define    DT_OBJECTINFODESTROY   5  // удалить диалог информации об объекте карты
#define    DT_CLOSEDATAFORMAP     6  // создать диалог закрытия данных карты
#define    DT_SETVIEWSTAFF        7  // установить состав отображения
#define    DT_SETFINDOBJECTPARAM  8  // установить состав поиска
#define    DT_OBJECTINFOCONTINUE  9  // настроить диалог информации об объекте карты на новые парметры
#define    DT_VIEWDATALIST        10 // диалог состава документа
#define    DT_LOADBMP             11 // диалог загрузки BMP
#define    DT_OBJECTINFOBYSEEK    12 // выбор обьекта по поиску
#define    DT_MEDSEEKOBJECT       13 // Диалог перебора объекто
#define    DT_PALETTER            14 // Диалог управления палитрой
#define    DT_GETSITENUMBER       15 // Диалог выбора номера карты       
#define    DT_ZONEFORSELECT       16 // Диалог построения зоны вокруг выделенных
#define    DT_SETPARMPRINT        17 // Диалог параметров печати
#define    DT_MAP2SXF             18 // Диалог выгрузки в sxf
#define    DT_SORT                19 // Диалог сортировки

#define    MainLinuxIdent 1

#define    MH_GETCURSORPOS        MainLinuxIdent       // 1 Запросить позицию курсора
#define    MH_SETCURSORPOS        MainLinuxIdent + 1   // 2 Установить позицию курсора
#define    MH_SCREENTOCLIENT      MainLinuxIdent + 2   // 3
#define    MH_CLIENTTOSCREEN      MainLinuxIdent + 3   // 4
#define    MH_CLOSE               MainLinuxIdent + 4   // 5 Удалить окно
#define    MH_GETKEYSTATE         MainLinuxIdent + 5   // 6 Запрос о состоянии клавиш
#define    MH_OPENFILE            MainLinuxIdent + 6   // 7 Открыть файл
#define    MH_GETXCONTEXT         MainLinuxIdent + 7   // 8 Запрос графического контекста  // 26/03/02
                                              //   текущего окна карты
#define    MH_AFTERSTART          MainLinuxIdent + 9   // 9 Панораам стартовала надо выполнить настройку меню команд и оболочки
#define    MH_SELVIEWOBJ          MainLinuxIdent +10   // 10 Работа с подсвеченным объектом
                                              // WPARAM = 1 - начать    LPARAM - HOBJ
                                              // WPARAM = 2 - закончить
					      
#endif  // MWHANDLR_H


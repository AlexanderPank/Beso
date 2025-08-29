/******* QDMGSRV.CPP  **********************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*         ОПИСАНИЕ КОМПОНЕНТА "ДОСТУП К ДАННЫМ ГИС СЕРВЕРА"        *
*                                                                  *
*******************************************************************/

#include <qapplication.h>

#include "qdmgsrv.h"
#include "mapapi.h"
#include "maplib.h"

// для динамической загрузки библиотеки
static const char *nf_svGetConnectParameters = "svGetConnectParameters";
static const char *nf_svGetUserData          = "svGetUserData";
static const char *nf_svOpenData             = "svOpenData";
static const char *nf_svAppendData           = "svAppendData";
static const char *nf_svStringToHash         = "svStringToHash";

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ ДОСТУПА К ДАННЫМ ГИС СЕРВЕРА"              ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//-------------------------------------------
//      Конструктор
//-------------------------------------------
QDMapGisServer::QDMapGisServer(QWidget *parent, const char *name)
  :QDMapComponent(parent, name)
{
  MainCodec = QTextCodec::codecForName("KOI8-R");

  svGetConnectParameters = (long int (WINAPI *)(TASKPARMEX *))
      mapLoadLibrary(MAPSCENALIB , &FhDllDlgs, nf_svGetConnectParameters);
  if (svGetConnectParameters != 0)
  {
    svGetUserData = (long int (WINAPI *)(TASKPARMEX *))
        mapGetProcAddress(FhDllDlgs, nf_svGetUserData);
    svOpenData    = (long int (WINAPI *)(TASKPARMEX *))
        mapGetProcAddress(FhDllDlgs, nf_svOpenData);
    svAppendData  = (long int (WINAPI *)(HMAP, TASKPARMEX *))
        mapGetProcAddress(FhDllDlgs, nf_svAppendData);
    svStringToHash= (long int (WINAPI *)(const char * source, char * target, long int size))
        mapGetProcAddress(FhDllDlgs, nf_svStringToHash);
  }
  hide();
}

QDMapGisServer::~QDMapGisServer()
{
  if (FhDllDlgs != 0)
  {
    mapFreeLibrary(FhDllDlgs);
    FhDllDlgs = 0;
  }
}

QDMapView *QDMapGisServer::GetMapView() const
{
  return (QDMapView *)VarMapView;
}

void QDMapGisServer::SetMapView(QDMapView *value)
{
  SetMapViewAcces(value);
}

//-------------------------------------------
// Вызвать диалог ввода параметров пользователя
//-------------------------------------------
int QDMapGisServer::DoDlgOpenData()
{
  TASKPARMEX TaskParm;
  int        Result = 0;
  char       sss1[MAX_PATH], path[MAX_PATH], name[MAX_PATH];

  if (svOpenData == 0) return Result;

  // Заполнение параметров прикладной задачи
  InitTaskParmEx(&TaskParm);
  strcpy(sss1, MainCodec->fromUnicode(qApp->applicationFilePath()).data());
  SplitThePath(sss1, NULL, path, name, NULL);
  MakeThePath(sss1, NULL, path, name, NULL);
  strcat(sss1, ".ini");
  TaskParm.IniName = sss1;

  Result = svOpenData(&TaskParm);

  return Result;
}

//-------------------------------------------
// Запросить имя текущего зарегистрированного пользователя
//-------------------------------------------
const char *QDMapGisServer::GetCurrentUserName()
{
  return mapGetCurrentUserName();
}

//-------------------------------------------
// Проверка активности соединения с ГИС - сервером
// если aUsername = '', то активность определяется по текущему подключенному,
// иначе сравнивается с заданным пользователем
//-------------------------------------------
bool QDMapGisServer::IsGISServerActive(const char *aUsername)
{
  const char *currname;
  bool  result = false;

  currname = mapGetCurrentUserName();
  if (strlen(currname) == 0) return result;
  if (aUsername == 0 || strlen(aUsername) == 0) return result = true;
  if (strcmp(currname, aUsername) == 0) result = true;

  return result;
}

//-------------------------------------------
// Преобразовать строку в хэш по алгоритму MD5
// source - исходная строка ANSI
// target - строка результата (32 символа и замыкающий ноль)
// size   - число байт, зарезервированных в строке (не менее 33)
// При ошибке параметров возвращает ноль
//-------------------------------------------
int QDMapGisServer::StringToHash(char *aSource, char *aTarget, int aSize)
{
  int        Result = 0;

  if ((aSource == 0) || (aTarget == 0) || aSize == 0) return Result;
  if (svStringToHash == 0) return Result;

  Result = svStringToHash((const char *)aSource, aTarget, aSize);

  return Result;
}

// Главный обработчик сообщений
long int MessageHandler(long int mapViewWindow, long int code,
                        long int wp, long int lp, long int typemsg = 0);  // qdmwina.cpp

//----------------------------------------------------
// Заполнить структуру для вызова задачи
//----------------------------------------------------
void QDMapGisServer::InitTaskParmEx(TASKPARMEX *taskparm)
{
  if (taskparm == 0) return;

  // Заполнение параметров прикладной задачи
  memset((char *)taskparm, 0, sizeof(TASKPARMEX));
  taskparm->Language  = mapGetMapAccessLanguage();        // 10/03/10
  taskparm->Handle    = &MessageHandler;
  taskparm->DocHandle = (HWND)GetMapView();
}

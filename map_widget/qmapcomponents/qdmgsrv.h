/******* QDMGSRV.H *************************************************
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

#ifndef QDMGSRV_H
#define QDMGSRV_H

#include "qdmcmp.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ ДОСТУПА К ДАННЫМ ГИС СЕРВЕРА"              ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapGisServer : public QDMapComponent
{
    Q_OBJECT

public  :
    QDMapGisServer(QWidget * parent = 0, const char * name = 0);
    ~QDMapGisServer();

    virtual void SetMapView(QDMapView *value);
    QDMapView    *GetMapView() const;
    bool         IsGISServerActive(const char *aUsername);
    // Вызвать диалог открытия данных
    int          DoDlgOpenData();
    // Запросить имя текущего зарегистрированного пользователя
    const char   *GetCurrentUserName();
    // Преобразовать строку в хэш по алгоритму MD5
    int          StringToHash(char *aSource, char* aTarget, int aSize);
    // Служебная часть
    //--------------------------------------------
    // Заполнить структуру для вызова задачи
    void         InitTaskParmEx(TASKPARMEX *taskparm);

public:
    QTextCodec   *MainCodec;
    HINSTANCE    FhDllDlgs;
    long int     (WINAPI *svGetConnectParameters)(TASKPARMEX *);
    long int     (WINAPI *svGetUserData)(TASKPARMEX *);
    long int     (WINAPI *svOpenData)(TASKPARMEX *);
    long int     (WINAPI *svAppendData)(HMAP, TASKPARMEX *);
    long int     (WINAPI *svStringToHash)(const char * source,
                                          char * target, long int size);
};

#endif // QDMGSRV_H

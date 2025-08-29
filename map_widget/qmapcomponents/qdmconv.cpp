/******** QDMCONV.CPP **********************************************
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
*  Компонент      : QDMapConvertor - конвертор электронной карты   *
*                                                                  *
*******************************************************************/

#include <QTextCodec>

#include "qdmconv.h"
#include "qdmcmp.h"
#include "qdmtype.h"
#include "maplib.h"

//------------------------------------------------------------------
//   КОНСТРУКТОР
//------------------------------------------------------------------
QDMapConvertor::QDMapConvertor(QWidget *parent, const char *name)
               :QDMapComponent(parent, name)
{
  VarSourceName.resize(0);
  VarDestName.resize(0);
  VarSourceType = TS_MAP;
  VarDestType   = TD_MAP;

  hide();
}

//------------------------------------------------------------------
//   ДЕСТРУКТОР
//------------------------------------------------------------------
QDMapConvertor::~QDMapConvertor()
{

}

QDMapView *QDMapConvertor::GetMapView() const
{
 return (QDMapView *)VarMapView;
}

void QDMapConvertor::SetMapView(QDMapView *value)
{
  if (!value || (value == GetMapView())) return;

  SetMapViewAcces(value);
}

void QDMapConvertor::SetSourceName(const QString value)
{
  if (VarSourceType == TS_VIEW) return;

  VarSourceName = value;
}

QString QDMapConvertor::GetSourceName()
{
  return VarSourceName;
}

void QDMapConvertor::SetDestName(const QString value)
{
  if (VarDestType == TD_VIEW) return;

  VarDestName = value;
}

QString QDMapConvertor::GetDestName()
{
  return VarDestName;
}

void QDMapConvertor::SetSourceType(int value)
{
  if (VarSourceType == value) return;
  if (value == TS_VIEW)
  {
    if (!GetMapView()) return;

    VarSourceName = GetMapView()->GetMapFileName();
  }
  else
  if (VarSourceType == TS_VIEW)
  {
    VarSourceName.resize(0);
  }
  VarSourceType = value;
}

int QDMapConvertor::GetSourceType()
{
  return VarSourceType;
}

void QDMapConvertor::SetDestType(int value)
{
  if (VarDestType == value) return;
  if (value == TD_VIEW)
  {
    if (!GetMapView()) return;

    VarDestName = GetMapView()->GetMapFileName();
  }
  else
  if (VarDestType == TD_VIEW)
  {
    VarDestName.resize(0);
  }
  VarDestType = value;
}

int QDMapConvertor::GetDestType()
{
  return VarDestType;
}

int MapConvertThread(QDMapConvertor *parm)
{
  int      ret = 0;
  void     *libinst = 0;
  void     *libinst_s57 = 0;
  FARPROC  Func = 0;
  TASKPARM taskparm;
  QTextCodec *codec = QTextCodec::codecForName("KOI8-R");

  memset((char *)&taskparm, 0x0, sizeof(TASKPARM));
  libinst = dlopen(MAPQTFRMLIB, RTLD_LAZY);
  libinst_s57 = dlopen(MAPS572MAPLIB, RTLD_LAZY);

  if (!libinst)
    return ret;

  switch (parm->VarSourceType)
  {
    case TS_VIEW:
    case TS_MAP:
      switch (parm->VarDestType)
      {
        case TD_DIRSXFWIN:
           Func = (FARPROC)dlsym(libinst, "SaveMap2Dir");
        break;
        case TD_SXFWIN:
           Func = (FARPROC)dlsym(libinst, "SaveMap2Sxf");
        break;
      }
      break;
    case TS_DIRSXFWIN:
      switch (parm->VarDestType)
      {
        case TD_VIEW:
        case TD_MAP:
           Func = (FARPROC)dlsym(libinst, "LoadDir2Map");
        break;
      }
      break;
    case TS_SXFWIN:
      switch (parm->VarDestType)
      {
        case TD_VIEW:
        case TD_MAP:
             Func = (FARPROC)dlsym(libinst, "LoadSxf2Map");
        break;
      }
      break;
    case TS_S57:
      switch (parm->VarDestType)
      {
        case TD_VIEW:
        case TD_MAP:
             if (libinst_s57)
               Func = (FARPROC)dlsym(libinst_s57, "LoadS57ToMap");
        break;
      }
      break;
  }

  if (!Func)
    return ret;

  char VarDestName[MAX_PATH];
  char VarSourceName[MAX_PATH];

  strncpy(VarDestName,   codec->fromUnicode(parm->VarDestName).data(), MAX_PATH);
  strncpy(VarSourceName, codec->fromUnicode(parm->VarSourceName).data(), MAX_PATH);

  switch (parm->VarSourceType)
  {
    case TS_VIEW:
    case TS_MAP:
      switch (parm->VarDestType)
      {
        case TD_DIRSXFWIN:
          ret = ((QDSaveMap2Dir)Func)(parm->GetMapView()->GetMapHandle(),
                                      VarDestName, &taskparm);
        break;
        case TD_SXFWIN:
          ret = ((QDSaveMap2Sxf)Func)(parm->GetMapView()->GetMapHandle(),
                                VarDestName, &taskparm);
        break;
      }
      break;
    case TS_DIRSXFWIN:
      switch (parm->VarDestType)
      {
        case TD_VIEW:
        case TD_MAP:
          ret = ((QDLoadDir2Map)Func)(VarSourceName,
                                      VarDestName, &taskparm);
          break;
      }
      break;
    case TS_SXFWIN:
      switch (parm->VarDestType)
      {
        case TD_VIEW:
        case TD_MAP:
          ret = ((QDLoadSxf2Map) Func)(VarSourceName, VarDestName, 0, &taskparm);
          break;
      }
      break;
    case TS_S57:
      switch (parm->VarDestType)
      {
        case TD_VIEW:
        case TD_MAP:
          ret = ((QDLoadS57ToMap)Func)(VarSourceName,
                                      VarDestName, &taskparm);
          break;
      }
      break;
    }

  parm->VarDestName   = codec->toUnicode(VarDestName);
  parm->VarSourceName = codec->toUnicode(VarSourceName);

  dlclose(libinst);
  dlclose(libinst_s57);
  libinst     = 0;
  libinst_s57 = 0;

  return ret;
}

void QDMapConvertor::Convert()
{
  if (!VarSourceName.compare(VarDestName))
    return;
  if (VarDestType == TD_VIEW)
  {
    QString qstring;

    qstring.resize(0);
    if (GetMapView())
    {
      // Установить новое имя файла карты
      GetMapView()->SetMapFileName(qstring);
      GetMapView()->SetMapActive(FALSE);
      GetMapView()->SetMapVisible(FALSE);
    }
  }
  if (!MapConvertThread(this))
    return;

  if (VarDestType == TD_VIEW)
  {
    if (GetMapView())
    {
      // Установить новое имя файла карты
      GetMapView()->SetMapFileName(VarDestName);
      GetMapView()->SetMapActive(TRUE);
      GetMapView()->SetMapVisible(TRUE);
    }
  }
}


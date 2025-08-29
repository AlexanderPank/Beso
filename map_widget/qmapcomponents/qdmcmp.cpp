/******* QDMCMP.CPP ************************************************
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
*        ОПИСАНИЕ КЛАССА ОТОБРАЖЕНИЯ ЭЛЕКТРОННОЙ КАРТЫ             *
*                                                                  *
*******************************************************************/

#include "qdmcmp.h"
#include "qdmselt.h"
#include "qdmsem.h"
#include "qdmwinw.h"
#include <qapplication.h>

// vc1

#include "mapapi.h"

// vc-site-context-declare

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++     КЛАСС ОТОБРАЖЕНИЯ ЭЛЕКТРОННОЙ КАРТОЙ         +++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------
//   Конструктор
//------------------------------------------------------------------
QDMapView::QDMapView(QWidget *parent) : QDMapViewWindow(parent)
{
    VarMapFileName  = "";
    VarMapLayers    = new QDMapLayers(this, NULL);
    VarMapMtrs      = new QDMapMtrs(this);
    VarMapRsts      = new QDMapRsts(this);
    VarMapSites     = new QDMapSites(this);
    VarViewSelect   = new QDMapSelect(this);
    VarMapSemantics = new QDMapSemantics(this);

    SetClassName("QDMapView");

    // vc-site-context-invoke

}

//------------------------------------------------------------------
//   Деструктор
//------------------------------------------------------------------
QDMapView::~QDMapView()
{
    VarRendering = false;

    SetMapFileName("");
    delete VarMapLayers;
    VarMapLayers = NULL;
    delete VarMapMtrs;
    VarMapMtrs = NULL;
    delete VarMapRsts;
    VarMapRsts = NULL;
    delete VarMapSites;
    VarMapSites = NULL;
    delete VarViewSelect;
    VarViewSelect = NULL;
    delete VarMapSemantics;
    VarMapSemantics = NULL;

    for (int i = 0; i < VarMapObjects.size(); i++)
    {
        (VarMapObjects.at(i))->Notification(this, OPREMOVE);
    }

    for (int i = 0; i < VarMapWindows.size(); i++)
    {
        (VarMapWindows.at(i))->Notification(this, OPREMOVE);
    }

    if (GetMapHandle() != 0)
    {
        mapCloseMap(GetMapHandle());
        VarMapHandle = 0;
    }
}

//------------------------------------------------------------------
//   Запросить имя карты
//------------------------------------------------------------------
QString QDMapView::GetMapName()
{
    QStrName = "";

    if (GetMapHandle() == 0)
        return QStrName;

    if (MainCodec)
        QStrName = MainCodec->toUnicode(mapGetMapName(GetMapHandle()));
    else
        QStrName = QString::fromLocal8Bit(mapGetMapName(GetMapHandle()));

    return QStrName;
}

//------------------------------------------------------------------
//   Запросить номенклатуру карты
//------------------------------------------------------------------
QString QDMapView::GetMapListName(int listnumber)
{
    QStrName = "";

    if (GetMapHandle() == 0)
        return QStrName;
    if (listnumber < 1)
        return QStrName;

    if (MainCodec)
        QStrName = MainCodec->toUnicode(mapGetListName(GetMapHandle(), listnumber));
    else
        QStrName = QString::fromLocal8Bit(mapGetListName(GetMapHandle(), listnumber));

    return QStrName;
}

//------------------------------------------------------------------
//   Запросить базовый масштаб карты
//------------------------------------------------------------------
int QDMapView::GetMapScale()
{
    if (!GetMapHandle()) return 0;

    return mapGetMapScale(GetMapHandle());
}

//------------------------------------------------------------------
//   Запросить тип карты
//------------------------------------------------------------------
int QDMapView::GetMapType()
{
    if (GetMapHandle() == 0) return 0;

    return mapGetMapType(GetMapHandle());
}

QString QDMapView::GetMapFileName() const
{
    return VarMapFileName;
}

//------------------------------------------------------------------
// Установить новое имя файла карты
//------------------------------------------------------------------
void QDMapView::SetMapFileName(const QString qstring)
{
    VarMapFileName.clear();
    VarMapFileName.append(qstring);
}

//------------------------------------------------------------------
//   Запросить высоту,ширину карты в пикселах в текущем масштабе
//------------------------------------------------------------------
void QDMapView::GetMapHW(long int *height, long int *width)
{
    if (GetMapHandle() == 0)
    {
        *height = 0;
        *width = 0;
    }
    else
    {
        *height = mapGetPictureHeight(GetMapHandle());
        *width  = mapGetPictureWidth(GetMapHandle());
    }

}

//------------------------------------------------------------------
// Открыть карту с именем filename
// filename - имя файла карты
// mapview  - в какое значение установить свойство отображения карты
//------------------------------------------------------------------
bool QDMapView::MapOpen(const QString filename, bool mapview)
{
    bool ret = FALSE;

    SetMapFileName(filename);
    if (VarMapFileName == filename)
    {
        ret     = TRUE;
        SetMapActive(TRUE);
        SetMapVisible(mapview);
    }

    return ret;
}

//------------------------------------------------------------------
// Закрыть карту
//------------------------------------------------------------------
void QDMapView::MapClose()
{
    SetMapActive(FALSE);
}

//------------------------------------------------------------------
//  Открыть/закрыть доступ к картографическим данным
//------------------------------------------------------------------
void QDMapView::SetMapActive(bool active)
{
    HMAP  NewMap;

    if (!active && !GetMapHandle()) return;

    if (active)
    {
        if (VarMapFileName.isEmpty())
        {
            return;
        }
        else
        {
            NewMap = mapOpenAnyData(VarMapFileName.utf16());

            if (NewMap == 0)
            {
                return;
            }
        }

        if (GetMapHandle())
        {
            AllMapRef(ACTBEFORECLOSE);
            mapCloseMap(GetMapHandle());
            VarMapHandle = 0;
        }

        VarMapHandle = NewMap;

        // Установим имя директории запуска программы
        QString path = qApp->applicationDirPath() + "/";
        mapSetPathShellUn( path.utf16() );

        VarVertPos = 0;
        VarHorzPos = 0;

        AllMapRef(ACTAFTEROPEN);

        VarViewSelected = false;
    }
    else
    {
        if (GetMapHandle())
        {
            AllMapRef(ACTBEFORECLOSE);
            mapCloseMap(GetMapHandle());
            VarMapHandle = 0;
        }
    }

    Repaint();
    AllMapRef(ACTDOREPAINTFORCHANGEDATA);
}

//------------------------------------------------------------------
// Активна ли в данный момент карта
//------------------------------------------------------------------
bool QDMapView::GetMapActive() const
{
    bool ret = FALSE;

    if (VarMapHandle)
    {
        ret = TRUE;
    }

    return ret;
}

//------------------------------------------------------------------
// Установить/cбросить отображение карты в контурном виде
//------------------------------------------------------------------
void QDMapView::SetMapContour(bool value)
{
    int contour;

    if (!GetMapHandle()) return;

    switch (mapGetViewType(GetMapHandle()))
    {
        case 1:if (value) mapSetViewType(GetMapHandle(),2); else return;
            break;
        case 2:if (value) return; else mapSetViewType(GetMapHandle(),1);
            break;
    }

    if (VarRendering) Repaint();
}

//------------------------------------------------------------------
// Отображена ли карта в контурном виде
//------------------------------------------------------------------
bool QDMapView::GetMapContour() const
{
    int vtype;

    if (!VarMapHandle) return FALSE;

    vtype = mapGetViewType(VarMapHandle);
    if ((vtype == 2) || (vtype == 4)) return TRUE;
    else return FALSE;
}

//-----------------------------------------------------------------
// Установить режим отображения карты
//-----------------------------------------------------------------
void QDMapView::SetMapViewType(int viewtype)
{
  if (!GetMapHandle()) return;
  mapSetViewType(GetMapHandle(), viewtype);
  if (VarRendering) Repaint();
}

int QDMapView::GetMapViewType()
{
  if (!GetMapHandle()) return 0;
  return mapGetViewType(GetMapHandle());
}


//------------------------------------------------------------------
// Установить текущее значение яркости карты
//------------------------------------------------------------------
void QDMapView::SetMapBright(int value)
{
    if (!GetMapHandle()) return;

    if ((value < -4) || (value > 4)) return;
    mapSetBright(GetMapHandle(), value);
    Repaint();
}

//------------------------------------------------------------------
// Запрос текущего значения яркости
//------------------------------------------------------------------
int QDMapView::GetMapBright() const
{
    if (!VarMapHandle) return 0;
    return mapGetBright(VarMapHandle);
}

//------------------------------------------------------------------
// Установить текущее значение контрастности карты
//------------------------------------------------------------------
void QDMapView::SetMapContrast(int value)
{
    if (!GetMapHandle()) return;

    if ((value < -4) || (value > 4)) return;
    mapSetContrast(GetMapHandle(), value);
    Repaint();
}

//------------------------------------------------------------------
// Запрос текущего значения контрастности
//------------------------------------------------------------------
int QDMapView::GetMapContrast() const
{
    if (!VarMapHandle) return 0;

    return mapGetContrast(VarMapHandle);
}

HSELECT QDMapView::GetViewSelectHandle()
{
    return VarViewSelect->GetHSelect(0);
}

//------------------------------------------------------------------
// Выполнить обновление границ  изображения района для все компонент
// связанных с MapView
//------------------------------------------------------------------
void QDMapView::UpdatePictureBorderForAll()
{
    AllMapRef(ACTAFTERCHANGEBORDER);
}

//------------------------------------------------------------------
// Обработка событий изменения состава карт и их отображения на уровне MapVeiw
//------------------------------------------------------------------
void QDMapView::AllMapRef(int action)
{
    int Col, ii;

    // Сообщим во все QDMapWindow
    Col = VarMapWindows.count();
    ii  = 0;

    while ((Col > 0) && (ii < Col))
    {
        VarMapWindows.at(ii)->AllMapRef(action);
        if (Col > VarMapWindows.count()) Col = VarMapWindows.count();
        else ii = ii + 1;
    }

    QDMapViewWindow::AllMapRef(action);
}

//------------------------------------------------------------------
// Работа со списком подключенных к нам QDMapWindow
//------------------------------------------------------------------
void QDMapView::AppendMapWindow(QDMapWindow *mapwindow)
{
    if (mapwindow == NULL) return;
    if (VarMapWindows.contains(mapwindow) == true)  // уже есть такой
        return;
    VarMapWindows.append(mapwindow);
}

void QDMapView::DeleteMapWindow(QDMapWindow *mapwindow)
{
    VarMapWindows.removeAll(mapwindow);
}

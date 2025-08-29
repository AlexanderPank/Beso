#include "qdmapcomp.h"

QDMapComponentsPlugin::QDMapComponentsPlugin(QObject *parent)
    : QObject(parent)
{
    m_plugins.append(new QDMapViewPlugin(this));
    m_plugins.append(new QDMapWindowPlugin(this));
    m_plugins.append(new QDMapPointPlugin(this));
    m_plugins.append(new QDMapObjPlugin(this));
    m_plugins.append(new QDMapFindPlugin(this));
    m_plugins.append(new QDMapSelectDialogPlugin(this));
    m_plugins.append(new QDMapDlgObjPlugin(this));
    m_plugins.append(new QDMapConvertorPlugin(this));
    m_plugins.append(new QDMapDataListPlugin(this));
    m_plugins.append(new QDMapSelectRectPlugin(this));
    m_plugins.append(new QDGetMapPointPlugin(this));
    m_plugins.append(new QDMapActionLinePlugin(this));
    m_plugins.append(new QDMapCreateMtwPlugin(this));
    #ifndef HIDE_PRINTSUPPORT
    m_plugins.append(new QDMapBuildEpsPlugin(this));
    #endif
    m_plugins.append(new QDMapSelectObjectRscPlugin(this));
    #ifndef HIDE_GISSERVER_SUPPORT
    m_plugins.append(new QDMapGisServerPlugin(this));
    #endif
    #ifndef HIDE_3DSUPPORT
    m_plugins.append(new QDMap3DPlugin(this));
    #endif
    m_plugins.append(new QDMapNetPlugin(this));
    m_plugins.append(new QDMapWMSPopupDialogPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> QDMapComponentsPlugin::customWidgets() const
{
    return m_plugins;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(QDMapComponentsPlugin)
#endif

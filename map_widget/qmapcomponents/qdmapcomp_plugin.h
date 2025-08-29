/******* QDMAPCOMP_PLUGIN.H  ***************************************
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
*                      PLUGIN'S  ŒÃœŒÕ≈Õ“                          *
*                                                                  *
*******************************************************************/

#ifndef QDMCMP_PLUGIN_H
#define QDMCMP_PLUGIN_H

#include <QtDesigner>
#include <QDesignerCustomWidgetInterface>

class QDMapViewPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapViewPlugin(QObject *parent = 0);
    virtual ~QDMapViewPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapWindowPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapWindowPlugin(QObject *parent = 0);
    virtual ~QDMapWindowPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapPointPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapPointPlugin(QObject *parent = 0);
    virtual ~QDMapPointPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapObjPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapObjPlugin(QObject *parent = 0);
    virtual ~QDMapObjPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapFindPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapFindPlugin(QObject *parent = 0);
    virtual ~QDMapFindPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapSelectDialogPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapSelectDialogPlugin(QObject *parent = 0);
    virtual ~QDMapSelectDialogPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapDlgObjPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapDlgObjPlugin(QObject *parent = 0);
    virtual ~QDMapDlgObjPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapConvertorPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapConvertorPlugin(QObject *parent = 0);
    virtual ~QDMapConvertorPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapDataListPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapDataListPlugin(QObject *parent = 0);
    virtual ~QDMapDataListPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapSelectRectPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapSelectRectPlugin(QObject *parent = 0);
    virtual ~QDMapSelectRectPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDGetMapPointPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDGetMapPointPlugin(QObject *parent = 0);
    virtual ~QDGetMapPointPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapActionLinePlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapActionLinePlugin(QObject *parent = 0);
    virtual ~QDMapActionLinePlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

#ifndef HIDE_PRINTSUPPORT
class QDMapBuildEpsPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapBuildEpsPlugin(QObject *parent = 0);
    virtual ~QDMapBuildEpsPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};
#endif

class QDMapCreateMtwPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapCreateMtwPlugin(QObject *parent = 0);
    virtual ~QDMapCreateMtwPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapSelectObjectRscPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapSelectObjectRscPlugin(QObject *parent = 0);
    virtual ~QDMapSelectObjectRscPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

#ifndef HIDE_GISSERVER_SUPPORT
class QDMapGisServerPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapGisServerPlugin(QObject *parent = 0);
    virtual ~QDMapGisServerPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};
#endif

#ifndef HIDE_3DSUPPORT
class QDMap3DPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMap3DPlugin(QObject *parent = 0);
    virtual ~QDMap3DPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};
#endif

class QDMapNetPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapNetPlugin(QObject *parent = 0);
    virtual ~QDMapNetPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

class QDMapWMSPopupDialogPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QDMapWMSPopupDialogPlugin(QObject *parent = 0);
    virtual ~QDMapWMSPopupDialogPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget *createWidget(QWidget *parent);

    virtual bool isInitialized() const;
    virtual void initialize(QDesignerFormEditorInterface *core);

    virtual QString domXml() const;

private:
    bool m_initialized;
};

#endif // QDMCMP_PLUGIN_H

#include <QtDesigner/QtDesigner>
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
  #include <QtUiPlugin/QDesignerCustomWidgetInterface>
  #include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#elif (QT_VERSION == QT_VERSION_CHECK(5, 3, 0))
  #include <QtDesigner/QDesignerCustomWidgetInterface>
  #include <QtDesigner/QDesignerCustomWidgetCollectionInterface>
#else
  #include <QDesignerCustomWidgetInterface>
  #include <QDesignerCustomWidgetCollectionInterface>
#endif

#include <QtCore/qplugin.h>
#include "qdmapcomp_plugin.h"


class QDMapComponentsPlugin: public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
#endif
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
    QDMapComponentsPlugin(QObject *parent = 0);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> m_plugins;
};


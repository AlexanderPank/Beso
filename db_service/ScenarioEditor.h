// scenarioeditor.h
#ifndef SCENARIOEDITOR_H
#define SCENARIOEDITOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QJsonDocument>
#include <QPushButton>

#include "ui_ScenarioEditor.h"

#include "../signs/SignBase.h"
#include "../signs/SignController.h"
#include "ScenarioParser.h"
#include "../map_widget/map_tools/SignDrawer.h"

#include "modules/daddscenarioobject.h"
#include "modules/daddscenariointeraction.h"
#include "../map_widget/SimulationEventWidgetInMap.h"
#include "../map_widget/MapWidget.h"

namespace Ui {
    class ScenarioEditor;
}

class ScenarioEditor : public QWidget
{
Q_OBJECT

public:
    explicit ScenarioEditor(QWidget *parent = nullptr);

    int loadScenario(const QString &filePath, const QString &mapPath, MapWidget *mapWidget);
    bool saveScenario(const QString &filePath);
    void createNewScenario();


    void updateObjectStateFromModel(QJsonArray arr, QJsonArray features);

    QString getTempFileName();
    static QList<QPointF> JsonArrayToQList(QJsonArray coord);
    QString getMapPathFromScenarioFile(const QString &scenarioFilePath);
    void moveAllElements(const QPointF &newCenter);

    void setEventLog(SimulationEventWidgetInMap* eventLog);

signals:
    void setMapCenter(float lon, float lat, int zoom);
    void selectSignOnMap(SignBase *sign);

    // void signCreated(SignBase *sign);
    void clearMap();
    void needMapRepaint();
    //void signUpdateGeoPosition(SignBase *sign, double lat, double lon,  double course);
    //void signUpdateGeoCoordinates(SignBase *sign,  QList<QPointF> points);

    void filterTacticalSign(QString, QString);

public slots:
    void addScenarioElement();
    void removeSelectedElement();
    void editSelectedElement();
    void cloneSelectedElement();
    void saveScenario(bool saveAs);

    void addTacticalSign(QString, QJsonObject data);

    /**
     * Обновление объектов из БД
     * - параметры добавляются только новые
     * - actions - заменяются
     */
    void updateObjectsFromBD();

private:
    Ui::ScenarioEditor *ui;

    void setupUi();
    void parseJsonToTree(const QJsonDocument &doc, QTreeWidgetItem *parent = nullptr);


//    QTreeWidget *treeWidget{};
//    QPushButton *addButton{};
//    QPushButton *removeButton{};
    QPushButton *editButton{};
//    QPushButton *cloneButton{};
    QPushButton *saveButton{};
    QPushButton *loadButton{};
    QPushButton *newButton{};

    QString m_scenario_file;

    QString findChildValue(QTreeWidgetItem *parent, const QString &name) const;
    QTreeWidgetItem* findChildItem(QTreeWidgetItem *parent, const QString &name) const;
    QStringList findGeoProperties(const QJsonObject&);

    void handlePropertyEdit(QTreeWidgetItem *item, int column);

    void createSignsFromObjects(QList<ObjectScenarioModel*> objects, QList<FeatureModel*> features);
    void createSignFromObject(ObjectScenarioModel* object);
    void createSignFromFeature(FeatureModel* object);
    void setupSignPropertyConnections(ObjectScenarioModel *item, SignBase *sign, const QString &field_name, const QString &geo_id="");
    void updateChildValue(QTreeWidgetItem *parent, const QString &name, const QString &value, QJsonObject obj={});
    void updateConnectedSign(FeatureModel* feature, ObjectScenarioModel* obj= nullptr);
    void updateConnectedSigns(ObjectScenarioModel* obj);
    void drawSignOnMap(QString objectID);

    QMap<QString, SignBase*> m_signObjects;
    QHash<QString, QMap<QString, FeatureModel*>> m_features;

    SignController*  m_signController;
    ScenarioParser* m_scenarioParser;
    SignDrawer* m_signDrawer;
    bool m_treeItemEditMode = false;

    QList<FeatureModel*> m_tmp_listFeatures;

    SimulationEventWidgetInMap *m_eventLog;
};

#endif // SCENARIOEDITOR_H

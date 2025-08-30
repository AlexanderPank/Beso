//
// Created by qq on 30.03.25.
//

#ifndef SCENARIOCLIENT_SCENARIOPARSER_H
#define SCENARIOCLIENT_SCENARIOPARSER_H


#include <QObject>
#include <QJsonDocument>
#include <QTreeWidget>
#include "../models/ObjectScenarioModel.h"
#include "../models/AlgorithmModel.h"
#include "../models/FileModel.h"
#include "../models/SimulationParametersModel.h"
#include "../models/InteractionModel.h"
#include "../models/FeatureModel.h"

enum TypeView {
    tvEnemy,
    tvOur,
    tvGeo,
    tvMeteo,
    tvInteraction,
    tvAll,
    tvLog
};

class ScenarioParser : public QObject {
Q_OBJECT
public:
    explicit ScenarioParser(QObject* parent = nullptr);

    // Загрузка/сохранение
    bool loadScenario(const QString& filePath);
    bool saveScenario(const QString& filePath);

    // Доступ к данным
    QString getId() const {return m_scenario_id;};
    QList<ObjectScenarioModel*> getObjects()  const {return m_objects;};
    QList<FeatureModel*>        getFeatures() const {return m_features;};
    QList<ObjectScenarioModel*> getClasses()  const {return m_classes;};
    QList<AlgorithmModel*>      getAlgorithms() const {return m_algorithms;};
    QList<FileModel*>           getFiles()    const {return m_files;};
    SimulationParametersModel* getSimulationParameters() const {return m_simulation_parameters;};

    ObjectScenarioModel* getObjectById(const QString& id);
    void clearAlgorithmModels();
    void clearFileModels();

    /**
     * Добавление объекта моделнирования в сценарий
     * @param objectScenarioModel
     */
    ObjectScenarioModel * addObjectScenarioModel(QJsonObject objectScenarioModel);

    /**
     * Проверка. Если нет алгоритма в сценарии,
     * то добавляем его
     * @param idAlgorithm
     * @return
     */
    AlgorithmModel* checkAndAddAlgorithmModel(const QString &idAlgorithm);

    /**
     * Добавление алгоритма в сценарий
     * @param algorithmModel
     * @return
     */
    AlgorithmModel* addAlgorithmModel(QJsonObject algorithm);

    /**
     * Добалвение файла библиотеки в сценарий
     * @param file
     * @return
     */
    FileModel* addFileModel(QJsonObject file);

    /**
     * Добавление тактических знаков в сценарий
     * @param featureModel
     * @return
     */
    FeatureModel* addFeatureModel(QJsonObject featureModel);

    /**
     * Добавление действий для объекта сценария
     * @param objectScenarioModel
     * @param featureModel
     * @return
     */
    InteractionModel* addInteractionModel(ObjectScenarioModel* objectScenarioModel, FeatureModel* featureModel);

    /**
     * Добавление взаимодейтсвий
     * @param jsonInteractionModel
     * @return
     */
    InteractionModel* addInteractionModel(QJsonObject jsonInteractionModel);
    InteractionModel* addInteractionModelWithAddTree(QJsonObject jsonInteractionModel);

    // Построение дерева
    void buildTree(QTreeWidget* treeWidget);

    void filterTreeByTypeView(const TypeView &typeView);

    void updateObjectById(const QString& id, QJsonObject obj);
    FeatureModel* updateFeatureById(const QString& id, QJsonObject obj);

    void removeObjectScenarioModel(ObjectScenarioModel* object);
    void removeFeatureModel(FeatureModel *featureModel);
    void removeInteractionModel(InteractionModel *iterationModel);

signals:
    void editInteractionModel(InteractionModel*);

private:
    QString                             m_scenario_id;
    double                              m_version = 1.0;
    QString                             m_map_path = "";

    QList<ObjectScenarioModel*>         m_objects;
    QList<ObjectScenarioModel*>         m_classes;
    QList<AlgorithmModel*>              m_algorithms;
    QList<FileModel*>                   m_files;
    QJsonObject                         m_rootJson;
    SimulationParametersModel*          m_simulation_parameters;
    QList<InteractionModel*>            m_interactions;
    QList<FeatureModel*>                m_features;

    QTreeWidgetItem* rootItem           = nullptr;
    QTreeWidgetItem* classesItem        = nullptr;
    QTreeWidgetItem* objectsItem        = nullptr;
    QTreeWidgetItem* tacticalObject     = nullptr;
    QTreeWidgetItem* algItem            = nullptr;
    QTreeWidgetItem* filesItem          = nullptr;
    QTreeWidgetItem* interactionItems   = nullptr;
    QTreeWidgetItem* featuresItem      = nullptr;
    QList<QTreeWidgetItem*>             m_topLevelObjects;

    QString getTreeItemObjectNameByType(const TypeView &typeView);
    void moveObjectItemsToRoot(QTreeWidgetItem* tree, bool toRoot);


};

#endif //SCENARIOCLIENT_SCENARIOPARSER_H

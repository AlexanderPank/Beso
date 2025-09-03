#include "ScenarioParser.h"
#include "../models/Feature.h"
#include <QFile>
#include <QJsonArray>
#include <QtAlgorithms>

#include "../models/ActionModel.h"
#include "../db_service/services/DataStorageServiceFactory.h"
#include "../db_service/services/DataStorageService.h"

#include "../core/QLogStream.h"

#include "../utils/ParametersChecker.h"

ScenarioParser::ScenarioParser(QObject* parent) : QObject(parent) {}

bool ScenarioParser::loadScenario(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    qDeleteAll(m_objects);
    m_objects.clear();
    qDeleteAll(m_classes);
    m_classes.clear();
    qDeleteAll(m_interactions);
    m_interactions.clear();
    qDeleteAll(m_algorithms);
    m_algorithms.clear();
    qDeleteAll(m_files);
    m_files.clear();
    qDeleteAll(m_features);
    m_features.clear();


    delete m_simulation_parameters; m_simulation_parameters = nullptr;
    delete rootItem        ;rootItem           = nullptr;
    delete classesItem     ;classesItem        = nullptr;
    delete objectsItem     ;objectsItem        = nullptr;
    delete tacticalObject  ;tacticalObject     = nullptr;
    delete algItem         ;algItem            = nullptr;
    delete filesItem       ;filesItem          = nullptr;
    delete interactionItems;interactionItems   = nullptr;
    delete featuresItem    ;featuresItem       = nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    m_rootJson = doc.object();

    m_scenario_id = m_rootJson["scenario_id"].toString();
    m_version  = m_rootJson.contains("version") ? m_rootJson["version"].toDouble() : 1.0;
    m_map_path = m_rootJson.contains("map_path") ? m_rootJson["map_path"].toString(): "";
    // Парсинг объектов
    for (const QJsonValue& v : m_rootJson["objects"].toArray())
        m_objects.append( ObjectScenarioModel::fromJson(v.toObject(), this));
    // Парсинг объектов
    for (const QJsonValue& v : m_rootJson["classes"].toArray())
        m_classes.append( ObjectScenarioModel::fromJson(v.toObject(), this));

    // Парсинг взаимодействий
    for (const QJsonValue& v : m_rootJson["interactions"].toArray())
        addInteractionModel(v.toObject());
//        m_interactions.append(InteractionModel::fromJson(v.toObject(), this));

    // Парсинг алгоритмов
    for (const QJsonValue& v : m_rootJson["algorithms"].toArray())
        m_algorithms.append(AlgorithmModel::fromJson(v.toObject(), this));

    // Парсинг файлов
    for (const QJsonValue& v : m_rootJson["files"].toArray())
        m_files.append(FileModel::fromJson(v.toObject(), this));

    // Парсинг файлов
    for (const QJsonValue& v : m_rootJson["features"].toArray())
        m_features.append(FeatureModel::fromJson(v.toObject(), this));

    m_simulation_parameters = SimulationParametersModel::fromJson(m_rootJson["simulation_parameters"].toObject(), this);

    return true;
}

bool ScenarioParser::saveScenario(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QJsonObject root;

    root["scenario_id"] = m_scenario_id;

    // Сохранение объектов
    QJsonArray objectsArray;
    for (const auto& obj : m_objects)
        objectsArray.append(obj->toJson());
    root["objects"] = objectsArray;

    // Сохранение классов
    QJsonArray classesArray;
    for (const auto& cls : m_classes)
        classesArray.append(cls->toJson());
    root["classes"] = classesArray;

    // Сохранение классов
    QJsonArray featuresArray;
    for (const auto& cls : m_features)
        featuresArray.append(cls->toJson());
    root["features"] = featuresArray;

    // Сохранение взаимодействий
    QJsonArray interactionsArray;
    for (const auto& interaction : m_interactions)
        interactionsArray.append(interaction->toJson());
    root["interactions"] = interactionsArray;

    // Сохранение алгоритмов
    QJsonArray algorithmsArray;
    for (const auto& alg : m_algorithms)
        algorithmsArray.append(alg->toJson());
    root["algorithms"] = algorithmsArray;

    // Сохранение файлов
    QJsonArray filesArray;
    for (const auto& fileModel : m_files)
        filesArray.append(fileModel->toJson());
    root["files"] = filesArray;

    // Сохранение параметров симуляции
    if (m_simulation_parameters)
        root["simulation_parameters"] = m_simulation_parameters->toJson();

    root["version"] = m_version;
    root["map_path"] = m_map_path;

    // Сохраняем в файл
    QJsonDocument doc(root);
    file.write(doc.toJson());//QJsonDocument::Compact));
    file.close();

    return true;
}

ObjectScenarioModel * ScenarioParser::addObjectScenarioModel(QJsonObject objectScenarioModel) {

    ObjectScenarioModel *newObjectScenarioModel = ObjectScenarioModel::fromJson(objectScenarioModel, this);
    QString idNewObjectScenarioModel = newObjectScenarioModel->getId() + "_";
    int maxCount = 0;
    for(ObjectScenarioModel* objectModel: m_objects) {
        if(objectModel->getId().startsWith(idNewObjectScenarioModel)) {
            bool isOk = false;
            int number = objectModel->getId().right(objectModel->getId().size() - idNewObjectScenarioModel.size()).toInt(&isOk);
            if(isOk && maxCount < number){
                maxCount = number;
            }
        }
    }
    maxCount++;
    idNewObjectScenarioModel += QString::number(maxCount);
    newObjectScenarioModel->setId(idNewObjectScenarioModel);
    QString cutTitle = newObjectScenarioModel->getTitle();
    int index = cutTitle.indexOf("№");
    if(index > 0)
        cutTitle = cutTitle.left(index).trimmed();

    newObjectScenarioModel->setTitle(QString("%1 №%2").arg(cutTitle).arg(maxCount));
    bool isOk = false;
    Feature* featureGeoPath = newObjectScenarioModel->getPropertyFeature("geo_path", isOk);
    if(isOk) {
        featureGeoPath->updateProperty("id", QString("%1_path").arg(newObjectScenarioModel->getId()));
//        newObjectScenarioModel->setPropertyValue("geo_path", featureGeoPath->toJson());
    }

    m_objects.append(newObjectScenarioModel);
    newObjectScenarioModel->getTreeWidgetItem(objectsItem);

    for(ActionModel* action: newObjectScenarioModel->actions()) {
//        bool isContains = false;
//        for(AlgorithmModel* algorithm: m_algorithms) {
//            if (action->algorithmId() == algorithm->getId()) {
//                isContains = true;
//                break;
//            }
//        }
//
//        if(isContains)
//            continue;
//
//        DataStorageItem* algorithm = DataStorageServiceFactory::getInstance()->findElement(TypeElement::teALGORITHM, action->algorithmId());
//        if(algorithm != nullptr) {
//            addAlgorithmModel(algorithm->getData());
//        } else {
//            qLog() << "Алгоритм "+action->algorithmId()+" не найден в хранилище";
//        }
        checkAndAddAlgorithmModel(action->algorithmId());
    }

    return newObjectScenarioModel;
}

AlgorithmModel *ScenarioParser::checkAndAddAlgorithmModel(const QString &idAlgorithm) {
    bool isContains = false;
    for(AlgorithmModel* algorithm: m_algorithms) {
        if (idAlgorithm == algorithm->getId()) {
            isContains = true;
            return algorithm;
        }
    }

    DataStorageItem *algorithm = DataStorageServiceFactory::getInstance()->findElement(TypeElement::teALGORITHM,
                                                                                       idAlgorithm);
    if (algorithm != nullptr) {
        return addAlgorithmModel(algorithm->getData());
    } else {
        qLog() << "Алгоритм " + idAlgorithm + " не найден в хранилище";
    }

    return nullptr;
}

AlgorithmModel *ScenarioParser::addAlgorithmModel(QJsonObject algorithm) {
    AlgorithmModel *algorithmModel = AlgorithmModel::fromJson(algorithm, this);
    m_algorithms.append(algorithmModel);
    algorithmModel->getTreeWidgetItem(algItem);

    bool isContains = false;
    QString function = algorithmModel->getFunction();
    QStringList splitFunction = function.split('.');
    if(splitFunction[0].startsWith("LIBCPP")) {
        QString fileId = splitFunction[0];
        for(FileModel* iter: m_files) {
            if(iter->getId() == fileId) {
                isContains = true;
                break;
            }
        }
        if(!isContains) {
            DataStorageItem *file = DataStorageServiceFactory::getInstance()->findElement(TypeElement::teLIBFILE,
                                                                                          fileId);
            if (file != nullptr) {
                addFileModel(file->getData());
            } else {
                qLog() << "Файл алгоритмов " + splitFunction[0] + " не найден в хранилище";
            }
        }
    }

    return algorithmModel;
}

FileModel *ScenarioParser::addFileModel(QJsonObject file) {
    FileModel* fileModel = FileModel::fromJson(file, this);
    m_files.append(fileModel);
    fileModel->getTreeWidgetItem(filesItem);
    return fileModel;
}

void ScenarioParser::buildTree(QTreeWidget* treeWidget) {
    treeWidget->clear();

    rootItem = new QTreeWidgetItem(treeWidget);
    rootItem->setText(0, m_scenario_id);

    // Добавление объектов
    objectsItem = new QTreeWidgetItem(treeWidget);
    objectsItem->setText(0, "Объекты");
    for (ObjectScenarioModel* obj : m_objects) {
        obj->getTreeWidgetItem(objectsItem, true);
    }

    classesItem = new QTreeWidgetItem(treeWidget);
    classesItem->setText(0, "Динамические классы");
    for (ObjectScenarioModel* obj : m_classes)
        obj->getTreeWidgetItem(classesItem);

    // создание вкладки с геобъектами

    featuresItem = new QTreeWidgetItem(treeWidget);
    featuresItem->setText(0, "Тактические знаки");

    for (FeatureModel* feature : m_features)
        feature->getTreeWidgetItem(featuresItem);

    // Добавление алгоритмов
    algItem = new QTreeWidgetItem(treeWidget);
    algItem->setText(0, "Алгоритмы");
    for (AlgorithmModel* alg : m_algorithms)
        alg->getTreeWidgetItem(algItem);

    // Добавление файлов
    filesItem = new QTreeWidgetItem(treeWidget);
    filesItem->setText(0, "Файлы");
    for (FileModel* file : m_files)
        file->getTreeWidgetItem(filesItem);

    // Добавление файлов
    interactionItems = new QTreeWidgetItem(treeWidget);
    interactionItems->setText(0, "Взаимодействия");
    for (InteractionModel* iter : m_interactions)
        iter->getTreeWidgetItem(interactionItems);

    m_simulation_parameters->getTreeWidgetItem(treeWidget);
}

void ScenarioParser::filterTreeByTypeView(const TypeView &typeView) {
    if(rootItem == nullptr)
        return;
    rootItem->setHidden(typeView!=TypeView::tvAll);

    if(typeView == TypeView::tvGeo ) {
        objectsItem->setHidden(true);
        featuresItem->setHidden(false);
        featuresItem->setExpanded(true);
//        return;
    } else {
        objectsItem->setHidden(false);
        objectsItem->setText(0, getTreeItemObjectNameByType(typeView));
    }

    if(typeView == TypeView::tvInteraction) {
        objectsItem->setHidden(true);
        featuresItem->setHidden(true);
        interactionItems->setHidden(false);
        interactionItems->setExpanded(true);
    }

    for(int i = 0; i < objectsItem->childCount(); i++){
        QTreeWidgetItem* child = objectsItem->child(i);
        ObjectScenarioModel* model = (ObjectScenarioModel*)child->data(0, Qt::UserRole).value<ObjectScenarioModel*>();
        bool isHidden = true;
        switch (typeView) {
            case TypeView::tvEnemy:
            case TypeView::tvOur:
                if(!model->properties().isEmpty() && model->properties().contains("is_own")) {
                    bool isOk = true;
                    int is_own = model->getPropertyInt("is_own", isOk);
                    if (isOk) {
                        if ((is_own == 1 && typeView == TypeView::tvOur) ||
                            (is_own == 0 && typeView == TypeView::tvEnemy)) {
                            isHidden = false;
                        }
                    }
                }
                break;
            case TypeView::tvMeteo:
                if(!model->properties().isEmpty() && model->properties().contains("type_object")) {
                    if (model->getPropertyString("type_object") == "meteo") {
                        isHidden = false;
                    }
                }
                break;
            default:
                isHidden = false;
                break;
        }
        child->setHidden(isHidden);
    }
    objectsItem->setExpanded(typeView!=TypeView::tvAll);
    classesItem->setHidden(typeView!=TypeView::tvAll);
    algItem->setHidden(typeView!=TypeView::tvAll);
    filesItem->setHidden(typeView!=TypeView::tvAll);
    interactionItems->setHidden(typeView!=TypeView::tvAll&&typeView!=TypeView::tvInteraction);
    m_simulation_parameters->filterTreeItem(typeView!=TypeView::tvAll);
    featuresItem->setHidden(typeView!=TypeView::tvAll&&typeView!=TypeView::tvGeo);
}

void ScenarioParser::updateObjectById(const QString& id, QJsonObject json){
    for (ObjectScenarioModel* obj : m_objects){
        if (obj->getId() != id) continue;

        for (auto key: json["properties"].toObject().keys()) {
            auto value = json["properties"].toObject()[key].toVariant();
            obj->setPropertyValue(key, value);
        }

    }
}


FeatureModel* ScenarioParser::updateFeatureById(const QString& id, QJsonObject json){
    for( auto feature: m_features) {
        if (feature->getId() != id) continue;

        QList<QPointF> points;
        for (auto jpoint: json["coordinates"].toArray() )
            points.append(QPointF(jpoint.toArray()[0].toDouble(), jpoint.toArray()[1].toDouble()) );
        feature->setCoordinates(points);

        if (json.contains("properties") && json["properties"].isObject()) {
            QJsonObject propertiesObj = json["properties"].toObject();

            for (auto it = propertiesObj.begin(); it != propertiesObj.end(); ++it) {
                feature->setProperty(it.key(), it.value().toVariant());
            }
        }
        return feature;
    }
    return nullptr;
}

QString ScenarioParser::getTreeItemObjectNameByType(const TypeView &typeView) {
    QString result = "Объекты";
    switch (typeView) {
        case TypeView::tvGeo:
            result = "Знаки";
            break;
        case TypeView::tvEnemy:
            result = "Боевой состав синих";
            break;
        case TypeView::tvOur:
            result = "Боевой состав красных";
            break;
        case TypeView::tvMeteo:
            result = "Гидрометеорологическая обстановка";
            break;
    }

    return result;
}

FeatureModel *ScenarioParser::addFeatureModel(QJsonObject featureModelData) {
    FeatureModel *newFeatureModel = FeatureModel::fromJson(featureModelData, this);
    QString idNewFeatureModel = newFeatureModel->getId() + "_";
    int maxCount = 0;
    for(FeatureModel* featureModel: m_features) {
        if(featureModel->getId().startsWith(idNewFeatureModel)) {
            bool isOk = false;
            int number = featureModel->getId().right(featureModel->getId().size() - idNewFeatureModel.size()).toInt(&isOk);
            if(isOk && maxCount < number){
                maxCount = number;
            }
        }
    }

    maxCount++;
    idNewFeatureModel += QString::number(maxCount++);
    newFeatureModel->setId(idNewFeatureModel);
    m_features.append(newFeatureModel);
    newFeatureModel->getTreeWidgetItem(featuresItem);

    return newFeatureModel;
}

InteractionModel *ScenarioParser::addInteractionModel(ObjectScenarioModel *objectScenarioModel, FeatureModel *featureModel) {
    QMap<QString, QString> mapParameters;
    auto *interactionModelPath = new InteractionModel(this);
    interactionModelPath->setSourceObjectId(objectScenarioModel->getId());
    interactionModelPath->setInteractionType("find_path");
    interactionModelPath->setTitle("Поиск маршрута");
    interactionModelPath->setTriggerCondition(QString("%1.properties['state'] == 'standing'").arg(objectScenarioModel->getId()));
    interactionModelPath->setResponseSetValues(QString("%1.properties['state'] = 'moving'; %1.properties['speed'] = %1.properties['slow_speed']").arg(objectScenarioModel->getId()));
    interactionModelPath->setResponseAction(QString("%1.properties['geo_path']['geometry']['coordinates'] = %1.actions.find_path(ship_lat=%1.properties['lat'], ship_lon=%1.properties['lon'], target_lat=self.features['%2'].coordinates[%3][0], target_lon=self.features['%2'].coordinates[%3][1], geo_area=Model_00.actions.filter_by_type(objects=self.features, obj_type=\"RESTRICTED_AREA\"))")
                                                    .arg(objectScenarioModel->getId(), featureModel->getId(), QString::number(featureModel->getCoordinates().size()-1)));
    m_interactions.append(interactionModelPath);
    interactionModelPath->getTreeWidgetItem(interactionItems);
    ParametersChecker::getParametersFromString(objectScenarioModel->getId(), interactionModelPath->getResponseAction(), mapParameters);
    ParametersChecker::getParametersFromString(objectScenarioModel->getId(), interactionModelPath->getResponseSetValues(), mapParameters);
    ParametersChecker::getParametersFromString(objectScenarioModel->getId(), interactionModelPath->getTriggerCondition(), mapParameters);

    QStringList listErrorParameters;
    for(auto key: mapParameters.keys()) {
        if(!objectScenarioModel->containsProperty(key)) {
            listErrorParameters.append("Свойства не существует: " + mapParameters[key]);
        }
    }
    if(!listErrorParameters.isEmpty())
        ParametersChecker::showMessage(listErrorParameters.join("\n"));


    return interactionModelPath;
}


void ScenarioParser::removeFeatureModel(FeatureModel *featureModel) {
    if(!featureModel) return;

    m_features.removeOne(featureModel);
    auto item = featureModel->getTreeWidgetItem(nullptr);
    if(item && item->parent())
        item->parent()->removeChild(item);

    delete featureModel;
}

void ScenarioParser::removeInteractionModel(InteractionModel *iterationModel) {
    if(!iterationModel) return;

    m_interactions.removeOne(iterationModel);
    auto item = iterationModel->getTreeWidgetItem(nullptr);
    if(item && item->parent())
        item->parent()->removeChild(item);

    delete iterationModel;
}


void ScenarioParser::removeObjectScenarioModel(ObjectScenarioModel *objectScenarioModel) {
    if(!objectScenarioModel) return;

    m_objects.removeOne(objectScenarioModel);
    for (int i = 0; i < m_interactions.size(); i++){
        if (m_interactions[i]->getSourceObjectId() == objectScenarioModel->getId()) {
            auto item = m_interactions.at(i)->getTreeWidgetItem(nullptr);
            if(item && item->parent())  item->parent()->removeChild(item);
            m_interactions.removeAt(i);
            i--;
        }
    }
    for (int i = 0; i < m_features.size(); i++){
        if (m_features[i]->getParentID() == objectScenarioModel->getId()) {
            auto item = m_features.at(i)->getTreeWidgetItem(nullptr);
            item->setData(0, Qt::UserRole, QVariant());
            if(item && item->parent())  item->parent()->removeChild(item);
            m_features.removeAt(i);
            i--;
        }
    }

    // TODO: надо сделать удаление всех связанных с объектом алгоритмов и файлов
    // но нужно удалять только те которые не используются другими объектами это задача для Сани )
    for (int i = 0; i < m_algorithms.size(); i++){

    }

    for (int i = 0; i < m_files.size(); i++){

    }
    auto item = objectScenarioModel->getTreeWidgetItem(nullptr);
    if(item && item->parent())  item->parent()->removeChild(item);

    delete objectScenarioModel;
}

InteractionModel* ScenarioParser::addInteractionModel(QJsonObject jsonInteractionModel) {
    InteractionModel* newInteractionModel = InteractionModel::fromJson(jsonInteractionModel, this);
    m_interactions.append(newInteractionModel);
    connect(newInteractionModel, &InteractionModel::editInteractionModel, this, &ScenarioParser::editInteractionModel);

    return newInteractionModel;
}

InteractionModel *ScenarioParser::addInteractionModelWithAddTree(QJsonObject jsonInteractionModel) {
    InteractionModel* newInteractionModel = addInteractionModel(jsonInteractionModel);
    newInteractionModel->getTreeWidgetItem(interactionItems);
    return newInteractionModel;
}

void ScenarioParser::clearAlgorithmModels() {
    if (!algItem)
        return;

    while (algItem->childCount() > 0) {
        QTreeWidgetItem *child = algItem->takeChild(0); // изымаем
        delete child;                                   // удаляем
    }

    qDeleteAll(m_algorithms);
    m_algorithms.clear();
}

void ScenarioParser::clearFileModels() {
    if (!filesItem)
        return;

    while (filesItem->childCount() > 0) {
        QTreeWidgetItem *child = filesItem->takeChild(0);
        delete child;
    }

    qDeleteAll(m_files);
    m_files.clear();
}


ObjectScenarioModel* ScenarioParser::getObjectById(const QString& id) {
    for (ObjectScenarioModel* obj : m_objects)
        if (obj->getId() == id) return obj;
    return nullptr;
}

#include "ScenarioEditor.h"
#include "../core/QLogStream.h"
#include "../signs/SignFactory.h"
#include "../delegates/PropertyItemDelegate.h"
#include "../signs/SignShip.h"
#include "services/DataStorageServiceFactory.h"
#include "../core/EnvConfig.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QApplication>
#include <QSettings>

ScenarioEditor::ScenarioEditor(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::ScenarioEditor)
{
    ui->setupUi(this);
    setupUi();
    m_signController =  SignController::getInstance();
    if (m_signController == nullptr) {
        QMessageBox::critical(this, "Критическая ошибка", "Не удалось инициализировать контроллер знаков. Приложение будет закрыто.");
        QTimer::singleShot(0, qApp, &QApplication::quit);
    }
    ui->tabWScenarioElements->widget(1)->setObjectName("enemyTab");
    ui->tabWScenarioElements->tabBar()->setObjectName("enemyTab");
    ui->tabWScenarioElements->tabBar()->setTabTextColor(0, QColor("#8aa6ff"));
    ui->tabWScenarioElements->tabBar()->setTabTextColor(1, QColor("#ffa3a9"));
}


void ScenarioEditor::setupUi()
{
    // Устанавливаем политику расширения по вертикали
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

//    auto *mainLayout = new QVBoxLayout(this);
//    mainLayout->setContentsMargins(0, 15, 0, 0);  // Убираем отступы
//    mainLayout->setSpacing(0);                   // Убираем промежутки

    // Инициализация дерева сценария
//    ui->treeWidget = new QTreeWidget(this);
    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setHeaderLabels({"Название", "Значение", ""});
    ui->treeWidget->setFont(QFont("Arial", 10));
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setColumnWidth(0, 280);
    ui->treeWidget->setColumnWidth(1, 140);
    ui->treeWidget->setColumnWidth(2, 50);


    ui->gridLayout->setContentsMargins(3, 1, 3, 3);
    ui->gridLayout->setSpacing(3);


    // Подключение сигналов
    connect(ui->addButton, &QPushButton::clicked, this, &ScenarioEditor::addScenarioElement);
    connect(ui->removeButton, &QPushButton::clicked, this, &ScenarioEditor::removeSelectedElement);
//    connect(editButton, &QPushButton::clicked, this, &ScenarioEditor::editSelectedElement);
    connect(ui->cloneButton, &QPushButton::clicked, this, &ScenarioEditor::cloneSelectedElement);

    connect(ui->setButton, &QPushButton::clicked, [this](){
        QList<QPointF> coords = {QPointF(43.53174, 39.46268)};
        SignBase *sign = SignFactory::createSign("131472600009", "test", true, coords, 10, {});
        auto m_editor = SignDrawer::instance();
        m_editor->startDrawing(sign);
        auto connectSignDrawer = std::make_shared<QMetaObject::Connection>();
        *connectSignDrawer = connect(m_editor, &SignDrawer::signDrawEnd, [this, sign, connectSignDrawer](){
            m_signController->addSignToMap(sign);
            QObject::disconnect(*connectSignDrawer);
        });

    });

    m_scenarioParser = new ScenarioParser(this);

    connect(m_scenarioParser, &ScenarioParser::editInteractionModel, [this](InteractionModel* editInteractionModel) {
        DAddScenarioInteraction dAddScenarioInteraction(true,this);
        dAddScenarioInteraction.setEditInteracionModel(*editInteractionModel);
        if (dAddScenarioInteraction.exec() == QDialog::Accepted) {
            editInteractionModel->getTreeWidgetItem();
        }
    });

    connect(ui->tabWScenarioElements, &QTabWidget::currentChanged, [this]() {
       TypeView typeView =  (TypeView)ui->tabWScenarioElements->currentIndex();
       m_eventLog->setVisible(typeView==TypeView::tvLog);
       ui->treeWidget->setVisible(typeView!=TypeView::tvLog);
       if(typeView != TypeView::tvLog) {
           m_scenarioParser->filterTreeByTypeView(typeView);
       }
    });


    // Подключаемся к моменту завершения редактирования
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int column) {
        m_treeItemEditMode = column == 1;
    });

    ui->treeWidget->setEditTriggers(QTreeWidget::DoubleClicked | QTreeWidget::EditKeyPressed);
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, [=](QTreeWidgetItem *item,int column){
        m_treeItemEditMode = false;
        if (!m_treeItemEditMode) return;
        handlePropertyEdit(item, column);

    });

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item,int column) {
        auto objectModel = item->data(0,Qt::UserRole).value<ObjectScenarioModel*>();
        if(objectModel) {
            emit filterTacticalSign(objectModel->getPropertyString("type"), objectModel->getPropertyString("subType"));
        }
    });

}

void ScenarioEditor::saveScenario(bool saveAs){
    if (saveAs) {
        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить сценарий", "", "JSON Files (*.json)");
        if (fileName.isEmpty()) return;
        m_scenario_file = fileName;
    }
    if (!m_scenario_file.contains(".json")) m_scenario_file += ".json";
    if (!m_scenario_file.isEmpty()) saveScenario(m_scenario_file);

    QMessageBox::information(this, "Сохранение", "Сценарий сохранен!");

    QSettings settings;
    settings.setValue("last_scenario", m_scenario_file);
}

QString ScenarioEditor::getMapPathFromScenarioFile(const QString &scenarioFilePath){
    QFile file(scenarioFilePath);
    if (!file.open(QIODevice::ReadOnly)) return "";
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    auto rootJson = doc.object();

    if (rootJson.contains("map_path")) { return rootJson["map_path"].toString();}
    return "";
}

void ScenarioEditor::moveAllElements(const QPointF &newCenter) {
    bool error;
    auto delta = newCenter - m_scenarioParser->getSimulationParameters()->getMapCenterDeg(error);

    for (auto obj: m_scenarioParser->getObjects()) {
        if (obj->getId() == "") continue;

        bool isOk;
        double lat = obj->getPropertyDouble("lat", isOk);
        double lon = obj->getPropertyDouble("lon", isOk);
        if (!isOk) continue;

        obj->setPropertyValue("lat", lat + delta.x());
        obj->setPropertyValue("lon", lon + delta.y());
        auto p = QPointF(lat + delta.x(), lon + delta.y());
        if (m_signObjects.contains(obj->getId())) {
            auto sign = m_signObjects[obj->getId()];
            sign->setCoordinatesInDegrees({p});
            m_signController->updateSignOnMap(sign);
        }

    }

    for (auto feature: m_scenarioParser->getFeatures()) {
        if (feature->getId() == "") continue;

        QList<QPointF> points =  feature->getCoordinates();
        for (int i = 0; i < points.size(); i++) {
            points[i] = QPointF(points[i].x() + delta.x(), points[i].y() +  delta.y());
        }

        feature->setCoordinates(points);
        if (!m_signObjects.contains(feature->getId())) continue;

        auto sign = m_signObjects[feature->getId()];
        sign->setCoordinatesInDegrees(points);
        m_signController->updateSignOnMap(sign);
    }


    emit needMapRepaint();
}

int ScenarioEditor::loadScenario(const QString &filePath, const QString &mapPath,MapWidget *mapWidget)
{
    if (!m_scenarioParser->loadScenario(filePath)){
        QMessageBox::warning(this, "Error", "Ошибка загрузки сценария " + filePath );
    }


    m_signController->clear();
    m_signObjects.clear();
    m_scenario_file = filePath;
    emit clearMap();

    if (mapPath != "" && mapWidget->loadMap(mapPath)<0)  return -1;

    ui->treeWidget->clear();
    m_scenarioParser->buildTree(ui->treeWidget);
    m_scenarioParser->filterTreeByTypeView((TypeView)ui->tabWScenarioElements->currentIndex());
    bool error = false;
    auto  centerDeg = m_scenarioParser->getSimulationParameters()->getMapCenterDeg(error);
    if (!error) {
        auto zoom = m_scenarioParser->getSimulationParameters()->getMapZoom();
        emit setMapCenter(centerDeg.x(), centerDeg.y(),  zoom);
    }

    // В классе, использующем дерево (например, в конструкторе окна)
    ui->treeWidget->setItemDelegate(new PropertyItemDelegate(this));
    createSignsFromObjects(m_scenarioParser->getObjects(), m_scenarioParser->getFeatures());

    QSettings settings;
    settings.setValue("last_scenario", m_scenario_file);
    return 0;
}

void ScenarioEditor::handlePropertyEdit(QTreeWidgetItem *item, int column) {
    if (column != 1) return;
    QString newValueStr = item->text(1);

    PropertyModel *prop = item->data(0, Qt::UserRole).value<PropertyModel*>();
    if (!prop) return;

    QVariant oldValue = prop->value();

    // Обновляем текст, если значение изменилось или ввод был некорректным
    if (prop->value() != newValueStr && prop->value().isValid())
         prop->setValue(newValueStr);
}

bool ScenarioEditor::saveScenario(const QString &filePath)
{
    if (!m_scenarioParser->saveScenario(filePath)){
        QMessageBox::warning(this, "Error", "Ошибка сохранения файла " + filePath);
        return false;
    } else
        m_scenario_file = filePath;
    return true;
}

void ScenarioEditor::createNewScenario()
{
    if (ui->treeWidget->topLevelItemCount() > 0) {
        if (QMessageBox::question(this, "New Scenario", "Current scenario will be lost. Continue?")
            != QMessageBox::Yes) {
            return;
        }
    }
    ui->treeWidget->clear();
}

void ScenarioEditor::addScenarioElement()
{
    switch (ui->tabWScenarioElements->currentIndex()) {
        case 0:
        case 1: {
            DAddScenarioObject dAddScenarioObject(this);
            if (dAddScenarioObject.exec() != QDialog::Accepted) return;

            auto jsonNewObject = dAddScenarioObject.getJsonObjectScenarioModel();
            ObjectScenarioModel *newObject = m_scenarioParser->addObjectScenarioModel(jsonNewObject);
            newObject->setPropertyValue("is_own", QVariant(ui->tabWScenarioElements->currentIndex()));
            m_scenarioParser->filterTreeByTypeView((TypeView) ui->tabWScenarioElements->currentIndex());
            createSignFromObject(newObject);
            ui->treeWidget->clearSelection();
            newObject->getTreeWidgetItem()->setSelected(true);
            ui->treeWidget->setCurrentItem(newObject->getTreeWidgetItem());

            if (jsonNewObject.contains("features")) {
                for (auto feature: jsonNewObject["features"].toArray()) {
                    if (!feature.toObject().contains("id")) continue;

                    FeatureModel *newFeature = m_scenarioParser->addFeatureModel(feature.toObject());
                    int idx = 1;
                    while (m_signObjects.contains(newFeature->getId())) idx++;
                    newFeature->setId(newFeature->getId() + QString::number(idx));
                    bool isOk;
                    newFeature->setOwn(newObject->getPropertyInt("is_own", isOk));
                    newFeature->setParentID(newObject->getId());
                    createSignFromFeature(newFeature);

                }
            }

            //Добавление ядра моделирования
            DAddScenarioInteraction dAddScenarioInteraction(false,this);
            dAddScenarioInteraction.setObjects(m_scenarioParser->getObjects(), m_scenarioParser->getFeatures());
            dAddScenarioInteraction.setCurrentObject(newObject);
            if (dAddScenarioInteraction.exec() == QDialog::Accepted) {
                QJsonArray arrayInteractionModels = dAddScenarioInteraction.getJsonInteractionModels();
                for(auto jsonInteractionModel: arrayInteractionModels) {
                    InteractionModel *newInteractionModel = m_scenarioParser->addInteractionModelWithAddTree(
                            jsonInteractionModel.toObject());
                }
            }

            if (QMessageBox::question(this, "Добавление объекта", "Нанести объект на карту?") != QMessageBox::Yes) return;

            drawSignOnMap( newObject->getId());

        }
            break;
        case 4: {
            DAddScenarioInteraction dAddScenarioInteraction(false,this);
            dAddScenarioInteraction.setObjects(m_scenarioParser->getObjects(), m_scenarioParser->getFeatures());
            if (dAddScenarioInteraction.exec() == QDialog::Accepted) {
                QJsonArray arrayInteractionModels = dAddScenarioInteraction.getJsonInteractionModels();
                for(auto jsonInteractionModel: arrayInteractionModels) {
                    InteractionModel *newInteractionModel = m_scenarioParser->addInteractionModelWithAddTree(
                            jsonInteractionModel.toObject());
                    ui->treeWidget->clearSelection();
                    newInteractionModel->getTreeWidgetItem()->setSelected(true);
                    ui->treeWidget->setCurrentItem(newInteractionModel->getTreeWidgetItem());
                }
            }
        }
            break;
    }

}

void ScenarioEditor::removeSelectedElement()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (!item) return;
    auto objectModel = ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<ObjectScenarioModel*>();
    auto pFeatureModel = ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<FeatureModel*>();
    auto pInteractionModel = ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<InteractionModel*>();
    if (!objectModel && !pFeatureModel && !pInteractionModel) {
        QMessageBox::warning(this, "Error", "Ошибка удаления элемента, необходимо выбрать объект моделирования");
        return;
    }
    SignBase *sign = nullptr;
    if (objectModel){
        for(auto featureItem: m_scenarioParser->getFeatures()) {
            if(featureItem->getParentID() == objectModel->getId()) {
                sign = m_signObjects[featureItem->getId()];
                m_signObjects.remove(featureItem->getId());
                m_signController->removeSign(sign->getGisID());
            }
        }
        m_scenarioParser->removeObjectScenarioModel(objectModel);
        sign = m_signObjects[objectModel->getId()];
        m_signObjects.remove(objectModel->getId());
    }
    if (pFeatureModel) {
        m_scenarioParser->removeFeatureModel(pFeatureModel);
        sign = m_signObjects[pFeatureModel->getId()];
        m_signObjects.remove(pFeatureModel->getId());
    }
    if (pInteractionModel) {
        m_scenarioParser->removeInteractionModel(pInteractionModel);
    }
    if (!sign) return;

    m_signController->removeSign(sign->getGisID());
    emit needMapRepaint();
}

void ScenarioEditor::editSelectedElement()
{
//    QTreeWidgetItem *item = treeWidget->currentItem();
//    if (!item) return;
//
//    bool ok;
//
//
//    QString value = QInputDialog::getText(this, "Edit Element",
//                                          "Parameter value:",
//                                          QLineEdit::Normal,
//                                          item->text(1), &ok);
//
//    item->setText(1, value);
}

void ScenarioEditor::cloneSelectedElement()
{
    switch (ui->tabWScenarioElements->currentIndex()) {
        case 0:
        case 1: {
            QTreeWidgetItem *item = ui->treeWidget->currentItem();
            if (!item) return;

            auto cloneObject = ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<ObjectScenarioModel*>();
            QJsonObject jsonObject = cloneObject->toJson();
            QString objectId = jsonObject["id"].toString();
            objectId = objectId.left(objectId.lastIndexOf("_"));
            jsonObject["id"] = objectId;

            ObjectScenarioModel *newObject = m_scenarioParser->addObjectScenarioModel(jsonObject);
            newObject->setPropertyValue("is_own", QVariant(ui->tabWScenarioElements->currentIndex()));
            m_scenarioParser->filterTreeByTypeView((TypeView) ui->tabWScenarioElements->currentIndex());
            createSignFromObject(newObject);
            ui->treeWidget->clearSelection();
            newObject->getTreeWidgetItem()->setSelected(true);
            ui->treeWidget->setCurrentItem(newObject->getTreeWidgetItem());
        }
        default: {
        }
    }

    //    QTreeWidgetItem *clone = item->clone();
    //    QTreeWidgetItem *parent = item->parent();
    //
    //    if (parent) {
    //        parent->addChild(clone);
    //    } else {
    //        treeWidget->addTopLevelItem(clone);
    //    }
    //
    //    treeWidget->setCurrentItem(clone);
}

QTreeWidgetItem* ScenarioEditor::findChildItem(QTreeWidgetItem *parent, const QString &name) const
{
    for (int i = 0; i < parent->childCount(); i++) {
        QTreeWidgetItem *child = parent->child(i);
        //qLog() << child->text(0);
        if (child->text(0) == name) {
            return child;
        }
    }
    return nullptr;
}

QString ScenarioEditor::findChildValue(QTreeWidgetItem *parent, const QString &name) const
{
    QTreeWidgetItem *item = findChildItem(parent, name);
    return item ? item->text(1) : QString();
}


void ScenarioEditor::createSignsFromObjects(QList<ObjectScenarioModel*> objects, QList<FeatureModel*> features){
    // Очищаем предыдущие связи
    m_signObjects.clear();
    m_tmp_listFeatures = features;
    for (auto obj: features) {
        createSignFromFeature(obj);
    }

    // Проходим по всем объектам
    for (auto obj: objects) {
        createSignFromObject(obj);
        updateConnectedSigns(obj);
    }

    emit needMapRepaint();
}

void ScenarioEditor::createSignFromFeature(FeatureModel* featureMain) {

    QString id = featureMain->getId();

    m_features[featureMain->getParentID()][featureMain->getId()] = featureMain;

    auto sign =  SignFactory::createSignFromFeature(featureMain);
    m_signObjects[featureMain->getId()] = sign;
    if(sign->getCoordinatesInDegrees().size() > 0)
        m_signController->addSignToMap(sign);

    connect(sign, &SignBase::coordinateChanged, [featureMain, sign](QList<QPointF>) {
        featureMain->setCoordinates(sign->getCoordinatesInDegrees());
    });

    // если поменяли значение в дереве, то меняем значение в знаке отображения
    connect(featureMain, &FeatureModel::redrawFeature, [featureMain,this](FeatureModel* feature){
        auto spline = featureMain->getSpline();
        auto sign = m_signObjects[feature->getId()];
        if (!sign) return;
        sign->setBaseColor(feature->getColor());
        sign->setColorRGB(feature->getColor());
        sign->setParamColorSign(feature->getColor());
        sign->setLineWidth(QString::number(feature->getLineWidth()));
        sign->setSpline(feature->getSpline());
        if (feature->getGeometryType() == FeatureModel::GeometryType::CIRCLE) {
            auto radius = feature->getRadius();
            if (radius <= 0) radius = feature->getPropertyInt("radius", 0);
            ((SignCircle*)sign)->setCoordinatesInDegrees(feature->getCoordinates(), radius);
        } else
            sign->setCoordinatesInDegrees(feature->getCoordinates());
        if (feature->getGeometryType()  == FeatureModel::GeometryType::TITLE ) {
            ((SignTitle*)sign)->setTitleText(feature->getText());
            ((SignTitle*)sign)->setFontSize(feature->getFontSize());
        }

        m_signController->updateSignOnMap(sign);
        emit needMapRepaint();
    });

    connect(featureMain, &FeatureModel::centerOnMap, [featureMain,this](double lat, double lon){
        emit selectSignOnMap(m_signObjects[featureMain->getId()]);
        emit setMapCenter(lat, lon, -1);
    });

//    connect(feature, &FeatureModel::createOnMap, [this](QString sign_id){
//        SignBase* sign = m_signObjects[sign_id];
//        SignDrawer::instance()->startDrawing(sign);
//        connect(SignDrawer::instance(), &SignDrawer::signDrawEnd, [this,sign]() {
//            m_signController->addSignToMap(sign);
//        });
//    });
}

void ScenarioEditor::createSignFromObject(ObjectScenarioModel* obj) {
    QString id = obj->getId();
    // Извлекаем необходимые параметры
    auto props = obj->toJson();

    // Преобразуем координаты
    bool okLon, okLat, okCourse, okOwn;

    QString classcode = obj->getPropertyString("class_code");

    bool isOwn = obj->getPropertyBool("is_own");

    // Проверяем обязательные параметры
    if (!classcode.isEmpty()) {

        double lon = obj->getPropertyDouble("lon", okLon);
        double lat = obj->getPropertyDouble("lat", okLat);
        double course = obj->getPropertyDouble("course", okCourse);
        if (!okCourse) course = 0;

        QString name = obj->getTitle(); // Имя берем из заголовка объекта
//        if (okLon && okLat) {
            QList<QPointF> coords = {QPointF(lat, lon)};
            SignBase *sign =  SignFactory::createSign(classcode, name, isOwn, coords, course, obj);

            if (!sign)  qWarning() << "Failed to create sign for" << name;
//            sign->setOwn();
//            sign->setParamState( QString::number(static_cast<int>(ESignParamState::DESTROYED)));
            m_signObjects[id] = sign;
            if (lon != 0 && lat != 0)
                m_signController->addSignToMap(sign);
            setupSignPropertyConnections(obj, sign, "");

            connect(obj, &ObjectScenarioModel::propertiesChanged, [this, obj](){
                updateConnectedSigns(obj);
            });
//        }
    }

    auto geo_props = obj->getPropertiesByType("feature");

    for (auto geo_key: geo_props) {
        bool  bOk;
        auto *feature = obj->getPropertyFeature(geo_key, bOk);
        if (!bOk) continue;

        QList<QPointF> coords = feature->getCoordinates();
        auto child =  SignFactory::createSign(feature->getClassCode(), feature->getId(), isOwn, coords, 0, obj);
        m_signObjects[feature->getId()] = child;
        m_signController->addSignToMap(child);
        setupSignPropertyConnections(obj, child, geo_key, feature->getId());
    }

    connect(obj, &ObjectScenarioModel::centerOnMap, [id,this](double lat, double lon){
        emit selectSignOnMap(m_signObjects[id]);
        emit setMapCenter(lat, lon, -1);
    });

    connect(obj, &ObjectScenarioModel::createOnMap, [this, id](QString idSign){
        drawSignOnMap( idSign);
    });
}

void ScenarioEditor::drawSignOnMap(QString objectID) {
    SignBase* sign = m_signObjects[objectID];
    SignDrawer::instance()->startDrawing(sign);
    auto connectSignDrawer = std::make_shared<QMetaObject::Connection>();
    *connectSignDrawer = connect(SignDrawer::instance(), &SignDrawer::signDrawEnd, [this,sign,connectSignDrawer, objectID]() {

        m_signController->addSignToMap(sign);
        QObject::disconnect(*connectSignDrawer);
        if (!m_features.contains(objectID)) return;
        for (auto feature: m_features[objectID])
            updateConnectedSign(feature);
    });
}

// обновляем связанные знаки с объектами
void ScenarioEditor::updateConnectedSign(FeatureModel* feature, ObjectScenarioModel* obj) {
    if (!feature) return;
    obj = obj == nullptr ?  m_scenarioParser->getObjectById(feature->getParentID()) : obj;
    SignBase* featureSign = m_signObjects[feature->getId()];
    if (!featureSign || !obj || !SignFactory::updateConnectedSign(featureSign, obj)) return;
    m_signController->updateSignOnMap(featureSign);
    emit needMapRepaint();
}

void ScenarioEditor::updateConnectedSigns(ObjectScenarioModel *item){
    if (!m_features.contains(item->getId())) return;
     for (auto feature: m_features[item->getId()])
            updateConnectedSign(feature, item);
}

void ScenarioEditor::setupSignPropertyConnections(ObjectScenarioModel *item, SignBase *sign, const QString &field_name, const QString &geo_id)
{
    // Обработка изменения позиции знака
    if (sign->getGeometryType() == SignBase::SignType::LOCAL_POINT) {
        disconnect(sign, &SignBase::positionChanged, nullptr, nullptr);
        connect(sign, &SignBase::positionChanged, this, [item, this](const QPointF &newPos) {
            qDebug() << "positionChanged " << item->getId();
            item->setPropertyValue("lat", newPos.x());
            item->setPropertyValue("lon", newPos.y());
            updateConnectedSigns(item);
        });
    }
    // Обработка изменения курса
    if (sign->getGeometryType() == SignBase::SignType::LOCAL_POINT) {
        disconnect(sign, &SignBase::courseChanged, nullptr, nullptr);
        connect(sign, &SignBase::courseChanged, this, [item, this](double value) {
            item->setPropertyValue("course", value);
            //updateConnectedSigns(item);
        });
    }

    if (sign->getGeometryType() != SignBase::SignType::LOCAL_POINT) {
        disconnect(sign, &SignBase::coordinateChanged, nullptr, nullptr);
        connect(sign, &SignBase::coordinateChanged, this,
                [geo_id, item, sign, field_name, this](const QList<QPointF> &newCoordinates) {
                    item->setPropertyValue(field_name.toUtf8(), sign->getJsonFeature(geo_id));
                });
    }

    disconnect(item, &ObjectScenarioModel::centerOnMap, nullptr, nullptr);
    connect(item, &ObjectScenarioModel::centerOnMap, [this](double lat, double lon){
       emit setMapCenter(lat, lon, -1);
    });

    disconnect(item, &ObjectScenarioModel::createOnMap, nullptr, nullptr);
    connect(item, &ObjectScenarioModel::createOnMap, [this](QString idSign){
        SignBase* sign = m_signObjects[idSign];
        SignDrawer::instance()->startDrawing(sign);
        auto connectSignDrawer = std::make_shared<QMetaObject::Connection>();
        *connectSignDrawer = connect(SignDrawer::instance(), &SignDrawer::signDrawEnd, [this,sign,connectSignDrawer]() {
           m_signController->addSignToMap(sign);
           QObject::disconnect(*connectSignDrawer);
        });
    });

    disconnect(item, &ObjectScenarioModel::createFeatureOnMap, nullptr, nullptr);
    connect(item, &ObjectScenarioModel::createFeatureOnMap, [this, item](QString objectId, QString classCode, QString featureId, bool isMulti){
        QList<QPointF> points = {};
        QList<QList<QPointF>> list_points = {};
        SignBase* sign;
        if (m_signObjects.contains(featureId))  {
            sign = m_signObjects[featureId];
        } else {
            sign = SignFactory::createSign(classCode, featureId, true, points, 0, item);
        }
        SignDrawer::instance()->startDrawing(sign);
        auto connectSignDrawer = std::make_shared<QMetaObject::Connection>();
        *connectSignDrawer = connect(SignDrawer::instance(), &SignDrawer::signDrawEnd, [this,sign,connectSignDrawer, item]() {
            m_signController->addSignToMap(sign);
            QObject::disconnect(*connectSignDrawer);
            updateConnectedSigns(item);
        });
    });

}

QStringList ScenarioEditor::findGeoProperties(const QJsonObject& properties) {

    // Получаем все ключи
    QStringList allKeys = properties.keys();
    // Список для хранения ключей, начинающихся с "geo_"
    QStringList geoKeys;
    // Фильтруем ключи, начинающиеся с "geo_"
    for (const QString& key : allKeys)
        if (key.startsWith("geo_")) geoKeys.append(key);

    return geoKeys;
}

QList<QPointF> ScenarioEditor::JsonArrayToQList(QJsonArray coord) {
    QList<QPointF> points;

    for (int i = 0; i < coord.size(); i++) {
        if (!coord[i].isArray()) {
            qWarning() << "Invalid coordinate at index" << i << ": not an array";
            continue;
        }

        QJsonArray pointArray = coord[i].toArray();
        if (pointArray.size() < 2) {
            qWarning() << "Invalid coordinate at index" << i << ": point array must have 2 elements";
            continue;
        }

        if (pointArray[0].isDouble() && pointArray[1].isDouble()) {
            qreal x = pointArray[0].toDouble();
            qreal y = pointArray[1].toDouble();
            points.append(QPointF(x, y));
        } else {
            qWarning() << "Invalid coordinate values at index" << i;
        }
    }
    return points;
}

void ScenarioEditor::updateObjectStateFromModel(QJsonArray jsonObjectList, QJsonArray features){
    qDebug() << "updateObjectStateFromModel";
    QList<SignBase*> topSigns;
    for (auto featureItem: features) {
        auto obj = featureItem.toObject();
        if (!obj.contains("id")) continue;
        auto feature = m_scenarioParser->updateFeatureById(obj["id"].toString(), obj);
        if (!feature) continue;
        auto *sign = m_signObjects[feature->getId()];
        if (!sign) continue;
        if (feature->getGeometryType() == FeatureModel::GeometryType::CIRCLE)
            ((SignCircle*) sign)->setCoordinatesInDegrees(feature->getCoordinates(), feature->getRadius());
        else
            sign->setCoordinatesInDegrees(feature->getCoordinates());
        if (feature->getPosition() == FeatureModel::PositionType::ALWAYS_TOP)
            topSigns.append(sign);
        else
            m_signController->updateSignOnMap(sign);
    }

    // TODO: прежде чем что-то обновлять на карте возможно нужно проверить изменение координат
    for(auto arrItem: jsonObjectList) {
        auto obj = arrItem.toObject();
        if (!obj.contains("id")) {
            qLog() << "Неизвестный объект моделирования" << (obj.contains("title") ? obj["title"].toString() : "");
            continue;
        }

        auto id = obj["id"].toString();
        if (!m_scenarioParser->getObjectById(id)) {
            qInfo() << "Неизвестный объект моделирования" << id;
            continue;
        }
        m_scenarioParser->updateObjectById(id, obj);

        if (!obj.contains("properties")) continue;
        auto props = obj["properties"].toObject();



        double lat = props.contains("lat") ? props["lat"].toDouble() : 0;
        double lon = props.contains("lon") ? props["lon"].toDouble() : 0;
        double course = props.contains("course") ? props["course"].toDouble() : 0;

        if (m_signObjects.contains(id)) {
            auto *sign = m_signObjects[id];
            if (props.contains("state") && props["state"].toString().startsWith("destroyed") ) {
                sign->setParamState(QString::number(static_cast<int>(ESignParamState::DESTROYED)));
                m_signController->updateSignOnMap(sign);
            }

            if (props.contains("state") && props["state"].toString() == "corrupted")
                sign->setParamState( QString::number(static_cast<int>(ESignParamState::CORRUPTED)));

            auto geo_list = findGeoProperties(props);
            for (const auto& geo_field: geo_list) {
                // если объект уничтожен то у него нет ни пути ни других гео объектов
                if (props.contains("state") && props["state"].toString().startsWith("destroyed")) continue;

                auto child_id = props[geo_field].toObject()["properties"].toObject()["id"].toString();
                SignBase *childSign = nullptr;
                if (m_signObjects.contains(child_id) )
                    childSign = m_signObjects[child_id];
                else
                    childSign  = sign->getChild(geo_field);

                if (!childSign) {
                    qLog() << "Появился гео знак неизвестно откуда";
                    continue;
                }
                qDebug() << "new geometry " << props[geo_field].toObject()["geometry"].toObject();
                auto coordinates = props[geo_field].toObject()["geometry"].toObject()["coordinates"].toArray();
                auto geoType = props[geo_field].toObject()["geometry"].toObject()["type"].toString().toLower();
                qDebug() << "geo_field " << geo_field << "geoType " << geoType ;
                if (geoType == "polygon") coordinates = coordinates[0].toArray();
                childSign->setCoordinatesInDegrees(JsonArrayToQList(coordinates));
                m_signController->updateSignOnMap(childSign);
            }

            if (lon !=0 && lat !=0) {
                sign->setCoordinatesInDegrees({QPointF(lat,lon)}, course);
                m_signController->updateSignOnMap(sign);
                // m_signController->updateSignOnMap(sign->getGisID(), lat, lon, course);
            }
            // смотрим появлись ли поля для отображения гео объектов

            // поля есть нужно проверить что они изменились

        } else {
            QString class_code = props.contains("class_code") ? props["class_code"].toString() : "";
            if (class_code =="") continue;
            QString title = props.contains("title") ? props["title"].toString() : 0;
            int is_own = props.contains("is_own") ? props["is_own"].toInt() : 0;
            // Создаем знак через фабрику
            auto coord = QList<QPointF>{QPointF(lat, lon)};
            SignBase *sign = SignFactory::createSign(class_code, title, is_own, coord, course, nullptr);
            if (!sign) {
                qWarning() << "Failed to create sign for" << title;
                continue;
            }
            m_signObjects[id] = sign;
            m_signController->addSignToMap(sign);
            // тут надо подумать, если появился объект то его нужно отображать и в дереве
        }
    }

    for (auto sign: topSigns) {
        // Skip signs without a name so they don't produce empty labels
        if (sign->getName().isEmpty())
            continue;
        m_signController->redrawSignOnMap(sign);
    }
    emit needMapRepaint();
}

QString ScenarioEditor::getTempFileName(){
    auto config = EnvConfig::getInstance();
    QString tempDirPath = config->get("PATH_TMP", "");
    QDir dir;

    // Проверяем наличие папки и создаём её при необходимости
    if (!dir.exists(tempDirPath)) {
        dir.mkpath(tempDirPath);
    }

    QString tmpFilePath = tempDirPath + "/current_scenario.json";

    return tmpFilePath;

}

void ScenarioEditor::addTacticalSign(QString classCod, QJsonObject data) {

    if(data["is_own"].toInt() < 2 && ui->tabWScenarioElements->currentIndex()>1) {
        QMessageBox::information(this, "Добавление тактического знака","Для добавление тактического должа быть выбрана вкладка \"Противник\" или \"Свои\"", QMessageBox::Ok);
        return;
    }

    if(data["is_own"].toInt() < 2) {
        data["is_own"] = ui->tabWScenarioElements->currentIndex();
    }

    ObjectScenarioModel* objectModel = nullptr;
    bool isGlobal = data["parent_object_id"].toString() == "no";
    if(!isGlobal) {
        if(ui->treeWidget->currentItem())
            objectModel = ui->treeWidget->currentItem()->data(0,Qt::UserRole).value<ObjectScenarioModel*>();
        if(!objectModel){
            QMessageBox::critical(this,"Добавление тактического знака", "Не выбран объект моделирования для \n которого нужно нанести тактический знак.", QMessageBox::Ok);
            return;
        }
        data["parent_object_id"] = objectModel->getId();
    }

    FeatureModel *featureModel = m_scenarioParser->addFeatureModel(data);
    createSignFromFeature(featureModel);

    SignBase* sign = m_signObjects[featureModel->getId()];
    SignDrawer::instance()->startDrawing(sign);
    auto connectSignDrawer = std::make_shared<QMetaObject::Connection>();
    *connectSignDrawer = connect(SignDrawer::instance(), &SignDrawer::signDrawEnd, [this,sign, featureModel, connectSignDrawer]() {
        m_signController->addSignToMap(sign);
        QList<QPointF> coordinates = sign->getCoordinatesInDegrees();

        if(featureModel->getType() == "DIRECTION_OF_MOVEMENT") {
            QList<ObjectScenarioModel*> scenarioObjects = m_scenarioParser->getObjects();
            for(ObjectScenarioModel* iterObject: scenarioObjects) {
                if(iterObject->getId() == featureModel->getParentID()) {
                    bool isOk = true;
                    double lat = iterObject->getPropertyDouble("lat", isOk);
                    double lon = iterObject->getPropertyDouble("lon", isOk);
                    QPointF coordinate(lat, lon);
                    coordinates.removeFirst();
                    coordinates.insert(0, coordinate);
                    featureModel->setCoordinates(coordinates);
                    m_scenarioParser->addInteractionModel(iterObject, featureModel);
                    sign->setCoordinatesInDegrees(coordinates);
                    break;
                }
            }
        } else {
            featureModel->setCoordinates(coordinates);
            if (featureModel->getGeometryType() == FeatureModel::GeometryType::CIRCLE) {
                auto radius = featureModel->getPropertyInt("radius", 0);
                if (radius > 0 ) ((SignCircle*) sign)->setCoordinatesInDegrees(sign->getCoordinatesInDegrees(), radius);
            }
        }

        QObject::disconnect(*connectSignDrawer);
    });
}

void ScenarioEditor::updateObjectsFromBD() {
    QList<DataStorageItem *> dataStorageItems = DataStorageServiceFactory::getInstance()->getElements(TypeElement::teOBJECT);
    m_scenarioParser->clearFileModels();
    m_scenarioParser->clearAlgorithmModels();
    for(ObjectScenarioModel* iterObject: m_scenarioParser->getObjects()) {
        QString objectId = (iterObject->getId()=="Model_00"||iterObject->getId()=="WEATHER")?iterObject->getId():iterObject->getId().left(iterObject->getId().lastIndexOf("_"));
        QJsonObject templateJsonObject;
        for(DataStorageItem* iterItem: dataStorageItems) {
            if(iterItem->getData()["id"] == objectId){
                templateJsonObject = iterItem->getData();
                break;
            }
        }
        if(templateJsonObject.isEmpty())
            continue;

        //Берем параметры шаблона и если у объекта нет параметра то добавляем
        ObjectScenarioModel* templateObject = ObjectScenarioModel::fromJson(templateJsonObject, this);
        for(QString iterKey: templateObject->properties().keys()) {
            if(!iterObject->containsProperty(iterKey)) {
                iterObject->addProperty(iterKey, templateObject->properties().value(iterKey));
            }
        }

        iterObject->clearActions();

        for(ActionModel* iterAction: templateObject->actions()) {
            iterObject->addAction(ActionModel::fromJson(iterAction->toJson(), iterObject));
            m_scenarioParser->checkAndAddAlgorithmModel(iterAction->algorithmId());
        }

        iterObject->getTreeWidgetItem(nullptr, true);
    }
}

void ScenarioEditor::setEventLog(SimulationEventWidgetInMap *eventLog) {
    this->m_eventLog = eventLog;
    ui->vlTab->addWidget(this->m_eventLog);
    this->m_eventLog->setVisible(false);
}

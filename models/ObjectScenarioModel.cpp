#include "ObjectScenarioModel.h"
#include "Feature.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QMessageBox>
#include <QHBoxLayout>

ObjectScenarioModel::ObjectScenarioModel(QObject *parent) : QObject(parent)
{
}

ObjectScenarioModel::~ObjectScenarioModel()
{
}

// Геттеры
QString ObjectScenarioModel::getId() const { return m_id; }
QString ObjectScenarioModel::getTitle() const { return m_title; }
QMap<QString, PropertyModel*> ObjectScenarioModel::properties() const { return m_properties; }
QList<ActionModel*> ObjectScenarioModel::actions() const { return m_actions; }

// Сеттеры
void ObjectScenarioModel::setId(const QString &id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

void ObjectScenarioModel::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void ObjectScenarioModel::setProperties(const QMap<QString, PropertyModel*> &properties)
{
    qDeleteAll(m_properties);
    m_properties = properties;
    emit propertiesChanged();
}

void ObjectScenarioModel::setActions(const QList<ActionModel*> &actions)
{
//    qDeleteAll(m_actions);
    for(ActionModel* iterAction: m_actions)
        delete iterAction;
    m_actions = actions;
    emit actionsChanged();
}
void ObjectScenarioModel::setFeatures(const QList<FeatureModel*> &features)
{
//    qDeleteAll(m_actions);
    for(FeatureModel* iterFeature: m_features)
        delete iterFeature;
    m_features = features;
}

// Управление свойствами
void ObjectScenarioModel::addProperty(const QString &key, PropertyModel *property)
{
    if (m_properties.contains(key)) {
        delete m_properties[key];
    }
    m_properties[key] = property;
    emit propertiesChanged();
}

QString ObjectScenarioModel::getPropertyString(const QString &key) const{
    if (!m_properties.contains(key)) return "";
    return m_properties[key]->value().toString();
}
double ObjectScenarioModel::getPropertyDouble(const QString &key, bool& ok) const{
    if (!m_properties.contains(key)) { ok = false; return 0;}
    return m_properties[key]->value().toDouble(&ok);
}

int ObjectScenarioModel::getPropertyInt(const QString &key, bool& ok) const{
    if (!m_properties.contains(key)) { ok = false; return 0;}
    return m_properties[key]->value().toInt(&ok);
}

bool ObjectScenarioModel::getPropertyBool(const QString &key) const{
    if (!m_properties.contains(key)) {   return false;}
    bool v = m_properties[key]->value().toBool();
    return v;
}

Feature*  ObjectScenarioModel::getPropertyFeature(const QString &key, bool& ok) const{
    ok = true;
    if (!m_properties.contains(key)) { ok = false; return nullptr;}
    if (!m_properties[key]->value().canConvert<Feature*>()) { ok = false; return nullptr;}
    return m_properties[key]->value().value<Feature*>();
}

PropertyModel* ObjectScenarioModel::getProperty(const QString &key) const
{
    return m_properties.value(key, nullptr);
}

void ObjectScenarioModel::removeProperty(const QString &key)
{
    if (m_properties.contains(key)) {
        delete m_properties.take(key);
        emit propertiesChanged();
    }
}

// Управление действиями
void ObjectScenarioModel::addAction(ActionModel *action)
{
    if (action && !m_actions.contains(action)) {
        m_actions.append(action);
        emit actionsChanged();
    }
}

void ObjectScenarioModel::removeAction(ActionModel *action)
{
    if (m_actions.removeOne(action)) {
        delete action;
        emit actionsChanged();
    }
}

void ObjectScenarioModel::clearActions()
{
    if (actionsItem!=nullptr)
        for(int i = actionsItem->childCount()-1; i >= 0; i--)
            actionsItem->removeChild(actionsItem->child(i));
    qDeleteAll(m_actions);
    m_actions.clear();
    emit actionsChanged();
}

// JSON сериализация
QJsonObject ObjectScenarioModel::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["title"] = m_title;

    // Сериализация свойств
    QJsonArray propertiesArr;
    for (auto it : m_properties)
        propertiesArr.append(it->toJson());

    json["properties"] = propertiesArr;

    // Сериализация действий
    QJsonArray actionsArray;
    for (ActionModel *action : m_actions) {
        if (action) {
            actionsArray.append(action->toJson());
        }
    }
    json["actions"] = actionsArray;

    // Сериализация features
    QJsonArray featuresArray;
    for (FeatureModel *feature : m_features) {
        if (feature) featuresArray.append(feature->toJson());

    }
    json["features"] = featuresArray;

    return json;
}

ObjectScenarioModel* ObjectScenarioModel::fromJson(const QJsonObject &json, QObject* parent)
{
    auto *model = new ObjectScenarioModel(parent);
    model->setId(json["id"].toString());
    model->setTitle(json["title"].toString());

    // Десериализация свойств
    if (json.contains("properties") && json["properties"].isArray()) {
        QJsonArray propertiesArr = json["properties"].toArray();
        QMap<QString, PropertyModel*> properties;

        for (auto p: propertiesArr) {
            auto  prop = PropertyModel::fromJson(p.toObject(), model);
            properties[prop->name()] = prop;
            if (prop->type() == PropertyModel::ValueType::FEATURE) {
                connect(prop, &PropertyModel::centerFeatureOnMap, model, [model](double lat, double lon) {
                    emit  model->centerOnMap(lat, lon);
                });

                connect(prop, &PropertyModel::createFeatureOnMap, model, [model](QString classCode, QString featureId, bool isMulti) {
                    emit  model->createFeatureOnMap(model->getId(), classCode, featureId, isMulti);
                });
                //connect(model, &ObjectScenarioModel::centerOnMap, prop, &PropertyModel::centerFeatureOnMap);
            } else {
//                connect(prop, &PropertyModel::valueChanged, model, [model](){
//                    emit model->propertiesChanged();}
//                );
            }
        }

        model->setProperties(properties);
    }

    // Десериализация действий
    if (json.contains("actions") && json["actions"].isArray()) {
        QJsonArray actionsArray = json["actions"].toArray();
        QList<ActionModel*> actions;

        for (const QJsonValue &value : actionsArray) {
            if (value.isObject()) {
                actions.append(ActionModel::fromJson(value.toObject(), model));
            }
        }

        model->setActions(actions);
    }

    // Десериализация знаков
    if (json.contains("features") && json["features"].isArray()) {
        QJsonArray featuresArray = json["features"].toArray();
        QList<FeatureModel*> features;

        for (const QJsonValue &value : featuresArray) {
            if (value.isObject()) {
                features.append(FeatureModel::fromJson(value.toObject(), model));
            }
        }

        model->setFeatures(features);
    }

    return model;
}

// Метод для отображения в дереве
void ObjectScenarioModel::addSearchButton() {

    bool isOk = true;
    double lat = getPropertyDouble("lat", isOk);
    if (!isOk) return;
    double lon = getPropertyDouble("lon", isOk);
    if (!isOk) return;

    btnMapAction = new QPushButton();
    btnMapAction->setFixedSize(20, 20);
    QPixmap pixmap((lat != 0 && lon != 0)?":/icons/SEARCH_ON_MAP":":/icons/ADD_TO_MAP_ICON");
    QIcon icon(pixmap);
    btnMapAction->setIcon(icon);
    btnMapAction->setIconSize(QSize(15, 15));

    auto *container = new QWidget(treeWidgetItem->treeWidget());
    auto *layout = new QHBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);  // Выравнивание по центру
    layout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    layout->addWidget(btnMapAction);
    treeWidgetItem->treeWidget()->setItemWidget(treeWidgetItem, 2, container);

    connect(btnMapAction, &QPushButton::clicked, [this]() {
        bool isOk = true;
        double lat = getPropertyDouble("lat", isOk);
        double lon = getPropertyDouble("lon", isOk);
        if (lat != 0 && lon != 0) {
            emit centerOnMap(lat, lon);
        } else {
            emit createOnMap(this->getId());
        }
    });
}


QTreeWidgetItem* ObjectScenarioModel::getTreeWidgetItem(QTreeWidgetItem *parent, bool addChilds)
{
    // После очистки дерева ранее созданные элементы становятся недействительными,
    // поэтому проверяем указатели на QTreeWidgetItem перед использованием.
    if (treeWidgetItem != nullptr && treeWidgetItem->treeWidget() == nullptr)
        treeWidgetItem = nullptr;
    if (propItem != nullptr && propItem->treeWidget() == nullptr)
        propItem = nullptr;
    if (actionsItem != nullptr && actionsItem->treeWidget() == nullptr)
        actionsItem = nullptr;

    if (treeWidgetItem == nullptr) {
        treeWidgetItem = new QTreeWidgetItem(parent);

        treeWidgetItem->setText(0, m_title);
        treeWidgetItem->setText(1, m_id);
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<ObjectScenarioModel *>(this)));
    }

    if(parent != nullptr) {
        QString classCode = getPropertyString("class_code");
        if (!classCode.isEmpty() && treeWidgetItem->treeWidget()->columnCount() > 2)
                addSearchButton();
    }

   // функция для добавления параметров
    auto addTreeWidgetItem = [this](const QString& name, const QString& value, QTreeWidgetItem* parent) -> QTreeWidgetItem* {
        auto * item = new QTreeWidgetItem(parent);
        item->setText(0, name);
        item->setText(1, value);
        return item;
    };

    if (addChilds){
        if(propItem == nullptr)
            propItem = addTreeWidgetItem("properties", "", treeWidgetItem);
        for(auto prop: m_properties) {
            prop->getTreeWidgetItem(propItem);
            disconnect(prop, &PropertyModel::valueChanged, nullptr, nullptr);
            connect(prop , &PropertyModel::valueChanged, [this](){
                emit propertiesChanged();
            }) ;

        }

        if(actionsItem == nullptr)
            actionsItem = addTreeWidgetItem("actions", "", treeWidgetItem);

        for (ActionModel *action : m_actions)
            if (action) action->getTreeWidgetItem(actionsItem);
    }
    return treeWidgetItem;
}

bool ObjectScenarioModel::setPropertyValue(QString key, double value){
    if (!m_properties.contains(key)) {  return false;}
    if (m_properties[key]->type() != "double" && m_properties[key]->type() != "float" ) {  return false;}
    m_properties[key]->setValue(value);

    if((key == "lat" || key == "lon") && btnMapAction) {
        btnMapAction->setIcon(QIcon(QPixmap(":/icons/SEARCH_ON_MAP")));
    }

    return true;
}

bool ObjectScenarioModel::setPropertyValue(QString key, QString value){
    if (!m_properties.contains(key)) {  return false;}
    if (m_properties[key]->type() != "string" && m_properties[key]->type() != "str" ) {  return false;}
    m_properties[key]->setValue(value);
    return true;
}

bool ObjectScenarioModel::setPropertyValue(QString key, QVariant value){
    if (!m_properties.contains(key)) {  return false;}
    m_properties[key]->setValue(value);
    return true;
}

bool ObjectScenarioModel::setPropertyValue(QString key, QJsonObject value){
    if (!m_properties.contains(key)) {  return false;}
    if (m_properties[key]->type() != "feature" && m_properties[key]->type() != "dict" && m_properties[key]->type() != "object" ) {  return false;}
    m_properties[key]->setValue(value);
    return true;
}


QStringList ObjectScenarioModel::getPropertiesByType(const QString &propType) const {
    QStringList lst;
    for (auto it : m_properties) {
        qDebug() << it->type() << " " << it->name();
        if (it->type() == propType.toLower()) lst.append(it->name());
    }
    return lst;
}

bool ObjectScenarioModel::containsProperty(const QString &key) const {
    return m_properties.contains(key);
}

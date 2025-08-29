#include "ActionModel.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QTreeWidgetItem>

ActionModel::ActionModel(QObject *parent) : QObject(parent)
{
}
// Деструктор
ActionModel::~ActionModel()
{
    qDeleteAll(m_outputMapping);
}

// Геттеры
QString ActionModel::name() const { return m_name; }
QString ActionModel::type() const { return m_type; }
QString ActionModel::algorithmId() const { return m_algorithmId; }
QList<AlgorithParamModel*> ActionModel::outputMapping() const { return m_outputMapping; }

// Сеттеры
void ActionModel::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void ActionModel::setType(const QString &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

void ActionModel::setAlgorithmId(const QString &algorithmId)
{
    if (m_algorithmId != algorithmId) {
        m_algorithmId = algorithmId;
        emit algorithmIdChanged();
    }
}

void ActionModel::setOutputMapping(const QList<AlgorithParamModel*> &outputMapping)
{
    qDeleteAll(m_outputMapping);
    m_outputMapping = outputMapping;
    emit outputMappingChanged();
}

// Управление outputMapping
void ActionModel::addOutputMapping(const QString &name, const QString &value, const QString &title)
{
    m_outputMapping.append(new AlgorithParamModel(name,value, title, this));
    emit outputMappingChanged();
}

void ActionModel::removeOutputMapping(const QString &key)
{
    for (auto p : m_outputMapping)
        if (p->name() == key) {
            m_outputMapping.removeAll(p);
            break;
        }
        emit outputMappingChanged();
}

// JSON сериализация
QJsonObject ActionModel::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["type"] = m_type;
    json["algorithm_id"] = m_algorithmId;

    // Сериализация outputMapping
    QJsonArray mappingObj;
    for (auto it: m_outputMapping)
        mappingObj.append(it->toJson());
    json["output_mapping"] = mappingObj;
    return json;
}

ActionModel* ActionModel::fromJson(const QJsonObject &json, QObject *parent)
{
    auto *model = new ActionModel(parent);
    model->setName(json["name"].toString());
    model->setType(json["type"].toString());
    model->setAlgorithmId(json["algorithm_id"].toString());

    // Десериализация outputMapping
    if (json.contains("output_mapping") && json["output_mapping"].isArray()) {
        QJsonArray mappingObj = json["output_mapping"].toArray();
        QList<AlgorithParamModel*> outputMapping;

        for (auto it: mappingObj)
            outputMapping.append(  AlgorithParamModel::fromJson(it.toObject(), model));

        model->setOutputMapping(outputMapping);
    }

    return model;
}

// Метод для отображения в дереве
QTreeWidgetItem* ActionModel::getTreeWidgetItem(QTreeWidgetItem *parent)
{
    if(treeWidgetItem == nullptr) {
        treeWidgetItem = new QTreeWidgetItem(parent);
        treeWidgetItem->setText(0, m_name);
        treeWidgetItem->setText(1, m_algorithmId);
        treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<ActionModel *>(this)));
    } else {
        for(QTreeWidgetItem* child: treeWidgetItem->takeChildren())
            treeWidgetItem->removeChild(child);
    }

    // Вспомогательная лямбда для создания свойств
    auto addProperty = [this](const QString& name, const QString& value)-> QTreeWidgetItem* {
        auto propItem = new QTreeWidgetItem(treeWidgetItem);
        propItem->setText(0, name);
        propItem->setText(1, value);
        return propItem;
    };

    addProperty("Тип", m_type);

    // Добавляем outputMapping
    if (m_outputMapping.isEmpty()) return treeWidgetItem;

    QTreeWidgetItem* item= nullptr;
    auto parentItem = addProperty("output_mapping", "");
    for (auto it : m_outputMapping)
        it->getTreeWidgetItem(parentItem);

    return item;
}


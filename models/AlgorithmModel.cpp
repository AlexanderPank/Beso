#include <QJsonArray>
#include "AlgorithmModel.h"
#include "AlgorithParamModel.h"

AlgorithmModel::AlgorithmModel(QObject *parent) : QObject(parent)
{
}

AlgorithmModel::~AlgorithmModel()
{
}

// Getters implementation
QString AlgorithmModel::getId() const { return m_id; }
QString AlgorithmModel::getTitle() const { return m_title; }
QString AlgorithmModel::getDescription() const { return m_description; }
QString AlgorithmModel::getType() const { return m_type; }
QString AlgorithmModel::getFunction() const { return m_function; }
QList<AlgorithParamModel*> AlgorithmModel::getInputParameters() const { return m_inputParameters; }
QList<AlgorithParamModel*> AlgorithmModel::getOutputParameters() const { return m_outputParameters; }
QString AlgorithmModel::getOutputSplitter() const { return m_outputSplitter; }

// Setters implementation
void AlgorithmModel::setId(const QString &id) { m_id = id; }
void AlgorithmModel::setTitle(const QString &title) { m_title = title; }
void AlgorithmModel::setDescription(const QString &description) { m_description = description; }
void AlgorithmModel::setType(const QString &type) { m_type = type; }
void AlgorithmModel::setFunction(const QString &function) { m_function = function; }
void AlgorithmModel::setInputParameters(const QList<AlgorithParamModel*> &inputParameters) { m_inputParameters = inputParameters; }
void AlgorithmModel::setOutputParameters(const QList<AlgorithParamModel*> &outputParameters) { m_outputParameters = outputParameters; }
void AlgorithmModel::setOutputSplitter(const QString &outputSplitter) { m_outputSplitter = outputSplitter; }

// JSON serialization
QJsonObject AlgorithmModel::toJson() const
{
    QJsonObject json;
    if (m_id==nullptr)
        return json;
    json["id"] = m_id;
    json["title"] = m_title;
    json["description"] = m_description;
    json["type"] = m_type;
    json["function"] = m_function;
    json["output_splitter"] = m_outputSplitter;
    json["subType"]=m_subType;

    // Сериализация входных параметров как объекта
    QJsonArray inputParams;
    for (auto param : m_inputParameters)
        inputParams.append(param->toJson());

    json["input_parameters"] = inputParams;

    // Сериализация выходных параметров как объекта
    QJsonArray outputParams;
    for (auto param : m_outputParameters)
        outputParams.append(param->toJson());

    json["output_parameters"] = outputParams;

    return json;
}


AlgorithmModel* AlgorithmModel::fromJson(const QJsonObject &json, QObject* parent)
{
    auto *model = new AlgorithmModel(parent);

    model->setId(json["id"].toString());
    model->setTitle(json["title"].toString());
    model->setDescription(json["description"].toString());
    model->setType(json["type"].toString());
    model->setFunction(json["function"].toString());
    model->setMSubType(json["subType"].toString());

    if (json.contains("output_splitter")) {
        model->setOutputSplitter(json["output_splitter"].toString());
    }

    // Парсинг входных параметров
    if (json.contains("input_parameters") && json["input_parameters"].isArray()) {
        QJsonArray inputObj = json["input_parameters"].toArray();
        QList<AlgorithParamModel*> inputParams;

        for (auto p: inputObj)
            inputParams.append(AlgorithParamModel::fromJson(p.toObject(), model));

        model->setInputParameters(inputParams);
    }

    // Парсинг выходных параметров
    if (json.contains("output_parameters") && json["output_parameters"].isArray()) {
        QJsonArray outputObj = json["output_parameters"].toArray();
        QList<AlgorithParamModel*> outputParams;

        for (auto p: outputObj)
            outputParams.append(AlgorithParamModel::fromJson(p.toObject(), model));

        model->setOutputParameters(outputParams);
    }

    return model;
}

// Управление параметрами
void AlgorithmModel::addInputParameter(const QString &name, const QString &value, const QString &title)
{
    m_inputParameters.append(new AlgorithParamModel(name, value, title, this) );
}

void AlgorithmModel::addOutputParameter(const QString &name, const QString &value, const QString &title)
{
    m_outputParameters.append(new AlgorithParamModel(name, value, title, this) );
}

QTreeWidgetItem *AlgorithmModel::getTreeWidgetItem(QTreeWidgetItem* parent, bool addChild)
{
    if (!parent) return nullptr;

    // Основной элемент алгоритма
    treeWidgetItem = new QTreeWidgetItem(parent);
    treeWidgetItem->setText(0, m_title.isEmpty() ? "unnamed" : m_title);
    treeWidgetItem->setText(1, m_id.isEmpty() ? "unnamed" : m_id);
    treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(this));
    if (!addChild)
        return treeWidgetItem;
    // Вспомогательная лямбда для создания свойств
    auto addProperty = [this](const QString& name, const QString& value, QTreeWidgetItem *propItem=nullptr) -> QTreeWidgetItem* {
        propItem = new QTreeWidgetItem(treeWidgetItem);
        propItem->setText(0, name);
        propItem->setText(1, value);
        return propItem;
    };

    // Добавляем основные свойства
    addProperty("description", m_description);
    addProperty("type", m_type);
    addProperty("function", m_function);

    // функция для добавления параметров
    auto addParameters = [this, addProperty](const QString& name, const QList<AlgorithParamModel*>& params, QTreeWidgetItem* parent) {
        for (auto p:params)
            p->getTreeWidgetItem(parent);

    };

    auto inp =  addProperty("input_parameters", "");
    auto out =  addProperty("output_parameters", "");
    // Добавляем параметры
    addParameters("input_parameters", m_inputParameters, inp);
    addParameters("output_parameters", m_outputParameters, out);
    return treeWidgetItem;
}

const QString &AlgorithmModel::getMSubType() const {
    return m_subType;
}

void AlgorithmModel::setMSubType(const QString &mSubType) {
    m_subType = mSubType;
}

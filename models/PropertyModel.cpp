#include "PropertyModel.h"
#include "Feature.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>  // Для работы с QJsonArray
#include <cmath>
#include <QHBoxLayout>

// Константы типов
const QString PropertyModel::ValueType::BOOL = "bool";
const QString PropertyModel::ValueType::FLOAT = "float";
const QString PropertyModel::ValueType::DOUBLE = "double";
const QString PropertyModel::ValueType::INT = "int";
const QString PropertyModel::ValueType::STRING = "string";
const QString PropertyModel::ValueType::FEATURE = "feature";

PropertyModel::PropertyModel(QObject *parent) : QObject(parent)
{
}

PropertyModel::PropertyModel(const QString &name, const QString &type, const QVariant &value, QObject *parent)
        : QObject(parent), m_name(name), m_type(type.toLower()), m_value(value), m_title("")
{}
PropertyModel::PropertyModel(const QString &name, const QString &type, const QVariant &value, const QString &title, QObject *parent)
        : QObject(parent), m_name(name), m_type(type.toLower()), m_value(value), m_title(title)
{
    qDebug() << m_name << " " << m_type << " " << m_value << " " << m_title;
}

PropertyModel::PropertyModel(const QString &name, const QString &type, const QJsonValue &jsonValue, QObject *parent)
        :PropertyModel(name, type, jsonValue, "", parent)
{}

PropertyModel::PropertyModel(const QString &name, const QString &type, const QJsonValue &jsonValue, const QString &title, QObject *parent)
        : QObject(parent), m_name(name), m_type(type.toLower()), m_title(title)
{
    // Определяем тип и значение из QJsonValue
    if (m_type == "bool") m_value = jsonValue.toBool(); else
    if (m_type == "float" || m_type == "double") m_value = jsonValue.toDouble(); else
    if (m_type == "int") m_value = int(jsonValue.toInt()); else
    if (m_type == "string" || m_type == "str") m_value = jsonValue.toString(); else
    if (m_type == "array" || m_type == "list") m_value = jsonValue.toArray().toVariantList(); else
    if (m_type == "object") m_value = jsonValue.toObject().toVariantMap(); else
    if (m_type == "feature") {
        QJsonObject obj = jsonValue.toObject();
        auto feature = new Feature(obj, this);
        m_value = QVariant::fromValue<Feature*>(feature);
        return;
    } else
        m_value = "";
}

void PropertyModel::setValueByType(const QVariant &value) {
    if (m_type == "bool") m_value = value.toBool(); else
    if (m_type == "float" || m_type == "double") m_value = value.toDouble(); else
    if (m_type == "int") m_value = int(value.toInt()); else
    if (m_type == "string" || m_type == "str") m_value = value.toString(); else
    if (m_type == "array" || m_type == "list") m_value = value.toList(); else
    if (m_type == "object") m_value = value.toMap(); else
     m_value = "";
}

QString PropertyModel::getJsonValueType(const QJsonValue &jsonValue)
{
    // Определяем тип и значение из QJsonValue
    switch (jsonValue.type()) {
        case QJsonValue::Bool:
            return "bool";
        case QJsonValue::Double:{
            double d = jsonValue.toDouble();
            // Получаем строковое представление числа
            QString numStr = jsonValue.toString();
            // Проверяем, содержит ли строка точку или экспоненциальную запись
            if (numStr.contains('.') || numStr.contains('e', Qt::CaseInsensitive)) {
                return "float";
            }
            else {
                // Проверяем, что число влезает в диапазон int
                if (d >= std::numeric_limits<int>::min() &&
                    d <= std::numeric_limits<int>::max() &&
                    d == floor(d)) {
                    return "int";
                }
                else {
                    return "float";
                }
            }
            break;
        }
        case QJsonValue::String:
            return  "string";
        case QJsonValue::Array:
            return  "array";
        case QJsonValue::Object:
            return "object";
        case QJsonValue::Null:
        case QJsonValue::Undefined:
        default:
            return  "null";
    }
}

// Геттеры
QString PropertyModel::name() const { return m_name; }
QVariant PropertyModel::value() const {
    return m_value;
}

QString PropertyModel::stringValue() const
{
    return variantToString(m_value);
}
QString PropertyModel::type() const { return m_type; }

// Сеттеры
void PropertyModel::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        if (currentTreeItem == nullptr) return;
        currentTreeItem->setText(0, m_name);
        emit nameChanged();
    }
}

void PropertyModel::setValue(const QVariant &value, bool updateTree)
{

    if (m_type == "feature") {
        QJsonObject obj = value.toJsonObject();
//        auto feature = m_value.value<Feature*>();
//        feature->fromJson(obj);
        auto feature = new Feature(obj, this);
        m_value = QVariant::fromValue<Feature*>(feature);
        QObject *obj2 = m_value.value<QObject*>();
        qDebug() << "Feature: m_value" << obj2->property("type").toString();
    } else
        setValueByType(value);

    if (currentTreeItem == nullptr || !updateTree) return;
    try {
        currentTreeItem->setText(1, variantToString(m_value));
    } catch (std::exception &e) {
        qDebug() << "Ошибка при установке значения в дерево: " << e.what();
    }
}

void PropertyModel::setType(const QString &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

// JSON-сериализация

QJsonObject PropertyModel::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    if (m_type != "feature")

        if (m_type == "bool") json["value"] = m_value.toBool(); else
        if (m_type == "float" || m_type == "double") json["value"] = m_value.toDouble(); else
        if (m_type == "int")  json["value"] = int(m_value.toInt()); else
        if (m_type == "string" || m_type == "str") json["value"] = m_value.toString(); else
        if (m_type == "array" || m_type == "list") json["value"] = m_value.toJsonArray(); else
        if (m_type == "object") json["value"] = m_value.toJsonObject(); else
        json["value"] = QJsonValue::fromVariant(m_value);
    else {
        Feature* v = qvariant_cast<Feature*>(m_value);
        json["value"] = v->toJson();
    }
    json["type"] = m_type;
    json["title"] = m_title;
    return json;
}

PropertyModel* PropertyModel::fromJson(const QJsonObject &json, QObject *parent)
{
    return new PropertyModel(json["name"].toString(), json["type"].toString(),
                             (const QJsonValue)json["value"],
                             json.contains("title") ? json["title"].toString() : "",
                             parent
                             );
}

QString PropertyModel::variantToString(const QVariant& value) const {
    if (value.canConvert<Feature*>()) {
        Feature* f = value.value<Feature*>();
        return f ? f->toString() : "";
    }
    return value.toString();  // по умолчанию
}

QTreeWidgetItem* PropertyModel::getTreeWidgetItem(QTreeWidgetItem* parent)
{
    if (currentTreeItem == nullptr) //return  currentTreeItem;
        currentTreeItem = new QTreeWidgetItem(parent);

    currentTreeItem->setText(0, m_title.trimmed().isEmpty() ? m_name : m_title);
    auto textValue =  variantToString(m_value);
    currentTreeItem->setText(1, textValue);

    currentTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<PropertyModel*>(this)));

    // Разрешаем редактирование
    currentTreeItem->setFlags(currentTreeItem->flags() | Qt::ItemIsEditable);

    // Tooltip с дополнительной информацией
    currentTreeItem->setToolTip(0, m_title);
    currentTreeItem->setToolTip(1, QString("Тип: %1\nЗначение: %2").arg(m_type).arg(m_value.toString()));

    connect(currentTreeItem->treeWidget(), &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int column) {
        m_treeItemEditMode = column == 1;
    });

    connect(currentTreeItem->treeWidget(), &QTreeWidget::itemChanged, this, [=](QTreeWidgetItem *item,int column){
        if (!m_treeItemEditMode || item != currentTreeItem || column != 1)  return;
        m_treeItemEditMode = false;
        setValue(item->text(1), false );
        emit valueChanged();
        currentTreeItem->setToolTip(1, QString("Тип: %1\nЗначение: %2").arg(m_type).arg(m_value.toString()));

    });

    // Подключаемся к моменту завершения редактирования




    // добавляем кнопку поиска на карте или нанесения объекта
    addSearchButton();

    return currentTreeItem;
}


void PropertyModel::updateFromTreeItem(QTreeWidgetItem *item)
{
    if (item) {
        QString newValue = item->text(1);
        if (m_value.toString() != newValue) {
            setValue(newValue);
        }
    }
}

QString PropertyModel::title() const
{
    return m_title;
}

void PropertyModel::setTitle(const QString &newTitle)
{
    m_title = newTitle;
}


// Метод для отображения в дереве
void PropertyModel::addSearchButton() {

    if (m_type != "feature") return;


    auto * feature = qvariant_cast<Feature*>(m_value);
    bool isMulti = feature->getProperties().contains("multy_features") && feature->getProperties().value("multy_features").toBool();
    auto coordinates = feature->getCoordinates();

    btnMapAction = new QPushButton();
    btnMapAction->setFixedSize(20, 20);
    QPixmap pixmap((coordinates.length() > 0 )?":/icons/SEARCH_ON_MAP": ":/icons/ADD_TO_MAP_ICON");
    QIcon icon(pixmap);
    btnMapAction->setIcon(icon);
    btnMapAction->setIconSize(QSize(15, 15));

    auto *container = new QWidget(currentTreeItem->treeWidget());
    auto *layout = new QHBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);  // Выравнивание по центру
    layout->setContentsMargins(3, 0, 3, 0); // Убираем отступы
    layout->addWidget(btnMapAction);
    currentTreeItem->treeWidget()->setItemWidget(currentTreeItem, 2, container);

    connect(btnMapAction, &QPushButton::clicked, [this, feature, coordinates, isMulti]() {
        if (coordinates.length() > 0) {
            emit centerFeatureOnMap(coordinates[0].x(), coordinates[0].y());
        } else {
            emit createFeatureOnMap( feature->getClassCode(), feature->getProperties()["id"].toString(), isMulti );
        }
    });

    if (!isMulti || coordinates.length() == 0) return;

    btnMapAction = new QPushButton();
    btnMapAction->setFixedSize(20, 20);
    QPixmap pixmap2( ":/icons/ADD_TO_MAP_ICON");
    QIcon icon2(pixmap2);
    btnMapAction->setIcon(icon2);
    btnMapAction->setIconSize(QSize(15, 15));
    layout->addWidget(btnMapAction);
    connect(btnMapAction, &QPushButton::clicked, [this, feature, isMulti]() {
        emit createFeatureOnMap( feature->getClassCode(), feature->getProperties()["id"].toString(), isMulti);
    });
    


}

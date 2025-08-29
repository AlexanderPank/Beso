#include "FeatureModel.h"
#include "../core/QLogStream.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QHBoxLayout>

FeatureModel::~FeatureModel(){
    qDeleteAll(m_properties);
}

FeatureModel* FeatureModel::fromJson(const QJsonObject &json, QObject* parent){
    auto *model = new FeatureModel(parent);

    model->setId(json["id"].toString());

   model->setOwn(json["is_own"].toInt());
   model->setTitle(json["title"].toString());
   model->setType(json["type"].toString());
   model->setParentID(json["parent_object_id"].toString());
   model->setClassCode(json["class_code"].toString());
   model->setLineWidth(json["line_width"].toDouble());
   model->setScale(json["scale"].toDouble());
   model->setSpline(json["spline"].toBool());
   model->setColor(json["color"].toString());

    int geometry_type = json["geometry_type"].toInt();
    if (geometry_type >= UNKNOWN && geometry_type <= SQUARE)
        model->setGeometryType(static_cast<GeometryType>(geometry_type));
    else
    {
       model->setGeometryType(UNKNOWN); // или обработка ошибки
       qLog() << "Invalid geometry type value:" << geometry_type;
    }

    model->setIcon(json["icon"].toString());
    model->setScheme(json["scheme"].toString());
    model->setShortName(json["short_name"].toString());
    model->setDescription(json["description"].toString());
    if (json.contains("radius")) model->setRadius(json["radius"].toDouble());
    if (json.contains("text")) model->setText(json["text"].toString());
    if (json.contains("font_size")) model->setFontSize(json["font_size"].toDouble());
    if (json.contains("position")) model->setPosition(json["position"].toInt());

    QList<QPointF> points;
    if (json["coordinates"].isArray()) {
        for (const auto& p : json["coordinates"].toArray()) {
            auto point = p.toArray();
            if (point.size() >= 2) {  // Проверка, что массив содержит минимум 2 элемента
                points.append(QPointF(point[0].toDouble(), point[1].toDouble()));
            }
        }
    }
    model->setCoordinates(points);

    // Десериализация свойств
    if (json.contains("properties") && json["properties"].isArray()) {
        QJsonArray propertiesArr = json["properties"].toArray();
        QMap<QString, PropertyModel*> properties;

        for (auto p: propertiesArr) {
            auto  prop = PropertyModel::fromJson(p.toObject(), model);
            properties[prop->name()] = prop;

        }

        model->setProperties(properties);
    }

    return model;
}



void FeatureModel::setPosition(const int &position) {
    if (position==1) m_position = PositionType::ALWAYS_TOP;
    else if (position==2) m_position = PositionType::ALWAYS_BOTTOM;
    else m_position = PositionType::DEFAULT;
}
QJsonObject FeatureModel::toJson() const{
    QJsonObject json;
    json["id"] = m_id;
    json["title"] = m_title;
    json["type"] = m_type;
    json["parent_object_id"] = m_parent_id;
    json["class_code"] = m_class_code;
    json["scale"] = m_scale;
    json["spline"] = m_spline;
    json["color"] = m_color;
    json["line_width"] = m_line_width;
    json["geometry_type"] = static_cast<int>(m_geometry_type);
    json["icon"] = m_icon;
    json["scheme"] = m_scheme;
    json["short_name"] = m_short_name;
    json["description"] = m_description;
    json["is_own"] = m_own;
    json["position"] = m_position;

    if (m_geometry_type == CIRCLE) json["radius"] = m_radius;
    if (m_geometry_type == TITLE) {
        json["text"] = m_text;
        json["font_size"] = m_font_size;
    }


    QJsonArray coordinatesArray;
    for (const auto& point : m_coordinates) {
        QJsonArray pointArray;
        if (point.x() == 0 && point.y() == 0) continue;
        pointArray.append(point.x());
        pointArray.append(point.y());
        pointArray.append(0);
        coordinatesArray.append(pointArray);
    }

    json["coordinates"] = coordinatesArray;

    QJsonArray propertiesArr;
    for (auto it : m_properties)
        propertiesArr.append(it->toJson());

    json["properties"] = propertiesArr;


    return json;
}

// Метод для отображения в дереве
void FeatureModel::addSearchButton() {

    btnMapAction = new QPushButton();
    btnMapAction->setFixedSize(20, 20);
    QPixmap pixmap((m_string_coordinates.size() > 0)?":/icons/SEARCH_ON_MAP":":/icons/ADD_TO_MAP_ICON");
    QIcon icon(pixmap);
    btnMapAction->setIcon(icon);
    btnMapAction->setIconSize(QSize(15, 15));

    auto *container = new QWidget(m_tree_widget_item->treeWidget());
    auto *layout = new QHBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);  // Выравнивание по центру
    layout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    layout->addWidget(btnMapAction);
    m_tree_widget_item->treeWidget()->setItemWidget(m_tree_widget_item, 2, container);

    connect(btnMapAction, &QPushButton::clicked, [this]() {

        if (m_string_coordinates.size() > 0) {
            emit centerOnMap(m_coordinates[0].x(),  m_coordinates[0].y());
        } else {
            emit createOnMap(this->getId());
        }
    });
}

// Метод для отображения в дереве
QTreeWidgetItem* FeatureModel::getTreeWidgetItem(QTreeWidgetItem *parent)
{
    if (m_tree_widget_item != nullptr) return m_tree_widget_item; // Если уже существует, возвращаем nullptr

    m_tree_widget_item = new QTreeWidgetItem(parent);

    m_tree_widget_item->setText(0, m_title);
    m_tree_widget_item->setText(1, m_id);
    m_tree_widget_item->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<FeatureModel*>(this)));

    if(parent != nullptr && !m_class_code.isEmpty() && m_tree_widget_item->treeWidget()->columnCount() > 2)
        addSearchButton();

    connect(m_tree_widget_item->treeWidget(), &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int column) {
        m_treeItemEditMode = column == 1;
    });
    // Добавляем свойства как дочерние элементы
    auto addProperty = [this](const QString &name, const QString &value,
                              std::function<void(QString)> callback = nullptr) -> QTreeWidgetItem* {

        auto *propItem = new QTreeWidgetItem(m_tree_widget_item);
        propItem->setText(0, name);
        propItem->setText(1, value);
        if (callback) {
            propItem->setFlags(propItem->flags() | Qt::ItemIsEditable);
            connect(m_tree_widget_item->treeWidget(), &QTreeWidget::itemChanged,
                    [propItem, callback, this](QTreeWidgetItem *item, int col) {
                        if (item == propItem && col == 1 && m_treeItemEditMode) {
                            callback(item->text(1));
                        }
                        m_treeItemEditMode = false;
                    });
        }
        return propItem;

    };

    addProperty("Тип", m_type);
    addProperty("parent_id", m_parent_id, [this](const QString value) { setParentID(value);});
    addProperty("Класс код", m_class_code, [this](const QString value) {  setClassCode(value); emit redrawFeature(this); });
    addProperty("Толщина линии", QString::number(m_line_width), [this](const QString value) { bool ok; double v = value.toDouble(&ok); if(ok) setLineWidth(v); emit redrawFeature(this); });
    addProperty("Сплайн", QString("%1").arg(m_spline ? "Да" : "Нет"), [this](const QString value) {  setSpline(value.toLower() == "да" || value.toLower() == "yes" || value.toLower() == "true"); emit redrawFeature(this); });
    addProperty("Цвет", m_color == "base" ? "" : m_color, [this](const QString value) { setColor(value); emit redrawFeature(this); });
    addProperty("Геометрия",  getGeometryTypeString());
    m_coordinates_item = addProperty("Координаты", m_string_coordinates);
    addProperty("Наименование",m_short_name, [this](const QString value) { setShortName(value);});
    addProperty("Описание",m_description, [this](const QString value) { setDescription(value);});
    if (m_geometry_type == GeometryType::TITLE) {
        addProperty("Текст", m_text, [this](const QString value) { setText(value); emit redrawFeature(this); });
        addProperty("Размер шрифта", QString::number(m_font_size), [this](const QString value) { bool ok; double v = value.toDouble(&ok); if(ok) setFontSize(v); emit redrawFeature(this); });
    }
    if (m_radius != 0)
        addProperty("Радиус", QString::number(m_radius), [this](const QString value) { bool ok; double v = value.toDouble(&ok); if(ok) setRadius(v); emit redrawFeature(this); });

    if(m_properties.size() > 0 && m_property_item == nullptr)
        m_property_item = addProperty("Параметры знака", " ");

    if(m_property_item != nullptr){
        for(auto prop: m_properties) {
            prop->getTreeWidgetItem(m_property_item);
            // Отключаем все предыдущие соединения valueChanged → redrawFeature
            disconnect(prop, &PropertyModel::valueChanged, this, nullptr);

            connect(prop, &PropertyModel::valueChanged,  [this](){
                emit redrawFeature(this);
            });
        }
    }


    return m_tree_widget_item;
}

QString FeatureModel::getGeometryTypeString(){
    if (m_geometry_type == GeometryType::LINE) return "LINE";
    if (m_geometry_type == GeometryType::POLYGON) return "POLYGON";
    if (m_geometry_type == GeometryType::POINT) return "POINT";
    if (m_geometry_type == GeometryType::CIRCLE) return "CIRCLE";
    if (m_geometry_type == GeometryType::RECTANGLE) return "RECTANGLE";
    if (m_geometry_type == GeometryType::SQUARE) return "SQUARE";
    if (m_geometry_type == GeometryType::TITLE) return "TITLE";
    return "UNKNOWN";
}

void FeatureModel::setCoordinates(QList<QPointF> coordinates) {
    m_coordinates = coordinates;
    emit coordinatesChanged(m_coordinates);

    m_string_coordinates = "";
    for (const auto p: coordinates)
        m_string_coordinates += QString("[%1, %2]").arg(p.x()).arg(p.y());
    m_string_coordinates = "[" + m_string_coordinates + "]";
    emit stringCoordinatesChanged(m_string_coordinates);

    if (m_coordinates_item != nullptr) {
        m_coordinates_item->setText(1, m_string_coordinates);
     }
}


QVariant  FeatureModel::getProperty(QString name){
    if(m_properties.contains(name))
        return m_properties[name]->value();
    else
        return QVariant();
}

int FeatureModel::getPropertyInt(QString name, int def ){
    if(m_properties.contains(name)) {
        auto val = m_properties[name]->value();
        return val.toInt();
    }
    else
        return def;
}

float FeatureModel::getPropertyFloat(QString name, float def ){
    if(m_properties.contains(name))
        return m_properties[name]->value().toFloat();
    else
        return def;
}

void FeatureModel::setProperty(QString name, QVariant val){
    if(m_properties.contains(name))
        m_properties[name]->setValue(val);
    else
        qWarning() << "Property " << name << " not found";
}

void FeatureModel::setLineWidth(const float &line_width) {
    if (line_width != m_line_width) {
        qWarning() << "Line width changed from " << m_line_width << " to " << line_width;
    }
    m_line_width = line_width;
}
void FeatureModel::setScale(const float &scale) {
    if (scale != m_scale) {
        qWarning() << "Line width changed from " << m_scale << " to " << scale;
    }
    m_scale = scale;
}
void FeatureModel::setSpline(const bool &spline) {
    m_spline = spline;
}
void FeatureModel::setColor(const QString &color) {
    if (m_color != color && m_color != "") {
        qWarning() << "Line width changed from " << m_color << " to " << color;
    }
    m_color = color;
}

float FeatureModel::getRadius() const {
    if (m_properties.contains("radius"))  return m_properties["radius"]->value().toFloat();
    return m_radius;
}
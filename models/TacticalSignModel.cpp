#include "TacticalSignModel.h"

TacticalSignModel::TacticalSignModel(QObject *parent) : QObject(parent) {

}

TacticalSignModel::~TacticalSignModel() {

}

TacticalSignModel *TacticalSignModel::fromJson(const QJsonObject &json, QObject *parent) {
    auto* model = new TacticalSignModel(parent);

    model->setId(json["id"].toString());
    model->setTitle(json["title"].toString());
    model->setClassCod(json["class_code"].toString());
    model->setGroup(json["group"].toString());
    model->setIconPath(json["icon"].toString());
    model->setDescription(json["description"].toString());
    model->setShortName(json["short_name"].toString());
    model->setSchemePath(json["scheme"].toString());
    model->setParentObjectId(json["parent_object_id"].toString());

    model->setType(json["type"].toString());
    model->setIs_own(json["is_own"].toInt());
    model->setLine_width(json["line_width"].toDouble());
    model->setScale(json["scale"].toInt());
    model->setSpline(json["spline"].toBool());
    model->setColor(json["color"].toString());
    model->setCoordinates(json["coordinates"].toArray());
    model->setGeometry_type(json["geometry_type"].toInt());

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

    model->setData(json);
    return model;
}

QJsonObject TacticalSignModel::toJson() const
{
    QJsonObject json;

    json["id"] = m_id;
    json["title"] = m_title;
    json["class_code"] = m_classCod;
    json["group"] = m_group;
    json["icon"] = m_iconPath;
    json["description"] = m_description;
    json["short_name"] = m_shortName;
    json["scheme"] = m_schemePath;
    json["parent_object_id"] = m_parentObjectId;

    json["type"] = m_type;
    json["is_own"] = m_is_own;
    json["line_width"] = m_line_width;
    json["scale"] = m_scale;
    json["spline"] = m_spline;
    json["color"] = m_color;
    json["coordinates"] = m_coordinates;
    json["geometry_type"] = m_geometry_type;

    // Сериализация свойств
    QJsonArray propertiesArr;
    for (auto it : m_properties)
        propertiesArr.append(it->toJson());

    json["properties"] = propertiesArr;

    return json;
}

QString TacticalSignModel::getId() const {
    return m_id;
}

QString TacticalSignModel::getTitle() const {
    return m_title;
}

QString TacticalSignModel::getClassCod() const {
    return m_classCod;
}

QString TacticalSignModel::getGroup() const {
    return m_group;
}

QString TacticalSignModel::getDescription() const {
    return m_description;
}

QPixmap TacticalSignModel::getIcon() const {
    QPixmap pixmap(m_iconPath);
    return pixmap.scaled(QSize(100, 80), Qt::IgnoreAspectRatio);
}

QString TacticalSignModel::getIconPath() const {
    return m_iconPath;
}

QPixmap TacticalSignModel::getSchemeIcon() const {
    QPixmap pixmap(m_schemePath);
    return pixmap.scaled(QSize(40, 40), Qt::IgnoreAspectRatio);
}

QString TacticalSignModel::getShortName() const {
    return m_shortName;
}

QString TacticalSignModel::getSchemePath() const {
    return m_schemePath;
}

void TacticalSignModel::setId(const QString &id) {
    m_id = id;
}

void TacticalSignModel::setIconPath(const QString &iconPath) {
    m_iconPath = iconPath;
}

void TacticalSignModel::setTitle(const QString &title) {
    m_title = title;
}

void TacticalSignModel::setClassCod(const QString &classCod) {
    m_classCod = classCod;
}

void TacticalSignModel::setGroup(const QString &group) {
    m_group = group;
}

void TacticalSignModel::setDescription(const QString &description) {
    m_description = description;
}

void TacticalSignModel::setShortName(const QString &shortName) {
    m_shortName = shortName;
}

void TacticalSignModel::setSchemePath(const QString &schemePath) {
    m_schemePath = schemePath;
}

void TacticalSignModel::setParentObjectId(const QString parentObjectId) {
    m_parentObjectId = parentObjectId;
}

QString TacticalSignModel::getParentObjectId() const {
    return m_parentObjectId;
}

QListWidgetItem *TacticalSignModel::getListWidgetItem(QListWidget* parent) {
    auto *listWidgetItem = new QListWidgetItem(getIcon(),m_shortName, parent);
    listWidgetItem->setSizeHint(QSize(132,100));
    listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(const_cast<TacticalSignModel*>(this)));
    listWidgetItem->setToolTip("<html><p><h3>"+m_title+"</h3></p>"+"<p><img src='"+m_schemePath+"'></img></p><p>"+m_description+"</p></html>");
    return listWidgetItem;
}

const QJsonArray TacticalSignModel::coordinates() const
{
    return m_coordinates;
}

void TacticalSignModel::setCoordinates(const QJsonArray newCoordinates)
{
    m_coordinates = newCoordinates;
}

const QString &TacticalSignModel::type() const
{
    return m_type;
}

void TacticalSignModel::setType(const QString &newType)
{
    m_type = newType;
}

int TacticalSignModel::is_own() const
{
    return m_is_own;
}

void TacticalSignModel::setIs_own(int newIs_own)
{
    m_is_own = newIs_own;
}

double TacticalSignModel::line_width() const
{
    return m_line_width;
}

void TacticalSignModel::setLine_width(double newLine_width)
{
    m_line_width = newLine_width;
}

int TacticalSignModel::scale() const
{
    return m_scale;
}

void TacticalSignModel::setScale(int newScale)
{
    m_scale = newScale;
}

bool TacticalSignModel::spline() const
{
    return m_spline;
}

void TacticalSignModel::setSpline(bool newSpline)
{
    m_spline = newSpline;
}

const QString &TacticalSignModel::color() const
{
    return m_color;
}

void TacticalSignModel::setColor(const QString &newColor)
{
    m_color = newColor;
}

int TacticalSignModel::geometry_type() const
{
    return m_geometry_type;
}

void TacticalSignModel::setGeometry_type(int newGeometry_type)
{
    m_geometry_type = newGeometry_type;
}

const QMap<QString, PropertyModel *> &TacticalSignModel::properties() const
{
    return m_properties;
}

void TacticalSignModel::setProperties(const QMap<QString, PropertyModel *> &newProperties)
{
    m_properties = newProperties;
}

void TacticalSignModel::removePropertys()
{
    m_properties.clear();
}

void TacticalSignModel::addProperty(QString p, PropertyModel *pm)
{
    m_properties.insert(p,pm);
}

QJsonObject TacticalSignModel::getData() const {
    return m_data;
}

void TacticalSignModel::setData(const QJsonObject data) {
    m_data = data;
}











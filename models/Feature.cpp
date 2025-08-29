#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include "Feature.h"
#include <QMetaType>
#include <QPointF>


QJsonObject Feature::toJson() const {
    QJsonObject json;
    json["type"] = m_type;
    json["properties"] = QJsonObject::fromVariantMap(m_properties);

    QJsonObject geometry;
    geometry["type"] = geometryTypeToString(m_geometryType);
    geometry["coordinates"] = QJsonArray::fromVariantList(m_coordinates);
    json["geometry"] = geometry;

    return json;
}

bool Feature::fromJson(const QJsonObject &json) {
    if (json["type"].toString() != "Feature") return false;

    // Парсинг свойств
    QJsonObject obj = json["properties"].toObject();
    auto fuck = obj.toVariantMap();
    this->m_properties = QVariantMap();
    m_properties = fuck;

    // Парсинг геометрии
    QJsonObject geometry = json["geometry"].toObject();
    this->m_geometryType = stringToGeometryType(geometry["type"].toString());
    this->m_coordinates = geometry["coordinates"].toArray().toVariantList();

    return true;
}

// Методы для работы с геометрией
void Feature::setPolygon(const QVariantList &coordinates) {
    m_geometryType = Polygon;
    m_coordinates = coordinates;
    emit geometryChanged();
}

void Feature::setLineString(const QVariantList &coordinates) {
    m_geometryType = LineString;
    m_coordinates = coordinates;
    emit geometryChanged();
}


// Вспомогательные методы
QString Feature::geometryTypeToString(Feature::GeometryType type) {
    switch (type) {
        case Polygon:
            return "Polygon";
        case LineString:
            return "LineString";
        case Point:
            return "Point";
        default:
            return "Unknown";
    }
}

Feature::GeometryType Feature::stringToGeometryType(const QString &type) {
    if (type == "Polygon") return Polygon;
    if (type == "LineString") return LineString;
    if (type == "Point") return Point;
    return Unknown;
}

QString Feature::toString(){
    QJsonDocument doc(toJson());
    return doc.toJson(QJsonDocument::Compact);
}


QList<QList<QPointF>>  Feature::getMultyCoordinates()  const{
    QList<QList<QPointF>> coordinates;

    for(const auto  arr: m_coordinates){
        QList<QPointF> points;
        for(const auto  p: arr.toList()){
            auto lst = p.toList();
            points.append( QPointF(lst[0].toDouble(), lst[1].toDouble()));
        }
        coordinates.append(points);
    }
    return coordinates;
}

QList<QPointF>  Feature::getCoordinates()  const{
    QList<QVariant> coordinates;

    if (m_geometryType == Polygon) coordinates.append(m_coordinates.at(0).toList());
    else coordinates.append(m_coordinates);

    QList<QPointF> points;
    for(const auto  p: coordinates){
        auto lst = p.toList();
        points.append( QPointF(lst[0].toDouble(), lst[1].toDouble()));
    }
    return points;
}

QString  Feature::getId() const{
    if (!m_properties.contains("id")) return  "";
    return m_properties["id"].toString();
};
QString  Feature::getClassCode() const{
    if (!m_properties.contains("class_code")) return  "";
    return m_properties["class_code"].toString();
};

void Feature::updateProperty(const QString key, const QVariant value) {
    if(!m_properties.contains(key)) return;
    m_properties.insert(key, value);
};

#ifndef SCENARIOCLIENT_FEATURE_H
#define SCENARIOCLIENT_FEATURE_H

#include <QObject>
#include <QVariantMap>



class Feature : public QObject
{
    Q_OBJECT
public:
    enum GeometryType {
        Polygon,
        LineString,
        Point,
        Unknown
    };
    Q_ENUM(GeometryType)

    explicit Feature(QObject *parent = nullptr) : QObject(parent), m_type("Feature") {}
    explicit Feature(const QJsonObject &json, QObject *parent = nullptr) : QObject(parent) {  fromJson(json);  }

    // Основные методы
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);
    void setPolygon(const QVariantList &coordinates) ;
    void setLineString(const QVariantList &coordinates) ;

    void updateProperty(const QString key, const QVariant value);

    QList<QPointF> getCoordinates() const;
    QList<QList<QPointF>>  getMultyCoordinates()const;

    QVariantMap getProperties() const { return m_properties; };
    Feature::GeometryType geometryType() const { return m_geometryType; }
    QString getId() const;
    QString getClassCode() const;

    // Вспомогательные методы
    static QString geometryTypeToString(Feature::GeometryType type) ;
    static Feature::GeometryType stringToGeometryType(const QString &type);

    QString toString();
signals:
    void geometryChanged();

private:
    QString m_type = "Feature";
    QVariantMap m_properties = {};
    GeometryType m_geometryType = Unknown;
    QVariantList m_coordinates = {};

};

Q_DECLARE_METATYPE(Feature*)



#endif // SCENARIOCLIENT_FEATURE_H

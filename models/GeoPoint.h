#ifndef SCENARIOCLIENT_GEOPOINT_H
#define SCENARIOCLIENT_GEOPOINT_H
#include <QObject>
#include <QVariant>

class QGeoPoint : public QObject
{
Q_OBJECT
    Q_PROPERTY(double lat READ lat WRITE setLat NOTIFY positionChanged)
    Q_PROPERTY(double lon READ lon WRITE setLon NOTIFY positionChanged)
    Q_PROPERTY(double alt READ alt WRITE setAlt NOTIFY positionChanged)

public:
    explicit QGeoPoint(QObject *parent = nullptr)
            : QObject(parent), m_lat(0), m_lon(0), m_alt(0) {}

    QGeoPoint(double lat, double lon, double alt = 0, QObject *parent = nullptr)
            : QObject(parent), m_lat(lat), m_lon(lon), m_alt(alt) {}

    // Конструктор из QVariant (для удобства интеграции)
    explicit QGeoPoint(const QVariant &variant, QObject *parent = nullptr);

    // Геттеры
    double lat() const { return m_lat; }
    double lon() const { return m_lon; }
    double alt() const { return m_alt; }

    // Альтернативные названия (x,y,z)
    double x() const { return m_lon; }  // longitude ~ x
    double y() const { return m_lat; }  // latitude ~ y
    double z() const { return m_alt; }  // altitude ~ z

    // Сеттеры
    void setLat(double lat) {
        if (!qFuzzyCompare(m_lat, lat)) {
            m_lat = lat;
            emit positionChanged();
        }
    }

    void setLon(double lon) {
        if (!qFuzzyCompare(m_lon, lon)) {
            m_lon = lon;
            emit positionChanged();
        }
    }

    void setAlt(double alt) {
        if (!qFuzzyCompare(m_alt, alt)) {
            m_alt = alt;
            emit positionChanged();
        }
    }

    // Конвертация в QVariantList
    QVariantList toVariantList() const {
        return {m_lat, m_lon, m_alt};
    }

    // Конвертация в строку
    QString toString() const ;

signals:
    void positionChanged();

private:
    double m_lat;  // Широта (latitude)
    double m_lon;  // Долгота (longitude)
    double m_alt;  // Высота (altitude)
};
#endif //SCENARIOCLIENT_GEOPOINT_H

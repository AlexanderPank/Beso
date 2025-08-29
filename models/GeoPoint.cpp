#include "GeoPoint.h"

QGeoPoint::QGeoPoint(const QVariant &variant, QObject *parent)
        : QObject(parent)
{
    if (variant.canConvert<QVariantList>()) {
        QVariantList list = variant.toList();
        if (list.size() >= 2) {
            m_lat = list[0].toDouble();
            m_lon = list[1].toDouble();
            m_alt = list.size() > 2 ? list[2].toDouble() : 0;
        }
    }
}
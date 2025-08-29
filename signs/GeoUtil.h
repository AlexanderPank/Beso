#ifndef GEOUTIL_H
#define GEOUTIL_H

#include <QObject>
#include <mapapi.h>
#include <maptype.h>
#include <QTextCodec>
#include <QPointF>


class GeoUtil
{
public:
    GeoUtil();

    HOBJ CreateSiteGisObject(HMAP hMap, HSITE hSite, const QString &gisObjectIncode) const;
    HOBJ getHObj(HMAP hMap, HSITE hSite, long key);
    int criticalError(QString str, int errorCode);
    QList<QPointF> PrepareGeometry(int  geometryType, const QList<QPointF> &coordinates);

    HOBJ DrawGisObject(HMAP hMap, HSITE hSite, long int gisObjectUid, const QString& gisObjectIncode,
                       const QMap<int, QString>& gisObjectCharacteristics, int gisObjectCoordinateSystemType,
                       const QList<QPointF>& gisObjectCoordinates, bool spline, int geometryType);
    long UpdateGisObject(HMAP hMap, HOBJ hObj, const QMap<int,QString>& gisObjectCharacteristics,
                         const QList<QPointF> &gisObjectCoordinates, bool spline, int geometryType);
    int  DeleteGisObject(HOBJ hObj);
    int DeleteGisObject(HMAP hMap, HSITE hSite, long int key);

    QTextCodec* coreCodec;
    QByteArray fromUnicode(QString str) const;
    QString toUnicode(QByteArray str) const;

    int AppendPointToGisObjectGeo(HMAP hMap, HOBJ hObj, const QPointF& coordinate, double sub, int commit );
    int AppendGeometryToGisObjectGeo(HMAP hMap, HOBJ hObj, const QList<QPointF>& geometry, double sub, bool spline);

    long int mapAppendSemanticLong(HOBJ info, long int code, long int value);
    static double toRadians(double v);
    static double toDegrees(double v);
    static QList<QPointF> toRadians(QList<QPointF> points);
    static QList<QPointF> toDegrees(QList<QPointF> points);
    static QPointF calculateNewPoint(double latitude, double longitude, double distanceMeters, double bearingDegrees);
    static QPointF calculateNewPointRadians(double latRad, double lonRad, double distanceMeters, double bearingDegrees);
    static double calculateAngleRad(const QPointF &pointRad1, const QPointF &pointRad2);
    static double calculateAngleDeg(const QPointF &pointDeg1, const QPointF &pointDeg2);
    static double distance(const QVector<QPointF> &points);
    static double distanceBetweenPoints(const QPointF &p1Rad, const QPointF &p2Rad);

    static QVector<QPointF> convertPointsPicToRadian(HMAP hMap,const QVector<QPointF>& points);
    static QVector<QPointF> calculateCirclePointsRadians(double centerLat, double centerLon, double radiusMeters);
    static QVector<QPointF> calculateViewSector( const QPointF& observer, const QPointF& target,
                                                 qreal angleWidth,   qreal distance, int numPoints = 16 );

    static QPointF calculateNewPointOffsetMeters(double latRad, double lonRad, double dxEastMeters, double dyNorthMeters);
};

#endif // GEOUTIL_H

#include "GeoUtil.h"
#include "../core/QLogStream.h"
#include "../map_widget/map_tools/CoordCtx.h"
#include <QDebug>
#include <QtMath>
#include "SignBase.h"

constexpr double EARTH_RADIUS_METERS = 6378137.0;

GeoUtil::GeoUtil(){ coreCodec = QTextCodec::codecForName("KOI8-R");}

double GeoUtil::toRadians(double v)  { return v * (M_PI / 180.0);}
double GeoUtil::toDegrees(double v)  { return v * (180.0 / M_PI);}

QList<QPointF> GeoUtil::toRadians(QList<QPointF> points)  {
    for(auto& point: points){
        point.setX(GeoUtil::toRadians(point.x()) );
        point.setY(GeoUtil::toRadians(point.y()) );
    }
    return points;
}

QList<QPointF> GeoUtil::toDegrees(QList<QPointF> points)  {
    for(auto& point: points){
        point.setX(GeoUtil::toDegrees(point.x()) );
        point.setY(GeoUtil::toDegrees(point.y()) );
    }
    return points;
}

static inline double normalizeLonDiff(double dlon) {
    // приведём разницу долгот к диапазону [-pi, pi]
    constexpr double pi = 3.14159265358979323846;
    const double twoPi = 2.0 * pi;
    dlon = std::fmod(dlon, twoPi);
    if (dlon >  pi) dlon -= twoPi;
    if (dlon < -pi) dlon += twoPi;
    return dlon;
}

double GeoUtil::distanceBetweenPoints(const QPointF &p1Rad, const QPointF &p2Rad) {
    // p.x = lon(rad), p.y = lat(rad)
    const double R = 6371000.0; // средний радиус Земли, м

    const double dlat = p2Rad.x() - p1Rad.x();
    const double dlon = normalizeLonDiff(p2Rad.y() - p1Rad.y());

    const double sdlat2 = qSin(0.5 * dlat);
    const double sdlon2 = qSin(0.5 * dlon);

    double a = sdlat2 * sdlat2 +
               qCos(p1Rad.x()) * qCos(p2Rad.x()) * (sdlon2 * sdlon2);

    // эквивалент clamp для C++11
    a = max(0.0, min(a, 1.0));

    const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
    return R * c;
}

double GeoUtil::distance(const QVector<QPointF> &pointsRad){
    if (pointsRad.size() < 2) return 0.0;

    double total = 0.0;
    for (int i = 1; i < pointsRad.size(); ++i) {
        qDebug() << pointsRad;
        total += distanceBetweenPoints(pointsRad[i-1], pointsRad[i]);
    }
    return total; // Общее расстояние в метрах
}

QVector<QPointF> GeoUtil::convertPointsPicToRadian(HMAP hMap,const QVector<QPointF>& points){
    QVector<QPointF> nPoints;
    for (auto  point: points) {
        auto ctx = new CoordCtx(hMap, PIC, point.x(), point.y());
        nPoints.append(ctx->geo());
    }
    return nPoints;
}

// Функция для вычисления новой точки
QPointF GeoUtil::calculateNewPoint(double latitudeDeg, double longitudeDeg, double distanceMeters, double bearingDegrees) {
    // Константа радиуса Земли в метрах
    const double earthRadius = 6371000.0;

    // Преобразуем угол направления в радианы
    double bearingRad = toRadians(bearingDegrees);

    // Преобразуем текущие координаты в радианы
    double latRad = toRadians(latitudeDeg);
    double lonRad = toRadians(longitudeDeg);

    // Вычисление новой координаты
    double newLatRad = asin(sin(latRad) * cos(distanceMeters / earthRadius) +
                            cos(latRad) * sin(distanceMeters / earthRadius) * cos(bearingRad));
    double newLonRad = lonRad + atan2(sin(bearingRad) * sin(distanceMeters / earthRadius) * cos(latRad),
                                      cos(distanceMeters / earthRadius) - sin(latRad) * sin(newLatRad));
    newLonRad = fmod(newLonRad + M_PI, 2 * M_PI) - M_PI;
    // Преобразуем координаты обратно в градусы
    double newLat = qRadiansToDegrees(newLatRad);
    double newLon = qRadiansToDegrees(newLonRad);

    return QPointF(newLat, newLon );
}

//QPointF GeoUtil::calculateNewPointRadians(double latRad, double lonRad, double distanceMeters, double bearingDegrees) {
//    // Константа радиуса Земли в метрах
//    const double earthRadius = 6371000.0;
//
//    // Преобразуем угол направления в радианы
//    double bearingRad = toRadians(bearingDegrees);
//
//    // Вычисление новой координаты
//    double newLatRad = asin(sin(latRad) * cos(distanceMeters / earthRadius) +
//                            cos(latRad) * sin(distanceMeters / earthRadius) * cos(bearingRad));
//    double newLonRad = lonRad + atan2(sin(bearingRad) * sin(distanceMeters / earthRadius) * cos(latRad),
//                                      cos(distanceMeters / earthRadius) - sin(latRad) * sin(newLatRad));
//    newLonRad = fmod(newLonRad + M_PI, 2 * M_PI) - M_PI;
//    // Преобразуем координаты обратно в градусы
//
//
//    return QPointF(newLatRad, newLonRad);
//}
static inline double clamp01(double v) {
    return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v);
}

static inline double normalizeLon(double lon) {
    // перевод в диапазон [-pi, pi]
    const double pi    = 3.14159265358979323846;
    const double twoPi = 2.0 * pi;
    lon = std::fmod(lon + pi, twoPi);
    if (lon < 0.0) lon += twoPi;
    return lon - pi;
}

// Возвращает QPointF(lonRad, latRad) в РАДИАНАХ
QPointF GeoUtil::calculateNewPointRadians(double latRad,
                                          double lonRad,
                                          double distanceMeters,
                                          double bearingDegrees)
{
    const double R     = 6371008.8; // средний сферический радиус Земли, м
    const double theta = qDegreesToRadians(bearingDegrees); // курс в рад
    const double delta = distanceMeters / R;                 // угловое расстояние (рад)

    const double sinLat = qSin(latRad);
    const double cosLat = qCos(latRad);
    const double sinDelta = qSin(delta);
    const double cosDelta = qCos(delta);
    const double sinTheta = qSin(theta);
    const double cosTheta = qCos(theta);

    // широта
    double arg = sinLat * cosDelta + cosLat * sinDelta * cosTheta;
    arg = clamp01(arg);                    // защита от округления
    double newLatRad = qAsin(arg);

    // долгота
    double newLonRad = lonRad + qAtan2(sinTheta * sinDelta * cosLat,
                                       cosDelta - sinLat * qSin(newLatRad));
    newLonRad = normalizeLon(newLonRad);   // в [-pi, pi]

    // ПОДЧЕРКИВАЮ: возвращаем (lat, lon) 0.77910185602562332 0.6604558477822956  newLat= 0.77941577805316797 newLon=0.66045584778229571
    return QPointF(newLatRad, newLonRad );
}

double GeoUtil::calculateAngleRad(const QPointF &pointRad1, const QPointF &pointRad2) {
    double lat1 = pointRad1.x();  // Широта точки 1
    double lon1 = pointRad1.y();  // Долгота точки 1
    double lat2 = pointRad2.x();  // Широта точки 2
    double lon2 = pointRad2.y();  // Долгота точки 2

    // Разница долгот
    double dLon = lon2 - lon1;

    // Вычисление азимута по сферической формуле
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double azimuthRad = atan2(y, x);

    // Нормализация угла в диапазон [0, 2π)
    if (azimuthRad < 0) {
        azimuthRad += 2 * M_PI;
    }

    return azimuthRad;
}

double GeoUtil::calculateAngleDeg(const QPointF &pointDeg1, const QPointF &pointDeg2) {
    // Преобразуем градусы в радианы
    double lat1 = qDegreesToRadians(pointDeg1.y());
    double lon1 = qDegreesToRadians(pointDeg1.x());
    double lat2 = qDegreesToRadians(pointDeg2.y());
    double lon2 = qDegreesToRadians(pointDeg2.x());

    // Разница долгот
    double dLon = lon2 - lon1;

    // Вычисляем азимут по формуле сферической геометрии
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double angleRad = atan2(y, x);

    // Конвертируем радианы в градусы (от 0° до 360°)
    double angleDeg = qRadiansToDegrees(angleRad);

    // Корректируем отрицательные углы
    if (angleDeg < 0) {
        angleDeg += 360.0;
    }

    return angleDeg;
}

HOBJ GeoUtil::CreateSiteGisObject(HMAP hMap, HSITE hSite, const QString &gisObjectIncode) const
{
    if( !hMap ) return -1;
    if( !hSite ) return -2;
    if( gisObjectIncode.isEmpty()) return -3;
    HOBJ hObj = mapCreateSiteObject(hMap,hSite,IDDOUBLE2,0);
    if(!hObj) return -4;
    const char *data = fromUnicode(gisObjectIncode).constData();
    long ret = mapRegisterObjectByKey(hObj, data);
    if( ret ) return hObj;
    mapFreeObject(hObj);
    return -5;
}

int GeoUtil::criticalError(QString str, int errorCode){
    qCritical() <<  str;
    return errorCode;
}

HOBJ GeoUtil::DrawGisObject(HMAP hMap,
                   HSITE hSite,
                   long int gisObjectUid,
                   const QString& gisObjectIncode,
                   const QMap<int, QString>& gisObjectCharacteristics,
                   int gisObjectCoordinateSystemType,
                   const QList<QPointF>& gisObjectCoordinates,
                   bool spline, int geometryType)
{
    // Проверка входных параметров
    if (!hMap) return criticalError(QString(Q_FUNC_INFO) + "Ошибка: HMAP недействителен.", -1);
    if (!hSite) return criticalError(QString(Q_FUNC_INFO) + "Ошибка: HSITE недействителен.", -2);
    if (gisObjectIncode.isEmpty()) return criticalError(QString(Q_FUNC_INFO) + "Ошибка: Код объекта (gisObjectIncode) не указан.", -3);

    // Создание объекта на карте
    HOBJ hObj = CreateSiteGisObject(hMap, hSite, gisObjectIncode);
    if (hObj <= 0) return criticalError(QString(Q_FUNC_INFO) + "Ошибка: Не удалось создать объект на карте.", -4);

    // Определение типа геометрии объекта
    //qint64 geometryType = mapObjectLocal(hObj);

    // Установка семантических характеристик объекта
    QMapIterator<int, QString> it(gisObjectCharacteristics);
    while (it.hasNext()) {
        it.next();
        int semanticCode = it.key();
        QString semanticValue = it.value();

        char buf[MAX_PATH] = {0};
        QByteArray encodedValue = fromUnicode(semanticValue);
        memcpy(buf, encodedValue.data(), qMin(encodedValue.size(), MAX_PATH - 1));

        // Проверка существования семантики
        long number = mapSemanticCodeValue(hObj, semanticCode, buf, MAX_PATH, 1);
        if (number) {
            // Обновление существующей семантики
            if (mapSetSemanticValue(hObj, number, buf, MAX_PATH) == 0) {
                qWarning() << Q_FUNC_INFO << "Ошибка обновления семантики:" << semanticCode << semanticValue;
            }
        } else {
            // Добавление новой семантики
            if (mapAppendSemantic(hObj, semanticCode, buf, MAX_PATH) == 0) {
                qWarning() << Q_FUNC_INFO << "Ошибка добавления семантики:" << semanticCode << semanticValue;
            }
        }

        // Нанесение текста на объект
        if (mapPutText(hObj, buf, 0) == 0) {
            qWarning() << Q_FUNC_INFO << "Ошибка нанесения текста для семантики:" << semanticCode << semanticValue;
        }
    }

    // Подготовка геометрии объекта
    QList<QPointF> gisObjectGeometry = PrepareGeometry(geometryType, gisObjectCoordinates);
    if (gisObjectGeometry.isEmpty()) {
        qCritical() << Q_FUNC_INFO << "Ошибка: Не удалось подготовить геометрию объекта.";
        mapFreeObject(hObj);
        return -5;
    }

    // Установка уникального ключа объекта
    if (gisObjectUid > 0) {
        if (mapSetObjectKey(hObj, gisObjectUid) == 0) {
            qCritical() << Q_FUNC_INFO << "Ошибка: Не удалось установить уникальный ключ объекта.";
            mapFreeObject(hObj);
            return -6;
        }
    }

    // Нанесение геометрии на карту
    long rc  = AppendGeometryToGisObjectGeo(hMap, hObj, gisObjectGeometry, 0, spline);


    // Проверка успешности нанесения объекта
    if (rc <= 0) {
        qCritical() << Q_FUNC_INFO << "Ошибка: Не удалось нанести объект на карту.";
        qCritical() << "Тип системы координат:" << gisObjectCoordinateSystemType;
        qCritical() << "Код ошибки:" << rc;
        mapFreeObject(hObj);
        return -7;
    }

    // Успешное завершение
    // qDebug() << Q_FUNC_INFO << "Объект успешно нанесен на карту. UID:" << gisObjectUid << "HOBJ:" << hObj;

    // Дополнительная отладка: вывод имени объекта (если есть)
    char objectName[256] = {0};
    if (mapSemanticValueName(hObj, 31005, objectName, sizeof(objectName)) > 0) {
        qDebug() << "Имя объекта:" << toUnicode(objectName);
    }

    return hObj;
}

HOBJ GeoUtil::getHObj(HMAP hMap, HSITE hSite, long key)
{
    if( !hMap  ) return -1;
    if( !hSite ) return -2;
    HOBJ hObjEmpty  = mapCreateSiteObject(hMap,hSite,IDDOUBLE2,0);
    if( !hObjEmpty )  return -3;
    HOBJ hObj = mapSeekSiteObject(hMap,hSite,hObjEmpty,key);
    return hObj;
}

int GeoUtil::DeleteGisObject(HMAP hMap, HSITE hSite, long int key)
{
    HOBJ hObj = getHObj(hMap, hSite, key);
    if( hObj <= 0 ) {
        qCritical() << Q_FUNC_INFO << "Ошибка поиска знака для удаления" << hObj;
        return hObj;
    }
    int rc = mapDeleteObject(hObj);
    if( rc != 0 ) {
        mapFreeObject(hObj);
        hObj= 0;
    }else {
        qCritical() << Q_FUNC_INFO << "Ошибка удаления знака" << hObj;
    }
    return rc;
}


QList<QPointF> GeoUtil::PrepareGeometry(int  _geometryType, const QList<QPointF> &coordinates)
{
    QList<QPointF> ret;
    if( coordinates.isEmpty() )  return ret;

    auto geometryType = static_cast<SignBase::SignType>(_geometryType);

    switch (geometryType) {
        case SignBase::SignType::LOCAL_TITLE:
        {

            if( coordinates.size() < 2 ) {
                ret.append(QPointF(coordinates[0].x(),coordinates[0].y()));
                ret.append(QPointF(coordinates[0].x(),coordinates[0].y()+200));
                return ret;
            }
            ret.append(QPointF(coordinates[0].x(),coordinates[0].y()));
            ret.append(QPointF(coordinates[1].x(),coordinates[1].y()));
            return ret;
        }
        case SignBase::SignType::LOCAL_LINE:
        {
            if( coordinates.size() < 2 ){
                return ret;
            }
            foreach (auto coord, coordinates) {
                ret.append(QPointF(coord.x(),coord.y()));
            }
            return ret;
        }
        case SignBase::SignType::LOCAL_POLYGON:
        {
            if( coordinates.size() < 3 ){
                return  ret;
            }
            foreach (auto coord, coordinates) {
                ret.append(QPointF(coord.x(),coord.y()));
            }
            if( coordinates.front()!=coordinates.back()) {
                auto coord = coordinates.front();
                ret.append(QPointF(coord.x(),coord.y()));
            }
            return ret;
        }
        case SignBase::SignType::LOCAL_POINT:
        {
            if ( coordinates.size() < 1 ) return ret;
            auto coord = coordinates.front();
            ret.append(QPointF(coord.x(),coord.y()));
            return {coordinates.front(), coordinates.back()};
        }
        case SignBase::SignType::LOCAL_SQUARE:
        case SignBase::SignType::LOCAL_CIRCLE:
        {
            if ( coordinates.size() < 2 ) return ret;
            auto radius = distanceBetweenPoints(coordinates[0],coordinates[1]);
            return calculateCirclePointsRadians(coordinates[0].x(),coordinates[0].y(), radius).toList();
        }
        case SignBase::SignType::LOCAL_RECTANGLE:
        {
            if ( coordinates.size() < 2 ) return ret;

            auto p1 = coordinates[0];
            auto p2 = QPointF(coordinates[1].x(),coordinates[0].y());
            auto p3 = coordinates[1];
            auto p4 = QPointF(coordinates[0].x(),coordinates[1].y());
            return {p1,p2,p3,p4,p1};

        }
//        case LOCAL_MIXED:
//            break;
        default:
            break;
    }
    return ret;
}

// subject - номер подобъекта (если = 0, обрабатывается объект)
int GeoUtil::AppendGeometryToGisObjectGeo( HMAP hMap, HOBJ hObj, const QList<QPointF>& geometry, double subject, bool spline)
{
    if( !hMap ) return -1;
    if( !hObj ) return -2;
    if( geometry.isEmpty() ) return -3;

    int rc = -4;

    auto coordinates = geometry;//.getCoordinates();
    if( coordinates.isEmpty() ) return -5;
    if( coordinates.isEmpty() ) return -6;
    for( int i =0; i< coordinates.count();i++) {
        rc = AppendPointToGisObjectGeo(hMap,hObj,coordinates[i],subject,0);
    }

    if(spline){
        qDebug() << "splining: " << mapBendSpline(hObj, 25, 10, -1);
    }
    long int error;
    rc = mapCommitObjectEx(hObj, &error);
    return rc;
}

// subject - номер подобъекта (если = 0, обрабатывается объект)
int GeoUtil::AppendPointToGisObjectGeo( HMAP hMap, HOBJ hObj, const QPointF& coordinate, double subject, int commit )
{

    double xRad = 0, yRad = 0;

    DFRAME dframe;

    if( !hMap ) return -1;
    if( !hObj ) return -2;
    int rc = mapGetTotalBorder(hMap,&dframe,PP_GEO);
    if( !rc ) return -3;

    xRad = coordinate.x();
    yRad = coordinate.y();
    if( xRad < dframe.X1  || xRad > dframe.X2  || yRad < dframe.Y1 || yRad > dframe.Y2 ) return -4;

    rc = mapAppendPointGeo(hObj,xRad,yRad,subject);
    if( commit )  rc = mapCommitObject(hObj);
    return rc;
}

QByteArray GeoUtil::fromUnicode(const QString str) const { return coreCodec->fromUnicode(str);}
QString GeoUtil::toUnicode(const QByteArray str) const {return coreCodec->toUnicode(str);}


long GeoUtil::UpdateGisObject(HMAP hMap, HOBJ hObj, const QMap<int,QString>& gisObjectCharacteristics,
                              const QList<QPointF> &gisObjectCoordinates, bool spline, int geometryType)
{
    long rc = -1;
    char buf[MAX_PATH] = {0};

    QList<QPointF> gisObjectGeometry;

    //qint64 geometryType = mapObjectLocal(hObj);

    QMapIterator<int,QString> it(gisObjectCharacteristics);
    while (it.hasNext()) {
        it.next();
        long number = mapSemanticCodeValue(hObj, it.key(), buf, MAX_PATH, 1);
        memset(buf, 0, MAX_PATH);
        memcpy(buf, fromUnicode(it.value()), MAX_PATH);
        if (number) {
            long rcSetSemantic = mapSetSemanticValue(hObj, number, buf, MAX_PATH);
            if( rcSetSemantic == 0 )
                qWarning() << Q_FUNC_INFO << "Ошибка установки семантики" << number << buf;
        }
        else {
            long rcAppendSemantic = mapAppendSemantic(hObj, it.key(), buf, MAX_PATH);
            if( rcAppendSemantic == 0 )
                qWarning() << Q_FUNC_INFO << "Ошибка добавление семантики" << number << buf;
        }
        rc = mapPutText(hObj, buf, 0);
        if( rc == 0 ) {
            qWarning() << Q_FUNC_INFO << "Ошибка нанесения текста по характеристике" << it.key() << rc;
            qWarning() << Q_FUNC_INFO << "значение"<< toUnicode(buf);
        }
    }


    if( !gisObjectCoordinates.isEmpty() ) {
        gisObjectGeometry  = PrepareGeometry(geometryType,gisObjectCoordinates);
    }

     auto p_count = (long int)mapPointCount(hObj,0);
     mapDeletePartObject(hObj, 1, p_count, 0);

    if(gisObjectCoordinates.isEmpty() )
        return 1;

/*    for (auto& point : gisObjectGeometry) {
        point.setX(toRadians(point.x()));
        point.setY(toRadians(point.y()));
    } */
    rc = AppendGeometryToGisObjectGeo(hMap, hObj, gisObjectGeometry, 0, spline);

    if( rc <= 0 ) {
        qLog() << Q_FUNC_INFO << "Ошибка перемещения знака по карте.";
        qLog() << Q_FUNC_INFO << "rc" << rc;
        qLog() << Q_FUNC_INFO << "hObj" << hObj;
//        mapFreeObject(hObj);
    }else {
        qDebug() << Q_FUNC_INFO << "Знак"  << "успешно перемещен на карте" << "hObj" << hObj;
    }
    return rc;
}

long int GeoUtil::mapAppendSemanticLong(HOBJ info, long int code, long int value){
    return mapAppendSemanticLong( info, code,  value);
}

int GeoUtil::DeleteGisObject(HOBJ hObj)
{
    try {
        int loding = mapIsObjectLoading(hObj);
        int rc = mapDeleteObject(hObj);
        if( rc != 0 ) {
            mapFreeObject(hObj);
            hObj= 0;
        }else {
            qCritical() << Q_FUNC_INFO << "Ошибка удаления знака" << hObj;
        }
        return rc;
    } catch (...) {
        if( hObj ) {
            mapFreeObject(hObj);
            hObj = 0;
        }
            return -1;
    }
}



QVector<QPointF> GeoUtil::calculateCirclePointsRadians(double centerLatRad, double centerLonRad, double radiusMeters) {

    const double earthRadius = 6371000.0;
    QVector<QPointF> points;

    // Углы направления (азимуты) в радианах: 0, 90, 180, 270 градусов
    //const QVector<double> bearings = {0.0, M_PI_2, M_PI, 3 * M_PI_2};
    int count_points = 8;
    for (int i = 0; i < count_points; ++i) {
        double bearing = i * M_PI/(count_points/2);  // π/4 = 45°
        double angularDistance = radiusMeters / earthRadius;

        double sinLat1 = std::sin(centerLatRad);
        double cosLat1 = std::cos(centerLatRad);
        double sinAngular = std::sin(angularDistance);
        double cosAngular = std::cos(angularDistance);
        double sinBearing = std::sin(bearing);
        double cosBearing = std::cos(bearing);

        // Вычисление широты новой точки
        double lat2 = std::asin(sinLat1 * cosAngular + cosLat1 * sinAngular * cosBearing);

        // Вычисление долготы новой точки
        double lon2 = centerLonRad + std::atan2(
                sinBearing * sinAngular * cosLat1,
                cosAngular - sinLat1 * std::sin(lat2)
        );

        // Нормализуем долготу в диапазон [-π, π]
        lon2 = std::fmod((lon2 + M_PI), 2 * M_PI);
        if (lon2 < 0)
            lon2 += 2 * M_PI;
        lon2 -= M_PI;

        points.append(QPointF(lat2, lon2));
    }
    points.append(points.first());
    return points;
}


//    QVector<QPointF> sectorPoints;
//
//    // 1. Вычисляем азимут (направление) от наблюдателя к цели (в градусах)
//    qreal azimuthDeg = calculateAngleRad(observerRad, targetRad);
//
//    // 2. Вычисляем крайние направления сектора
//    qreal halfAngleDeg = angleWidthDeg / 2;
//    qreal azimuth1 = azimuthDeg - halfAngleDeg;
//    qreal azimuth2 = azimuthDeg + halfAngleDeg;
//
//    // 3. Добавляем центральную точку (наблюдателя) в радианах
//    sectorPoints.append(observerRad);
//
//    // 4. Добавляем точки дуги сектора (в радианах)
//    const int steps = 20;
//    for (int i = 0; i <= steps; ++i) {
//        qreal angleDeg = azimuth1 + (azimuth2 - azimuth1) * i / steps;
//        QPointF edgePoint = calculateNewPointRadians( observerRad.x(), observerRad.y(),
//                                                      distance, qDegreesToRadians(angleDeg));
//        sectorPoints.append(edgePoint);
//    }
//
//    // 5. Замыкаем сектор (возвращаемся к наблюдателю)
//    sectorPoints.append(observerRad);
//
//    return sectorPoints;  // Все точки в радианах

QVector<QPointF> GeoUtil::calculateViewSector(
        const QPointF& observerRad,  // Исходная точка (lat, lon) в радианах
        const QPointF& targetRad,    // Точка, куда смотрим (lat, lon) в радианах
        qreal angleWidthDeg,         // Угол обзора (в градусах)
        qreal distance,              // Дальность (в метрах)
        int numPoints           // Количество точек на дуге
) {


    QVector<QPointF> sector;
    // Добавляем исходную точку
    sector.append(observerRad);

    // Радиус Земли в метрах
    const double earthRadius = 6371000.0;

    // Переводим дистанцию в радианы
    double angularDistance = distance / earthRadius;

    double fov = angleWidthDeg * M_PI / 180.0;
    auto azimuthRad = calculateAngleRad(observerRad, targetRad);

    // Начальный угол (левый край сектора)
    double startAngle = azimuthRad - fov / 2.0;

    // Получаем координаты наблюдателя
    double lat1 = observerRad.x();
    double lon1 = observerRad.y();

    // Функция для вычисления точки по азимуту и дистанции
    auto calculatePointFromAzimuth = [lat1, lon1, angularDistance](double azimuth) {
        double lat2 = asin(sin(lat1) * cos(angularDistance) +
                           cos(lat1) * sin(angularDistance) * cos(azimuth));

        double lon2 = lon1 + atan2(sin(azimuth) * sin(angularDistance) * cos(lat1),
                                   cos(angularDistance) - sin(lat1) * sin(lat2));

        return QPointF(lat2, lon2);
    };

    // Вычисляем равномерно распределенные точки на дуге
    for (int i = 0; i < numPoints; i++) {
        double angle = startAngle + (fov * i) / (numPoints - 1);
        sector.append(calculatePointFromAzimuth(angle));
    }

    return sector;
}

QPointF GeoUtil::calculateNewPointOffsetMeters(double latRad, double lonRad, double dxEastMeters, double dyNorthMeters)
{
    // Смещения в радианах
    double dLat = dyNorthMeters / EARTH_RADIUS_METERS;
    double dLon = dxEastMeters / (EARTH_RADIUS_METERS * std::cos(latRad));

    // Новые координаты
    double newLat = latRad + dLat;
    double newLon = lonRad + dLon;

    return  QPointF(toDegrees(newLat), toDegrees(newLon));
}

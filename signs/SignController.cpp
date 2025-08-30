#include "SignController.h"
#include "../core/QLogStream.h"
#include <QTextCodec>
#include <cmath>
#include <limits>

static SignController* instance = nullptr;


SignController::SignController(HMAP  hMap, HSITE hSite, QObject *parent):
    QObject(parent),_hMap(hMap), _hSite(hSite)
{
       _geoUtil = new GeoUtil();
}

SignController* SignController::getInstance(HMAP hMap, HSITE hSite, QObject* parent ) {

    if (!instance)  instance = new SignController(hMap, hSite, parent);
    else {
        instance->setHSite(hSite);
        instance->setHMap(hMap);
        instance->setParent(parent);
    }
    return instance;
}

SignController* SignController::getInstance() {
    if (!instance) return nullptr;
    return instance;
}

void SignController::setHSite( HSITE hSite) {_hSite = hSite;}
void SignController::setHMap( HMAP hMap) {_hMap = hMap;}

int SignController::getFreeId() {
    static int counter = 1;  // Статическая переменная для хранения последнего использованного id
    while (counter <= 10000)
        if (!signs.contains(counter)) return counter;
        else counter++;
    throw std::runtime_error("No free ID available");  // Если все id заняты
}

int SignController::addSignToMap(SignBase *sign) {
    if (sign == nullptr) return NO_SIGN;
    if (!sign->getVisibility() ) return NO_SIGN;

    if (sign->getGisID() != NO_SIGN) {
        updateSignOnMap(sign->getGisID(), sign->getCoordinatesInRadians());
        return sign->getGisID();
    }

    if (sign->getGeometryType() == 4){
        qLog() << sign->getName() ;
    }
    if (sign->getCoordinatesInRadians().size() < 1) {
        qLog() << "Ошибка У объекта " << sign->getName() << " нет координат";
        return NO_SIGN;
    }
    int gisObjectUid = sign->getGisID() == -1 ?  getFreeId() : sign->getGisID();
    HOBJ hObj = _geoUtil->DrawGisObject(_hMap, _hSite, gisObjectUid,
                                        sign->getClasscode(),
                                        sign->getCharacteristics(),
                                        GIS_COORDINATE_SYSTEM_TYPE,
                                        sign->getCoordinatesInRadians(),
                                        sign->useSpline(),
                                        sign->getGeometryType()
    );
    if (hObj <= 0) {
        qLog() << "Ошибка при создании объекта";
        return NO_SIGN;
    }
    sign->setGisID(gisObjectUid);
    signs[gisObjectUid] = {hObj, sign};
    qLog() << "Объект успешно создан и нанесен на карту.";
    return gisObjectUid;
}

int SignController::redrawSignOnMap(SignBase *sign) {
    if (!signs.contains(sign->getGisID())) {
        return addSignToMap(sign);

    }
    auto signId = sign->getGisID();
    auto hObj = signs[signId].first;
    if (hObj > 0) _geoUtil->DeleteGisObject(hObj);

    hObj = _geoUtil->DrawGisObject(_hMap, _hSite, signId,
                                        sign->getClasscode(),
                                        sign->getCharacteristics(),
                                        GIS_COORDINATE_SYSTEM_TYPE,
                                        sign->getCoordinatesInRadians(),
                                        sign->useSpline(),
                                        sign->getGeometryType()
    );
    if (hObj <= 0) {
        signs[signId] = {-5, sign};
        qLog() << "Ошибка при создании объекта";
        return 0;
    }
    signs[signId] = {hObj, sign};
    return 1;
}

QMap<int, SignBase*> SignController::allSigns() const {
    QMap<int, SignBase*> result;
    for (auto it = signs.begin(); it != signs.end(); ++it) {
        result[it.key()] = it.value().second;
    }
    return result;
}
void SignController::clearHighlight() {
    for (auto v: signs) {
        auto hObj = v.first;  // Второе значение пары (SignBase*)
        if (hObj <0) continue;

        SignBase *sign = v.second;  // Второе значение пары (SignBase*)
        //if (sign->getHighlight())
        {
            sign->setHighlight(false);
            auto ret = _geoUtil->UpdateGisObject(_hMap, hObj,
                                            sign->getCharacteristics(),
                                            sign->getCoordinatesInRadians(),
                                            sign->useSpline(), sign->getGeometryType());
            qLog() << "Обновление значений знака " << ret;
            mapCommitObject(hObj);
        }
    }
    _highlightSignID = NO_SIGN;
}

void SignController::highlightSignByID(int singId) {
    if (!signs.contains(singId)){
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(singId);
        return;
    }
    auto sign = signs[singId].second;
    auto hObj = signs[singId].first;
    if (hObj < 0) return;
    int ret, semantic = 31002;

    sign->setHighlight(true);


    ret = _geoUtil->UpdateGisObject(_hMap, hObj,
                                    sign->getCharacteristics(),
                                    sign->getCoordinatesInRadians(),
                                    sign->useSpline(),
                                    sign->getGeometryType()
                                 );
    if (!ret) {
        qLog() << QString("Ошибка изменения семантики %1 у объекта ").arg(semantic) + sign->getName();
        return ;
    }
    mapCommitObject(hObj);
    _highlightSignID = singId;
   // if (refresh) { requestUpdate(); }

}

double SignController::haversineDistanceInKM(const QPointF& coord1, const QPointF& coord2) {
    double lat1 = coord1.x();
    double lon1 = coord1.y();

    double lat2 = coord2.x();
    double lon2 = coord2.y();

    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;

    double a = pow(sin(dlat / 2), 2) +
               cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS_KM * c;
}

/**
 * @brief Метод для поиска ближайшего знака в радиусе 1000 км.
 * @param coord Координата (lon, lat), относительно которой ищем ближайший знак.
 * @return ID ближайшего знака или -1, если знаков в радиусе нет.
 */
int SignController::findNearestSignID(QPointF coordInRadians, int zoom) {
    int nearestSignId = NO_SIGN;
    double minDistanceKM = 3 + zoom*1.5 + zoom;
    auto coordInDegrees = GeoUtil::toDegrees({coordInRadians})[0];
    QMap<int, QPair<HOBJ, SignBase*>>::iterator it;
    for (it = signs.begin(); it != signs.end(); ++it) {
        int key = it.key();                  // Ключ (int)
        HOBJ hobj = it.value().first;        // Первое значение пары (HOBJ)
        if (hobj < 0) continue;
        SignBase* sign = it.value().second;  // Второе значение пары (SignBase*)

        if (!sign) continue;

        for (const QPointF& signCoord : sign->getCoordinatesInDegrees()) {
            double distance = haversineDistanceInKM(coordInDegrees, signCoord);

            if (distance > minDistanceKM) continue;
            minDistanceKM = distance;
            nearestSignId = key;

        }
    }

    return nearestSignId;
}


void SignController::DragCurrentObject(double dx, double dy, int singId) {
    if (!signs.contains(singId)) {
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(singId);
        return;
    }

    auto sign = signs[singId].second;


    auto hObj = signs[singId].first;
    if (hObj < 0) return;
    auto points = sign->getCoordinatesInRadians();
    // Обновляем координаты всех точек объекта
    for (auto& point : points) {
        point.setX(point.x() + dx);
        point.setY(point.y() + dy);
    }
    sign->setCoordinatesInRadians(points);
    _geoUtil->UpdateGisObject(_hMap, hObj, sign->getCharacteristics(), points, sign->useSpline(), sign->getGeometryType());
    // requestUpdate();
}


SignBase *SignController::getSignByID(int singId) const{
    if (!signs.contains(singId)){
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(singId);
        return nullptr;
    }
    auto sign = signs[singId].second;
    return sign;
}

void SignController::rotateSelectedObject(int singId, double _angle, QVector<QPointF> &m_editPoints, QPointF center){

    qreal radians = _angle * M_PI / 180;
    qreal cosVal = cos(radians);
    qreal sinVal = sin(radians);

    for (int i = 0; i < m_editPoints.size(); ++i) {
        QPointF delta = m_editPoints[i] - center;
        m_editPoints[i].setX(center.x() + delta.x() * cosVal - delta.y() * sinVal);
        m_editPoints[i].setY(center.y() + delta.x() * sinVal + delta.y() * cosVal);
    }

    auto sign = signs[singId].second;
    auto hObj = signs[singId].first;
    if (hObj < 0) return;
    QList<QPointF> points = sign->getCoordinatesInRadians(); // !!!!

    // Находим центр объекта
    foreach (const QPointF& point, points) center += point;
    center /= points.size();

    QList<QPointF> newPoints;
    foreach (QPointF point, points) {
          QPointF delta = point - center;
          point.setX(center.x() + delta.x() * cosVal - delta.y() * sinVal);
          point.setY(center.y() + delta.x() * sinVal + delta.y() * cosVal);
          newPoints.append(point);
    }

    sign->setCoordinatesInRadians(m_editPoints.toList()); // !!!!
    _geoUtil->UpdateGisObject(_hMap, hObj, sign->getCharacteristics(), m_editPoints.toList(), sign->useSpline(), sign->getGeometryType());
}

void SignController::resizeSelectedObject(int singId, QVector<QPointF> newPoints) {

    if (!signs.contains(singId)){
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(singId);
        return;
    }
    auto sign = signs[singId].second;
    auto hObj = signs[singId].first;
    if (hObj < 0) return;

    // Для LOCAL_POINT берем первые две точки (центр и крайнюю точку)
    if (sign->getCoordinatesInRadians().size() == 2 && newPoints.size() >= 2) {
        sign->setCoordinatesInRadians({newPoints[0], newPoints[1]}); // !!!
    }
        // Для других типов берем все точки кроме последней (центральной)
    else if (newPoints.size() > 1) {
        sign->setCoordinatesInRadians(newPoints.toList()); //!!!
    }
    _geoUtil->UpdateGisObject(_hMap, hObj, sign->getCharacteristics(), sign->getCoordinatesInRadians(), sign->useSpline(), sign->getGeometryType());
}

SignBase::SignType SignController::getSignType(int signId) const{
    if (!signs.contains(signId)){
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(signId);
        return SignBase::SignType::NONE;
    }
    return  signs[signId].second->getGeometryType();
}


void SignController::removeSign(int signId){
    if (!signs.contains(signId)){
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(signId);
        return;
    }
    auto hObj = signs[signId].first;
    signs.remove(signId);
    if (hObj < 0) return;
    _geoUtil->DeleteGisObject(hObj);

}

void SignController::clear() {
    for (auto signId: signs.keys())
        removeSign(signId);
    signs.clear();
}

int SignController::updateSignOnMap(SignBase *signBase){
    if (signBase->getGisID() == NO_SIGN) return addSignToMap(signBase);
    return updateSignOnMap(signBase->getGisID(),  signBase->getCoordinatesInRadians());
}

bool SignController::updateSignOnMap(int singId,  QList<QPointF> pointsInRadians){
    auto sign = signs[singId].second;

    if (!sign) return false;

    // Если объекта есть на карте, но нужно его скрыть, то удаляем его с карты
    if (!sign->getVisibility() && sign->getGisID() != NO_SIGN)  {
        removeSign(sign->getGisID());
        sign->setGisID(NO_SIGN);
        return true;
    }

    // Если объекта нет в карте, то добавляем его
    if (sign->getGisID() == NO_SIGN) return addSignToMap(sign);


    auto hObj = signs[singId].first;
    if (hObj < 0) return false;
    sign->setCoordinatesInRadians(pointsInRadians);

    auto ret = _geoUtil->UpdateGisObject(_hMap, hObj,
                                         sign->getCharacteristics(),
                                         sign->getCoordinatesInRadians(),
                                         sign->useSpline(), sign->getGeometryType() );
    if (!ret) {
        qLog() << QString("Ошибка изменения семантики у объекта ") << sign->getName();
        return false;
    }
    mapCommitObject(hObj);
    return true;
}

void SignController::updateSignOnMap(int singId, double latDeg, double lonDeg, double courseDeg){

    if (!signs.contains(singId)){
        qLog() << QString("Ошибка Идентификатор %1 объекта не найден").arg(singId);
        return;
    }

    auto sign = signs[singId].second;
    auto hObj = signs[singId].first;
    if (hObj < 0) return;

    QList<QPointF> points;
    if (sign->getClasscode() == "1314050003300000") { // это для рубежа пуска ракет чтоб она была длиннее
        points.append( GeoUtil::calculateNewPoint(latDeg, lonDeg, 2000, courseDeg-90));
        points.append( QPointF(latDeg, lonDeg) );
        points.append(GeoUtil::calculateNewPoint(latDeg, lonDeg, 2000, courseDeg+90));
    } else {
        points.append( QPointF(latDeg, lonDeg) );
        points.append(GeoUtil::calculateNewPoint(latDeg, lonDeg, 1000, courseDeg));
    }

    points = GeoUtil::toRadians(points);

    sign->setCoordinatesInRadians(points);
    auto ret = _geoUtil->UpdateGisObject(_hMap, hObj,
                                         sign->getCharacteristics(),
                                         sign->getCoordinatesInRadians(),
                                         sign->useSpline(), sign->getGeometryType() );
    if (!ret) {
        qLog() << QString("Ошибка изменения семантики у объекта ") << sign->getName();
        return ;
    }
    mapCommitObject(hObj);
}

void SignController::destroyInstance() {
    if (instance) {
        instance->deleteLater();  // Безопасное удаление в цикле событий
        instance = nullptr;
    }
}

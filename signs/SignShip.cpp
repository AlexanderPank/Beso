
#include "SignShip.h"
#include "../core/QLogStream.h"
#include <QtMath>

SignShip::SignShip(QList<QPointF> coord, QString name):
    SignBase("131472600009", "Транспортное судно") // 131492002001
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setParamColorSign(QString("%1").arg(EParamSignColorStates::RED) );
    this->setLineWidth("0.5");
    this->setScale("80");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
    this->setOwn();
}
SignShipEnemy::SignShipEnemy(QList<QPointF> coord, QString name):SignShip(coord, name)
{
    this->setTitle("Транспортное судно противника");
    this->setParamColorSign(QString("%1").arg(EParamSignColorStates::BLUE) );
    this->setEnemy();

}

SignRegion::SignRegion(QList<QPointF> coord, QString name):
        SignBase("312655004106000000000000", "Район")
{
    setName(name);
    this->setCoordinatesInRadians(coord);

    this->setLineWidth("0.75");
    this->setScale("100");
    this->setGeometryType(SignType::LOCAL_POLYGON);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
    this->setBaseColor( QString("%1").arg(ESignColorStates::BLACK));
}

SignWay::SignWay(QList<QPointF> coord, QString name):
        SignBase("131405000210000000", "Движение в пункт назначения")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("0.5");
    this->setScale("100");
    this->setGeometryType(SignType::LOCAL_LINE);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
    this->setBaseColor( "10066431");
}

SignWayEnemy::SignWayEnemy(const QList<QPointF> coord,const QString name):SignWay(coord, name){
    this->setEnemy();
    this->setBaseColor( "16751001");
}


SignIntelligencePlane::SignIntelligencePlane(QList<QPointF> coord, QString name):
        SignBase("10185", "Разведывательный самолет срд нб")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("0.03");
    this->setScale("80");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}
SignAntiBoatPlane::SignAntiBoatPlane(QList<QPointF> coord, QString name, bool is_own):
        SignBase("10188", "Противолодочный самолет")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    if (is_own) this->setOwn();
    else
        this->setEnemy();
    this->setLineWidth("0.03");
    this->setScale("80");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}

SignAntiSubmarineBoat::SignAntiSubmarineBoat(QList<QPointF> coord, QString name):
        SignBase("10213", "Противолодочный катер")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("70");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setParamState( QString::number(static_cast<int>(ESignParamState::DESTROYED)));
}

SignAntiSubmarineBoatEnemy::SignAntiSubmarineBoatEnemy(QList<QPointF> coord, QString name):
    SignAntiSubmarineBoat(coord,  name)
{
    this->setEnemy();
}

SignNavalBase::SignNavalBase(QList<QPointF> coord, QString name):
        SignBase("131417004001000000000005", "Военно-морская база")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("200");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}

SignNavalBaseEnemy::SignNavalBaseEnemy(QList<QPointF> coord, QString name):
        SignBase("131417004001000000000005", "Военно-морская база противника")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("200");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}
SignInterceptPoint::SignInterceptPoint(QList<QPointF> coord, QString name):
        SignBase("7132250034230000000", "Точка перехвата невоенного судна")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setLineWidth("1");
    this->setScale("50");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}
SignWingsRocket::SignWingsRocket(QList<QPointF> coord, QString name):
        SignBase("1314120020040000000000", "Крылатая ракета")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("80");
    this->setGeometryType(SignType::LOCAL_POINT);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}

SignWingsRocketEnemy::SignWingsRocketEnemy(QList<QPointF> coord, QString name):SignWingsRocket(coord,name){
    this->setEnemy();
}

SignRocketLaunchLine::SignRocketLaunchLine(QList<QPointF> coord, QString name):
        SignBase("1314050003300000", "Рубеж пуска ракет - нанесения удара")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("100");
    this->setGeometryType(SignType::LOCAL_LINE);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}

SignRegionSearch::SignRegionSearch(QList<QPointF> coord, QString name):
        SignBase("31261500011300000000", "Район воздушной разведки")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("100");
    this->setGeometryType(SignType::LOCAL_POLYGON);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}

SignRegionRound::SignRegionRound(QList<QPointF> coord, QString name):
        SignBase("312615000113", "Район круглый")
{
    setName(name);
    this->setCoordinatesInRadians(coord);
    this->setOwn();
    this->setLineWidth("1");
    this->setScale("100");
    this->setSpline(true);
    this->setGeometryType(SignType::LOCAL_POLYGON);
    this->setCharacteristics({{static_cast<int>(ESignCharacteristic::COURCE), "120"}});
}



SignCircle::SignCircle(QList<QPointF> _coordinates, QString _name, double radius, QString classCode):
        SignBase("31261500011300000001", "Район круглый")
{
    radius = abs(radius);

    setName(_name);
    this->m_radius = radius;
    this->_classCode = classCode;
    this->setOwn();
    this->setLineWidth("0.5");
    this->setScale("50");
    this->setSpline(true);
    this->setGeometryType(SignType::LOCAL_CIRCLE);
    if (_coordinates.size() > 0) {
        auto points = GeoUtil::calculateNewPointRadians(_coordinates[0].x(), _coordinates[0].y(), radius, 0);
        _coordinates = {_coordinates[0], points};
        SignBase::setCoordinatesInRadians(_coordinates);
    }
}

void SignCircle::setCoordinatesInDegrees(QList<QPointF> coord, double radius, bool emit_signals){
    coord = GeoUtil::toRadians(coord);
    auto point = GeoUtil::calculateNewPointRadians(coord[0].x(), coord[0].y(), radius, 0);
    SignBase::setCoordinatesInRadians( {coord[0], point}, emit_signals);
}



SignSector::SignSector(QList<QPointF> coordRad, QString name,  QPointF targetRad, double angleWidth,double distance):
        SignBase("31261500011300000002", "Сектор обнаружения")
{
    setName(name);
    this->_classCode = "31261500011300000000";
    this->setOwn();
    this->setLineWidth("0.5");
    this->setScale("50");
    this->setSpline(false);
    this->setGeometryType(SignType::LOCAL_POLYGON);
    auto points=GeoUtil::calculateViewSector(coordRad[0], targetRad, angleWidth, distance).toList();
    SignBase::setCoordinatesInRadians(points);
}


SignWaitingRegion::SignWaitingRegion(QList<QPointF> coordRad, QString name,  bool isOwn):
        SignBase("111015000101", "Район расположения, исходный район, район ожидания")
{
    setName(name);
    if (isOwn)  this->setOwn(); else this->setEnemy();
    this->setLineWidth("0.5");
    this->setScale("50");
    this->setSpline(false);
    this->setGeometryType(SignType::LOCAL_POLYGON);
    SignBase::setCoordinatesInRadians(coordRad);
}

SignDirectionAction::SignDirectionAction(QList<QPointF> coordRad, QString name,  bool isOwn):
        SignBase("111015000523", "Направление действий (движение)")
{
    setName(name);
    if (isOwn)  this->setOwn(); else this->setEnemy();
    this->setLineWidth("0.5");
    this->setScale("50");
    this->setSpline(false);
    this->setGeometryType(SignType::LOCAL_LINE);
    SignBase::setCoordinatesInRadians(coordRad);
}


SignRestrictedArea::SignRestrictedArea(QList<QPointF> coordRad, QString name):
        SignBase("131405000123", "Районы, закрытые для плав, РЭС")
{
    setName(name);
   // this->setOwn();
    this->setLineWidth("0.5");
    this->setScale("50");
    this->setSpline(false);
    this->setGeometryType(SignType::LOCAL_POLYGON);
    SignBase::setCoordinatesInRadians(coordRad);
}


SignTitle::SignTitle(QList<QPointF> coordRad, QString name, QString text, float fontSize):
        SignBase("915300000000", "Подпись")
{
    setName(name);

    this->setLineWidth("1");
    this->setScale("50");
    this->setSpline(false);
    this->setGeometryType(SignType::LOCAL_TITLE);
    this->m_font_size = fontSize;

    QMap<int, QString> characteristics = QMap<int, QString>();
    characteristics.insert(static_cast<int>(ESignCharacteristic::TEXT_STYLE), "1");
    characteristics.insert(static_cast<int>(ESignCharacteristic::TEXT_HEIGHT), QString::number(fontSize));

    //auto coreCodec = QTextCodec::codecForName("utf-8");
    characteristics.insert(static_cast<int>(ESignCharacteristic::TEXT), text);

    characteristics.insert(32804, text);
    characteristics.insert(static_cast<int>(ESignCharacteristic::TEXT_BACKGROUND), "1");
    this->setCharacteristics( characteristics );

    SignBase::setCoordinatesInRadians(coordRad);
}

float SignTitle::getFontSize(){ return this->m_font_size; }

void SignTitle::setTitleText(const QString text){
    QMap<int, QString> characteristics = QMap<int, QString>();

    //auto coreCodec = QTextCodec::codecForName("utf-8");
    characteristics.insert(static_cast<int>(ESignCharacteristic::TEXT), text);

    characteristics.insert(32804, text);
    this->setCharacteristics( characteristics );
}
void SignTitle::setFontSize(const float size){
    QMap<int, QString> characteristics = QMap<int, QString>();
    characteristics.insert(static_cast<int>(ESignCharacteristic::TEXT_HEIGHT), QString::number(size));
    this->setCharacteristics( characteristics );
    this->m_font_size = size;
}

SignBorderOES::SignBorderOES(QList<QPointF> coordRad, QString name):
        SignBase("222215005301", "Граница зоный ОЭС"){
    setName(name);
    this->setBaseColor("16755200");
    this->setSpline(false);
    this->setGeometryType(SignType::LOCAL_POLYGON);
}


void SignBorderOES::setWidthAndLength(double sensor_width_px, double sensor_height_px, double f_mm, double height_m){

    double pixel_size_mm = 0.0014; // мм на пиксель

    // Углы FOV в радианах
    double sensor_width_mm = sensor_width_px * pixel_size_mm;
    double sensor_height_mm = sensor_height_px * pixel_size_mm;

    double fov_w_rad = 2 * atan(sensor_width_mm / (2 * f_mm));
    double fov_h_rad = 2 * atan(sensor_height_mm / (2 * f_mm));

    // Размеры зоны охвата
    auto w = 2 * height_m * tan(fov_w_rad / 2);
    auto h= 2 * height_m * tan(fov_h_rad / 2);
    m_width = w, m_height = h;

}

void SignBorderOES::redraw(QPointF centerInDegrees, double bearing){
    if (m_width == 0 || m_height == 0)return;
    auto points = buildCoveragePolygon(centerInDegrees.x(), centerInDegrees.y(), m_width, m_height, bearing);
    setCoordinatesInRadians(points);
}

// Функция возвращает 4 вершины прямоугольника в порядке:
// 0 = верх-лево, 1 = верх-право, 2 = низ-право, 3 = низ-лево
//QList<QPointF> SignBorderOES::buildCoveragePolygon( double latInDegress,    // центральная точка, град.
//                                                      double lonInDegress,    // центральная точка, град.
//                                                      double widthM,  // ширина FOV, м
//                                                      double heightM, // высота FOV, м
//                                                      double bearing) // направление «верха» кадра, град.
//{
//    auto degnormalizeBearing = [&](double deg){
//        deg = fmod(deg, 360.0);
//        if (deg < 0) deg += 360.0;
//        return deg;
//    };
//
//    const double halfW = widthM  / 2.0;
//    const double halfH = heightM / 2.0;
//
//    // углы к 4-м направлениям (в градусах)
//    const QVector<double> bearings = {
//            degnormalizeBearing( bearing - 135),  // top-left
//            degnormalizeBearing( bearing - 45),   // top-right
//            degnormalizeBearing( bearing + 45),   // bottom-right
//            degnormalizeBearing( bearing + 135)   // bottom-left
//    };
//
//    // расстояния от центра до вершины по диагонали
//    const double diag = std::sqrt(halfW * halfW + halfH * halfH);
//
//    QList<QPointF> poly;
//    for (int i = 0; i < 4; ++i) {
//        auto point = GeoUtil::calculateNewPointRadians(GeoUtil::toRadians(latInDegress),
//                                                    GeoUtil::toRadians(lonInDegress),
//                                                    diag,
//                                                    bearings[i]);
//        poly.append(point);
//    }
//    return poly;
//}

QList<QPointF> SignBorderOES::buildCoveragePolygon(double latInDegress,
                                                   double lonInDegress,
                                                   double widthM,
                                                   double heightM,
                                                   double bearingDeg)
{
    if(latInDegress == 0 && lonInDegress == 0) return {};

    const double halfW = widthM  / 2.0;
    const double halfH = heightM / 2.0;

    // Углы прямоугольника (до поворота)
    const QVector<QPointF> localOffsets = {
            QPointF(-halfW,  halfH), // top-left
            QPointF( halfW,  halfH), // top-right
            QPointF( halfW, -halfH), // bottom-right
            QPointF(-halfW, -halfH)  // bottom-left
    };

    const double bearingRad = GeoUtil::toRadians(bearingDeg);
    const double cosA = std::cos(bearingRad);
    const double sinA = std::sin(bearingRad);

    QList<QPointF> polygon;

    for (const QPointF& offset : localOffsets) {
        // Поворот вектора
        double dy = offset.x() * cosA - offset.y() * sinA;
        double dx = offset.x() * sinA + offset.y() * cosA;

        // Преобразование смещения в координаты
        auto point = GeoUtil::calculateNewPointOffsetMeters(
                GeoUtil::toRadians(latInDegress),
                GeoUtil::toRadians(lonInDegress),
                dx,
                dy
        );

        polygon.append(point);
    }

    return  GeoUtil::toRadians(polygon);
}


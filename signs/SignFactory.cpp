// sign_factory.cpp
#include "SignFactory.h"
#include "SignShip.h"
#include "GeoUtil.h"
#include "../core/QLogStream.h"

SignBase* SignFactory::createSign(const QString& classcode,
                                  const QString& name,
                                  bool is_own,
                                  QList<QPointF>& coordinatesInDegrees,
                                  double course, ObjectScenarioModel* obj)
{
    if (coordinatesInDegrees.size() == 1) {

        if (classcode=="1314050003300000") {
                // это для рубежа пуска ракет чтоб она была длиннее
            auto pnt = coordinatesInDegrees[0];
            auto pnt2 = GeoUtil::calculateNewPoint(0.0, 0.0, 1000.0, 0.0);
            coordinatesInDegrees.clear();
            coordinatesInDegrees.append(GeoUtil::calculateNewPoint(pnt.x(), pnt.y(), 2000, course-90));
            coordinatesInDegrees.append(pnt);
            coordinatesInDegrees.append(GeoUtil::calculateNewPoint(pnt.x(), pnt.y(), 2000, course+90));

        } else {
            coordinatesInDegrees.append(GeoUtil::calculateNewPoint(coordinatesInDegrees[0].x(), coordinatesInDegrees[0].y(), 1000, course));
        }
    }


    auto coordinatesInRadian = GeoUtil::toRadians(coordinatesInDegrees);

    // Словарь для соответствия classcode и функций создания объектов
    const QMap<QString, std::function<SignBase*(const QList<QPointF>&, const QString&)>> creators = {
            {"131472600009", [is_own](const QList<QPointF>& coord, const QString& name) {
                return is_own ?  static_cast<SignBase*>(new SignShip(coord, name)) :
                                    static_cast<SignBase*>(new SignShipEnemy(coord, name));

            }},
            {"312655004106000000000000", [](const QList<QPointF>& coord, const QString& name) { return new SignRegion(coord, name); }},
            {"131405000210000000", [is_own](const QList<QPointF>& coord, const QString& name) {
                return is_own ? static_cast<SignBase*>(new SignWay(coord, name))
                              : static_cast<SignBase*>(new SignWayEnemy(coord, name));
            }},
            {"10185", [](const QList<QPointF>& coord, const QString& name) { return new SignIntelligencePlane(coord, name); }},
            {"10213", [is_own](const QList<QPointF>& coord, const QString& name) {
                return is_own ? static_cast<SignBase*>(new SignAntiSubmarineBoat(coord, name))
                              : static_cast<SignBase*>(new SignAntiSubmarineBoatEnemy(coord, name));
             }},
            {"7132250034230000000", [](const QList<QPointF>& coord, const QString& name) { return new SignInterceptPoint(coord, name); }},
            {"1314050003300000", [](const QList<QPointF>& coord, const QString& name) { return new SignRocketLaunchLine(coord, name); }},
            {"131417004001000000000005", [is_own](const QList<QPointF>& coord, const QString& name) {
                return is_own ? static_cast<SignBase*>(new SignNavalBase(coord, name))
                              : static_cast<SignBase*>(new SignNavalBaseEnemy(coord, name));
            }},
            {"1314120020040000000000", [is_own](const QList<QPointF>& coord, const QString& name) {
                return is_own ? static_cast<SignBase*>(new SignWingsRocket(coord, name))
                              : static_cast<SignBase*>(new SignWingsRocketEnemy(coord, name));
            }},
            {"31261500011300000000", [is_own](const QList<QPointF>& coord, const QString& name) {
                return  static_cast<SignBase*>(new SignRegionSearch(coord, name)) ;
            }},

            {"312615000113", [is_own](const QList<QPointF> &coord, const QString &name) {
                return static_cast<SignBase *>(new SignRegionRound(coord, name));
            }},
            {"10188", [is_own](const QList<QPointF> &coord, const QString &name) {
                return static_cast<SignBase *>(new SignAntiBoatPlane(coord, name, is_own));
            }},{"111015000101", [is_own](const QList<QPointF> &coord, const QString &name) {
                return static_cast<SignBase *>(new SignWaitingRegion(coord, name, is_own));
            }},

            {"31261500011300000001", [is_own, obj](const QList<QPointF>& coord, const QString& name) {
                if (obj) {
                    bool bOk;
                    auto radius = obj->getPropertyDouble("detection_radius", bOk);
                    return  static_cast<SignBase*>(new SignCircle(coord, name,  radius)) ;
                } else {
                    qLog() << "Объект не obj обнаружен передаем радиус 0 ";
                    return  static_cast<SignBase*>(new SignCircle(coord, name,  0 )) ;
                }

            }},

            {"31261500011300000002", [is_own, obj](const QList<QPointF>& coord, const QString& name) {
                if (obj) {
                    bool bOk;
                    auto radius = obj->getPropertyDouble("detection_radius", bOk);
                    auto look_lat = obj->getPropertyDouble("look_lat", bOk);
                    auto look_lon = obj->getPropertyDouble("look_lon", bOk);
                    auto d_alfa = obj->getPropertyDouble("dAlfa", bOk);
                    auto target =  GeoUtil::toRadians({QPointF(look_lat, look_lon)})[0];
                    return  static_cast<SignBase*>(new SignSector(coord, name, target, d_alfa, radius)) ;
                } else {
                    qLog() << "Объект не obj обнаружен передаем радиус 0 ";
                    return  static_cast<SignBase*>(new SignCircle(coord, name,  0 )) ;
                }

            }}
    };

    if (creators.contains(classcode)) {
        return creators[classcode](coordinatesInRadian, name);
    }

    // Если класс не найден, создаем базовый объект
    auto *sign = new SignBase(classcode, name, QUuid::createUuid(), nullptr);
    sign->setName(name);
    sign->setCoordinatesInRadians(coordinatesInRadian);
    sign->setObjectName(name);
    if (is_own) sign->setOwn(); else sign->setEnemy();
    return sign;
}

SignBase* SignFactory::createSignFromFeature(FeatureModel* feature){

    SignBase *sign = nullptr;
    auto coordinatesInRadian = GeoUtil::toRadians(feature->getCoordinates());

    if ( feature->getGeometryType() == FeatureModel::GeometryType::CIRCLE) {
        auto radius = feature->getRadius();
        if (feature->getRadius() <= 0) {
            radius = feature->getPropertyInt("radius");
        }
        sign = new SignCircle(coordinatesInRadian, feature->getTitle(), radius < 0 ? 0 : radius, feature->getClassCode());

    } else
    if (feature->getClassCode() == "915300000000") sign = new SignTitle(coordinatesInRadian, feature->getTitle(), feature->getText(), feature->getFontSize());
    else
    if (feature->getClassCode() == "222215005301")  sign = new SignBorderOES(coordinatesInRadian, feature->getTitle());
    else {
        sign = new SignBase(feature->getClassCode(), feature->getTitle(), QUuid::createUuid(), nullptr);
        sign->setName(feature->getTitle());
        sign->setCoordinatesInRadians(coordinatesInRadian);
    }

    sign->setLineWidth( QString("%1").arg(feature->getLineWidth()));
    sign->setScale( QString("%1").arg(feature->getScale()));
    sign->setSpline( feature->getSpline());
    sign->setFeature(true);
    if (feature->getColor() != "base") {
        sign->setColorRGB(feature->getColor());
        sign->setBaseColor(feature->getColor());
    } else
        if (feature->getOwn() == 1) sign->setOwn();
     else
        if (feature->getOwn() == 0) sign->setEnemy();


    if (feature->getGeometryType() == FeatureModel::GeometryType::POLYGON)
        sign->setGeometryType(SignBase::LOCAL_POLYGON);
    else if (feature->getGeometryType() == FeatureModel::GeometryType::LINE)
        sign->setGeometryType(SignBase::LOCAL_LINE);

    return sign;
}

SignBase* SignFactory::updateConnectedSign(SignBase* sign, ObjectScenarioModel *obj){

    if (auto border = dynamic_cast<SignBorderOES*>(sign)) {
        bool bOk;
        QPointF center(obj->getPropertyDouble("lat", bOk), obj->getPropertyDouble("lon", bOk) );
        auto bearing = obj->getPropertyDouble("course", bOk);
        auto oes_res_height = obj->getPropertyDouble("oes_res_height", bOk);
        auto oes_res_width = obj->getPropertyDouble("oes_res_width", bOk);
        auto oes_fov = obj->getPropertyDouble("oes_fov", bOk);
        auto alt = obj->getPropertyDouble("alt", bOk);
        border->setWidthAndLength( oes_res_width,  oes_res_height,  oes_fov,  alt);
        border->redraw(center, bearing);
        return border;
    }
    if (auto border = dynamic_cast<SignCircle*>(sign)) {
        bool bOk;
        QPointF center(obj->getPropertyDouble("lat", bOk), obj->getPropertyDouble("lon", bOk) );
        auto radius = border->getRadius();
        border->setCoordinatesInDegrees({center}, radius, false);
        return border;
    }

    return nullptr;
}
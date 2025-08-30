#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>
#include <QString>
#include <QDebug>
#include <QJsonArray>

#include "SignBase.h"
#include "GeoUtil.h"
#include "../core/QLogStream.h"


SignBase::SignBase( QString classcode, QString title, QUuid uuid, QObject *parent ): QObject(parent),
       _uuid(uuid), _title(title), _classCode(classcode), _name(""), _spline(false)
{}

void SignBase::setName(const QString& name) {
    _name = name;
}

// Метод для сериализации объекта в JSON
QJsonObject SignBase::serialize() const {
    QJsonObject json;
    json["uuid"] = _uuid.toString(); // Преобразуем QUuid в строку
    json["class_code"] = _classCode;
    json["title"] = _title;
    json["name"] = _name;
    json["base_color"] = _base_color;

    // Сериализуем QMap<int, QString> в QJsonObject
    QJsonObject characteristicsJson;
    for (auto it = _characteristics.begin(); it != _characteristics.end(); ++it) {
        characteristicsJson[QString::number(it.key())] = it.value();
    }
    json["characteristics"] = characteristicsJson;

    return json;
}

// Статический метод для десериализации JSON в объект
SignBase *SignBase::deserialize(const QJsonObject& obj) {
    // Извлекаем значения из JSON
    QString uuidStr = obj["uuid"].toString();
    QString classcode = obj["class_code"].toString();
    QString title = obj["title"].toString();
    QString name = obj["name"].toString();
    QString base_color = obj["base_color"].toString();

    // Создаем объект SignBase
    auto *sign = new SignBase(classcode, title, QUuid(uuidStr), nullptr);
    sign->setBaseColor(base_color);
    sign->setName(name);

    // Десериализуем характеристики из QJsonObject в QMap<int, QString>
    QJsonObject characteristicsJson = obj["characteristics"].toObject();
    for (auto it = characteristicsJson.begin(); it != characteristicsJson.end(); ++it) {
        sign->_characteristics[it.key().toInt()] = it.value().toString();
    }

    return sign;
}


// Метод для вывода информации (для тестирования)
void SignBase::printInfo() const {
    qDebug() << "UUID:" << _uuid.toString();
    qDebug() << "Class_code:" << _classCode;
    qDebug() << "title:" << _title;
}

void SignBase::setCharacteristics(QMap<int, QString> value){
    _characteristics.insert(value);
};


void SignBase::setOwn(){
    _base_color = QString("%1").arg(ESignColorStates::RED);
    setNationality( QString("%1").arg(ENationalityStates::OWN));
    setColorRGB( QString("%1").arg(ESignColorStates::RED));
    setParamColorSign( QString("%1").arg(EParamSignColorStates::RED) );
}


void SignBase::setEnemy(){
    _base_color = QString("%1").arg(ESignColorStates::BLUE);
    setNationality( QString("%1").arg(ENationalityStates::ENEMY));
    setColorRGB( QString("%1").arg(ESignColorStates::BLUE) );
    setParamColorSign(QString("%1").arg(EParamSignColorStates::BLUE));
}

void SignBase::setHighlight(bool value){
    _is_highlighted = value;
    if (value) {
        setColorRGB( QString("%1").arg(ESignColorStates::YELLOW));
    } else {
       setColorRGB(_base_color);
       setParamColorSign(_base_color);
    }
}

void SignBase::setCoordinatesInDegrees(QList<QPointF> coord, double courseDeg) {
    if (coord.size() == 0) {
        _coordinates = {};
        return;
    }

    if (courseDeg==-1) {
        coord = GeoUtil::toRadians(coord);
        setCoordinatesInRadians(coord);
        return;
    }

    if (_classCode == "1314050003300000") {
        QList<QPointF> points;// это для рубежа пуска ракет чтоб она была длиннее
        points.append( GeoUtil::calculateNewPoint(coord[0].x(), coord[0].y(), 2000, courseDeg-90));
        points.append( QPointF(coord[0].x(), coord[0].y()) );
        points.append(GeoUtil::calculateNewPoint(coord[0].x(), coord[0].y(), 2000, courseDeg+90));
        coord = points;
    } else {
        if (_geometryType == LOCAL_POINT) {
            QList<QPointF> points;
            points.append(QPointF(coord[0].x(), coord[0].y()));
            points.append(GeoUtil::calculateNewPoint(coord[0].x(), coord[0].y(), 1000, courseDeg));
            coord = points;
        }
    }

    setCoordinatesInRadians(GeoUtil::toRadians(coord));

}


void SignBase::setCoordinatesInRadians(QList<QPointF> coord, bool emit_signals) {

    if (!this) {
        return;
    }
    if (coord.size() == 0) {
        _coordinates = {};
        return;
    }

    for(const auto c: coord)
        if (c.x()>1.7 || c.y() > 3.15) {
            qLog() << "Координаты указаны не в радианах";
            std::abort();
        }

    if (_geometryType == SignType::LOCAL_POINT) {
        auto angle = GeoUtil::calculateAngleRad(coord.first(), coord.last());
        angle = angle * (180.0 / M_PI);
        if (emit_signals)  emit courseChanged(angle);
    }
    _coordinates = coord;
    if (!emit_signals) return;

    auto _tmp = GeoUtil::toDegrees(coord);
    if (_geometryType == SignType::LOCAL_POINT)  emit positionChanged(_tmp[0]);
    else emit coordinateChanged(_tmp);

}




QJsonObject SignBase::getJsonFeature(QString objID) {
    QJsonObject feature;
    QJsonObject properties;
    QJsonObject geometry;
    QJsonArray coordArray;


    // Преобразуем координаты в градусы (предполагается, что _coordinates - массив точек)

        for (const auto& point : _coordinates) {
            QJsonArray pointArray;
                pointArray.append(GeoUtil::toDegrees(point.x())); // Долгота (lat)
                pointArray.append(GeoUtil::toDegrees(point.y())); // Широта (lon)
                pointArray.append(0); // Широта (alt)
            coordArray.append(pointArray);
        }


    // Заполняем properties
    properties["id"] = objID;
    properties["class_code"] = _classCode;
    properties["name"] = _name;

    // Определяем тип геометрии
    geometry["type"] = (_geometryType == SignType::LOCAL_LINE) ? "LineString" : "Polygon";

    // Для Polygon нужно обернуть координаты в дополнительный массив
    if (_geometryType != SignType::LOCAL_LINE) {
        QJsonArray polygonArray;
        polygonArray.append(coordArray);
        geometry["coordinates"] = polygonArray;
    } else {
        geometry["coordinates"] = coordArray;
    }

    // Формируем итоговый объект Feature
    feature["type"] = "Feature";
    feature["properties"] = properties;
    feature["geometry"] = geometry;

    return feature;
}


// Добавление дочернего элемента в карту по идентификатору
void SignBase::addChild(QString id, SignBase *sign) {
    if (!sign) {
        qWarning() << "Attempted to add nullptr as child with id:" << id;
        return;
    }

    // Проверка на существование элемента с таким id
    if (m_children.contains(id)) {
        qWarning() << "Child with id" << id << "already exists. Overwriting.";
    }

    // Добавляем или перезаписываем элемент в карту
    m_children[id] = sign;
}

// Получение дочернего элемента по идентификатору
SignBase *SignBase::getChild(const QString& _id) {
    // Проверка на существование элемента с таким id
    if (!m_children.contains(_id)) {
        qWarning() << "Child with id" << _id << "not found. Returning default SignBase.";
        return nullptr; // Возвращаем пустой объект SignBase
    }

    // Возвращаем копию элемента
    return m_children[_id];
}

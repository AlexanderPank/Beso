#ifndef SIGNBASE_H
#define SIGNBASE_H

#define NOMINMAX
#include <QObject>
#include <QMap>
#include <QList>
#include <QPointF>
#include <QUuid>
#include <QString>
#include <QJsonObject>
#include <maptype.h>
#include "sign_enums.h"
#include "GeoUtil.h"

/**
 * @brief Класс SignBase представляет собой базовый объект знака.
 */
class SignBase: public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор SignBase.
     * @param classcode Код класса знака.
     * @param title Название знака.
     * @param uuid Уникальный идентификатор (по умолчанию создаётся новый).
     * @param parent Родительский объект.
     */
    SignBase(QString classcode, QString title, QUuid uuid = QUuid::createUuid(), QObject *parent = nullptr);

    /**
     * @brief Получить UUID знака.
     * @return Уникальный идентификатор (UUID).
     */
    QUuid getUuid() { return _uuid; }

    /**
     * @brief Получить заголовок знака.
     * @return Заголовок знака.
     */
    QString getTitle() { return _title; }

    /**
     * @brief Получить имя знака.
     * @return Имя знака.
     */
    QString getName() { return _name; }

    /**
     * @brief Установить имя знака.
     * @param name Новое имя знака.
     */
    void setName(const QString& name);

    /**
     * @brief Получить код класса знака.
     * @return Код класса знака.
     */
    QString getClasscode() { return _classCode; }

    /**
     * @brief Установить характеристики знака.
     * @param c QMap с характеристиками (ключ - идентификатор, значение - строка).
     */
    void setCharacteristics(QMap<int, QString> c);

    /**
     * @brief Получить характеристики знака.
     * @return QMap<int, QString> с характеристиками.
     */
    QMap<int, QString> getCharacteristics() { return _characteristics; }

    /**
     * @brief Установить видимость знака.
     * @param v true, если знак видим, иначе false.
     */
    void setVisibility(bool v) { m_visibility = v; }
    bool getVisibility() const { return m_visibility; }

    /**
     * @brief Установить координаты знака.
     * @param coord Список координат (QList<QPointF>).
     */
    void setCoordinatesInRadians(QList<QPointF> coord, bool emit_signals=true);
    void setCoordinatesInDegrees(QList<QPointF> coord, double course = -1);
    void appendMultiCoordinatesInRadians(QList<QPointF> coord);


    void setMultyCoordinatesInDegrees(QList<QList<QPointF>> coordList);
    void setMultyCoordinatesInRadians(QList<QList<QPointF>> coordList);



    /**
     * @brief Получить координаты знака.
     * @return Список координат (QList<QPointF>).
     */
    QList<QPointF> getCoordinatesInRadians() { return _coordinates; }
    QList<QPointF> getCoordinatesInDegrees() { return GeoUtil::toDegrees(_coordinates);}

    /**
     * @brief Использует ли знак сплайн?
     * @return true, если используется сплайн, иначе false.
     */
    bool useSpline() { return _spline; }
    void setSpline(bool v) { _spline=v; }

    /**
     * @brief Вывод информации о знаке в консоль.
     */
    void printInfo() const;

    /**
     * @brief Сериализует объект в JSON.
     * @return QJsonObject, содержащий данные объекта.
     */
    QJsonObject serialize() const;

    /**
     * @brief Десериализует объект из JSON.
     * @param obj JSON-объект с данными знака.
     * @return Указатель на созданный объект SignBase.
     */
    static SignBase *deserialize(const QJsonObject& obj);


    /**
     * @brief Установить характеристику "госпринадлежность".
     * @param value Значение характеристики.
     */
    void setNationality(const QString& value) {
        _characteristics[ESignCharacteristic::NATIONALITY] = value;
    }

    /**
     * @brief Получить характеристику "госпринадлежность".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getNationality() const {
        return _characteristics.value(ESignCharacteristic::NATIONALITY, "");
    }
/**
     * @brief Установить признак объекта уничтожен, разрушен, поврежд
     * @param value Значение характеристики.
     */
    void setParamState(const QString& value) {
        _characteristics[ESignCharacteristic::PARAM_STATE] = value;
    }

    /**
     * @brief Установить характеристику "характер действия".
     * @param value Значение характеристики.
     */
    void setParamAction(const QString& value) {
        _characteristics[ESignCharacteristic::PARAM_ACTION] = value;
    }

    /**
     * @brief Получить характеристику "характер действия".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getParamAction() const {
        return _characteristics.value(ESignCharacteristic::PARAM_ACTION, "");
    }

    /**
     * @brief Установить характеристику "цвет условного знака".
     * @param value Значение характеристики.
     */
    void setParamColorSign(const QString& value) {
        _characteristics[ESignCharacteristic::PARAM_COLOR_SIGN] = value;
    }

    /**
     * @brief Получить характеристику "цвет условного знака".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getParamColorSign() const {
        return _characteristics.value(ESignCharacteristic::PARAM_COLOR_SIGN, "");
    }

    /**
     * @brief Установить характеристику "подпись знака".
     * @param value Значение характеристики.
     */
    void setText(const QString& value) {
        _characteristics[ESignCharacteristic::TEXT] = value;
    }

    /**
     * @brief Получить характеристику "подпись знака".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getText() const {
        return _characteristics.value(ESignCharacteristic::TEXT, "");
    }

    /**
     * @brief Установить характеристику "фон подписи".
     * @param value Значение характеристики.
     */
    void setTextBackground(const QString& value) {
        _characteristics[ESignCharacteristic::TEXT_BACKGROUND] = value;
    }

    /**
     * @brief Получить характеристику "фон подписи".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getTextBackground() const {
        return _characteristics.value(ESignCharacteristic::TEXT_BACKGROUND, "");
    }

    /**
     * @brief Установить характеристику "наклон текста".
     * @param value Значение характеристики.
     */
    void setTextStyle(const QString& value) {
        _characteristics[ESignCharacteristic::TEXT_STYLE] = value;
    }

    /**
     * @brief Получить характеристику "наклон текста".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getTextStyle() const {
        return _characteristics.value(ESignCharacteristic::TEXT_STYLE, "");
    }

    /**
     * @brief Установить характеристику "масштаб отображения".
     * @param value Значение характеристики.
     */
    void setScale(const QString& value) {
        _characteristics[ESignCharacteristic::SCALE] = value;
    }

    /**
     * @brief Получить характеристику "масштаб отображения".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getScale() const {
        return _characteristics.value(ESignCharacteristic::SCALE, "");
    }

    /**
     * @brief Установить характеристику "цвет отображения в RGB".
     * @param value Значение характеристики.
     */
    void setColorRGB(const QString& value) {
        _characteristics[ESignCharacteristic::COLOR_RGB] = value;
    }

    /**
     * @brief Получить характеристику "цвет отображения в RGB".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getColorRGB() const {
        return _characteristics.value(ESignCharacteristic::COLOR_RGB, "");
    }

    /**
     * @brief Установить характеристику "высота шрифта".
     * @param value Значение характеристики.
     */
    void setTextHeight(const QString& value) {
        _characteristics[ESignCharacteristic::TEXT_HEIGHT] = value;
    }

    /**
     * @brief Получить характеристику "высота шрифта".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getTextHeight() const {
        return _characteristics.value(ESignCharacteristic::TEXT_HEIGHT, "");
    }

    /**
     * @brief Установить характеристику "толщина линии".
     * @param value Значение характеристики.
     */
    void setLineWidth(const QString& value) {
        _characteristics[ESignCharacteristic::LINE_WIDTH] = value;
    }

    /**
     * @brief Получить характеристику "толщина линии".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getLineWidth() const {
        return _characteristics.value(ESignCharacteristic::LINE_WIDTH, "");
    }

    /**
     * @brief Установить характеристику "толщина окончания линии".
     * @param value Значение характеристики.
     */
    void setLineEndWidth(const QString& value) {
        _characteristics[ESignCharacteristic::LINE_END_WIDTH] = value;
    }

    /**
     * @brief Получить характеристику "толщина окончания линии".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getLineEndWidth() const {
        return _characteristics.value(ESignCharacteristic::LINE_END_WIDTH, "");
    }

    /**
     * @brief Установить характеристику "цвет отображения полигона".
     * @param value Значение характеристики.
     */
    void setPolygonColor(const QString& value) {
        _characteristics[ESignCharacteristic::POLYGON_COLOR] = value;
    }

    /**
     * @brief Получить характеристику "цвет отображения полигона".
     * @return Значение характеристики или пустая строка, если значение отсутствует.
     */
    QString getPolygonColor() const {
        return _characteristics.value(ESignCharacteristic::POLYGON_COLOR, "");
    }

    enum SignType{
        NONE = 0,
        LOCAL_POINT = 1,
        LOCAL_LINE = 2,
        LOCAL_POLYGON = 3,
        LOCAL_CIRCLE = 4,
        LOCAL_RECTANGLE = 5,
        LOCAL_TITLE = 6,
        LOCAL_SQUARE = 7
    };

    /**
    * @brief Установить характеристику "геометрия объекта".
    */
    void setGeometryType(SignType v){ _geometryType =v; }
    /**
     * @brief Получить характеристику "геометрия объекта".
     * @return Значение характеристики .
     */
    SignType getGeometryType(){ return  _geometryType; }

    void setHighlight(bool value);
    bool getHighlight() const {return _is_highlighted;};

    void setOwn();
    void setEnemy();

    /**
     * @brief Устанавливает базовый цвет знака, если не используется setOwn или setEnemy.
     */
    void setBaseColor(QString value){ _base_color = value;}

    void setTitle(QString value){_title = value;}

    QJsonObject getJsonFeature(QString objID);

    /**
    * @brief Устанавливает идентификатор объекта для гис системы (SignController->signs).
    */
    void setGisID(int gisObjectUid){ _gis_id = gisObjectUid;}
    int getGisID() const {return _gis_id;}

    void addChild(QString id, SignBase *sign);
    SignBase *getChild(const QString& id);

signals:
    void positionChanged(const QPointF &newPos);
    void coordinateChanged(const QList<QPointF> &newCoordinates);
    void courseChanged(const double &value);

protected:
    QUuid _uuid; ///< Уникальный идентификатор знака.
    QString _title; ///< Заголовок знака.
    QString _classCode; ///< Код класса знака.
    QString _name; ///< Имя знака.

private:
    QMap<int, QString> _characteristics; ///< Характеристики знака.
    QList<QPointF> _coordinates; ///< Координаты знака в радианах
    bool _spline; ///< Флаг использования сплайна.
    bool _is_highlighted = false; ///< Флаг что знак подсвечен.
    QString _base_color = ""; ///< при установлении подсветки на знак, запоминается базовый цвет, для восстановления
    SignType _geometryType = SignType::LOCAL_POINT; //< тип геометрии
    int _gis_id = -1; //< идентификатор объекта в гис системе на карте, для быстрого поиска
    QMap<QString, SignBase*> m_children;
    bool m_visibility = true;

};

#endif // SIGNBASE_H

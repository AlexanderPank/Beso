//
// Created by qq on 29.04.25.
//

#ifndef SCENARIOCLIENT_FEATUREMODEL_H
#define SCENARIOCLIENT_FEATUREMODEL_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QTreeWidgetItem>
#include <QPushButton>
#include "PropertyModel.h"

class FeatureModel: public QObject {
Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle )
    Q_PROPERTY(QString type READ getType WRITE setType)
    Q_PROPERTY(QString parent_id READ getParentID WRITE setParentID)
    Q_PROPERTY(QString class_code READ getClassCode WRITE setClassCode )
    Q_PROPERTY(float line_width READ getLineWidth WRITE setLineWidth)
    Q_PROPERTY(float scale READ getScale WRITE setScale )
    Q_PROPERTY(bool spline READ getSpline WRITE setSpline)
    Q_PROPERTY(bool is_own READ getOwn WRITE setOwn)
    Q_PROPERTY(QString color READ getColor WRITE setColor)
    Q_PROPERTY(QList<QPointF> coordinates READ getCoordinates WRITE setCoordinates)
    Q_PROPERTY(GeometryType geometry_type READ getGeometryType WRITE setGeometryType)
    Q_PROPERTY(QString icon READ getIcon WRITE setIcon)
    Q_PROPERTY(QString scheme READ getScheme WRITE setScheme)
    Q_PROPERTY(QString short_name READ getShortName WRITE setShortName)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription)
    Q_PROPERTY(QString stringCoordinates READ getStringCoordinates WRITE setStringCoordinates)
    Q_PROPERTY(QString text READ getText WRITE setText)
    Q_PROPERTY(QString radius READ getRadius WRITE setRadius)
    Q_PROPERTY(QString font_size READ getFontSize WRITE setFontSize)

signals:
    void coordinatesChanged(const QList<QPointF> &points);
    void centerOnMap(double lat, double lon);
    void createOnMap(QString id);
    void stringCoordinatesChanged(QString coordinates);
    void redrawFeature(FeatureModel*);

public:
    enum GeometryType{
        UNKNOWN = 0,
        POINT = 1,
        LINE = 2,
        POLYGON = 3,
        CIRCLE = 4,
        RECTANGLE = 5,
        TITLE = 6,
        SQUARE = 7
    };

    enum PositionType{
        DEFAULT = 0,
        ALWAYS_TOP = 1,
        ALWAYS_BOTTOM = 2
    };

    explicit FeatureModel(QObject *parent = nullptr) : QObject(parent) {}
    ~FeatureModel();

    // Getters
    QString getId() const { return m_id; }
    QString getTitle() const { return m_title; }
    QString getType() const { return m_type; }
    QString getParentID() const { return m_parent_id; }
    QString getClassCode() const { return m_class_code; }
    float getLineWidth() const { return m_line_width; }
    float getScale() const { return m_scale; }
    bool getSpline() const { return m_spline; }
    QString getColor() const { return m_color; }
    QList<QPointF> getCoordinates() const { return m_coordinates; }
    GeometryType getGeometryType() const { return m_geometry_type; }
    QString getIcon() const { return m_icon; }
    QString getScheme() const { return m_scheme; }
    QString getShortName() const { return m_short_name; }
    QString getDescription() const { return m_description; }
    int getOwn() const { return m_own;}
    QString getStringCoordinates() const { return m_string_coordinates; };
    QString getText() const { return m_text; };
    float getRadius() const ;
    float getFontSize() const { return m_font_size; };
    PositionType getPosition() const { return m_position; };
    QVariant getProperty(QString name);
    int getPropertyInt(QString name, int def=-1);
    float getPropertyFloat(QString name, float def=-1.0f);


    // Setters
    void setId(const QString &id) { m_id = id;}
    void setTitle(const QString &title) { m_title = title;}
    void setType(const QString &type) {m_type = type;}
    void setParentID(const QString &parent_id) { m_parent_id = parent_id;}
    void setClassCode(const QString &class_code) { m_class_code = class_code;}
    void setLineWidth(const float &line_width);
    void setScale(const float &scale) ;
    void setSpline(const bool &spline) ;
    void setColor(const QString &color);
    void setCoordinates(QList<QPointF> coordinates) ;
    void setGeometryType(const GeometryType v) { m_geometry_type = v; }
    void setIcon(const QString &icon) { m_icon = icon;}
    void setScheme(const QString &scheme) { m_scheme = scheme;}
    void setShortName(const QString &short_name) {  m_short_name = short_name;}
    void setDescription(const QString &description) { m_description = description; }
    void setOwn(int global) { m_own = global;}
    void setStringCoordinates(const QString stringCoordinates) { m_string_coordinates = stringCoordinates; }
    void setText(const QString &text) { m_text = text;}
    void setRadius(const QString &radius) { m_radius = radius.toFloat(); }
    void setRadius(const float &radius) { m_radius = radius; }
    void setFontSize(const QString &font_size) { m_font_size = font_size.toDouble(); }
    void setFontSize(const float &font_size) { m_font_size = font_size; }
    void setPosition(const int &position);
    void setProperties(const QMap<QString, PropertyModel*> props) { qDeleteAll(m_properties); m_properties.clear(); m_properties = props; }
    void setProperty(QString name, QVariant val);



    static FeatureModel* fromJson(const QJsonObject &json, QObject* parent);
    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem *parent);
    QString getGeometryTypeString();

    QJsonObject toJson() const;

private:
    // Приватные члены класса
    QString m_id;
    QString m_title;
    QString m_type;
    QString m_parent_id;
    QString m_class_code;
    float m_line_width;
    float m_scale;
    bool m_spline;
    QString m_color;
    QList<QPointF> m_coordinates;
    GeometryType m_geometry_type;
    QString m_icon;
    QString m_scheme;
    QString m_short_name;
    QString m_description;
    QString m_string_coordinates;
    QString m_text = "";
    float m_radius = 0;
    float m_font_size = 12;
    int m_own = 1;
    PositionType m_position = PositionType::DEFAULT; // Позиция по отношению к другим объектам 1-всегда наверху, 2-всегда внизу, 0-по порядку
    bool m_treeItemEditMode = false;

    QTreeWidgetItem* m_tree_widget_item = nullptr;
    QTreeWidgetItem* m_property_item = nullptr;
    QTreeWidgetItem* m_coordinates_item = nullptr;
    QPushButton *btnMapAction;
    QMap<QString, PropertyModel*> m_properties;


    void addSearchButton();
};

#endif //SCENARIOCLIENT_FEATUREMODEL_H
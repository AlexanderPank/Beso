#ifndef SCENARIOCLIENT_TACTICALSIGNMODEL_H
#define SCENARIOCLIENT_TACTICALSIGNMODEL_H

#include <QObject>
#include <QIcon>
#include <QPixmap>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QJsonArray>
#include "PropertyModel.h"

class TacticalSignModel : public QObject
{
    Q_OBJECT
public:
    explicit TacticalSignModel(QObject *parent = nullptr);
    ~TacticalSignModel();

    //Setter
    void setId(const QString &id);
    void setIconPath(const QString &iconPath);
    void setTitle(const QString &title);
    void setClassCod(const QString &classCod);
    void setGroup(const QString &group);
    void setDescription(const QString &description);
    void setShortName(const QString &shortName);
    void setSchemePath(const QString &schemePath);
    void setParentObjectId(const QString parentObjectId);
    void setData(const QJsonObject data);

    //Getter
    QString getId() const;
    QString getIconPath() const;
    QString getTitle() const;
    QString getClassCod() const;
    QString getGroup() const;
    QString getDescription() const;
    QString getShortName() const;
    QString getSchemePath() const;
    QPixmap getIcon() const;
    QPixmap getSchemeIcon() const;
    QString getParentObjectId() const;
    QJsonObject getData() const;

    static TacticalSignModel* fromJson(const QJsonObject &json, QObject* parent);
    QJsonObject toJson() const;

    QListWidgetItem* getListWidgetItem(QListWidget* parent);

    const QJsonArray coordinates() const;
    void setCoordinates(const QJsonArray newCoordinates);

    const QString &type() const;
    void setType(const QString &newType);

    int is_own() const;
    void setIs_own(int newIs_own);

    double line_width() const;
    void setLine_width(double newLine_width);

    int scale() const;
    void setScale(int newScale);

    bool spline() const;
    void setSpline(bool newSpline);

    const QString &color() const;
    void setColor(const QString &newColor);

    int geometry_type() const;
    void setGeometry_type(int newGeometry_type);

    const QMap<QString, PropertyModel *> &properties() const;
    void setProperties(const QMap<QString, PropertyModel *> &newProperties);

    void removePropertys();
    void addProperty(QString p, PropertyModel *pm);

private:
    QString m_id;
    QString m_iconPath;
    QString m_classCod;
    QString m_group;
    QString m_title;
    QString m_description;
    QString m_shortName;
    QString m_schemePath;
    QString m_parentObjectId;

    int     m_geometry_type;
    QString m_color;
    bool    m_spline;
    int     m_scale;
    double  m_line_width;
    int     m_is_own;
    QString m_type;
    QJsonArray m_coordinates;

    QJsonObject m_data;

    QMap<QString, PropertyModel*> m_properties;
};


#endif //SCENARIOCLIENT_TACTICALSIGNMODEL_H

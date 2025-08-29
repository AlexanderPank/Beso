#ifndef OBJECTSCENARIOMODEL_H
#define OBJECTSCENARIOMODEL_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QJsonObject>
#include <QPushButton>
#include <QTreeWidgetItem>
#include "PropertyModel.h"
#include "ActionModel.h"
#include "Feature.h"
#include "FeatureModel.h"

class ObjectScenarioModel : public QObject
{
Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)

public:
    explicit ObjectScenarioModel(QObject *parent = nullptr);
    ~ObjectScenarioModel();

    // Геттеры
    QString getId() const;
    QString getTitle() const;
    QMap<QString, PropertyModel*> properties() const;
    QList<ActionModel*> actions() const;
    QList<FeatureModel*> features() const{ return m_features;};

    // Сеттеры
    void setId(const QString &id);
    void setTitle(const QString &title);
    bool setPropertyValue(QString key, double value);
    bool setPropertyValue(QString key, QString value);
    bool setPropertyValue(QString key, QJsonObject value);
    bool setPropertyValue(QString key, QVariant value);

    void setProperties(const QMap<QString, PropertyModel*> &properties);
    void setActions(const QList<ActionModel*> &actions);
    void setFeatures(const QList<FeatureModel*> &features);

    // Управление свойствами
    void addProperty(const QString &key, PropertyModel *property);
    PropertyModel* getProperty(const QString &key) const;
    QString getPropertyString(const QString &key) const;
    double getPropertyDouble(const QString &key, bool& ok) const;
    int getPropertyInt(const QString &key, bool& ok) const;
    bool getPropertyBool(const QString &key) const;
    Feature* getPropertyFeature(const QString &key, bool& ok) const;

    bool containsProperty(const QString &key) const;

    void removeProperty(const QString &key);

    // Управление действиями
    void addAction(ActionModel *action);
    void removeAction(ActionModel *action);
    void clearActions();

    // JSON методы
    QJsonObject toJson() const;
    static ObjectScenarioModel* fromJson(const QJsonObject &json, QObject* parent);

    // Метод для дерева
    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem *parent = nullptr, bool addChilds = true);
    QStringList getPropertiesByType(const QString &propType) const;

signals:
    void idChanged();
    void titleChanged();
    void propertiesChanged();
    void actionsChanged();

    void centerOnMap(double &lat, double &lon);
    void createOnMap(QString id);
    void createFeatureOnMap(QString objectId, QString classCode, QString featureId, bool isMulti);

private:
    QString m_id;
    QString m_title;
    QMap<QString, PropertyModel*> m_properties;
    QList<ActionModel*> m_actions;
    QTreeWidgetItem *treeWidgetItem= nullptr;
    QTreeWidgetItem *propItem = nullptr;
    QTreeWidgetItem *actionsItem = nullptr;
    QPushButton *btnMapAction = nullptr;
    QList<FeatureModel*> m_features;


    void addSearchButton(); // добавить кнопку поиска на карте
};

#endif // OBJECTSCENARIOMODEL_H

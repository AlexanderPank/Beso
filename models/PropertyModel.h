#ifndef PROPERTYMODEL_H
#define PROPERTYMODEL_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QTreeWidgetItem>
#include <QPushButton>

class PropertyModel : public QObject
{
Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit PropertyModel(QObject *parent = nullptr);
    explicit PropertyModel(const QString &name, const QString &type, const QJsonValue &jsonValue, QObject *parent = nullptr);
    explicit PropertyModel(const QString &name, const QString &type, const QJsonValue &jsonValue, const QString &title, QObject *parent);
    explicit PropertyModel(const QString &name, const QString &type, const QVariant &value, QObject *parent = nullptr);
    explicit PropertyModel(const QString &name, const QString &type, const QVariant &value, const QString &title, QObject *parent = nullptr);

    class ValueType {
        public:
        static const QString FEATURE;
        static const QString BOOL;
        static const QString FLOAT;
        static const QString DOUBLE;
        static const QString INT;
        static const QString STRING;
    };
    // Геттеры
    QString name() const;
    QVariant value() const;
    QString stringValue() const;
    QString type() const;
    QString title() const;

    // Сеттеры
    void setName(const QString &name);
    void setValue(const QVariant &value, bool updateTree = true);
    void setType(const QString &type);
    void setTitle(const QString &newTitle);

    // JSON-сериализация
    QJsonObject toJson() const;
    static PropertyModel* fromJson(const QJsonObject &json, QObject *parent);

    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem* parent = nullptr);

    static QString getJsonValueType(const QJsonValue &jsonValue) ;

public slots:
    void updateFromTreeItem(QTreeWidgetItem *item);

signals:
    void nameChanged();
    void valueChanged();
    void typeChanged();
    //void valueChanged(const QVariant &newValue);

    void centerFeatureOnMap(double lat, double lon);
    void createFeatureOnMap(QString class_code, QString featureId, bool isMulti);

private:
    QString m_name;
    QVariant m_value;
    QString m_type;
    QString m_title;

    QString variantToString(const QVariant& value) const;
    QTreeWidgetItem* currentTreeItem = nullptr;

    void addSearchButton();
    QPushButton *btnMapAction;

    void setValueByType(const QVariant &value);
    bool m_treeItemEditMode = false;
};



#endif // PROPERTYMODEL_H

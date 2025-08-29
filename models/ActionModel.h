#ifndef ACTIONMODEL_H
#define ACTIONMODEL_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QJsonObject>
#include "PropertyModel.h"
#include "AlgorithParamModel.h"

class ActionModel : public QObject
{
Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString algorithmId READ algorithmId WRITE setAlgorithmId NOTIFY algorithmIdChanged)

public:
    explicit ActionModel(QObject *parent = nullptr);
    ~ActionModel();

    // Геттеры
    QString name() const;
    QString type() const;
    QString algorithmId() const;
    QList<AlgorithParamModel*> outputMapping() const;

    // Сеттеры
    void setName(const QString &name);
    void setType(const QString &type);
    void setAlgorithmId(const QString &algorithmId);
    void setOutputMapping(const QList<AlgorithParamModel*>  &outputMapping);

    // Управление outputMapping
    void addOutputMapping(const QString &name, const QString &value, const QString &title);
    void removeOutputMapping(const QString &key);

    // JSON методы
    QJsonObject toJson() const;
    static ActionModel* fromJson(const QJsonObject &json, QObject *parent);

    // Метод для дерева
    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem *parent = nullptr);

signals:
    void nameChanged();
    void typeChanged();
    void algorithmIdChanged();
    void outputMappingChanged();

private:
    QString m_name;
    QString m_type;
    QString m_algorithmId;
    QList<AlgorithParamModel*> m_outputMapping;
    QTreeWidgetItem *treeWidgetItem= nullptr;
};

#endif // ACTIONMODEL_H
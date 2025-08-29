//
// Created by qq on 07.04.25.
//

#ifndef SCENARIOCLIENTTESTS_ALGORITHPARAMMODEL_H
#define SCENARIOCLIENTTESTS_ALGORITHPARAMMODEL_H


#include <QObject>
#include <QString>
#include <QTreeWidgetItem>
#include <QJsonObject>

class AlgorithParamModel: public QObject
{
    Q_OBJECT
public:
    explicit AlgorithParamModel(QObject *parent): QObject(parent){};
    explicit AlgorithParamModel(const QString& name,const QString& value, const QString&  title,  QObject *parent):
                                QObject(parent), m_name(name), m_value(value), m_title(title){};

    // Геттеры
    QString name() const {return m_name;};
    QVariant value() const {return m_value;};
    QString value_string() const {return m_value;};
    QVariant title() const {return m_title;};

    // Сеттеры
    void setName(const QString &name);
    void setValue(const QString &value);
    void setTitle(const QString &value);

    // JSON-сериализация
    QJsonObject toJson() const;
    static AlgorithParamModel* fromJson(const QJsonObject &json, QObject *parent);

    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem* parent = nullptr) const;

public slots:
    void updateFromTreeItem(QTreeWidgetItem *item);

signals:
    void nameChanged();
    void titleChanged();
    void valueChanged(const QVariant &newValue);

private:
    QString m_name;
    QString m_value;
    QString m_title;

};


#endif //SCENARIOCLIENTTESTS_ALGORITHPARAMMODEL_H

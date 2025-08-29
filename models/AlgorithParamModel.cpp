//
// Created by qq on 07.04.25.
//

#include "AlgorithParamModel.h"

QJsonObject AlgorithParamModel::toJson() const{
    QJsonObject json;
    json[m_name] = m_value;
    json["title"] = m_title;
    return json;
}

AlgorithParamModel* AlgorithParamModel::fromJson(const QJsonObject &json, QObject *parent){
    if (json.isEmpty()) return nullptr;

    // Предполагаем, что json содержит один ключ-значение
    QString key = json.keys().first() != "title" ? json.keys().first() : json.keys()[1] ;
    QString value = json.value(key).toString();
    QString title = json.contains("title") ? json.value("title").toString() : "";

    return new AlgorithParamModel(key, value, title, parent);
}

QTreeWidgetItem* AlgorithParamModel::getTreeWidgetItem(QTreeWidgetItem* parent) const
{
    auto *item = new QTreeWidgetItem(parent);
    item->setText(0, m_name);
    item->setText(1, m_value);
    item->setData(3, Qt::UserRole, QVariant::fromValue(const_cast<AlgorithParamModel*>(this)));

    // Разрешаем редактирование
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    // Tooltip с дополнительной информацией
    item->setToolTip(0, QString(m_title));
    item->setToolTip(1, QString(m_title));

    return item;
}

void AlgorithParamModel::updateFromTreeItem(QTreeWidgetItem *item)
{
    if (item) {
        QString newValue = item->text(1);
        if (m_value != newValue) {
            setValue(newValue);
        }

    }
}


// Сеттеры
void AlgorithParamModel::setName(const QString &name)
{
    if (m_name == name) return;
    m_name = name;
    emit nameChanged();
}
// Сеттеры
void AlgorithParamModel::setTitle(const QString &title)
{
    if (m_title == title) return;
    m_title = title;
    emit titleChanged();
}

void AlgorithParamModel::setValue(const QString &value)
{
    if (m_value == value) return;
    m_value = value;
    emit valueChanged(value);
}

//
// Created by qq on 13.04.25.
//

#include "DataStorageItem.h"
DataStorageItem::DataStorageItem(QObject *parent) : QObject(parent)
{
    m_modify = false;
}

// Деструктор
DataStorageItem::~DataStorageItem()
{
}

void DataStorageItem::setTitle(const QString &title){
    if(m_title != title) {
        this->m_title = title;
        emit titleChanged();
    }
}

QString DataStorageItem::getTitle() const{
    return m_title;
}

void DataStorageItem::setId(const QString &id){
    if(m_id != id) {
        this->m_id = id;
        emit idChanged();
    }

}

QString DataStorageItem::getId() const{
    return m_id;
}

void DataStorageItem::setData(const QJsonObject &data){
    if(m_data != data) {
        this->m_data = data;
        emit dataChanged();
    }
}

QJsonObject DataStorageItem::getData() const{
    return m_data;
}

DataStorageItem *DataStorageItem::fromJson(const QJsonObject &json, const QString &absolutePath, QObject *parent) {
    auto *dataStorageItem = new DataStorageItem(parent);
    dataStorageItem->setId(json["id"].toString());
    dataStorageItem->setTitle(json["title"].toString());
    dataStorageItem->setAbsolutePath(absolutePath);
    dataStorageItem->setData(json);

    return dataStorageItem;
}

void DataStorageItem::setAbsolutePath(const QString &absolutePath) {
    if(m_absolutePath != absolutePath) {
        this->m_absolutePath = absolutePath;
        emit absolutePathChanged();
    }
}

QString DataStorageItem::getAbsolutePath() const {
    return m_absolutePath;
}

void DataStorageItem::setModify(const bool &modify) {
    m_modify = modify;
}

bool DataStorageItem::isModify() const {
    return m_modify;
}

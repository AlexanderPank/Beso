#include "DataStorageService.h"

DataStorageService::DataStorageService(QObject *parent): QObject(parent) {
}

DataStorageService::~DataStorageService()= default;

QList<DataStorageItem *> DataStorageService::getElements(const TypeElement &typeElement) {
    return m_elements[typeElement];
}

DataStorageItem *DataStorageService::findElement(const TypeElement &typeElement, const QString &id) {
    for(DataStorageItem* iter: m_elements[typeElement]){
        if(iter->getId() == id)
            return iter;
    }
    return nullptr;
}

#include "DBDataStorageService.h"

#include "../../core/QLogStream.h"

DBDataStorageService::DBDataStorageService(QObject *parent) : DataStorageService(parent) {

}

DBDataStorageService::~DBDataStorageService() {

}

void DBDataStorageService::loadData() {
    qLog() << "DBDataStorageService::loadData()";
}

void DBDataStorageService::addElement(TypeElement &typeElement, QJsonObject &jsonObject) {

}

void DBDataStorageService::updateElement(TypeElement &typeElement, QJsonObject &jsonObject) {

}

void DBDataStorageService::removeElement(TypeElement &typeElement, QJsonObject &jsonObject) {

}

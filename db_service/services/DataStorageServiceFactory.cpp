#include "DataStorageServiceFactory.h"
#include "../../core/EnvConfig.h"

DataStorageService* DataStorageServiceFactory::service = nullptr;

DataStorageService* DataStorageServiceFactory::getInstance() {
    if (!service) {
        if (EnvConfig::getInstance()->get("LOCAL_STORAGE").toInt() == 1) {
            QString pathLocalStorage = EnvConfig::getInstance()->get("PATH_LOCAL_STORAGE");
            service = new FileDataStorageService(pathLocalStorage);
            service->loadData();
        } else {
            service = new DBDataStorageService();
        }
    }

    return service;
}

DataStorageServiceFactory::DataStorageServiceFactory() {
}

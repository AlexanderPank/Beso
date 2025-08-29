#ifndef SCENARIOCLIENT_DATASTORAGESERVICEFACTORY_H
#define SCENARIOCLIENT_DATASTORAGESERVICEFACTORY_H

#include "DataStorageService.h"
#include "FileDataStorageService.h"
#include "DBDataStorageService.h"

class DataStorageServiceFactory {
public:

    static DataStorageService *getInstance();

private:
    DataStorageServiceFactory();
    // Initialize static member
    static DataStorageService *service;
    static bool m_isLocalStorage;
    static QString m_configLine;
};




#endif //SCENARIOCLIENT_DATASTORAGESERVICEFACTORY_H
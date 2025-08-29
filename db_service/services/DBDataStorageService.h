#ifndef SCENARIOCLIENT_DBDATASTORAGESERVICE_H
#define SCENARIOCLIENT_DBDATASTORAGESERVICE_H

#include "DataStorageService.h"

class DBDataStorageService: public DataStorageService {
    Q_OBJECT
public:
    explicit  DBDataStorageService(QObject *parent = nullptr);
    ~DBDataStorageService() override;

    void addElement(TypeElement &typeElement, QJsonObject &jsonObject) override;
    void updateElement(TypeElement &typeElement, QJsonObject &jsonObject) override;
    void removeElement(TypeElement &typeElement, QJsonObject &jsonObject) override;

public slots:
    void loadData() override;

};


#endif //SCENARIOCLIENT_DBDATASTORAGESERVICE_H

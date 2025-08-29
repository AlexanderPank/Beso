#ifndef SCENARIOCLIENT_DATASTORAGESERVICE_H
#define SCENARIOCLIENT_DATASTORAGESERVICE_H

#include <QList>

#include "models/DataStorageItem.h"

enum TypeElement {
    teOBJECT = 0,
    teOBJECT_TEMPLATE = 5,
    teALGORITHM = 1,
    teALGORITHM_TEMPLATE = 6,
    teLIBFILE = 2,
    teLIBFILE_TEMPLATE = 7,
    teTACTICAL_SIGN = 3,
    teTACTICAL_SIGN_TEMPLATE = 8,
    teINTERACTION = 4,
    teINTERACTION_TEMPLATE = 9,
    teDISTRICT = 10
};

class DataStorageService: public  QObject{
    Q_OBJECT
public:
    explicit DataStorageService(QObject *parent = nullptr);
    ~DataStorageService() override;

    /**
     * Добавление объекта
     * @param jsonObject
     */
    virtual  void addElement(TypeElement &typeElement, QJsonObject &jsonObject) = 0;

    /**
     * Метод обновления данных
     * @param jsonObject
     */
    virtual void updateElement(TypeElement &typeElement, QJsonObject &jsonObject) = 0;

    /**
     * Метод удаления данных
     * @param jsonObject
     */
    virtual void removeElement(TypeElement &typeElement, QJsonObject &jsonObject) = 0;

    /**
     * Получение списка элементов по типу
     * @param typeElement
     * @return
     */
    QList<DataStorageItem*> getElements(const TypeElement &typeElement);

    /**
     * Получение элемента по типу и идентификатору
     * @param typeElement
     * @param id
     * @return
     */
    DataStorageItem* findElement(const TypeElement &typeElement, const QString &id);

public slots:
    /**
     * Метод загрузки данных
     * @param pathToStorage
     */
    virtual void loadData() = 0;


protected:
    QMap<TypeElement, QList<DataStorageItem*>> m_elements;
};


#endif //SCENARIOCLIENT_DATASTORAGESERVICE_H

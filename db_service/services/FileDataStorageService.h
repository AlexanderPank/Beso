#ifndef SCENARIOCLIENT_FILEDATASTORAGESERVICE_H
#define SCENARIOCLIENT_FILEDATASTORAGESERVICE_H

#include <QDir>
#include <QFile>
#include <QList>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "DataStorageService.h"

#define MAIN_DIR_DEFAULT "../data/BD"
#define SUB_DIR_OBJECTS "/BD_objects"
#define SUB_DIR_ALGORITHMS "/BD_algorithms"
#define SUB_DIR_LIBFILES "/BD_files"
#define TEMPLATE_DIR "/template"
#define SUB_DIR_TACTICAL_SIGNS "/BD_tactical_signs"
#define SUB_DIR_INTERACTION "/BD_interactions"
#define SUB_DIR_DISTRICT "/BD_districts"

class FileDataStorageService: public DataStorageService {
    Q_OBJECT
public:
    explicit  FileDataStorageService(QString &mainPath, QObject *parent = nullptr);
    ~FileDataStorageService() override;

    void addElement(TypeElement &typeElement, QJsonObject &jsonObject) override;
    void updateElement(TypeElement &typeElement, QJsonObject &jsonObject) override;
    void removeElement(TypeElement &typeElement, QJsonObject &jsonObject) override;

public slots:
    void loadData() override;

private:
    //Пути хранилища
    QString m_mainPath;
    //Пути хранилища элементов
    QMap<TypeElement, QString> m_pathElements;

    /**
     * Обновление данных с жесткого диска
     * @param path
     * @param items
     */
    void updateDataFromDir(const TypeElement &typeElement);

    void saveTypeData(const QList<DataStorageItem*> &items);


    /**
     * Загрузка Json в модель
     * @param absolutePath
     * @return
     */
    DataStorageItem* loadDataFromJson(const QString &absolutePath);

    /**
     * Считывание данных из файла в JsonObject
     * @param pathFile
     * @return
     */
    QJsonObject loadFileToJson(const QString& pathFile);

    /**
     * Сохранение данных из JsonObject в файл
     * @param pathFile
     * @param jsonObject
     * @return
     */
    bool saveJsonToFile(const QString &pathFile, const QJsonObject &jsonObject);

    /**
     * Поиск елемента по идентификатору в списке
     * @param items
     * @param id
     * @return
     */
    DataStorageItem* findElementById(const QList<DataStorageItem*> &items, const QString &id);
};


#endif //SCENARIOCLIENT_FILEDATASTORAGESERVICE_H

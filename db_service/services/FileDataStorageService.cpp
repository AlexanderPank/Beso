#include "FileDataStorageService.h"

#include "../../core/QLogStream.h"

FileDataStorageService::FileDataStorageService(QString &mainPath, QObject *parent) : DataStorageService(parent) {
    m_mainPath =        mainPath.isEmpty()?MAIN_DIR_DEFAULT:mainPath;
    m_pathElements.insert(TypeElement::teOBJECT, m_mainPath + SUB_DIR_OBJECTS);
    m_pathElements.insert(TypeElement::teOBJECT_TEMPLATE, m_mainPath + SUB_DIR_OBJECTS + TEMPLATE_DIR);
    m_pathElements.insert(TypeElement::teALGORITHM, m_mainPath + SUB_DIR_ALGORITHMS);
    m_pathElements.insert(TypeElement::teALGORITHM_TEMPLATE, m_mainPath + SUB_DIR_ALGORITHMS + TEMPLATE_DIR);
    m_pathElements.insert(TypeElement::teLIBFILE, m_mainPath + SUB_DIR_LIBFILES);
    m_pathElements.insert(TypeElement::teLIBFILE_TEMPLATE, m_mainPath + SUB_DIR_LIBFILES + TEMPLATE_DIR);
    m_pathElements.insert(TypeElement::teTACTICAL_SIGN, m_mainPath + SUB_DIR_TACTICAL_SIGNS);
    m_pathElements.insert(TypeElement::teTACTICAL_SIGN_TEMPLATE, m_mainPath + SUB_DIR_TACTICAL_SIGNS+TEMPLATE_DIR);
    m_pathElements.insert(TypeElement::teINTERACTION, m_mainPath + SUB_DIR_INTERACTION);
    m_pathElements.insert(TypeElement::teINTERACTION_TEMPLATE, m_mainPath + SUB_DIR_INTERACTION+TEMPLATE_DIR);
    m_pathElements.insert(TypeElement::teDISTRICT, m_mainPath + SUB_DIR_DISTRICT);
}

FileDataStorageService::~FileDataStorageService() = default;

void FileDataStorageService::loadData() {
    //Загрузка данных по объектам моделирования
    updateDataFromDir(TypeElement::teOBJECT);
    updateDataFromDir(TypeElement::teOBJECT_TEMPLATE);
    //Загрузка данных по алгоритмам
    updateDataFromDir(TypeElement::teALGORITHM);
    updateDataFromDir(TypeElement::teALGORITHM_TEMPLATE);
    //Загрузка данных по библиотеки файлов
    updateDataFromDir(TypeElement::teLIBFILE);
    updateDataFromDir(TypeElement::teLIBFILE_TEMPLATE);
    //Загрузка тактических знаков
    updateDataFromDir(TypeElement::teTACTICAL_SIGN);
    updateDataFromDir(TypeElement::teTACTICAL_SIGN_TEMPLATE);
    //Загрузка взаимодействий
    updateDataFromDir(TypeElement::teINTERACTION);
    updateDataFromDir(TypeElement::teINTERACTION_TEMPLATE);
    //Загрузка районов моделирования
    updateDataFromDir(TypeElement::teDISTRICT);
}

void FileDataStorageService::updateDataFromDir(const TypeElement &typeElement) {
    m_elements[typeElement].clear();
    QString path = m_pathElements[typeElement];
    QDir dir(m_pathElements[typeElement]);
    dir.setNameFilters({"*.json"});
    QFileInfoList fileListInfo = dir.entryInfoList();
    QStringList fileList;
    foreach (const QFileInfo &fileInfo, fileListInfo) {
        if(fileInfo.fileName().startsWith("new"))
            continue;

        DataStorageItem* loadItem = loadDataFromJson(fileInfo.absoluteFilePath());
        if(loadItem)
            m_elements[typeElement].append(loadItem);
    }
}

DataStorageItem* FileDataStorageService::loadDataFromJson(const QString &absolutePath) {
    QJsonObject loadJsonObject = loadFileToJson(absolutePath);
    if(loadJsonObject.isEmpty())
        return nullptr;

//    if(!loadJsonObject.contains("id")) {
//        qLog() << "В объекте " << absolutePath << ", отсутствует параметр 'id'!";
//        return nullptr;
//    }


//    if(!loadJsonObject.contains("title")) { мешает при работе с файлом

//        qLog() << "В объекте " << absolutePath << ", отсутствует параметр 'title'!";
//        return nullptr;
//    }

    return DataStorageItem::fromJson(loadJsonObject, absolutePath, this);
}

QJsonObject FileDataStorageService::loadFileToJson(const QString& pathFile) {
    QFile file(pathFile);
    QJsonObject object;
    if(!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qLog() << "Файл " << pathFile << ", не существует или не может быть прочитан!";
        return object;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            object = doc.object();
        }
        else
        {
            qLog() << "При преобразование получили пустой документ";
        }
    }
    else
    {
        qLog() << "Невалидный JSON..." << pathFile;
    }

    file.close();
    return object;
}

bool FileDataStorageService::saveJsonToFile(const QString &pathFile, const QJsonObject &jsonObject) {
    QFile file(pathFile);
    if(!file.open(QIODevice::WriteOnly)) {
        qLog() << "Файл " << pathFile << ", не существует или не может быть прочитан!";
        return false;
    }

    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    file.write(strJson.toUtf8());
    file.close();

    return true;
}

void FileDataStorageService::saveTypeData(const QList<DataStorageItem *> &items) {
    for(DataStorageItem* item: items) {
        if(item->isModify()) {
            if(saveJsonToFile(item->getAbsolutePath(), item->getData())) {
                item->setModify(false);
            }
        }
    }
}

void FileDataStorageService::addElement(TypeElement &typeElement, QJsonObject &jsonObject) {
    QString id = jsonObject["id"].toString();
    QString pathNewObject = QString("%1/%2.json").arg(m_pathElements[typeElement], id);
    m_elements[typeElement].append(DataStorageItem::fromJson(jsonObject, pathNewObject, nullptr));

    saveJsonToFile(pathNewObject, jsonObject);
}

void FileDataStorageService::updateElement(TypeElement &typeElement, QJsonObject &jsonObject) {
    QString id = jsonObject["id"].toString();
    DataStorageItem* dataStorageItem = findElementById(m_elements[typeElement], id);
    dataStorageItem->setData(jsonObject);
    dataStorageItem->setTitle(jsonObject["title"].toString());

    saveJsonToFile(dataStorageItem->getAbsolutePath(), jsonObject);
}

void FileDataStorageService::removeElement(TypeElement &typeElement, QJsonObject &jsonObject) {
    QString id = jsonObject["id"].toString();
    DataStorageItem* item = findElementById(m_elements[typeElement], id);
    if(item != nullptr) {
        QString pathFile = item->getAbsolutePath();
        m_elements[typeElement].removeOne(item);
        QFile file(pathFile);
        if(file.exists())
            file.remove();
    }
}

DataStorageItem *FileDataStorageService::findElementById(const QList<DataStorageItem *> &items, const QString &id) {
    for(DataStorageItem* item: items) {
        if(item->getId() == id)
            return item;
    }
    return nullptr;
}

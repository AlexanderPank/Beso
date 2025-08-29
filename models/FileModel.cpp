#include "FileModel.h"
#include <QJsonObject>
#include <QTreeWidgetItem>

FileModel::FileModel(QObject *parent) : QObject(parent)
{
}
FileModel::~FileModel(){
   // delete treeWidgetItem;
}
// Геттеры
QString FileModel::getId() const { return m_id; }
QString FileModel::getFileName() const { return m_fileName; }
QString FileModel::getType() const { return m_type; }
QString FileModel::getMd5Hash() const { return m_md5Hash; }

// Сеттеры
void FileModel::setId(const QString &id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

void FileModel::setFileName(const QString &fileName)
{
    if (m_fileName != fileName) {
        m_fileName = fileName;
        emit fileNameChanged();
    }
}

void FileModel::setType(const QString &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

void FileModel::setMd5Hash(const QString &md5Hash)
{
    if (m_md5Hash != md5Hash) {
        m_md5Hash = md5Hash;
        emit md5HashChanged();
    }
}

// JSON сериализация
QJsonObject FileModel::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["file_name"] = m_fileName;
    json["type"] = m_type;
    json["md5_hash"] = m_md5Hash;
    return json;
}

FileModel* FileModel::fromJson(const QJsonObject &json, QObject* parent)
{
    auto *model = new FileModel(parent);
    model->setId(json["id"].toString());
    model->setFileName(json["file_name"].toString());
    model->setType(json["type"].toString());
    model->setMd5Hash(json["md5_hash"].toString());
    return model;
}

// Метод для отображения в дереве
QTreeWidgetItem* FileModel::getTreeWidgetItem(QTreeWidgetItem *parent)
{
    treeWidgetItem = new QTreeWidgetItem(parent);
    treeWidgetItem->setText(0, "идентификатор");
    treeWidgetItem->setText(1, m_id);
    treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue(const_cast<FileModel*>(this)));

    // Добавляем свойства как дочерние элементы
    auto addProperty = [this](const QString &name, const QString &value) {
        if (!value.isEmpty()) {
            auto *propItem = new QTreeWidgetItem(treeWidgetItem);
            propItem->setText(0, name);
            propItem->setText(1, value);
        }
    };

    addProperty("Файл", m_fileName);
    addProperty("Type", m_type);
    addProperty("MD5 Hash", m_md5Hash);

    return treeWidgetItem;
}
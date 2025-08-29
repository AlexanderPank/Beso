#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QObject>
#include <QString>
class  QTreeWidgetItem;

class FileModel : public QObject
{
Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString fileName READ getFileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString md5Hash READ getMd5Hash WRITE setMd5Hash NOTIFY md5HashChanged)

public:
    explicit FileModel(QObject *parent = nullptr);
    ~FileModel();

    // Геттеры
    QString getId() const;
    QString getFileName() const;
    QString getType() const;
    QString getMd5Hash() const;

    // Сеттеры
    void setId(const QString &id);
    void setFileName(const QString &fileName);
    void setType(const QString &type);
    void setMd5Hash(const QString &md5Hash);

    // JSON методы
    QJsonObject toJson() const;
    static FileModel* fromJson(const QJsonObject &json, QObject* parent);

    // Метод для дерева
    QTreeWidgetItem* getTreeWidgetItem(QTreeWidgetItem *parent = nullptr);

signals:
    void idChanged();
    void fileNameChanged();
    void typeChanged();
    void md5HashChanged();

private:
    QString m_id;
    QString m_fileName;
    QString m_type;
    QString m_md5Hash;
    QTreeWidgetItem *treeWidgetItem= nullptr;
};

#endif // FILEMODEL_H
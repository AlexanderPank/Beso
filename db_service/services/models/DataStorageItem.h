#ifndef SCENARIOCLIENT_DATASTORAGEITEM_H
#define SCENARIOCLIENT_DATASTORAGEITEM_H

#include <QObject>
#include <QJsonObject>

class DataStorageItem: public QObject {
    Q_OBJECT
        Q_PROPERTY(QString id READ getId WRITE setId NOTIFY idChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(QString absolutePath READ getAbsolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
        Q_PROPERTY(QJsonObject data READ getData WRITE setData NOTIFY dataChanged)

public:
    explicit DataStorageItem(QObject *parent = nullptr);
    ~DataStorageItem() override;

    void setTitle(const QString &title);
    QString getTitle() const;

    void setId(const QString &id);
    QString getId() const;

    void setData(const QJsonObject &data);
    QJsonObject getData() const;

    void setAbsolutePath(const QString &absolutePath);
    QString getAbsolutePath() const;

    void setModify(const bool &modify);
    bool isModify() const;

    // JSON методы
    static DataStorageItem* fromJson(const QJsonObject &json, const QString &absolutePath, QObject* parent);

signals:
    void idChanged();
    void titleChanged();
    void absolutePathChanged();
    void dataChanged();

private:
    QString m_title;
    QString m_id;
    QString m_absolutePath;
    QJsonObject m_data;
    bool m_modify;
};
#endif //SCENARIOCLIENT_DATASTORAGEITEM_H

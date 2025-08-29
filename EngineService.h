#ifndef ENGINESERVICE_H
#define ENGINESERVICE_H

#include <QFile>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QHttpMultiPart>
#include <QUrlQuery>
#include <QMessageBox>
#include <QMap>

/**
 * @struct ResponseStatusInfo
 * @brief Хранит информацию о статусе ответа от сервера.
 */
struct ResponseStatusInfo {
    QString detail; /**< Подробное сообщение о статусе ответа. */
    QString status; /**< Статус ответа, например, "успех" или "ошибка". */
};

/**
 * @struct UpdateSimObjectModel
 * @brief Представляет модель для обновления объекта симуляции.
 */
struct UpdateSimObjectModel {
    QString id; /**< Идентификатор объекта симуляции. */
    QString title; /**< Название объекта симуляции. */
    QMap<QString, QVariant> properties; /**< Свойства объекта симуляции. */
};

/**
 * @struct UploadFile
 * @brief Структура, представляющая файл для загрузки.
 */
struct UploadFile {
    QString type; /**< Тип файла. */
    QFile *file; /**< Указатель на файл, предназначенный для загрузки. */
};

/**
 * @struct ResponseLoadInfo
 * @brief Содержит информацию об ответе на запрос загрузки данных.
 */
struct ResponseLoadInfo {
    QString detail; /**< Подробное сообщение о статусе загрузки. */
    QString status; /**< Статус загрузки. */
    QList<QVariantMap> objects; /**< Список объектов симуляции. */
    QVariant geoJson; /**< Географическая информация в формате GeoJSON. */
    int code; /**< Код ответа. */
    QList<QVariantMap> notExistsFiles; /**< Список файлов, которых не существует. */
};

/**
 * @class EngineService
 * @brief Класс для работы с API симуляционного движка.
 *
 * Класс предоставляет методы для управления симуляцией, включая запуск, остановку,
 * загрузку файлов, обновление данных объектов и изменение скорости симуляции.
 */
class EngineService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Конструктор EngineService.
     * @param apiUrl URL-адрес API.
     * @param parent Родительский объект.
     */
    EngineService(QString apiUrl, QObject *parent = nullptr);

    /**
     * @brief Запускает симуляцию.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument startEngine();

    /**
     * @brief Загружает файл для симуляции.
     * @param filePath Путь к файлу сценария.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument loadEngine(QString filePath);

    /**
     * @brief Останавливает симуляцию.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument stopEngine();

    /**
     * @brief Устанавливает скорость симуляции.
     * @param speed Значение скорости симуляции.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument setSpeed(int speed);

    /**
     * @brief Приостанавливает симуляцию.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument pauseEngine();

    /**
     * @brief Возобновляет симуляцию.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument resumeEngine();

    /**
     * @brief Обновляет данные объекта симуляции.
     * @param updateData Данные для обновления.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument updateEngine(UpdateSimObjectModel updateData);

    /**
     * @brief Загружает файлы для симуляции.
     * @param files Список файлов для загрузки.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument uploadFiles(QList<UploadFile> files);

    /**
     * @brief Запускает/останавливает симуляцию в режиме 3D.
     * @return Документ JSON с ответом от сервера.
    */
    QJsonDocument start3Mode();


private:
    QString apiUrl = ""; /**< URL-адрес API. */
    QNetworkAccessManager *manager; /**< Менеджер для отправки сетевых запросов. */

    /**
     * @brief Отправляет POST-запрос на указанный рест.
     * @param path Путь запроса.
     * @param data Данные для отправки.
     * @param multiPart (Необязательно) Объект для отправки данных в формате multipart.
     * @return Документ JSON с ответом от сервера.
     */
    QJsonDocument sendPostRequest(const QString &path, const QByteArray &data, QHttpMultiPart *multiPart = nullptr);


};

#endif // ENGINESERVICE_H

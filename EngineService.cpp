#include "EngineService.h"


#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QHttpMultiPart>
#include <QUrlQuery>
#include <QMessageBox>
#include <QEventLoop>

EngineService::EngineService(QString apiUrl, QObject *parent):
    QObject(parent), apiUrl(apiUrl), manager(new QNetworkAccessManager(this)) {

}

QJsonDocument EngineService::startEngine() {return sendPostRequest("/engine/start", {}); }

QJsonDocument EngineService::loadEngine(QString filePath) {
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file" << filePath << file->errorString();
        delete file;
        delete multiPart;
        return {};
    }
    file->setParent(multiPart);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"json_file\"; filename=\"" + file->fileName() + "\""));
    filePart.setBodyDevice(file);
    multiPart->append(filePart);

    return sendPostRequest("/engine/load", {}, multiPart);
}

QJsonDocument EngineService::stopEngine() {return  sendPostRequest("/engine/stop", {}); }
QJsonDocument EngineService::setSpeed(int speed) {
    QJsonObject json;
    json["speed"] = speed;
    QString ss = QJsonDocument(json).toJson();
    ss = ss.replace("\n", "").replace(" ", "");
    return sendPostRequest("/engine/speed", ss.toUtf8());
}

QJsonDocument EngineService::pauseEngine() { return sendPostRequest("/engine/pause", {}); }
QJsonDocument EngineService::resumeEngine() { return sendPostRequest("/engine/resume", {}); }
QJsonDocument EngineService::start3Mode() { return sendPostRequest("/engine/toggle3D", {}); }

QJsonDocument EngineService::updateEngine(UpdateSimObjectModel updateData) {
    QJsonObject json;
    json["id"] = updateData.id;
    json["title"] = updateData.title;
    QJsonObject properties;
    for (auto key : updateData.properties.keys()) {
        properties[key] = QJsonValue::fromVariant(updateData.properties[key]);
    }
    json["properties"] = properties;
    return sendPostRequest("/engine/update", QJsonDocument(json).toJson());
}

QJsonDocument EngineService::uploadFiles(QList<UploadFile> files) {
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    for (auto &fileData : files) {
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"files\"; filename=\"" + fileData.file->fileName() + "\""));
        filePart.setBodyDevice(fileData.file);
        multiPart->append(filePart);

        QHttpPart typePart;
        typePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"types\""));
        typePart.setBody(fileData.type.toUtf8());
        multiPart->append(typePart);
    }
    return sendPostRequest("/engine/upload_files", {}, multiPart);
}

QJsonDocument EngineService::sendPostRequest(const QString &path, const QByteArray &data, QHttpMultiPart *multiPart) {
    QUrl url(apiUrl + path);
    QNetworkRequest request(url);

    QNetworkReply *reply;
    if (multiPart) {
        reply = manager->post(request, multiPart);
        multiPart->setParent(reply);
    } else {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = manager->post(request,  data );
    }

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    QJsonDocument responseDoc;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        responseDoc = QJsonDocument::fromJson(responseData);
    } else {
        qWarning() << "Error:" << reply->errorString();
    }
    reply->deleteLater();
    return responseDoc;
}




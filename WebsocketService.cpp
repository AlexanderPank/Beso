#include "WebsocketService.h"
#include "core/QLogStream.h"

#include <QJsonDocument>
#include <QJsonObject>

WebSocketService::WebSocketService(QString wsUrl, QObject *parent)
    : QObject(parent),  socket(new QWebSocket), wsUrl(wsUrl), reconnectTimer(new QTimer(this)) {
    connect(socket, &QWebSocket::connected, this, &WebSocketService::onConnected);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketService::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &WebSocketService::onError);
    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketService::onMessageReceived);

    reconnectTimer->setInterval(500); // 500 ms interval for reconnection
    reconnectTimer->setSingleShot(true);
    connect(reconnectTimer, &QTimer::timeout, this, &WebSocketService::createWebSocket);

    createWebSocket();
}

WebSocketService::~WebSocketService() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->close();  // Инициируем закрытие (асинхронное)
    }

    // Удаляем сокет (безопасно, даже если close() ещё не завершился)
    if (socket) {
        socket->close();  // Закрываем соединение
        socket->deleteLater();  // Безопасное удаление
    }
}

void WebSocketService::createWebSocket() {
    qLog() << "Attempting to connect to WebSocket server...";
    socket->open(QUrl(wsUrl));
}

void WebSocketService::onConnected() {
    qLog() << "WebSocket connection established";
}

void WebSocketService::onDisconnected() {
    qLog() << "WebSocket connection closed, attempting to reconnect...";
    reconnectTimer->start();
}

void WebSocketService::onError(QAbstractSocket::SocketError ) {
  //  qLog() << "WebSocket error occurred:" << socket->errorString();
    if (socket) socket->close();  // Close the socket and attempt reconnection
    reconnectTimer->start();
}

void WebSocketService::onMessageReceived(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        auto simulationState = doc.object();
        if (simulationState.contains("objects")) {
            emit simulationStateReceived(doc);
        } else
            qLog() << "Invalid JSON format, not found objects properties";
    } else {
        qLog() << "Invalid JSON received";
    }
}

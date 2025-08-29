#ifndef WEBSOCKETSERVICE_H
#define WEBSOCKETSERVICE_H

#include <QAbstractSocket>
#include <QWebSocket>
#include <QObject>
#include <QTimer>

class WebSocketService: public QObject{
    Q_OBJECT
public:
    explicit WebSocketService(QString wsUrl, QObject *parent = nullptr);
    ~WebSocketService();

signals:
   void simulationStateReceived(const QJsonDocument &simulationState);

private slots:
   void createWebSocket();
   void onMessageReceived(const QString &message);
   void onConnected();
   void onDisconnected();
   void onError(QAbstractSocket::SocketError error);

private:
   QWebSocket *socket;
   QString wsUrl;
   QTimer *reconnectTimer;

};

#endif // WEBSOCKETSERVICE_H

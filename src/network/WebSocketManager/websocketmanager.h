#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QtWebSockets/QtWebSockets>
#include <QJsonDocument>
#include <QJsonObject>

struct WebSocketParameters {
    QByteArray apiKey;
    QByteArray apiSecret;
    QByteArray baseUrl;
    bool isPrivate;
};

class WebSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketManager(const WebSocketParameters& param, QObject *parent = nullptr);
    ~WebSocketManager();

    void connectToServer(const QJsonDocument &parameters = QJsonDocument(), const QString &urlSuffix = "");
    void disconnectFromServer();
    bool sendMessage(const QJsonDocument &message);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QJsonDocument &message);
    void errorOccurred(const QString &errorMessage);
    void authResult(bool success, const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);

private:
    QByteArray genQueryStr(const QJsonObject &parameters);
    QByteArray signAuth(qint64 expires);

    QWebSocket* m_socket;
    WebSocketParameters m_param;
};

#endif // WEBSOCKETMANAGER_H

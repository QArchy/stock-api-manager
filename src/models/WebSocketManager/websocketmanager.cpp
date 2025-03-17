#include "websocketmanager.h"
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QDateTime>
#include <QUrlQuery>

WebSocketManager::WebSocketManager(const QString &apiKey, const QString &apiSecret, const QString &baseUrl, QObject *parent)
    : QObject(parent), apiKey(apiKey.toUtf8()), apiSecret(apiSecret.toUtf8()), baseUrl(baseUrl.toUtf8()), isPrivate(false) {
    socket = nullptr;
}

WebSocketManager::~WebSocketManager() { disconnectFromServer(); }

void WebSocketManager::connectToServer(const QJsonDocument &parameters, const QString &urlSuffix, bool isPrivate) {
    if (socket) {
        socket->deleteLater();
        socket = nullptr;
    }

    isPrivate = isPrivate;
    QByteArray query = genQueryStr(parameters.object());

    QUrl url(QString::fromUtf8(baseUrl) + urlSuffix);
    QUrlQuery urlQuery;
    urlQuery.setQuery(QString::fromUtf8(query));
    url.setQuery(urlQuery);

    socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(socket, &QWebSocket::connected, this, &WebSocketManager::onConnected);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketManager::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &WebSocketManager::onError);
    connect(socket, &QWebSocket::textMessageReceived,
            this, &WebSocketManager::onTextMessageReceived);

    socket->open(url);
}

void WebSocketManager::disconnectFromServer() {
    if (socket) {
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
}

void WebSocketManager::sendMessage(const QJsonDocument &message) {
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        socket->sendTextMessage(message.toJson(QJsonDocument::Compact));
    }
}

QByteArray WebSocketManager::genQueryStr(const QJsonObject &parameters) {
    QByteArray query;
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        if (!query.isEmpty()) {
            query.append('&');
        }
        query.append(QUrl::toPercentEncoding(it.key()));
        query.append('=');
        query.append(QUrl::toPercentEncoding(it.value().toString()));
    }
    return query;
}

QByteArray WebSocketManager::signAuth(qint64 expires) {
    QByteArray message = "GET/realtime" + QByteArray::number(expires);
    return QMessageAuthenticationCode::hash(message, apiSecret, QCryptographicHash::Sha256).toHex();
}

void WebSocketManager::onConnected() {
    if (isPrivate) {
        qint64 expires = QDateTime::currentMSecsSinceEpoch() + 1000;
        QByteArray signature = signAuth(expires);

        QJsonObject authMsg;
        authMsg["op"] = "auth";
        authMsg["args"] = QJsonArray() << QString::fromUtf8(apiKey) << QString::number(expires) << QString::fromUtf8(signature);
        socket->sendTextMessage(QJsonDocument(authMsg).toJson(QJsonDocument::Compact));
    }
    emit connected();
}

void WebSocketManager::onDisconnected() {
    emit disconnected();
}

void WebSocketManager::onTextMessageReceived(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("op") && obj["op"] == "auth") {
            bool success = obj["success"].toBool();
            QString msg = obj["ret_msg"].toString();
            emit authResult(success, msg);
            return;
        }
    }
    emit messageReceived(doc);
}

void WebSocketManager::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    emit errorOccurred(socket->errorString());
}

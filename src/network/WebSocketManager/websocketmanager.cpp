#include "websocketmanager.h"
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QDateTime>
#include <QUrlQuery>

WebSocketManager::WebSocketManager(const WebSocketParameters& param, QObject *parent)
    : QObject(parent), m_param(param) {
    m_socket = nullptr;
}

WebSocketManager::~WebSocketManager() { disconnectFromServer(); }

void WebSocketManager::connectToServer(const QJsonDocument &parameters, const QString &urlSuffix) {
    if (m_socket) {
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    QUrl url(m_param.baseUrl + urlSuffix);
    url.setQuery(genQueryStr(parameters.object()));

    m_socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_socket, &QWebSocket::connected, this, &WebSocketManager::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &WebSocketManager::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &WebSocketManager::onError);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &WebSocketManager::onTextMessageReceived);

    m_socket->open(url);
}

void WebSocketManager::disconnectFromServer() {
    if (m_socket) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

bool WebSocketManager::sendMessage(const QJsonDocument &message) {
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->sendTextMessage(message.toJson(QJsonDocument::Compact));
        return true;
    }
    return false;
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
    return QMessageAuthenticationCode::hash(message, m_param.apiSecret, QCryptographicHash::Sha256).toHex();
}

void WebSocketManager::onConnected() {
    if (m_param.isPrivate) {
        qint64 expires = QDateTime::currentMSecsSinceEpoch() + 1000;
        QByteArray signature = signAuth(expires);

        QJsonObject authMsg;
        authMsg["op"] = "auth";
        authMsg["args"] = QJsonArray() << QString(m_param.apiKey) << QString::number(expires) << QString(signature);
        m_socket->sendTextMessage(QJsonDocument(authMsg).toJson(QJsonDocument::Compact));
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
    emit errorOccurred(m_socket->errorString());
}

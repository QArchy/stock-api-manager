#include "websocketmanager.h"
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QDateTime>
#include <QUrlQuery>

WebSocketManager::WebSocketManager(const QString &apiKey,
                                   const QString &apiSecret,
                                   const QString &baseUrl,
                                   QObject *parent)
    : QObject(parent),
    m_apiKey(apiKey.toUtf8()),
    m_apiSecret(apiSecret.toUtf8()),
    m_baseUrl(baseUrl.toUtf8()),
    m_isPrivate(false)
{
    m_webSocket = nullptr;
}

WebSocketManager::~WebSocketManager()
{
    disconnectFromServer();
}

void WebSocketManager::connectToServer(const QJsonDocument &parameters,
                                       const QString &urlSuffix,
                                       bool isPrivate)
{
    if (m_webSocket) {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }

    m_isPrivate = isPrivate;
    QByteArray query = genQueryStr(parameters.object());

    QUrl url(QString::fromUtf8(m_baseUrl) + urlSuffix);
    QUrlQuery urlQuery;
    urlQuery.setQuery(QString::fromUtf8(query));
    url.setQuery(urlQuery);

    m_webSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketManager::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketManager::onDisconnected);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &WebSocketManager::onError);
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketManager::onTextMessageReceived);

    m_webSocket->open(url);
}

void WebSocketManager::disconnectFromServer()
{
    if (m_webSocket) {
        m_webSocket->close();
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }
}

void WebSocketManager::sendMessage(const QJsonDocument &message)
{
    if (m_webSocket && m_webSocket->state() == QAbstractSocket::ConnectedState) {
        m_webSocket->sendTextMessage(message.toJson(QJsonDocument::Compact));
    }
}

QByteArray WebSocketManager::genQueryStr(const QJsonObject &parameters)
{
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

QByteArray WebSocketManager::signAuth(qint64 expires)
{
    QByteArray message = "GET/realtime" + QByteArray::number(expires);
    return QMessageAuthenticationCode::hash(message, m_apiSecret,
                                            QCryptographicHash::Sha256).toHex();
}

void WebSocketManager::onConnected()
{
    if (m_isPrivate) {
        qint64 expires = QDateTime::currentMSecsSinceEpoch() + 1000;
        QByteArray signature = signAuth(expires);

        QJsonObject authMsg;
        authMsg["op"] = "auth";
        authMsg["args"] = QJsonArray()
                          << QString::fromUtf8(m_apiKey)
                          << QString::number(expires)
                          << QString::fromUtf8(signature);

        m_webSocket->sendTextMessage(QJsonDocument(authMsg).toJson(QJsonDocument::Compact));
    }
    qDebug() << "Connected to server";
    emit connected();
}

void WebSocketManager::onDisconnected()
{
    qDebug() << "Disconnected from server";
    emit disconnected();
}

void WebSocketManager::onTextMessageReceived(const QString &message)
{
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
    qDebug() << "Message received: " << message;
    emit messageReceived(doc);
}

void WebSocketManager::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    qDebug() << "Error occurred: " << m_webSocket->errorString();
    emit errorOccurred(m_webSocket->errorString());
}

#include "httpmanager.h"
#include "src/models/nam.h"

#include <QNetworkReply>
#include <QDateTime>
#include <QUrlQuery>
#include <QJsonObject>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

HttpManager::HttpManager(const HttpParameters& params, QObject* parent)
    : QObject(parent), params(params) {
    connect(NAM::getInstance().getManager(), &QNetworkAccessManager::finished, this, &HttpManager::handleReply);
}

void HttpManager::post(const QJsonDocument& data, const QString& endpoint) {
    const QByteArray timestamp = QByteArray::number(QDateTime::currentMSecsSinceEpoch());
    const QByteArray payload = data.toJson(QJsonDocument::Compact);

    QNetworkRequest request = createRequest(endpoint, timestamp, payload);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    NAM::getInstance().getManager()->post(request, payload);
}

void HttpManager::get(const QJsonDocument& params, const QString& endpoint) {
    const QByteArray timestamp = QByteArray::number(QDateTime::currentMSecsSinceEpoch());
    const QByteArray query = generateQueryString(params.object());

    QNetworkRequest request = createRequest(endpoint + "?" + query, timestamp, query);
    NAM::getInstance().getManager()->get(request);
}

QByteArray HttpManager::createSignature(const QByteArray& timestamp, const QByteArray& payload) const {
    const QByteArray data = timestamp + params.apiKey + params.recvWindow + payload;
    return QMessageAuthenticationCode::hash(data, params.apiSecret, QCryptographicHash::Sha256).toHex();
}

QNetworkRequest HttpManager::createRequest(const QString& endpoint, const QByteArray& timestamp, const QByteArray& payload) const {
    QNetworkRequest request(QUrl(params.baseUrl + endpoint));

    request.setRawHeader("X-BAPI-API-KEY", params.apiKey);
    request.setRawHeader("X-BAPI-SIGN-TYPE", "2");
    request.setRawHeader("X-BAPI-TIMESTAMP", timestamp);
    request.setRawHeader("X-BAPI-RECV-WINDOW", params.recvWindow);
    request.setRawHeader("X-BAPI-SIGN", createSignature(timestamp, payload));

    return request;
}

QByteArray HttpManager::generateQueryString(const QJsonObject& params) const {
    QByteArray query;
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (!query.isEmpty()) query.append('&');
        query.append(QUrl::toPercentEncoding(it.key()));
        query.append('=');
        query.append(QUrl::toPercentEncoding(it.value().toString()));
    }
    return query;
}

void HttpManager::handleReply(QNetworkReply* reply) {
    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
        emit requestFailed(reply->errorString(), statusCode);
        reply->deleteLater();
        return;
    }

    const QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit requestFailed("Failed to parse JSON response: " + parseError.errorString(), statusCode);
    } else {
        emit requestCompleted(jsonResponse);
    }

    reply->deleteLater();
}

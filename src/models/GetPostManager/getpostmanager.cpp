#include "getpostmanager.h"
#include "src/models/nam.h"

QByteArray GetPostManager::genQueryStr(const QJsonObject& parameters) {
    QByteArray query;
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        if (!query.isEmpty()) query.append('&');
        query.append(QUrl::toPercentEncoding(it.key()));
        query.append('=');
        query.append(QUrl::toPercentEncoding(it.value().toString()));
    }
    return query;
}

QByteArray GetPostManager::sign(const QByteArray& data) {
    return QMessageAuthenticationCode::hash(data, this->apiSecret, QCryptographicHash::Sha256).toHex();
}

void GetPostManager::setReqHeader(QNetworkRequest& req, QByteArray& Timestamp) {
    req.setRawHeader("X-BAPI-API-KEY", this->apiKey);
    req.setRawHeader("X-BAPI-SIGN-TYPE", "2");
    req.setRawHeader("X-BAPI-TIMESTAMP", Timestamp);
    req.setRawHeader("X-BAPI-RECV-WINDOW", this->recvWindow);
}

GetPostManager::GetPostManager(const QString ApiKey, const QString ApiSecret, const QString RecvWindow, const QString BaseUrl, QObject* parent)
    : QObject(parent), apiKey(ApiKey.toUtf8()), apiSecret(ApiSecret.toUtf8()), recvWindow(RecvWindow.toUtf8()), baseUrl(BaseUrl.toUtf8()) {}

GetPostManager::~GetPostManager() {}

void GetPostManager::post(const QJsonDocument& doc, const QString& url) {
    QByteArray Timestamp = QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8();
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    QNetworkRequest req((QUrl(baseUrl + url)));
    setReqHeader(req, Timestamp);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-BAPI-SIGN", sign(Timestamp + this->apiKey + this->recvWindow + jsonData));

    NAM::getInstance().getManager()->post(req, jsonData);
}

void GetPostManager::get(const QJsonDocument& doc, const QString& url) {
    QByteArray Timestamp = QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8();
    QByteArray jsonQuery = genQueryStr(doc.object());

    QNetworkRequest req(QUrl(baseUrl + url + jsonQuery));
    setReqHeader(req, Timestamp);
    req.setRawHeader("X-BAPI-SIGN", sign(Timestamp + this->apiKey + this->recvWindow + jsonQuery));

    NAM::getInstance().getManager()->get(req);
}

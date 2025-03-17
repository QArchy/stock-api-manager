#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QByteArray>

struct HttpParameters {
    QByteArray apiKey;
    QByteArray apiSecret;
    QByteArray recvWindow;
    QByteArray baseUrl;
};

class HttpManager : public QObject
{
    Q_OBJECT
public:
    explicit HttpManager(const HttpParameters& params, QObject* parent = nullptr);
    ~HttpManager() = default;

    void post(const QJsonDocument& data, const QString& endpoint);
    void get(const QJsonDocument& params, const QString& endpoint);

signals:
    void requestCompleted(const QJsonDocument& response);
    void requestFailed(const QString& errorString, int statusCode);

private slots:
    void handleReply(QNetworkReply* reply);

private:
    QByteArray createSignature(const QByteArray& timestamp, const QByteArray& payload) const;
    QNetworkRequest createRequest(const QString& endpoint, const QByteArray& timestamp, const QByteArray& payload) const;
    QByteArray generateQueryString(const QJsonObject& params) const;

    HttpParameters params;
};

#endif // HTTPMANAGER_H

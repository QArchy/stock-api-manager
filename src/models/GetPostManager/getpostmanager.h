#ifndef GETPOSTMANAGER_H
#define GETPOSTMANAGER_H

#include <QMessageAuthenticationCode>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

class GetPostManager: public QObject {
    Q_OBJECT
private:
    QByteArray apiKey;
    QByteArray apiSecret;
    QByteArray recvWindow;
    QByteArray baseUrl;

    QByteArray genQueryStr(const QJsonObject& parameters);
    inline QByteArray sign(const QByteArray& data);
    inline void setReqHeader(QNetworkRequest& req, QByteArray& Timestamp);
public:
    explicit GetPostManager(const QString ApiKey, const QString ApiSecret, const QString RecvWindow, const QString BaseUrl, QObject* parent = nullptr);
    GetPostManager(const GetPostManager&) = delete;
    GetPostManager& operator=(const GetPostManager&) = delete;
    ~GetPostManager();
public slots:
    void post(const QJsonDocument& doc, const QString& url);
    void get(const QJsonDocument& doc, const QString& url);
};

#endif // GETPOSTMANAGER_H

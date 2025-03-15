#include "api/APISettings.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDebug>

class GetPostManager : public QNetworkAccessManager {
private:
    QString ApiKey;
    QString ApiKeySecret;
    QString RecvWindow;
public:
    GetPostManager(QString ApiKey, QString ApiKeySecret, QString RecvWindow = "5000") :
        QNetworkAccessManager(), ApiKey(ApiKey), ApiKeySecret(ApiKeySecret), RecvWindow(RecvWindow) {

    }
};

class Encryption {
private:
    static const QString ApiKey;
    static const QString ApiSecret;
    static const QString Timestamp;
    static const QString RecvWindow;

public:
    void PlaceOrder();
    void GetOpenOrder();

private:
    static QString GeneratePostSignature(const QJsonObject& parameters);
    static QString GenerateGetSignature(const QJsonObject& parameters);
    static QString ComputeSignature(const QString& data);
    static QString GenerateQueryString(const QJsonObject& parameters);
};

const QString Encryption::ApiKey = "cXR6lZwBELWqoNA2hG";
const QString Encryption::ApiSecret = "bYpfCSs9y4Jl7GYpMH6T6gPRxPGnNTDbb4CP";
const QString Encryption::Timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
const QString Encryption::RecvWindow = "5000";

QString Encryption::GeneratePostSignature(const QJsonObject& parameters) {
    QString paramJson = QJsonDocument(parameters).toJson(QJsonDocument::Compact);
    QString rawData = Timestamp + ApiKey + RecvWindow + paramJson;
    return ComputeSignature(rawData);
}

QString Encryption::GenerateGetSignature(const QJsonObject& parameters) {
    QString queryString = GenerateQueryString(parameters);
    QString rawData = Timestamp + ApiKey + RecvWindow + queryString;
    return ComputeSignature(rawData);
}

QString Encryption::ComputeSignature(const QString& data) {
    QByteArray secretKey = ApiSecret.toUtf8();
    QByteArray dataToSign = data.toUtf8();

    QByteArray hash = QMessageAuthenticationCode::hash(dataToSign, secretKey, QCryptographicHash::Sha256);
    QString result = hash.toHex();

    return result;
}

QString Encryption::GenerateQueryString(const QJsonObject& parameters) {
    QStringList queryParts;
    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        queryParts.append(it.key() + "=" + it.value().toString());
    }
    return queryParts.join("&");
}

void Encryption::PlaceOrder() {
    QJsonObject parameters;
    parameters["category"] = "linear";
    parameters["symbol"] = "BTCUSDT";
    parameters["side"] = "Buy";
    parameters["positionIdx"] = 0;
    parameters["orderType"] = "Limit";
    parameters["qty"] = "0.001";
    parameters["price"] = "18900";
    parameters["timeInForce"] = "GTC";

    QString signature = GeneratePostSignature(parameters);
    QString jsonPayload = QJsonDocument(parameters).toJson(QJsonDocument::Compact);

    QNetworkAccessManager* manager = new QNetworkAccessManager();

    QNetworkRequest request(QUrl("https://api-demo.bybit.com/v5/order/create"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-BAPI-API-KEY", ApiKey.toUtf8());
    request.setRawHeader("X-BAPI-SIGN", signature.toUtf8());
    request.setRawHeader("X-BAPI-SIGN-TYPE", "2");
    request.setRawHeader("X-BAPI-TIMESTAMP", Timestamp.toUtf8());
    request.setRawHeader("X-BAPI-RECV-WINDOW", RecvWindow.toUtf8());

    manager->post(request, jsonPayload.toUtf8());
    QObject::connect(manager, &QNetworkAccessManager::finished, [](QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Response: " << reply->readAll();
        } else {
            qDebug() << "Error: " << reply->errorString();
        }
        reply->deleteLater();
    });
}

void Encryption::GetOpenOrder() {
    QJsonObject parameters;
    parameters["category"] = "linear";
    parameters["symbol"] = "BTCUSDT";

    QString signature = GenerateGetSignature(parameters);
    QString queryString = GenerateQueryString(parameters);

    QNetworkAccessManager* manager = new QNetworkAccessManager();

    QNetworkRequest request(QUrl("https://api-demo.bybit.com/v5/order/realtime?" + queryString));
    request.setRawHeader("X-BAPI-API-KEY", ApiKey.toUtf8());
    request.setRawHeader("X-BAPI-SIGN", signature.toUtf8());
    request.setRawHeader("X-BAPI-SIGN-TYPE", "2");
    request.setRawHeader("X-BAPI-TIMESTAMP", Timestamp.toUtf8());
    request.setRawHeader("X-BAPI-RECV-WINDOW", RecvWindow.toUtf8());

    manager->get(request);
    QObject::connect(manager, &QNetworkAccessManager::finished, [](QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Response: " << reply->readAll();
        } else {
            qDebug() << "Error: " << reply->errorString();
        }
        reply->deleteLater();
    });
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    APISettings settings;

    Encryption encryptionTest;
    encryptionTest.PlaceOrder();
    encryptionTest.GetOpenOrder();

    return a.exec();
}

//#include "mainwindow.h"
//
//#include <QApplication>
//#include <QLocale>
//#include <QTranslator>
//
//void translation(QApplication& a);
//void startMainWindow();
//
//int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    translation(a);
//    MainWindow w; w.show();
//    return a.exec();
//}
//
//void translation(QApplication& a) {
//    QTranslator translator;
//    const QStringList uiLanguages = QLocale::system().uiLanguages();
//    for (const QString &locale : uiLanguages) {
//        const QString baseName = "BybitManager_" + QLocale(locale).name();
//        if (translator.load(":/i18n/" + baseName)) {
//            a.installTranslator(&translator);
//            break;
//        }
//    }
//}
//

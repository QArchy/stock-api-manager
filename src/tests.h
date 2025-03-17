#ifndef TESTS_H
#define TESTS_H

#include "src/models/HttpManager/httpmanager.h"
#include "src/models/WebSocketManager/websocketmanager.h"
#include "src/controllers/Settings/apiSettings.h"
#include <QJsonDocument>
#include <QNetworkReply>
#include <QDateTime>

void dateTimeTest(bool dir) {
    QString format = "yyyy-MM-dd hh:mm:ss:zzz";
    if (dir == true) { // check from string to msecs
        QString dateStart = "2025-03-24 23:45:00:000";
        qDebug() << "Date start: " << dateStart;
        QString dateEnd = "2025-03-24 23:50:00:000";
        qDebug() << "Date end: " << dateEnd;

        quint64 start = QDateTime::fromString(dateStart, format).toMSecsSinceEpoch();
        qDebug() << "Start ms: " << start;
        quint64 end =  QDateTime::fromString(dateEnd, format).toMSecsSinceEpoch();
        qDebug() << "End ms: " << end;

        QDateTime dateTimeStart = QDateTime::fromMSecsSinceEpoch(start);
        QDateTime dateTimeEnd = QDateTime::fromMSecsSinceEpoch(end);

        qDebug() << (dateStart == dateTimeStart.toString("yyyy-MM-dd hh:mm:ss:zzz"));
        qDebug() << (dateEnd == dateTimeEnd.toString("yyyy-MM-dd hh:mm:ss:zzz"));
    }
    else { // check from msec to string
        quint64 start = 1742205600000;
        qDebug() << "Start ms: " << start;
        quint64 end =  1742205600000;
        qDebug() << "End ms: " << end;

        QString dateStart = QDateTime::fromMSecsSinceEpoch(start).toString("yyyy-MM-dd hh:mm:ss:zzz");;
        qDebug() << "Date start: " << dateStart;
        QString dateEnd = QDateTime::fromMSecsSinceEpoch(end).toString("yyyy-MM-dd hh:mm:ss:zzz");
        qDebug() << "Date end: " << dateEnd;

        quint64 dateTimeStart = QDateTime::fromString(dateStart, format).toMSecsSinceEpoch();
        quint64 dateTimeEnd = QDateTime::fromString(dateEnd, format).toMSecsSinceEpoch();

        qDebug() << (start == dateTimeStart);
        qDebug() << (end == dateTimeEnd);
    }
}

void onFinished(QNetworkReply* reply) {
    QByteArray responseData = reply->readAll();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
            qDebug() << "JSON data: " << jsonDoc;
        } else {
            qDebug() << "JSON parse error: " << parseError.errorString();
        }
    } else {
        qDebug() << "Error: " << reply->errorString();
    }

    reply->deleteLater();
}

void WsTest() {
    WebSocketParameters param;
    param.apiKey = ApiSettings::getInstance().getbApi()->keys.apiKeyDemo.toUtf8();
    param.apiSecret = ApiSettings::getInstance().getbApi()->keys.apiSecretDemo.toUtf8();
    param.baseUrl = ApiSettings::getInstance().getbApi()->wsParams.publicUrl.toUtf8();
    param.isPrivate = false;

    WebSocketManager manager(param);
    manager.connectToServer(QJsonDocument(), "linear");

    QObject::connect(&manager, &WebSocketManager::connected, [&]() {
        QJsonObject subMsg;
        subMsg["op"] = "subscribe";
        subMsg["args"] = QJsonArray() << "kline.1.BTCUSDT";
        manager.sendMessage(QJsonDocument(subMsg));
    });

    QObject::connect(&manager, &WebSocketManager::messageReceived, [](const QJsonDocument &doc) {
        qDebug() << "Kline Update:" << doc.toJson();
    });
}

void HttpResponse(const QJsonDocument &response) {
    qDebug() << "Response: " << response.toJson();
}

void HttpResponseFail(const QString &errorString, int statusCode) {
    qDebug() << "Error: " << errorString << " Status code: " << statusCode;
}

void HttpGetPostTest() {
    QJsonObject parametersPost;
    parametersPost["category"] = "linear";
    parametersPost["symbol"] = "BTCUSDT";
    parametersPost["side"] = "Buy";
    parametersPost["positionIdx"] = 0;
    parametersPost["orderType"] = "Limit";
    parametersPost["qty"] = "0.001";
    parametersPost["price"] = "18900";
    parametersPost["timeInForce"] = "GTC";
    QString postUrl("order/create");

    QJsonObject parametersGet;
    parametersGet["category"] = "linear";
    parametersGet["symbol"] = "BTCUSDT";
    QString getUrl("order/realtime");

    HttpParameters params = {
        ApiSettings::getInstance().getbApi()->keys.apiKeyDemo.toUtf8(),
        ApiSettings::getInstance().getbApi()->keys.apiSecretDemo.toUtf8(),
        ApiSettings::getInstance().getbApi()->httpParams.recvWindow.toUtf8(),
        ApiSettings::getInstance().getbApi()->httpParams.demoNetBaseUrl.toUtf8()
    };

    HttpManager gpmanager(params);
    QObject::connect(&gpmanager, &HttpManager::requestCompleted, HttpResponse);
    QObject::connect(&gpmanager, &HttpManager::requestFailed, HttpResponseFail);
    gpmanager.post(QJsonDocument(parametersPost), postUrl);
    gpmanager.get(QJsonDocument(parametersGet), getUrl);
}

void HttpGetKlineTest() {
    QJsonObject parametersGetKline;
    parametersGetKline["category"] = "linear";
    parametersGetKline["symbol"] = "BTCUSDT";
    parametersGetKline["interval"] = "60";

    QDateTime dtStart = QDateTime::fromString("2025-03-17 06:00:00:000", "yyyy-MM-dd hh:mm:ss:zzz");
    dtStart.setTimeZone(QTimeZone::utc());
    qint64 msStart = dtStart.toMSecsSinceEpoch();
    parametersGetKline["start"] = QString::number(msStart);

    QDateTime dtEnd = QDateTime::fromString("2025-03-17 09:00:00:000", "yyyy-MM-dd hh:mm:ss:zzz");
    dtEnd.setTimeZone(QTimeZone::utc());
    qint64 msEnd = dtEnd.toMSecsSinceEpoch();
    parametersGetKline["end"] = QString::number(msEnd);

    parametersGetKline["limit"] = "4";
    QString getKlineUrl("market/kline");

    HttpParameters params = {
        ApiSettings::getInstance().getbApi()->keys.apiKeyDemo.toUtf8(),
        ApiSettings::getInstance().getbApi()->keys.apiSecretDemo.toUtf8(),
        ApiSettings::getInstance().getbApi()->httpParams.recvWindow.toUtf8(),
        ApiSettings::getInstance().getbApi()->httpParams.demoNetBaseUrl.toUtf8()
    };

    HttpManager gpmanager(params);
    QObject::connect(&gpmanager, &HttpManager::requestCompleted, HttpResponse);
    QObject::connect(&gpmanager, &HttpManager::requestFailed, HttpResponseFail);

    gpmanager.get(QJsonDocument(parametersGetKline), getKlineUrl);
    //gpmanager.get(QJsonDocument(), "market/time");
}

#endif // TESTS_H

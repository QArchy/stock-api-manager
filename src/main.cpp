#include "src/controllers/Settings/apiSettings.h"
#include "src/controllers/Logger/logger.h"
#include "src/controllers/Translator/translator.h"
#include "src/models/GetPostManager/getpostmanager.h"
#include "src/models/networkSingleton.h"
#include "src/models/WebSocketManager/websocketmanager.h"
#include "src/ui/mainwindow/mainwindow.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>

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

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Logger::install();

    //QJsonObject parametersPost;
    //parametersPost["category"] = "linear";
    //parametersPost["symbol"] = "BTCUSDT";
    //parametersPost["side"] = "Buy";
    //parametersPost["positionIdx"] = 0;
    //parametersPost["orderType"] = "Limit";
    //parametersPost["qty"] = "0.001";
    //parametersPost["price"] = "18900";
    //parametersPost["timeInForce"] = "GTC";
    //QString postUrl("order/create");

    //QJsonObject parametersGet;
    //parametersGet["category"] = "linear";
    //parametersGet["symbol"] = "BTCUSDT";
    //QString getUrl("order/realtime?");

    //QJsonObject parametersGetKline;
    //parametersGetKline["category"] = "linear";
    //parametersGetKline["symbol"] = "BTCUSDT";
    //parametersGetKline["interval"] = "60";

    //QDateTime dtStart = QDateTime::fromString("2025-03-17 06:00:00:000", "yyyy-MM-dd hh:mm:ss:zzz");
    //dtStart.setTimeSpec(Qt::UTC);
    //qint64 msStart = dtStart.toMSecsSinceEpoch();
    //parametersGetKline["start"] = QString::number(msStart);

    //QDateTime dtEnd = QDateTime::fromString("2025-03-17 09:00:00:000", "yyyy-MM-dd hh:mm:ss:zzz");
    //dtEnd.setTimeSpec(Qt::UTC);
    //qint64 msEnd = dtEnd.toMSecsSinceEpoch();
    //parametersGetKline["end"] = QString::number(msEnd);

    //parametersGetKline["limit"] = "4";
    //QString getKlineUrl("market/kline?");

    //GetPostManager gpmanager(
    //    APISettings::getApiKeyDemo(),
    //    APISettings::getApiSecretDemo(),
    //    APISettings::getRecvWindow(),
    //    APISettings::getApiDemoBaseUrl()
    //);
    //QObject::connect(Network::getInstance().getManager(), &QNetworkAccessManager::finished, onFinished);

    //gpmanager.post(QJsonDocument(parametersPost), postUrl);
    //gpmanager.get(QJsonDocument(parametersGet), getUrl);
    //gpmanager.customGet(QJsonDocument(parametersGetKline), getKlineUrl);
    //gpmanager.customGet(QJsonDocument(), "market/time");

    // dateTimeTest(false);

    WebSocketManager manager("", "", "wss://stream.bybit.com/v5/public/linear");
    // Connect to server
    manager.connectToServer();

    // When connected, send subscription
    QObject::connect(&manager, &WebSocketManager::connected, [&]() {
        QJsonObject subMsg;
        subMsg["op"] = "subscribe";
        subMsg["args"] = QJsonArray() << "kline.1.BTCUSDT";
        manager.sendMessage(QJsonDocument(subMsg));
    });

    // Handle incoming kline data
    QObject::connect(&manager, &WebSocketManager::messageReceived, [](const QJsonDocument &doc) {
        qDebug() << "Kline Update:" << doc.toJson();
    });

    translation(a);
    //MainWindow w; w.show();

    return a.exec();
}

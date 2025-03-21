#include "src/controllers/Translator/translator.h"
#include "src/controllers/Logger/logger.h"
#include "src/ui/mainwindow/mainwindow.h"
#include "src/network/WebSocketManager/websocketmanager.h"
#include "src/controllers/Settings/apiSettings.h"
#include "src/tests/loggerTest/loggerworker.h"
#include <QApplication>
#include <QThread>

//#include <QMap>
//
//// Local orderbook storage structure
//struct OrderBook {
//    QMap<double, double> bids; // Key: price, Value: quantity (sorted descending)
//    QMap<double, double> asks; // Key: price, Value: quantity (sorted ascending)
//    qint64 lastUpdateId = 0;
//};
//
//OrderBook g_orderbook;
//
//void handleOrderbookUpdate(const QJsonDocument& doc) {
//    const QJsonObject data = doc.object()["data"].toObject();
//    const QString type = doc.object()["type"].toString();
//    const qint64 updateId = data["u"].toVariant().toLongLong();
//    const QString symbol = data["s"].toString();
//
//    if (type == "snapshot") {
//        // Reset orderbook
//        g_orderbook.bids.clear();
//        g_orderbook.asks.clear();
//
//        // Process bids (sorted descending)
//        for (const auto& bid : data["b"].toArray()) {
//            const auto entry = bid.toArray();
//            double price = entry[0].toString().toDouble();
//            double qty = entry[1].toString().toDouble();
//            if (qty > 0) g_orderbook.bids.insert(-price, qty); // Negative for descending
//        }
//
//        // Process asks (sorted ascending)
//        for (const auto& ask : data["a"].toArray()) {
//            const auto entry = ask.toArray();
//            double price = entry[0].toString().toDouble();
//            double qty = entry[1].toString().toDouble();
//            if (qty > 0) g_orderbook.asks.insert(price, qty);
//        }
//
//        g_orderbook.lastUpdateId = updateId;
//        qDebug() << "Snapshot processed for" << symbol << "Update ID:" << updateId;
//    }
//    else if (type == "delta") {
//        // Check sequence consistency
//        if (updateId <= g_orderbook.lastUpdateId) {
//            qFatal() << "Out-of-order delta received";
//            return;
//        }
//
//        if (updateId > g_orderbook.lastUpdateId + 1) {
//            qFatal() << "Out-of-order delta received";
//            return;
//        }
//
//        // Process bid updates
//        for (const auto& bid : data["b"].toArray()) {
//            const auto entry = bid.toArray();
//            double price = entry[0].toString().toDouble();
//            double qty = entry[1].toString().toDouble();
//
//            if (qty == 0) {
//                g_orderbook.bids.remove(-price);
//            } else {
//                g_orderbook.bids.insert(-price, qty);
//            }
//        }
//
//        // Process ask updates
//        for (const auto& ask : data["a"].toArray()) {
//            const auto entry = ask.toArray();
//            double price = entry[0].toString().toDouble();
//            double qty = entry[1].toString().toDouble();
//
//            if (qty == 0) {
//                g_orderbook.asks.remove(price);
//            } else {
//                g_orderbook.asks.insert(price, qty);
//            }
//        }
//
//        g_orderbook.lastUpdateId = updateId;
//        qDebug() << "Delta processed for" << symbol << "Update ID:" << updateId;
//    }
//
//    // Optional: Print top 5 bids/asks
//    qDebug() << "Top 5 of " << g_orderbook.bids.size() << "Bids:";
//    auto bidIt = g_orderbook.bids.constBegin();
//    for (int i = 0; i < 5 && bidIt != g_orderbook.bids.constEnd(); ++i, ++bidIt) {
//        qDebug() << -bidIt.key() << ":" << bidIt.value();
//    }
//
//    qDebug() << "Top 5 of " << g_orderbook.asks.size() << "Asks:";
//    auto askIt = g_orderbook.asks.constBegin();
//    for (int i = 0; i < 5 && askIt != g_orderbook.asks.constEnd(); ++i, ++askIt) {
//        qDebug() << askIt.key() << ":" << askIt.value();
//    }
//}
//
//void onMessageReceied(const QJsonDocument& doc) {
//    static QElapsedTimer timerCalc;
//    static QElapsedTimer fCallTimer;
//    qDebug() << "New fCall:\t" << fCallTimer.elapsed() << "ms";
//
//    qDebug() << "Msg sent:\t" << QDateTime::fromMSecsSinceEpoch(doc.object()["ts"].toInteger()).toUTC();
//    qDebug() << "Msg received:\t" << QDateTime::currentDateTimeUtc();
//    qDebug() << "Msg latency:\t" <<
//        QDateTime::currentDateTimeUtc() - QDateTime::fromMSecsSinceEpoch(doc.object()["ts"].toInteger()).toUTC();
//
//    if (doc.object()["topic"].toString().startsWith("orderbook")) {
//        timerCalc.start();
//        //handleOrderbookUpdate(doc);
//        qDebug() << doc;
//        qDebug() << "Calc time:\t" << timerCalc.elapsed() << "ms\n\n";
//    } else {
//        timerCalc.start();
//        qDebug() << '\n' << doc;
//        qDebug() << "Calc time:\t" << timerCalc.elapsed() << "ms\n\n";
//    }
//
//    fCallTimer.start();
//}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    translation(a);
    Logger::initialize();

    QThread* thread[5];
    LoggerWorker* worker[5];
    for (int i = 0; i < 5; i++) {
        thread[i] = new QThread();
        worker[i] = new LoggerWorker(i);
        worker[i]->moveToThread(thread[i]);
        QObject::connect(thread[i], &QThread::started, worker[i], &LoggerWorker::doWork);
        thread[i]->start();
    }

    //WebSocketParameters param;
    //param.apiKey = ApiSettings::getInstance().getbApi()->keys.apiKeyDemo.toUtf8();
    //param.apiSecret = ApiSettings::getInstance().getbApi()->keys.apiSecretDemo.toUtf8();
    //param.baseUrl = ApiSettings::getInstance().getbApi()->wsParams.publicUrl.toUtf8();
    //param.isPrivate = false;

    //WebSocketManager* wsManager = new WebSocketManager(param);
    //wsManager->connectToServer(QJsonDocument(), "linear");

    //QObject::connect(wsManager, &WebSocketManager::connected, [wsManager]() {
    //    qDebug() << "Connected to WebSocket, subscribing to orderbook...";

    //    QJsonObject subMsg;
    //    subMsg["op"] = "subscribe";
    //    subMsg["args"] = QJsonArray() << "orderbook.500.BTCUSDT";
    //    //subMsg["args"] = QJsonArray() << "tickers.BTCUSDT";

    //    if (!wsManager->sendMessage(QJsonDocument(subMsg))) {
    //        qCritical() << "Failed to send subscription message";
    //    }
    //});

    //QObject::connect(wsManager, &WebSocketManager::messageReceived, onMessageReceied);

    //QObject::connect(wsManager, &WebSocketManager::errorOccurred, [](const QString& error) {
    //    qCritical() << "WebSocket error:" << error;
    //});

    //MainWindow w;
    //w.show();
    return a.exec();
}

#include "src/controllers/Translator/translator.h"
#include "src/controllers/Logger/logger.h"
#include "src/dao/OrderBook/orderbook.h"
#include "src/ui/mainwindow/mainwindow.h"
#include "src/network/WebSocketManager/websocketmanager.h"
#include "src/controllers/Settings/apiSettings.h"
#include "src/tests/loggerTest/loggerworker.h"
#include "src/models/obanalyzer.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    translation(a);
    Logger::initialize();

    OrderBook* ob = new OrderBook("linear.BTCUSDT", 500);
    obAnalyzer* oba = new obAnalyzer();
    QObject::connect(ob, &OrderBook::updated, oba, &obAnalyzer::analyzeOB);

    QObject::connect(oba, &obAnalyzer::buySignal, [](float price, float amount) {
        // Implement buy logic here
        qDebug() << "Executing BUY order at" << price << "Amount:" << amount;
    });

    QObject::connect(oba, &obAnalyzer::sellSignal, [](float price, float amount) {
        // Implement sell logic here
        qDebug() << "Executing SELL order at" << price << "Amount:" << amount;
    });

    WebSocketParameters param;
    param.apiKey = ApiSettings::getInstance().getbApi()->keys.apiKeyDemo.toUtf8();
    param.apiSecret = ApiSettings::getInstance().getbApi()->keys.apiSecretDemo.toUtf8();
    param.baseUrl = ApiSettings::getInstance().getbApi()->wsParams.publicUrl.toUtf8();
    param.isPrivate = false;

    WebSocketManager* wsManager = new WebSocketManager(param);
    wsManager->connectToServer(QJsonDocument(), "linear");

    QObject::connect(wsManager, &WebSocketManager::connected, [wsManager]() {
        qDebug() << "Connected to WebSocket, subscribing to orderbook...";

        QJsonObject subMsg;
        subMsg["op"] = "subscribe";
        subMsg["args"] = QJsonArray() << "orderbook.500.BTCUSDT";

        if (!wsManager->sendMessage(QJsonDocument(subMsg))) {
            qCritical() << "Failed to send subscription message";
        }
    });

    QObject::connect(wsManager, &WebSocketManager::messageReceived, ob, &OrderBook::update);

    QObject::connect(wsManager, &WebSocketManager::errorOccurred, [](const QString& error) {
        qCritical() << "WebSocket error:" << error;
    });

    MainWindow w;
    //w.show();
    return a.exec();
}

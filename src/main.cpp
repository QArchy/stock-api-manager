#include "src/controllers/Translator/translator.h"
#include "src/controllers/Logger/logger.h"
#include "src/dao/OrderBook/orderbook.h"
#include "src/ui/mainwindow/mainwindow.h"
#include "src/network/WebSocketManager/websocketmanager.h"
#include "src/controllers/Settings/apiSettings.h"
#include "src/tests/loggerTest/loggerworker.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    translation(a);
    Logger::initialize();

    OrderBook* ob = new OrderBook("linear.BTCUSDT", 500);

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
        //subMsg["args"] = QJsonArray() << "tickers.BTCUSDT";

        if (!wsManager->sendMessage(QJsonDocument(subMsg))) {
            qCritical() << "Failed to send subscription message";
        }
    });

    QObject::connect(wsManager, &WebSocketManager::messageReceived, ob, &OrderBook::update);

    QObject::connect(wsManager, &WebSocketManager::errorOccurred, [](const QString& error) {
        qCritical() << "WebSocket error:" << error;
    });

    //MainWindow w;
    //w.show();
    return a.exec();
}

#include "src/controllers/Translator/translator.h"
#include "src/controllers/Logger/logger.h"
#include "src/dao/OrderBook/orderbook.h"
#include "src/ui/mainwindow/mainwindow.h"
#include "src/network/HttpManager/httpmanager.h"
#include "src/network/WebSocketManager/websocketmanager.h"
#include "src/controllers/Settings/apiSettings.h"
#include "src/tests/loggerTest/loggerworker.h"
#include "src/models/obanalyzer.h"
#include <QApplication>

QString currentOrderLinkId; // Track active orders
const QString TRADING_SYMBOL = "BTCUSDT";

void placeOrder (QString side, float price, float qty, HttpManager* httpManager) {
    static QString s_side = side;

    if (s_side == side)
        return;
    s_side = side;

    // 1. Cancel existing order
    if (!currentOrderLinkId.isEmpty()) {
        QJsonObject cancelParams{
            {"category", "linear"},
            {"symbol", TRADING_SYMBOL},
            {"orderLinkId", currentOrderLinkId}
        };
        httpManager->post(QJsonDocument(cancelParams), "order/cancel");
    }

    // 2. Place new order
    QString newOrderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    currentOrderLinkId = newOrderId;
    QJsonObject orderParams{
        {"category", "linear"},
        {"symbol", TRADING_SYMBOL},
        {"side", s_side},
        {"positionIdx", 0},
        {"orderType", "Market"},
        {"qty", "0.001"},  // 3 decimal places
        {"price", QString::number(price, 'f', 1)}, // 1 decimal place
        {"timeInForce", "GTC"},
        {"orderLinkId", newOrderId}
    };
    httpManager->post(QJsonDocument(orderParams), "order/create");
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    translation(a);
    Logger::initialize();

    // --- Order Management Setup ---

    OrderBook* ob = new OrderBook("linear.BTCUSDT", 500);
    obAnalyzer* oba = new obAnalyzer();
    QObject::connect(ob, &OrderBook::updated, oba, &obAnalyzer::analyzeOB);

    // HTTP Manager Setup
    HttpParameters httpParams;
    httpParams.apiKey = ApiSettings::getInstance().getbApi()->keys.apiKeyDemo.toUtf8();
    httpParams.apiSecret = ApiSettings::getInstance().getbApi()->keys.apiSecretDemo.toUtf8();
    httpParams.baseUrl = ApiSettings::getInstance().getbApi()->httpParams.demoNetBaseUrl.toUtf8();
    httpParams.recvWindow = ApiSettings::getInstance().getbApi()->httpParams.recvWindow.toUtf8();
    HttpManager* httpManager = new HttpManager(httpParams, &a);

    // --- Response Handling ---
    QObject::connect(httpManager, &HttpManager::requestCompleted, [&](const QJsonDocument& response) {
        QJsonObject obj = response.object();
        if (obj["retCode"].toInt() == 0) {
            QJsonObject result = obj["result"].toObject();
            if (result.contains("orderLinkId")) {
                //currentOrderLinkId = result["orderLinkId"].toString();
                qInfo() << "Order placed successfully. ID:" << currentOrderLinkId;
            }
        }
    });

    // Connect analyzer signals
    QObject::connect(oba, &obAnalyzer::buySignal, [&](float price, float amount) {
        qInfo() << "Attempting BUY order at" << price << "Qty:" << amount;
        placeOrder("Buy", price, amount, httpManager);
    });

    QObject::connect(oba, &obAnalyzer::sellSignal, [&](float price, float amount) {
        qInfo() << "Attempting SELL order at" << price << "Qty:" << amount;
        placeOrder("Sell", price, amount, httpManager);
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

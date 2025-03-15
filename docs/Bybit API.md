### Introduction
---
REST API Base Endpoint:
- ***Test-net*** 
	https://api-testnet.bybit.com
- ***Main-net (both endpoints are available):***
	https://api.bybit.com
	https://api.bytick.com
### Data
---
- Bybit data return type: Json
- Qt classes for Json manipulation:
	`QJsonDocument`
	- Used to **parse** raw JSON data from a `QString` or `QByteArray`.
	- Converts between **QJsonObject/QJsonArray** and a JSON-formatted string.
	`QJsonObject`
	- Represents a JSON **object** (key-value pairs).
	- You can **extract values** using `.value("key")` and **modify values** using `.insert("key", value)`.
	`QJsonArray`
	- Represents a JSON **array**.
	- You can **iterate over elements** using `for` loops or `.at(index)`.
	`QJsonValue`
	- Represents a **value inside JSON** (string, number, object, array, boolean).
	- Convert it to a Qt type using `.toString()`, `.toInt()`, `.toObject()`, `.toArray()`, etc.

```
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // JSON string (simulating API response)
    QString jsonString = R"({
        "retCode": 0,
        "retMsg": "OK",
        "result": {
            "total": 735,
            "list": [
                {
                    "title": "New Listing: Arbitrum (ARB) — Deposit, Trade and Stake ARB to Share a 400,000 USDT Prize Pool!",
                    "description": "Bybit is excited to announce the listing of ARB on our trading platform!",
                    "type": {
                        "title": "New Listings",
                        "key": "new_crypto"
                    },
                    "tags": ["Spot", "Spot Listings"],
                    "url": "https://announcements.bybit.com/en-US/article/new-listing-arbitrum-arb-deposit-trade-and-stake-arb-to-share-a-400-000-usdt-prize-pool--bltf662314c211a8616/",
                    "dateTimestamp": 1679045608000
                }
            ]
        },
        "time": 1679415136117
    })";

    // Convert JSON string to QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (doc.isNull()) {
        qWarning() << "Failed to parse JSON!";
        return -1;
    }

    // Extract the root JSON object
    QJsonObject rootObj = doc.object();

    // Retrieve basic info
    int retCode = rootObj.value("retCode").toInt();
    QString retMsg = rootObj.value("retMsg").toString();
    qDebug() << "Return Code:" << retCode;
    qDebug() << "Return Message:" << retMsg;

    // Extract "result" object
    QJsonObject resultObj = rootObj.value("result").toObject();
    int total = resultObj.value("total").toInt();
    qDebug() << "Total Announcements:" << total;

    // Extract "list" array
    QJsonArray listArray = resultObj.value("list").toArray();
    for (const QJsonValue &value : listArray) {
        QJsonObject announcement = value.toObject();
        qDebug() << "\nTitle:" << announcement.value("title").toString();
        qDebug() << "Description:" << announcement.value("description").toString();
        qDebug() << "URL:" << announcement.value("url").toString();

        // Extract "type" object
        QJsonObject typeObj = announcement.value("type").toObject();
        qDebug() << "Category:" << typeObj.value("title").toString();

        // Extract tags array
        QJsonArray tagsArray = announcement.value("tags").toArray();
        QStringList tags;
        for (const QJsonValue &tagValue : tagsArray) {
            tags.append(tagValue.toString());
        }
        qDebug() << "Tags:" << tags.join(", ");

        // Extract timestamp
        qint64 timestamp = announcement.value("dateTimestamp").toVariant().toLongLong();
        qDebug() << "Timestamp:" << timestamp;
    }

    return app.exec();
}
```
### API
---
#### API Rate limit
`GET`/`POST` method (shared) on all nets combined (**Main-net, Test-net**):
- No more than `600` requests are allowed in any `5-second` window.
Every `GET`/`POST` command has it's own limits, see https://bybit-exchange.github.io/docs/v5/rate-limit
What does it mean for API implementation:
- For each command there is a command counter that counts **command limit** according to documentation and resets every `second/minute`
- For all commands there is a counter that counts all **commands limit** (600) and resets every `5 seconds`
- The command blocking algorithm due to the limit reach must work like **Swiss clock**
#### API Data flow
##### HTTP API (GET/POST requests)
- Used for **request-response interactions** (e.g., fetching market data, placing/canceling orders, retrieving account balances).
- Qt classes for HTTP manipulation:
	`QNetworkAccessManager`
	- The main class for handling network requests (GET, POST, etc.).
	- You create an instance and use it to send requests.
	`QNetworkRequest`
	- Represents an HTTP request (URL, headers, etc.).
	`QNetworkReply`
	- Holds the response received from the server.
	- You connect its `finished()` signal to handle the response.
- GET request example
```
void YourClass::fetchMarketData() {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.bybit.com/v2/public/tickers"));
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [reply]() {
        QByteArray response = reply->readAll();
        qDebug() << "Response:" << response;
        reply->deleteLater();
    });
}
```
- POST request example
```
void YourClass::placeOrder() {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.bybit.com/v2/private/order/create"));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["symbol"] = "BTCUSDT";
    json["side"] = "Buy";
    json["order_type"] = "Limit";
    json["qty"] = 0.1;
    json["price"] = 40000;

    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [reply]() {
        QByteArray response = reply->readAll();
        qDebug() << "Response:" << response;
        reply->deleteLater();
    });
}

```
##### WebSockets
- Used for **real-time updates** (e.g., price changes, order book updates, live trade execution).
- Qt classes for WebSockets manipulation:
	`QWebSocket`
	- Connects to Bybit WebSocket API (`wss://stream.bybit.com/v5/public/spot`).
	- Sends and receives real-time data from Bybit.

```
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class BybitWebSocket : public QObject {
    Q_OBJECT

public:
    BybitWebSocket(QObject *parent = nullptr) : QObject(parent) {
        connect(&socket, &QWebSocket::connected, this, &BybitWebSocket::onConnected);
        connect(&socket, &QWebSocket::textMessageReceived, this, &BybitWebSocket::onMessageReceived);

        // Connect to Bybit WebSocket API
        socket.open(QUrl("wss://stream.bybit.com/v5/public/spot"));
    }

private slots:
    void onConnected() {
        qDebug() << "Connected to Bybit WebSocket!";
        
        // JSON request to subscribe to live BTCUSDT ticker updates
        QJsonObject request;
        request["op"] = "subscribe";
        request["args"] = QJsonArray{"tickers.BTCUSDT"};

        socket.sendTextMessage(QJsonDocument(request).toJson());
    }

    void onMessageReceived(const QString &message) {
        qDebug() << "Received:" << message;
    }

private:
    QWebSocket socket;
};

```
### Links and sources
---
1. https://bybit-exchange.github.io/docs/v5/intro
2. https://bybit-exchange.github.io/docs/v5/rate-limit

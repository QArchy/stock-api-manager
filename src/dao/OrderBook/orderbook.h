#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "src/libs/Eigen/Dense"
#include <QObject>

struct OrderBookAsks {
    float val[500]; // String
    float qty[500]; // String
};

struct OrderBookBids {
    float val[500]; // String
    float qty[500]; // String
};

struct OrderBookData {
    qint64 ts; // Number
    OrderBookAsks a;
    OrderBookBids b;
    QString s; // String
    qint64 seq; // Integer
    qint64 u; // Integer
};

class OrderBook : public QObject {
    Q_OBJECT
public:
    explicit OrderBook(QObject* parent = nullptr);
public slots:
    void handleOrderbookUpdate(const QJsonDocument& doc);
private:
    OrderBookData* m_obCurrent;
    OrderBookData* m_obOld;
};

#endif // ORDERBOOK_H

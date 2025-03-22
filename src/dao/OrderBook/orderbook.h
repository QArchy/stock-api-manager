#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "src/libs/Eigen/Dense"
#include <QObject>

#pragma pack(push, 16)  // 16-byte alignment for SSE/AVX
struct OrderBookSide {
    alignas(16) float val[500];  // C++11 alignment
    alignas(16) float qty[500];
};
#pragma pack(pop)

struct OrderBookData {
    qint64 ts; // Number
    OrderBookSide a;
    OrderBookSide b;
    QString s; // String
    qint64 seq; // Integer
    qint64 u; // Integer
};

class OrderBook : public QObject {
    Q_OBJECT
public:
    explicit OrderBook(QObject* parent = nullptr);
public slots:
    __attribute__((hot))
    void handleOrderbookUpdate(const QJsonDocument& doc);
private:
    OrderBookData* m_obCurrent;
    OrderBookData* m_obOld;
};

#endif // ORDERBOOK_H

#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <Eigen/Core>
#include <QObject>

struct OrderBookSide {
    Eigen::VectorXf val;
    Eigen::VectorXf qty;
    OrderBookSide(qint16 depth) : val(depth), qty(depth) {}
};

struct OrderBookData {
    OrderBookSide* a;
    OrderBookSide* b;
    qint64 seq;
    qint64 ts;
    qint64 u;
    OrderBookData(qint16 depth) : a(new OrderBookSide(depth)), b(new OrderBookSide(depth)) {}
    ~OrderBookData() { delete a; delete b; }
};

class OrderBook : public QObject {
    Q_OBJECT
public:
    explicit OrderBook(QString currency, qint16 depth, QObject* parent = nullptr);
    ~OrderBook();
public slots:
    void update(const QJsonDocument& doc);
signals:
    void updated(const OrderBookData* data);
public:
    QString getCurrency() const;
    qint16 getDepth() const;
private:
    void snapshotSide(QJsonArray& side, OrderBookSide *obSide);
    void deltaSide(QJsonArray& side, OrderBookSide *obSideCurrent, OrderBookSide *obSideOld, bool bid);
    #ifdef DEBUG_DAO_LIB
    bool validCheckSide(OrderBookSide *obSide, bool bid);
    void printSide(OrderBookSide *obSide);
    #endif
    OrderBookData* m_obCurrent;
    OrderBookData* m_obOld;
    QString m_currency;
    qint16 m_depth;
};

#endif // ORDERBOOK_H

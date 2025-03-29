#include "orderbook.h"
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

OrderBook::OrderBook(QString currency, qint16 depth, QObject *parent) : QObject(parent) {
    m_obCurrent = new OrderBookData(depth);
    m_obOld = new OrderBookData(depth);
    m_currency = currency;
    m_depth = depth;
}

OrderBook::~OrderBook() {
    delete m_obCurrent;
    delete m_obOld;
}

void OrderBook::update(const QJsonDocument &doc) {
    #ifdef DEBUG_DAO_LIB
    static QElapsedTimer timerCalc;
    static QElapsedTimer fCallTimer;
    qDebug() << doc;
    #endif

    QJsonObject dObj = doc.object();
    if (!(dObj["topic"].toString().startsWith("orderbook")))
        return;
    QJsonObject objData = dObj["data"].toObject();

    #ifdef DEBUG_DAO_LIB
    qDebug() << "New fCall:\t" << fCallTimer.elapsed() << "ms";
    qDebug() << "Msg sent:\t" << QDateTime::fromMSecsSinceEpoch(dObj["ts"].toInteger()).toUTC();
    qDebug() << "Msg received:\t" << QDateTime::currentDateTimeUtc();
    qDebug() << "Msg latency:\t" << QDateTime::currentDateTimeUtc() - QDateTime::fromMSecsSinceEpoch(dObj["ts"].toInteger()).toUTC();
    timerCalc.start();
    #endif

    m_obCurrent->seq = objData["seq"].toVariant().toLongLong();
    m_obCurrent->ts = dObj["ts"].toVariant().toLongLong();
    qint64 newU = objData["u"].toVariant().toLongLong();
    bool needUpdate = (m_obCurrent->u != (newU - 1)) && (m_obCurrent->u != 1);
    m_obCurrent->u = newU;

    QJsonArray bids = objData["b"].toArray();
    QJsonArray asks = objData["a"].toArray();

    if (dObj["type"].toString() == "snapshot" || needUpdate) {
        snapshotSide(bids, m_obCurrent->b);
        snapshotSide(asks, m_obCurrent->a);
        #ifdef DEBUG_DAO_LIB
        if (!validCheckSide(m_obCurrent->b, true)) {
            qDebug() << "Bids snapshot update:";
            printSide(m_obCurrent->b);
            qFatal() << "Bids snapshot are not valid";
        }
        if (!validCheckSide(m_obCurrent->a, false)) {
            qDebug() << "Asks snapshot update:";
            printSide(m_obCurrent->a);
            qFatal() << "Asks snapshot are not valid";
        }
        #endif
    } else {
        deltaSide(bids, m_obCurrent->b, m_obOld->b, true);
        std::swap(m_obCurrent->b, m_obOld->b);
        deltaSide(asks, m_obCurrent->a, m_obOld->a, false);
        std::swap(m_obCurrent->a, m_obOld->a);
        #ifdef DEBUG_DAO_LIB
        if (!validCheckSide(m_obCurrent->b, true)) {
            qDebug() << "Bids delta update:";
            printSide(m_obCurrent->b);
            qFatal() << "Bids delta are not valid";
        }
        if (!validCheckSide(m_obCurrent->a, false)) {
            qDebug() << "Asks delta update:";
            printSide(m_obCurrent->a);
            qFatal() << "Asks delta are not valid";
        }
        #endif
    }
    emit updated(m_obCurrent);

    #ifdef DEBUG_DAO_LIB
    qDebug() << "Calc time:\t" << timerCalc.elapsed() << "ms\n\n";
    fCallTimer.start();
    #endif
}

QString OrderBook::getCurrency() const { return m_currency; }

qint16 OrderBook::getDepth() const { return m_depth; }

void OrderBook::snapshotSide(QJsonArray &side, OrderBookSide *obSide) {
    for (int i = 0; i < m_depth; i++) {
        obSide->val[i] = side[i][0].toString().toFloat();
        obSide->qty[i] = side[i][1].toString().toFloat();
    }
}

void OrderBook::deltaSide(QJsonArray &side, OrderBookSide *obSideCurrent, OrderBookSide *obSideOld, bool bid) {
    int j = 0, k = 0;
    for (int i = 0; i < m_depth; i++) { /* comparison with existing elements */
        if (j == side.size()) { /* no more updates, just copy the last elements */
            std::memcpy(&obSideOld->val[k], &obSideCurrent->val[i], sizeof(obSideCurrent->val[i]) * (m_depth - i));
            std::memcpy(&obSideOld->qty[k], &obSideCurrent->qty[i], sizeof(obSideCurrent->qty[i]) * (m_depth - i));
            break;
        }

        float new_val = side[j][0].toString().toFloat();
        float new_qty = side[j][1].toString().toFloat();
        bool orderBroken = bid ? (obSideCurrent->val[i] <= new_val) : (obSideCurrent->val[i] >= new_val);

        if (orderBroken)
        { /* order broken */
            if (obSideCurrent->val[i] == new_val)
            { /* update qty or skip zero qty entry */
                if (new_qty != 0)
                { /* update qty */
                    obSideOld->val[k] = obSideCurrent->val[i];
                    obSideOld->qty[k] = new_qty;
                    k++;
                } /* else - skip outdated entry */
            } else
            { /* add new_val frist, make i - 1 for comparison with next update */
                obSideOld->val[k] = new_val;
                obSideOld->qty[k] = new_qty;
                k++;
                i--;
            }
            j++; /* poceed to the next update */
        }
        else
        { /* order is kept, add element */
            obSideOld->val[k] = obSideCurrent->val[i];
            obSideOld->qty[k] = obSideCurrent->qty[i];
            k++;
        }
    }
    for (/* j = previous */; j < side.size(); j++) { /* insertion of new values that are left after comparison */
        obSideOld->val[k] = side[j][0].toString().toFloat();
        obSideOld->qty[k] = side[j][1].toString().toFloat();
        k++;
    }
}

#ifdef DEBUG_DAO_LIB
bool OrderBook::validCheckSide(OrderBookSide *obSide, bool bid) {
    float tmp = obSide->val[0];
    bool valid = true;
    for (int i = 1; i < m_depth; i++) {
        if (bid) {
            if (tmp <= obSide->val[i]) {
                return false;
            }
        } else {
            if (tmp >= obSide->val[i]) {
                return false;
            }
        }
        if (obSide->val[i] == 0) {
            return false;
        }
        if (obSide->qty[i] == 0) {
            return false;
        }
        if (obSide->qty[i] > 1000) {
            return false;
        }
        tmp = obSide->val[i];
    }
    return valid;
}

void OrderBook::printSide(OrderBookSide *obSide) {
    QString out = "";
    for (int i = 0; i < m_depth; i++) {
        out += "[" + QString::number(obSide->val[i]) + " : " + QString::number(obSide->qty[i]) + "] ";
    }
    qDebug() << out;
}
#endif

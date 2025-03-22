#include "orderbook.h"
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

OrderBook::OrderBook(QObject *parent) : QObject(parent), m_obCurrent(new OrderBookData()), m_obOld(new OrderBookData()) {}

void OrderBook::handleOrderbookUpdate(const QJsonDocument &doc) {
    static QElapsedTimer timerCalc;
    static QElapsedTimer fCallTimer;
    qDebug() << "New fCall:\t" << fCallTimer.elapsed() << "ms";

    qDebug() << "Msg sent:\t\t" << QDateTime::fromMSecsSinceEpoch(doc.object()["ts"].toInteger()).toUTC();
    qDebug() << "Msg received:\t" << QDateTime::currentDateTimeUtc();
    qDebug() << "Msg latency:\t" <<
        QDateTime::currentDateTimeUtc() - QDateTime::fromMSecsSinceEpoch(doc.object()["ts"].toInteger()).toUTC();
    timerCalc.start();


    QJsonObject dObj = doc.object();
    if (!doc["topic"].toString().startsWith("orderbook"))
        return;

    QJsonObject objData = dObj["data"].toObject();

    m_obCurrent->s = objData["s"].toString();
    m_obCurrent->seq = objData["seq"].toVariant().toLongLong();
    m_obCurrent->ts = dObj["ts"].toVariant().toLongLong();

    qint64 newU = objData["u"].toVariant().toLongLong();
    bool needUpdate = (m_obCurrent->u != (newU - 1)) && (m_obCurrent->u != 1);
    m_obCurrent->u = newU;

    QString type = dObj["topic"].toString();
    QJsonArray bids = objData["b"].toArray();
    QJsonArray asks = objData["a"].toArray();

    if (type == "snapshot" || needUpdate) {
        #pragma GCC unroll 4
        for (int i = 0; i < 500; i++) {
            float new_bVal = bids[i][0].toString().toFloat();
            float new_bQty = bids[i][1].toString().toFloat();
            m_obCurrent->b.val[i] = new_bVal;
            m_obCurrent->b.qty[i] = new_bQty;
        }
        #pragma GCC unroll 4
        for (int i = 0; i < 500; i++) {
            float new_aVal = bids[i][0].toString().toFloat();
            float new_aQty = bids[i][1].toString().toFloat();
            m_obCurrent->a.val[i] = new_aVal;
            m_obCurrent->a.qty[i] = new_aQty;
        }
        qDebug() << "Snapshot processed for" << m_obCurrent->s << "Update ID:" << m_obCurrent->u;
    } else {
        #pragma GCC ivdep
        for (int i = 0, j = 0, k = 0; i < 500; i++) {
            if (j == bids.size()) { /* no more updates, just copy the last elements */
                std::memcpy(&m_obOld->b.val[k], &m_obCurrent->b.val[i], sizeof(m_obCurrent->b.val[i]) * (500 - i));
                std::memcpy(&m_obOld->b.qty[k], &m_obCurrent->b.qty[i], sizeof(m_obCurrent->b.qty[i]) * (500 - i));
                break;
            }

            float new_bVal = bids[j][0].toString().toFloat();
            float new_bQty = bids[j][1].toString().toFloat();

            if (m_obCurrent->b.val[i] <= new_bVal)
            { /* descending order broken */
                if (m_obCurrent->b.val[i] == new_bVal)
                { /* update qty or skip zero qty entry */
                    if (new_bQty != 0)
                    { /* update qty */
                        m_obOld->b.val[k] = m_obCurrent->b.val[i];
                        m_obOld->b.qty[k] = new_bQty;
                        k++;
                    } /* else - skip outdated entry */
                } else
                { /* add new_bVal frist, make i - 1 for comparison with next update */
                    m_obOld->b.val[k] = new_bVal;
                    m_obOld->b.qty[k] = new_bQty;
                    k++;
                    i--;
                }
                j++; /* poceed to the next update */
            }
            else
            { /* descending order is kept, add element */
                m_obOld->b.val[k] = m_obCurrent->b.val[i];
                m_obOld->b.qty[k] = m_obCurrent->b.qty[i];
                k++;
            }
        }
        /* swap new and old bids */
        std::swap(m_obOld->b, m_obCurrent->b);

        // ... nearly similar algo for asks
        #pragma GCC ivdep
        for (int i = 0, j = 0; i < 500; i++) {
            float new_aVal = asks[j][0].toString().toFloat();
            float new_aQty = asks[j][1].toString().toFloat();
            // ...
        }
        qDebug() << "Delta processed for" << m_obCurrent->s << "Update ID:" << m_obCurrent->u;
    }

    qDebug() << doc;
    qDebug() << "Calc time:\t" << timerCalc.elapsed() << "ms\n\n";
    fCallTimer.start();
}

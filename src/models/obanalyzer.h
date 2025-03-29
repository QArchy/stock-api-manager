// obanalyzer.h
#ifndef OBANALYZER_H
#define OBANALYZER_H

#include <OrderBook/orderbook.h>
#include <QObject>

class obAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit obAnalyzer(QObject *parent = nullptr);

signals:
    void buySignal(float price, float amount);
    void sellSignal(float price, float amount);

public slots:
    void analyzeOB(const OrderBookData* data);

private:
    // Analysis parameters
    const float IMBALANCE_THRESHOLD = 1.5f;  // 150% ratio for significant imbalance
    const int LOOKBACK_PERIOD = 5;           // Number of entries to consider for momentum
    const float MIN_VOLUME = 10.0f;          // Minimum quantity to consider

    // State tracking
    float lastMidPrice = 0;
    QVector<float> priceHistory;
};
#endif // OBANALYZER_H

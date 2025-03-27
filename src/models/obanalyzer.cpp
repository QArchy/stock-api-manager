// obanalyzer.cpp
#include "obanalyzer.h"
#include <QDebug>
#include <numeric>

obAnalyzer::obAnalyzer(QObject *parent) : QObject{parent} {}

void obAnalyzer::analyzeOB(const OrderBookData* data) {
    if(!data || !data->b || !data->a) return;

    // bids are sorted DESCENDING (data->b->val[0] = best bid)
    // asks are sorted ASCENDING (data->a->val[0] = best ask)
    const OrderBookSide* bids = data->b;
    const OrderBookSide* asks = data->a;

    const int depthToConsider = bids->val.size() * 0.1;

    // 1. Calculate order book imbalance using top 5 levels
    float bidVolume = 0, askVolume = 0;
    for(int i = 0; i < depthToConsider; i++) {
        bidVolume += bids->qty[i];
        askVolume += asks->qty[i];
    }
    const float imbalanceRatio = bidVolume / (bidVolume + askVolume);

    // 2. Calculate spread and mid-price
    const float bestBid = bids->val[0];
    const float bestAsk = asks->val[0];
    const float spread = bestAsk - bestBid;
    const float midPrice = (bestBid + bestAsk) / 2;

    // 3. Calculate VWAP for market depth
    float bidVWAP = 0, askVWAP = 0;
    float totalBidQty = 0, totalAskQty = 0;

    // Calculate for first 10% of depth (most liquid area)
    for(int i = 0; i < depthToConsider; i++) {
        bidVWAP += bids->val[i] * bids->qty[i];
        totalBidQty += bids->qty[i];

        askVWAP += asks->val[i] * asks->qty[i];
        totalAskQty += asks->qty[i];
    }
    bidVWAP /= totalBidQty;
    askVWAP /= totalAskQty;

    // 4. Generate signals
    const bool strongBidPressure = imbalanceRatio > 0.7 && (midPrice > bidVWAP);
    const bool strongAskPressure = imbalanceRatio < 0.3 && (midPrice < askVWAP);

    if(strongBidPressure && spread < (midPrice * 0.0005)) {  // Tight spread
        const float targetPrice = bestAsk;  // Market buy at best ask
        emit buySignal(targetPrice, bidVolume);
        qDebug() << "STRONG BUY at" << targetPrice
                 << "Imbalance:" << imbalanceRatio
                 << "VWAP Diff:" << (midPrice - bidVWAP);
    }
    else if(strongAskPressure && spread < (midPrice * 0.0005)) {
        const float targetPrice = bestBid;  // Market sell at best bid
        emit sellSignal(targetPrice, askVolume);
        qDebug() << "STRONG SELL at" << targetPrice
                 << "Imbalance:" << imbalanceRatio
                 << "VWAP Diff:" << (askVWAP - midPrice);
    }
}

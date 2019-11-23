#ifndef EXCHANGE_BINANCE_H
#define EXCHANGE_BINANCE_H

#include "exchange.h"

class Exchange_Binance : public Exchange {

    Q_OBJECT

public:
    Exchange_Binance();

public slots:
    void startWork();

private:
    Downloader downloader;

    QString apiKey;
    QString apiSecret;

    qint64 deltaTime{};

    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair);
    void updateMarketTrades(QString pair);

    void updateBalances    ();
    void createOrder       (QString pair, int type, double rate, double amount);
    void cancelOrder       (qint64 orderID);
    void updateActiveOrders(QString pair);
    void updateOrderInfo   (qint64 orderID);

    Ticker parseRawTickerData(QNetworkReply *reply);
    void   parseRawDepthData (QNetworkReply *reply);
    void   parseRawTradesData(QNetworkReply *reply);

    void   parseRawBalancesData        (QNetworkReply *reply);
    qint64 parseRawOrderCreationData   (QNetworkReply *reply);
    void   parseRawOrderCancelationData(QNetworkReply *reply);
    void   parseRawActiveOrdersData    (QNetworkReply *reply);
    int    parseRawOrderInfoData       (QNetworkReply *reply);

    bool exchangeErrorCheck(QJsonObject *jsonObj);

    double roundUp  (double value);
    double roundDown(double value);
};

#endif // EXCHANGE_BINANCE_H

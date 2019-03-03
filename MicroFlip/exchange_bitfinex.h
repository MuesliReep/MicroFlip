#ifndef EXCHANGE_BITFINEX_H
#define EXCHANGE_BITFINEX_H

#include "exchange.h"

class Exchange_bitfinex : public Exchange {

    Q_OBJECT

public:
    Exchange_bitfinex();

public slots:
    void startWork();

private:
    Downloader downloader;

    QString apiKey;
    QString apiSecret;

    uint    lastNonce;
    QString createNonce();

    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair) ;
    void updateMarketTrades(QString pair);

    void updateBalances    ();
    void createOrder       (QString Pair, int Type, double Rate, double Amount);
    void cancelOrder       (qint64 orderID);
    void updateActiveOrders(QString pair);
    void updateOrderInfo   (qint64 OrderID);

    Ticker parseRawTickerData(QNetworkReply *reply);
    void   parseRawDepthData (QNetworkReply *reply);
    void   parseRawTradesData(QNetworkReply *reply);

    void    parseRawBalancesData        (QNetworkReply *reply);
    qint64  parseRawOrderCreationData   (QNetworkReply *reply);
    void    parseRawOrderCancelationData(QNetworkReply *reply);
    void    parseRawActiveOrdersData    (QNetworkReply *reply);
    int     parseRawOrderInfoData       (QNetworkReply *reply);

};

#endif // EXCHANGE_BITFINEX_H

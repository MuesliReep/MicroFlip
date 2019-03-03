#ifndef EXCHANGE_BITSTAMP_H
#define EXCHANGE_BITSTAMP_H

#include "exchange.h"

class Exchange_bitstamp : public Exchange {

    Q_OBJECT

public:
    Exchange_bitstamp();

public slots:
    void startWork();

private:
    Downloader downloader;

    QString apiKey;
    QString apiSecret;
    QString customerID;

    uint lastNonce{};
    void createNonce(QByteArray *nonce);

    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair) ;
    void updateMarketTrades(QString pair);

    void updateBalances    ();
    void createOrder       (QString pair, int type, double rate, double amount);
    void cancelOrder       (qint64 orderID);
    void updateActiveOrders(QString pair);
    void updateOrderInfo   (qint64 orderID);

    Ticker parseRawTickerData(QNetworkReply *reply);
    void   parseRawDepthData (QNetworkReply *reply);
    void   parseRawTradesData(QNetworkReply *reply);

    void    parseRawBalancesData        (QNetworkReply *reply);
    qint64  parseRawOrderCreationData   (QNetworkReply *reply);
    void    parseRawOrderCancelationData(QNetworkReply *reply);
    void    parseRawActiveOrdersData    (QNetworkReply *reply);
    int     parseRawOrderInfoData       (QNetworkReply *reply);

};

#endif // EXCHANGE_BITSTAMP_H

#ifndef EXCHANGE_SIM_H
#define EXCHANGE_SIM_H

#include "exchange.h"

class Exchange_Sim : public Exchange {

    Q_OBJECT

public:
    Exchange_Sim();

public slots:
    void startWork();

private:
    QString apiKey;
    QString apiSecret;

    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair);
    void updateMarketTrades(QString pair);

    void updateBalances    ();
    void createOrder       (QString pair, int type, double rate, double amount);
    void cancelOrder       (qint64 orderID);
    void updateActiveOrders(QString pair)   ;
    void updateOrderInfo   (qint64 orderID)   ;
};

#endif // EXCHANGE_SIM_H

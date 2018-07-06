#ifndef EXCHANGE_BINANCE_H
#define EXCHANGE_BINANCE_H

#include "exchange.h"

class Exchange_Binance : public Exchange
{
public:
    Exchange_Binance();

    // Exchange interface
public:
    void startWork();

private:
    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair);
    void updateMarketTrades(QString pair);
    void updateBalances    ();
    void createOrder       (QString pair, int type, double rate, double amount);
    void cancelOrder       (quint64 orderID);
    void updateActiveOrders(QString pair);
    void updateOrderInfo   (quint64 orderID);
};

#endif // EXCHANGE_BINANCE_H

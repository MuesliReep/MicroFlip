#ifndef EXCHANGE_SIM_H
#define EXCHANGE_SIM_H

#include "exchange.h"

class Exchange_Sim : public Exchange
{
public:
    Exchange_Sim();

    void startWork();

private:

  QString apiKey;
  QString apiSecret;

  void updateMarketTicker(QString pair);
  void updateMarketDepth (QString pair);
  void updateMarketTrades(QString pair);

  void updateBalances    ();
  void createOrder       (QString pair, int type, double rate, double amount);
  void cancelOrder       (quint64 orderID);
  void updateActiveOrders(QString pair)   ;
  void updateOrderInfo(quint64 orderID)   ;
};

#endif // EXCHANGE_SIM_H

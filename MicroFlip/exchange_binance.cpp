#include "exchange_binance.h"

// Binance API: https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md

Exchange_Binance::Exchange_Binance()
{
    currentTask = ExchangeTask();

    this->fee = 0.2;
}

void Exchange_Binance::startWork()
{
}

void Exchange_Binance::updateMarketTicker(QString pair)
{
    "GET /api/v3/ticker/price";
}

void Exchange_Binance::updateMarketDepth(QString pair)
{
  "GET /api/v1/depth";
}

void Exchange_Binance::updateMarketTrades(QString pair)
{
  "GET /api/v1/trades";
}

void Exchange_Binance::updateBalances()
{
  "GET /api/v3/account";
}

void Exchange_Binance::createOrder(QString pair, int type, double rate, double amount)
{
  "POST /api/v3/order";
}

void Exchange_Binance::cancelOrder(quint64 orderID)
{
  "DELETE /api/v3/order";
}

void Exchange_Binance::updateActiveOrders(QString pair)
{
  "GET /api/v3/openOrders";
}

void Exchange_Binance::updateOrderInfo(quint64 orderID)
{
  "GET /api/v3/order";
}

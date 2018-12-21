#ifndef EXCHANGE_BITFINEX_H
#define EXCHANGE_BITFINEX_H

#include "exchange.h"

class Exchange_bitfinex : public Exchange
{
    Q_OBJECT
public:
    Exchange_bitfinex();

    void startWork();
private:

  Downloader downloader;

  QString apiKey;
  QString apiSecret;

  uint lastNonce;
  QString createNonce();

  void updateMarketTicker(QString pair);
  void updateMarketDepth (QString pair) ;
  void updateMarketTrades(QString pair);

  void updateBalances();
  void createOrder(QString Pair, int Type, double Rate, double Amount);
  void cancelOrder(quint64 orderID);
  void updateActiveOrders(QString pair);
  void updateOrderInfo(quint64 OrderID);

  Ticker parseRawTickerData(QNetworkReply *reply);
  void parseRawDepthData (QNetworkReply *reply);
  void parseRawTradesData(QNetworkReply *reply);

  quint64 parseRawOrderCreationData(QNetworkReply *reply);

  bool checkSuccess(QJsonObject *object);
  QString getRequestErrorMessage(QJsonObject *object);


public slots:
  void UpdateMarketTickerReply (QNetworkReply *reply);
  void UpdateMarketDepthReply  (QNetworkReply *reply);
  void UpdateMarketTradesReply (QNetworkReply *reply);
  void UpdateBalancesReply     (QNetworkReply *reply);
  void CreateOrderReply        (QNetworkReply *reply);
  void CancelOrderReply        (QNetworkReply *reply);
  void UpdateActiveOrdersReply (QNetworkReply *reply);
  void UpdateOrderInfoReply    (QNetworkReply *reply);

signals:
  void sendTicker(Ticker ticker);
  void sendOrderID(quint64 orderID);
  void sendOrderStatus(int status);

};

#endif // EXCHANGE_BITFINEX_H

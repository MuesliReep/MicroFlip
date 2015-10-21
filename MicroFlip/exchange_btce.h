#ifndef EXCHANGE_BTCE_H
#define EXCHANGE_BTCE_H

#include <QDebug>

#include "exchange.h"

class Exchange_btce : public Exchange
{
  Q_OBJECT
public:
  Exchange_btce();

  void startWork();
private:

  Downloader  d;

  QNetworkAccessManager* tickerDownloadManager;
  QNetworkAccessManager* depthDownloadManager;

  void updateMarketTicker(QString pair);
  void updateMarketDepth (QString pair) ;
  void updateMarketTrades(QString pair);

  void updateBalances();
  void createOrder(QString pair, int type, double rate, double amount);
  void cancelOrder(uint orderID);
  void updateActiveOrders(QString pair);
  void updateOrderInfo(uint orderID);

  void executeExchangeTask(ExchangeTask *exchangeTask);

  bool getObjectFromDocument(QNetworkReply *reply, QJsonObject *object);
  Ticker parseRawTickerData(QJsonObject *rawData);

public slots:
  void receiveUpdateMarketTicker(QString pair, QObject *sender);
  void receiveUpdateMarketDepth(QString pair, QObject *sender) ;
  void receiveUpdateMarketTrades(QString pair, QObject *sender);
  void receiveUpdateBalances(QObject *sender);
  void receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender);
  void receiveCancelOrder(uint orderID, QObject *sender);
  void receiveUpdateActiveOrders(QString pair, QObject *sender);
  void receiveUpdateOrderInfo(uint orderID, QObject *sender);

  void UpdateMarketTickerReply (QNetworkReply *reply);
  void UpdateMarketDepthReply  (QNetworkReply *reply);
  void UpdateMarketTradesReply (QNetworkReply *reply);
  void UpdateBalancesReply     (QNetworkReply *reply);
  void CreateOrderReply        (QNetworkReply *reply);
  void CancelOrderReply        (QNetworkReply *reply);
  void UpdateActiveOrdersReply (QNetworkReply *reply);
  void UpdateOrderInfoReply    (QNetworkReply *reply);

private slots:
  void updateTick();
  void updateTick2();

signals:
  sendTicker(Ticker ticker);

};

#endif // EXCHANGE_BTCE_H

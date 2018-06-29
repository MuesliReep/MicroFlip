#ifndef EXCHANGE_BITSTAMP_H
#define EXCHANGE_BITSTAMP_H

#include "exchange.h"

class Exchange_bitstamp : public Exchange
{
    Q_OBJECT
public:
    Exchange_bitstamp();

    void startWork();
private:

  Downloader downloader;

  QString apiKey;
  QString apiSecret;
  QString customerID;

  QNetworkAccessManager* tickerDownloadManager;
  QNetworkAccessManager* createTradeDownloadManager;
  QNetworkAccessManager* orderInfoDownloadManager;

  uint lastNonce;
  void createNonce(QByteArray *nonce);

  void updateMarketTicker(QString pair);
  void updateMarketDepth (QString pair) ;
  void updateMarketTrades(QString pair);

  void updateBalances();
  void createOrder(QString Pair, int Type, double Rate, double Amount);
  void cancelOrder(uint orderID);
  void updateActiveOrders(QString pair);
  void updateOrderInfo(uint OrderID);

  bool getObjectFromDocument(QNetworkReply *reply, QJsonObject *object);
  Ticker parseRawTickerData(QJsonObject *rawData);

  bool checkSuccess(QJsonObject *object);
  bool checkUpdateOrderInfoSuccess(QJsonObject *object);
  bool checkCreateOrderSucces(QJsonObject *object);

public slots:
  void receiveUpdateMarketTicker(QString pair, QObject *sender, int SenderID);
  void receiveUpdateMarketDepth(QString pair, QObject *sender, int SenderID) ;
  void receiveUpdateMarketTrades(QString pair, QObject *sender, int SenderID);
  void receiveUpdateBalances(QObject *sender, int SenderID);
  void receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender, int SenderID);
  void receiveCancelOrder(uint orderID, QObject *sender, int SenderID);
  void receiveUpdateActiveOrders(QString pair, QObject *sender, int SenderID);
  void receiveUpdateOrderInfo(uint orderID, QObject *sender, int SenderID);

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
  void sendOrderID(int orderID);
  void sendOrderStatus(int status);

};

#endif // EXCHANGE_BITSTAMP_H

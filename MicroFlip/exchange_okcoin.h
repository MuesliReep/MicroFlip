#ifndef EXCHANGE_OKCOIN_H
#define EXCHANGE_OKCOIN_H

#include "exchange.h"

class Exchange_OKCoin : public Exchange
{
  Q_OBJECT
public:
  Exchange_OKCoin();

  void startWork();

private:

  Downloader  d;

  QString apiKey;
  QString apiSecret;

  QNetworkAccessManager* tickerDownloadManager;
  QNetworkAccessManager* createTradeDownloadManager;
  QNetworkAccessManager* orderInfoDownloadManager;


private slots:
  void updateTick();
  void updateTick2();

signals:
  void sendTicker(Ticker ticker);
  void sendOrderID(int orderID);
  void sendOrderStatus(int status);

public slots:

  void receiveUpdateOrderInfo(uint orderID, QObject *sender);

  void UpdateOrderInfoReply    (QNetworkReply *reply);
};

#endif // EXCHANGE_OKCOIN_H

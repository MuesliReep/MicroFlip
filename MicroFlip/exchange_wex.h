#ifndef EXCHANGE_WEX_H
#define EXCHANGE_WEX_H

#include "exchange.h"

class Exchange_wex : public Exchange
{
    Q_OBJECT
  public:
    Exchange_wex();

    void startWork();
  private:

    Downloader downloader;

    QString apiKey;
    QString apiSecret;

    QNetworkAccessManager* tickerDownloadManager;
    QNetworkAccessManager* updateMarketTradesManager;
    QNetworkAccessManager* createTradeDownloadManager;
    QNetworkAccessManager* orderInfoDownloadManager;
    QNetworkAccessManager* cancelOrderDownloadManager;

    uint lastNonce;
    void createNonce(QByteArray *nonce);

    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair) ;
    void updateMarketTrades(QString pair);

    void updateBalances();
    void createOrder(QString Pair, int Type, double Rate, double Amount);
    void cancelOrder(quint64 orderID);
    void updateActiveOrders(QString pair);
    void updateOrderInfo(quint64 OrderID);

    void executeExchangeTask(ExchangeTask *exchangeTask);

    bool getObjectFromDocument(QNetworkReply *reply, QJsonObject *object);
    Ticker parseRawTickerData(QJsonObject *rawData);
    bool checkSuccess(QJsonObject *object);
    QString getRequestErrorMessage(QJsonObject *object);

  public slots:
    void receiveUpdateMarketTicker(QString pair, QObject *sender, int SenderID);
    void receiveUpdateMarketDepth(QString pair, QObject *sender, int SenderID) ;
    void receiveUpdateMarketTrades(QString pair, QObject *sender, int SenderID);
    void receiveUpdateBalances(QObject *sender, int SenderID);
    void receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender, int SenderID);
    void receiveCancelOrder(quint64 orderID, QObject *sender, int SenderID);
    void receiveUpdateActiveOrders(QString pair, QObject *sender, int SenderID);
    void receiveUpdateOrderInfo(quint64 orderID, QObject *sender, int SenderID);

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
    void sendTicker(Ticker ticker);
    void sendOrderID(int orderID);
    void sendOrderStatus(int status);

  };

#endif // EXCHANGE_WEX_H

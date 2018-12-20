#ifndef EXCHANGE_BINANCE_H
#define EXCHANGE_BINANCE_H

#include "exchange.h"

class Exchange_Binance : public Exchange
{
    Q_OBJECT
public:
    Exchange_Binance();

    // Exchange interface
public:
    void startWork();

private:

    Downloader downloader;

    QNetworkAccessManager* tickerDownloadManager;

    void updateMarketTicker(QString pair);
    void updateMarketDepth (QString pair);
    void updateMarketTrades(QString pair);
    void updateBalances    ();
    void createOrder       (QString pair, int type, double rate, double amount);
    void cancelOrder       (quint64 orderID);
    void updateActiveOrders(QString pair);
    void updateOrderInfo   (quint64 orderID);


    Ticker parseRawTickerData   (QJsonObject *rawData);

    bool getDocumentFromNetworkReply(QNetworkReply *reply,   QJsonDocument *document);
    bool getObjectFromDocument      (QJsonDocument *jsonDoc, QJsonObject *object);
    bool getArrayFromDocument       (QJsonDocument *jsonDoc, QJsonArray *array);

public slots:
  void UpdateMarketTickerReply (QNetworkReply *reply);

signals:
  void sendTicker(Ticker ticker);
  void sendOrderID(quint64 orderID);
  void sendOrderStatus(int status);

};

#endif // EXCHANGE_BINANCE_H

#include "exchange_binance.h"

#include "json_helper.h"

// Binance API: https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md

Exchange_Binance::Exchange_Binance()
{
    currentTask = ExchangeTask();

    this->fee = 0.1;

    // Initiate download managers
    tickerDownloadManager             = new QNetworkAccessManager(this);
    updateMarketDepthDownloadManager  = new QNetworkAccessManager(this);
    updateMarketTradesDownloadManager = new QNetworkAccessManager(this);
    updateBalancesDownloadManager     = new QNetworkAccessManager(this);
    createTradeDownloadManager        = new QNetworkAccessManager(this);
    orderInfoDownloadManager          = new QNetworkAccessManager(this);
    cancelOrderDownloadManager        = new QNetworkAccessManager(this);
    activeOrdersDownloadManager       = new QNetworkAccessManager(this);

    // Create the interval timer
    timer  = new QTimer(this);
    connect(timer,  SIGNAL(timeout()), this, SLOT(updateTick()));
}

void Exchange_Binance::startWork() {

    // 30 requests per min
    timer->start(2000);
}

void Exchange_Binance::updateMarketTicker(QString pair) {

    // Create the request to download new data
    QNetworkRequest request = downloader.generateRequest(QUrl("https://www.binance.com/api/v1/ticker/24hr?symbol="+pair.toUpper()));

    // Execute the download
    downloader.doDownload(request, tickerDownloadManager, this, SLOT(updateMarketTickerReply(QNetworkReply*)));
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

//----------------------------------//
//             Parsers              //
//----------------------------------//

Ticker Exchange_Binance::parseRawTickerData(QNetworkReply *reply) {

    Ticker ticker;

    if(reply->error()) {
        qDebug() << "Ticker Packet error: " << reply->errorString();

        return ticker;
    }

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Values are stored as strings, so first convert to string then further
            double high = jsonObj.value("highPrice")       .toString("-1.0").toDouble();
            double low  = jsonObj.value("lowPrice")        .toString("-1.0").toDouble();
            double avg  = jsonObj.value("weightedAvgPrice").toString("-1.0").toDouble();
            double last = jsonObj.value("lastPrice")       .toString("-1.0").toDouble();
            double buy  = jsonObj.value("bidPrice")        .toString("-1.0").toDouble();
            double sell = jsonObj.value("askPrice")        .toString("-1.0").toDouble();
            int    age  = jsonObj.value("closeTime")       .toString("-1").toInt();

            ticker = Ticker(high, low, avg, last, buy, sell, age);
        }
    }

    return ticker;
}

void Exchange_Binance::parseRawDepthData(QNetworkReply *reply)
{
    // TODO
    (void)reply;
}

void Exchange_Binance::parseRawTradesData(QNetworkReply *reply)
{
    // TODO
    (void)reply;
}

void Exchange_Binance::parseRawBalancesData(QNetworkReply *reply)
{
    // TODO
    (void)reply;
}

quint64 Exchange_Binance::parseRawOrderCreationData(QNetworkReply *reply)
{

}

void Exchange_Binance::parseRawOrderCancelationData(QNetworkReply *reply)
{
    // TODO
    (void)reply;
}

void Exchange_Binance::parseRawActiveOrdersData(QNetworkReply *reply)
{
    // TODO
    (void)reply;
}

int Exchange_Binance::parseRawOrderInfoData(QNetworkReply *reply)
{

}

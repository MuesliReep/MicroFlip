#include "exchange_binance.h"

#include <QJsonArray>
#include <QJsonParseError>

// Binance API: https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md

Exchange_Binance::Exchange_Binance()
{
    currentTask = ExchangeTask();

    this->fee = 0.1;

    // Initiate download managers
    tickerDownloadManager             = new QNetworkAccessManager(this);

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
    downloader.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketTickerReply(QNetworkReply*)));
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
//           Task Replies           //
//----------------------------------//

void Exchange_Binance::UpdateMarketTickerReply(QNetworkReply *reply) {

    Ticker ticker;

    if(!reply->error()) {

        QJsonObject jsonObj;

        // Extract JSON object from network reply
        getObjectFromDocument(reply, &jsonObj);

        // Parse the raw data
        ticker = parseRawTickerData(&jsonObj);

    } else {
        qDebug() << "Ticker Packet error: " << reply->errorString();

        // TODO: send empty ticker!
    }

    QByteArray replyData = reply->readAll();

    // Connect & send to the initiator
    connect(this, SIGNAL(sendTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));
    emit sendTicker(ticker);
    disconnect(this, SIGNAL(sendTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(tickerDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

bool Exchange_Binance::getDocumentFromNetworkReply(QNetworkReply *reply, QJsonDocument *document) {

    QJsonParseError parseResult;

    *document = QJsonDocument().fromJson(reply->readAll(), &parseResult);

    if(parseResult.error == QJsonParseError::NoError) {

        return true;
    }

    qDebug() << "Error parsing JSON Document: " << parseResult.errorString();

    return false;
}

bool Exchange_Binance::getObjectFromDocument(QJsonDocument *document, QJsonObject *object) {

    if(document->isObject()) {
        *object = document->object();

        return true;
    }

    qDebug() << "JSON Document does not contain object";

    return false;
}

bool Exchange_Binance::getArrayFromDocument(QJsonDocument *document, QJsonArray *array) {


    if(document->isArray()) {
        *array  = document->array();

        return true;
    }

    qDebug() << "JSON Document does not contain array";

    return false;
}

Ticker Exchange_Binance::parseRawTickerData(QJsonObject *rawData) {

  // Values are stored as strings, so first convert to string then further
  double high = rawData->value("highPrice")       .toString("-1.0").toDouble();
  double low  = rawData->value("lowPrice")        .toString("-1.0").toDouble();
  double avg  = rawData->value("weightedAvgPrice").toString("-1.0").toDouble();
  double last = rawData->value("lastPrice")       .toString("-1.0").toDouble();
  double buy  = rawData->value("bidPrice")        .toString("-1.0").toDouble();
  double sell = rawData->value("askPrice")        .toString("-1.0").toDouble();
  int    age  = rawData->value("closeTime")       .toString("-1").toInt();

  return Ticker(high, low, avg, last, buy, sell, age);
}

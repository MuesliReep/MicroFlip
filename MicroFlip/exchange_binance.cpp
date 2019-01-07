#include "exchange_binance.h"

#include <math.h>

#include "json_helper.h"
#include "workorder.h"

// Binance API: https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md

Exchange_Binance::Exchange_Binance()
{
    currentTask = ExchangeTask();

    this->fee = 0.1;
    this->exchangeName = "Binance";

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

    this->apiKey    = config->getApiKey();
    this->apiSecret = config->getApiSecret();

    // 30 requests per min
    timer->start(2000);
}

void Exchange_Binance::updateMarketTicker(QString pair) {

    // Create the request to download new data
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.binance.com/api/v1/ticker/24hr?symbol="+pair.toUpper()));

    // Execute the download
    downloader.doDownload(request, tickerDownloadManager, this, SLOT(updateMarketTickerReply(QNetworkReply*)));
}

void Exchange_Binance::updateMarketDepth(QString pair) {

    QString limit = "100";

    // Create the request to download new data
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.binance.com/api/v1/depth?symbol="+pair.toUpper()+"&limit="+limit));

    // Execute the download
    downloader.doDownload(request, updateMarketDepthDownloadManager, this, SLOT(updateMarketDepthReply(QNetworkReply*)));
}

void Exchange_Binance::updateMarketTrades(QString pair) {

    QString limit = "500";

    // Create the request to download new data
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.binance.com/api/v1/trades?symbol="+pair.toUpper()+"&limit="+limit));

    // Execute the download
    downloader.doDownload(request, updateMarketTradesDownloadManager, this, SLOT(updateMarketTradesReply(QNetworkReply*)));
}

void Exchange_Binance::updateBalances() {

  "GET /api/v3/account";
}

void Exchange_Binance::createOrder(QString pair, int type, double rate, double amount) {

  "POST /api/v3/order";

    QByteArray query;

    // Symbol
    query.append("symbol=");
    query.append(pair);

    // Side
    query.append("&side=");
    query.append(type == 0 ? "BUY" : "SELL");

    // Type
    query.append("&type=");
    query.append("LIMIT");

    // Time in Force
    query.append("&timeInForce=");
    query.append("GTC"); // Good-Til-Canceled

    // Quantity
    query.append("&quantity=");
    query.append(QByteArray::number(amount,'f',5)); // TODO

    // Price
    query.append("&price="); // If buy, round down, else round up
    if(type == 0) {
        query.append(QByteArray::number(roundDown(rate),'f',2));
    } else {
        query.append(QByteArray::number(roundUp(rate),'f',2)); // TODO
    }

    // Response Type
    query.append("&newOrderRespType=");
    query.append("ACK");

    // Receive window
    query.append("&recvWindow=");
    query.append("5000"); // TODO

    // Timestamp
    query.append("&timestamp=");
    query.append(QByteArray::number(QDateTime::currentDateTime().toMSecsSinceEpoch() - deltaTime)); // TODO

    // Create signature
    QByteArray signature = QMessageAuthenticationCode::hash(query, this->apiSecret.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Add signature to query
    query.append("&signature=");
    query.append(signature);

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.binance.com/api/v3/order"));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-Type"), QByteArray("application/x-www-form-urlencoded"));
//    downloader.addHeaderToRequest(&request, QByteArray("Content-Type"), QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("Accept"),       QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("X-MBX-APIKEY"), QByteArray::fromStdString(this->apiKey.toStdString()));

    // Execute the download
    downloader.doPostDownload(request, createTradeDownloadManager, query, this, SLOT(createOrderReply(QNetworkReply*)));
}

void Exchange_Binance::cancelOrder(qint64 orderID) {

  "DELETE /api/v3/order";
}

void Exchange_Binance::updateActiveOrders(QString pair) {

  "GET /api/v3/openOrders";
}

void Exchange_Binance::updateOrderInfo(qint64 orderID) {

  "GET /api/v3/order";

    QByteArray query;

    // Symbol
    query.append("symbol=");
    query.append(((WorkOrder*)currentTask.getSender())->getPair());

//    // Side
//    query.append("&side=");
//    query.append(((WorkOrder*)currentTask.getSender())->getOrderSide() == 0 ? "BUY" : "SELL");

    // Order ID
    query.append("&orderId=");
    query.append(QByteArray::number(orderID)); // TODO

    // Receive Window
    query.append("&recvWindow=");
    query.append("5000"); // TODO

    // Timestamp
    query.append("&timestamp=");
    query.append(QByteArray::number(QDateTime::currentDateTime().toMSecsSinceEpoch() - deltaTime)); // TODO

    // Create signature
    QByteArray signature = QMessageAuthenticationCode::hash(query, this->apiSecret.toUtf8(), QCryptographicHash::Sha256).toHex();

    // Add signature to query
    query.append("&signature=");
    query.append(signature);

    // Prepend questionmark to querystring
    query.prepend("?");

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.binance.com/api/v3/order" + query));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
//    downloader.addHeaderToRequest(&request, QByteArray("Content-Type"), QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("Accept"),       QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("X-MBX-APIKEY"), QByteArray::fromStdString(this->apiKey.toStdString()));

    // Execute the download
    downloader.doDownload(request, orderInfoDownloadManager, this, SLOT(updateOrderInfoReply(QNetworkReply*)));
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
            double age  = jsonObj.value("closeTime")       .toDouble(-1);

            deltaTime = QDateTime::currentDateTime().toMSecsSinceEpoch() - (qint64)age;

            ticker = Ticker(high, low, avg, last, buy, sell, (qint64)age);
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

qint64 Exchange_Binance::parseRawOrderCreationData(QNetworkReply *reply) {

    qint64 orderID = -1;

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // TODO: check if object contains "error code"
            if(exchangeErrorCheck(&jsonObj)) {
                orderID = (qint64)jsonObj.value("orderId").toDouble(-1.0);
            }
        }
    }

    return orderID;
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
    int status = -1;

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // TODO: check if object contains "error code"
            if(exchangeErrorCheck(&jsonObj)) {

                QString statusString = jsonObj.value("status").toString();

                if(statusString == "FILLED") {
                    status = 1;
                } else if(statusString == "NEW" || statusString == "PARTIALLY_FILLED") {
                    status = 0;
                }
            }
        }
    }

    return status;
}

bool Exchange_Binance::exchangeErrorCheck(QJsonObject *jsonObj) {

    bool result = false;

    if(jsonObj->contains("code")) {

        qDebug() << "Error code: " << jsonObj->value("code").toInt() << " " << jsonObj->value("msg").toString();
    } else {
        result = true;
    }

    return result;
}

double Exchange_Binance::roundUp(double value)
{
    return ceil(value * 100) / 100;
}

double Exchange_Binance::roundDown(double value)
{
    return floor(value * 100) / 100;
}

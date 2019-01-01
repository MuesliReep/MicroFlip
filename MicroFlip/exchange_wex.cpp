#include "exchange_wex.h"

#include <QJsonArray>

#include <trade.h>
#include "json_helper.h"

Exchange_wex::Exchange_wex() {

  currentTask = ExchangeTask();

  fee = 0.2;

  // Initiate download managers
  tickerDownloadManager             = new QNetworkAccessManager(this);
  updateMarketDepthDownloadManager  = new QNetworkAccessManager(this);
  updateMarketTradesDownloadManager = new QNetworkAccessManager(this);
  updateBalancesDownloadManager     = new QNetworkAccessManager(this);
  createTradeDownloadManager        = new QNetworkAccessManager(this);
  orderInfoDownloadManager          = new QNetworkAccessManager(this);
  cancelOrderDownloadManager        = new QNetworkAccessManager(this);
  activeOrdersDownloadManager       = new QNetworkAccessManager(this);

  // Start the interval timers
  timer  = new QTimer(this);
  //timer2 = new QTimer(this);

  connect(timer,  SIGNAL(timeout()), this, SLOT(updateTick()));
  //connect(timer2, SIGNAL(timeout()), this, SLOT(updateTick2()));
}

void Exchange_wex::startWork() {

  this->apiKey    = config->getApiKey();
  this->apiSecret = config->getApiSecret();

  timer->start(2*1100);
  //timer2->start(1*1100); // TODO: determine correct amount
}

void Exchange_wex::createNonce(QByteArray *nonce) {

  uint now = QDateTime::currentDateTime().toMSecsSinceEpoch() / 250;

  if(lastNonce == now) {
    lastNonce+=2;
  }
  else
    lastNonce = now;

  nonce->setNum(lastNonce);
}

//----------------------------------//
//              Tasks               //
//----------------------------------//

void Exchange_wex::updateMarketTicker(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = downloader.generateRequest(QUrl("https://wex.nz/api/3/ticker/"+pair));

  // Execute the download
  downloader.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketTickerReply(QNetworkReply*)));
}

void Exchange_wex::updateMarketDepth(QString pair) {
  (void) pair;

  // TODO
}

void Exchange_wex::updateMarketTrades(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = downloader.generateRequest(QUrl("https://wex.nz/api/3/trades/"+pair+"?&limit=5000"));

  // Execute the download
  downloader.doDownload(request, updateMarketTradesDownloadManager, this, SLOT(updateMarketTradesReply(QNetworkReply*)));
}

void Exchange_wex::updateBalances() {

  // TODO
}

void Exchange_wex::createOrder(QString Pair, int Type, double Rate, double Amount) {

  // Create POST data from method and nonce
  QByteArray method("method=Trade");
  QByteArray nonce;
  createNonce(&nonce);
  nonce.prepend("nonce=");

  QByteArray pair("pair=");
  pair.append(Pair);

  QByteArray type("type=");
  if(Type == 0)
    type.append("buy");
  else
    type.append("sell");

  QByteArray price("rate=");
  price.append(QString::number(Rate,'f',3));

  QByteArray amount("amount=");
  amount.append(QString::number(Amount,'f',8));

  QByteArray data(method +"&"+ nonce +"&"+ pair +"&"+ type +"&"+ price +"&"+ amount);

  // Sign the data
  QByteArray sign = QMessageAuthenticationCode::hash(data, apiSecret.toUtf8(), QCryptographicHash::Sha512).toHex();

  // Create request
  QNetworkRequest request = downloader.generateRequest(QUrl("https://wex.nz/tapi/"));

  // Add headers
  downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
  downloader.addHeaderToRequest(&request, QByteArray("Key"), apiKey.toUtf8());
  downloader.addHeaderToRequest(&request, QByteArray("Sign"), sign);

  // Execute the download
  downloader.doPostDownload(request, createTradeDownloadManager, data, this, SLOT(createOrderReply(QNetworkReply*)));
}

void Exchange_wex::cancelOrder(qint64 orderID) {
  (void) orderID;
  // TODO
}

void Exchange_wex::updateActiveOrders(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_wex::updateOrderInfo(qint64 OrderID) {

  // Create POST data from method and nonce
  QByteArray method("method=OrderInfo");
  QByteArray nonce;
  createNonce(&nonce);
  nonce.prepend("nonce=");

  QByteArray orderID("order_id=");
  orderID.append(QString::number(OrderID));

  QByteArray data(method +"&"+ nonce + "&" + orderID);

  // Sign the data
  QByteArray sign = QMessageAuthenticationCode::hash(data, apiSecret.toUtf8(), QCryptographicHash::Sha512).toHex();

  // Create request
  QNetworkRequest request = downloader.generateRequest(QUrl("https://wex.nz/tapi/"));

  // Add headers
  downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
  downloader.addHeaderToRequest(&request, QByteArray("Key"), apiKey.toUtf8());
  downloader.addHeaderToRequest(&request, QByteArray("Sign"), sign);

  // Execute the download
  downloader.doPostDownload(request, orderInfoDownloadManager, data, this, SLOT(updateOrderInfoReply(QNetworkReply*)));
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

Ticker Exchange_wex::parseRawTickerData(QNetworkReply *reply) {

    Ticker ticker;

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Parse individual ticker data items
            double high = jsonObj.value("high").toDouble();
            double low  = jsonObj.value("low") .toDouble();
            double avg  = jsonObj.value("avg") .toDouble();
            double last = jsonObj.value("last").toDouble();
            double buy  = jsonObj.value("buy") .toDouble();
            double sell = jsonObj.value("sell").toDouble();
            int    age  = jsonObj.value("age") .toInt();

            ticker = Ticker(high, low, avg, last, buy, sell, age);
        }
    }

    return ticker;
}

void Exchange_wex::parseRawDepthData(QNetworkReply *reply)
{
    // TODO
    (void) reply;
}

void Exchange_wex::parseRawTradesData(QNetworkReply *reply) {

    // TODO

    QList<Trade> marketTrades;

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;
    QJsonArray    jsonArr;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            QString pair = "ltc_usd"; // FIX!!!

            // Check if authentication was a succes
            if(checkSuccess(&jsonObj)) {

                // Extract the pair data we want
                jsonArr = jsonObj.value(pair).toArray();

                for(int i = 0; i < jsonArr.size(); i++) {

                    QJsonObject currentTrade = jsonArr.at(i).toObject();

                    marketTrades.append(Trade(currentTrade.value("price")    .toDouble(),
                                              currentTrade.value("amount")   .toDouble(),
                                              currentTrade.value("tid")      .toInt(),
                                              currentTrade.value("timestamp").toInt()));
                }

            } else {
                qDebug() << "Update Market Trades authentication error";
            }

        }
    }

    //    return marketTrades;
}

void Exchange_wex::parseRawBalancesData(QNetworkReply *reply)
{
    // TODO
    (void) reply;
}

qint64 Exchange_wex::parseRawOrderCreationData(QNetworkReply *reply) {

    qint64 orderID = -1;

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Check if authentication was a succes
            if(checkSuccess(&jsonObj)) {

                QJsonObject tradeData;

                // Extract the info data we want
                tradeData = jsonObj.value("return").toObject();

                // Parse new data
                orderID = tradeData.value("order_id").toInt(-1);

                qDebug() << "Trade created successfully, ID: " << orderID;
            }
            else {
                qDebug() << "Create Trade authentication error";
            }
        }
    }

    return orderID;
}

void Exchange_wex::parseRawOrderCancelationData(QNetworkReply *reply)
{
    // TODO
    (void) reply;
}

void Exchange_wex::parseRawActiveOrdersData(QNetworkReply *reply)
{
    // TODO
    (void) reply;
}

int Exchange_wex::parseRawOrderInfoData(QNetworkReply *reply)
{
    int status = -1;

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Check if authentication was a succes
            if(checkSuccess(&jsonObj)) {

                QJsonObject returnInfoData;
                QJsonObject orderInfoData;

                // Extract the info data we want
                returnInfoData = jsonObj.value("return").toObject();

                int orderID = currentTask.getAttributes().at(0).toInt();
                orderInfoData = returnInfoData.value(QString::number(orderID)).toObject();

                // Parse new data
                status = orderInfoData.value("status").toInt(-1);
            }
            else {
                qDebug() << "Order Info authentication error";
            }
        }
    }

    return status;
}

bool Exchange_wex::checkSuccess(QJsonObject *object) {

  bool result = false;

  if(!object->contains("success"))
    return result;

  if(object->value("success").toInt() == 1)
    result = true;

  return result;
}

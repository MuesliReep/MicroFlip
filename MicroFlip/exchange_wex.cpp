#include "exchange_wex.h"

#include <QJsonArray>

#include <trade.h>

Exchange_wex::Exchange_wex() {

  currentTask = ExchangeTask();

  fee = 0.2;

  // Initiate download managers
  tickerDownloadManager      = new QNetworkAccessManager(this);
  createTradeDownloadManager = new QNetworkAccessManager(this);
  orderInfoDownloadManager   = new QNetworkAccessManager(this);

  // Start the interval timers
  timer  = new QTimer(this);
  //timer2 = new QTimer(this);

  connect(timer,  SIGNAL(timeout()), this, SLOT(updateTick()));
  //connect(timer2, SIGNAL(timeout()), this, SLOT(updateTick2()));

}

void Exchange_wex::startWork() {

  this->apiKey    = c->getApiKey();
  this->apiSecret = c->getApiSecret();

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
  downloader.doDownload(request, updateMarketTradesManager, this, SLOT(UpdateMarketTradesReply(QNetworkReply*)));
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
  downloader.doPostDownload(request, createTradeDownloadManager, data, this, SLOT(CreateOrderReply(QNetworkReply*)));
}

void Exchange_wex::cancelOrder(quint64 orderID) {
  (void) orderID;
  // TODO
}

void Exchange_wex::updateActiveOrders(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_wex::updateOrderInfo(quint64 OrderID) {

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
  downloader.doPostDownload(request, orderInfoDownloadManager, data, this, SLOT(UpdateOrderInfoReply(QNetworkReply*)));
}

//----------------------------------//
//           Task Replies           //
//----------------------------------//

void Exchange_wex::UpdateMarketTickerReply(QNetworkReply *reply) {

  Ticker ticker;

  if(!reply->error()) {

    QJsonObject jsonObj;
    QJsonObject tickerData;

    // Extract JSON object from network reply
    getObjectFromDocument(reply, &jsonObj);

    // Extract the market data we want
    QString pair = currentTask.getAttributes().at(0);
    tickerData = jsonObj.value(pair).toObject();

    // Parse the raw data
    Ticker ticker = parseRawTickerData(&tickerData);

    // Connect & send to the initiator
    connect(this, SIGNAL(sendTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));
    emit sendTicker(ticker);
    disconnect(this, SIGNAL(sendTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));

  }
  else {
    qDebug() << "Ticker Packet error: " << reply->errorString();
  }

  reply->deleteLater();

  // Disconnect the download signal and release
  disconnect(tickerDownloadManager, 0, this, 0);

  // Mark this task complete
  currentTask = ExchangeTask();
}

void Exchange_wex::UpdateMarketDepthReply(QNetworkReply *reply) {

  (void) reply;

    // TODO:
}

void Exchange_wex::UpdateMarketTradesReply(QNetworkReply *reply) {

    // TODO:

    QList<Trade> marketTrades;

    if(!reply->error()) {

        QJsonObject jsonObj;
        QJsonArray  tradeData;

        QString pair = "ltc_usd"; // FIX!!!

        // Extract JSON object from network reply
        getObjectFromDocument(reply, &jsonObj);

        // Check if authentication was a succes
        if(checkSuccess(&jsonObj)) {

            // Extract the info data we want
            tradeData = jsonObj.value(pair).toArray();

            for(int i = 0; i < tradeData.size(); i++) {

                QJsonObject currentTrade = tradeData.at(i).toObject();

                marketTrades.append(Trade(currentTrade.value("price").toDouble(),
                                          currentTrade.value("amount").toDouble(),
                                          currentTrade.value("tid").toInt(),
                                          currentTrade.value("timestamp").toInt()));
            }
        } else {
            qDebug() << "UpdateMarketTrades error: " << getRequestErrorMessage(&jsonObj);
        }
    } else {
        qDebug() << "UpdateMarketTrades Packet error";
    }
}

void Exchange_wex::UpdateBalancesReply(QNetworkReply *reply) {

  (void) reply;

    // TODO:
}

void Exchange_wex::CreateOrderReply(QNetworkReply *reply) {

  int orderID = -1;

  if(!reply->error()) {

    QJsonObject jsonObj;
    QJsonObject tradeData;

    // Extract JSON object from network reply
    getObjectFromDocument(reply, &jsonObj);

    // Check if authentication was a succes
    if(checkSuccess(&jsonObj)) {

      // Extract the info data we want
      tradeData = jsonObj.value("return").toObject();

      // Parse new data
      orderID = tradeData.value("order_id").toInt(-1);

      qDebug() << "Trade created successfully, ID: " << orderID;
    }
    else {
      qDebug() << "Trade error: " << getRequestErrorMessage(&jsonObj);
    }
  } else {
    qDebug() << "Trade Packet error";
  }

  // Connect & send order ID to the initiator
  connect(this, SIGNAL(sendOrderID(quint64)), currentTask.getSender(), SLOT(orderCreateReply(quint64)));
  emit sendOrderID((quint64)orderID);
  disconnect(this, SIGNAL(sendOrderID(quint64)), currentTask.getSender(), SLOT(orderCreateReply(quint64)));

  reply->deleteLater();

  disconnect(createTradeDownloadManager, 0, this, 0);

  // Mark this task complete
  currentTask = ExchangeTask();
}

void Exchange_wex::CancelOrderReply(QNetworkReply *reply) {

  int status = -1;

  if(!reply->error()) {

  }
}

void Exchange_wex::UpdateActiveOrdersReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_wex::UpdateOrderInfoReply(QNetworkReply *reply) {

  int status = -1;

  if(!reply->error()) {

    QJsonObject jsonObj;
    QJsonObject returnInfoData;
    QJsonObject orderInfoData;

    // Extract JSON object from network reply
    getObjectFromDocument(reply, &jsonObj);

    // Check if authentication was a succes
    if(checkSuccess(&jsonObj)) {

      // Extract the info data we want
      returnInfoData = jsonObj.value("return").toObject();

      int orderID = currentTask.getAttributes().at(0).toInt();
      orderInfoData = returnInfoData.value(QString::number(orderID)).toObject();

      // Parse new data
      status = orderInfoData.value("status").toInt(-1);
    }
    else {
      qDebug() << "OrderInfo error: " << getRequestErrorMessage(&jsonObj);
    }
  } else {
    qDebug() << "OrderInfo Packet error";
    status = -2;
  }

  // Connect & send order ID to the initiator
  connect(this, SIGNAL(sendOrderStatus(int)), currentTask.getSender(), SLOT(orderInfoReply(int)));
  emit sendOrderStatus(status);
  disconnect(this, SIGNAL(sendOrderStatus(int)), currentTask.getSender(), SLOT(orderInfoReply(int)));

  reply->deleteLater();

  disconnect(orderInfoDownloadManager, 0, this, 0);

  // Mark this task complete
  currentTask = ExchangeTask();
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

// Grabs a JSON object from a Network reply
// Returns true if succesfull
bool Exchange_wex::getObjectFromDocument(QNetworkReply *reply, QJsonObject *object) {

  QJsonDocument   jsonDoc;
  QJsonParseError error;

  jsonDoc = QJsonDocument().fromJson(reply->readAll(), &error);
  *object = jsonDoc.object();

  return true; // TODO: check json validity
}

Ticker Exchange_wex::parseRawTickerData(QJsonObject *rawData) {

  // Parse individual ticker data items
  double high = rawData->value("high").toDouble();
  double low  = rawData->value("low") .toDouble();
  double avg  = rawData->value("avg") .toDouble();
  double last = rawData->value("last").toDouble();
  double buy  = rawData->value("buy") .toDouble();
  double sell = rawData->value("sell").toDouble();
  int    age  = rawData->value("age") .toInt();

  return Ticker(high, low, avg, last, buy, sell, age);
}

bool Exchange_wex::checkSuccess(QJsonObject *object) {

  bool result = false;

  if(!object->contains("success"))
    return result;

  if(object->value("success").toInt() == 1)
    result = true;

  return result;
}

QString Exchange_wex::getRequestErrorMessage(QJsonObject *object) {

  return object->value("error").toString();
}

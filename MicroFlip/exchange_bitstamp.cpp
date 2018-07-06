#include "exchange_bitstamp.h"

Exchange_bitstamp::Exchange_bitstamp() {

  currentTask = ExchangeTask();

  fee = 0.25;

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

void Exchange_bitstamp::startWork() {

  apiKey     = config->getApiKey();
  apiSecret  = config->getApiSecret();
  customerID = config->getCustomerID();

  //timer->start(c->getCoolDownTime()*1100);
  //timer2->start(1*1100); // TODO: determine correct amount

  // TODO: bitstamp is 600 calls per 10mins
  uint coolDownTime = 1010;
  timer->start(coolDownTime);
}

void Exchange_bitstamp::createNonce(QByteArray *nonce) {

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

void Exchange_bitstamp::updateMarketTicker(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = downloader.generateRequest(QUrl("https://www.bitstamp.net/api/v2/ticker/"+pair));

  // Execute the download
  downloader.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketTickerReply(QNetworkReply*)));
}

void Exchange_bitstamp::updateMarketDepth(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_bitstamp::updateMarketTrades(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_bitstamp::updateBalances() {

  // TODO
}

void Exchange_bitstamp::createOrder(QString Pair, int Type, double Rate, double Amount) {

    // Set the type of order, 0 = buy, 1 = sell
    QByteArray type(Type == 0 ? "buy" : "sell");

    int precision = 2;
    if(Pair == "xrpusd")
        precision = 5;

    QByteArray price("price=");
    price.append(QString::number(Rate,'f',precision));

    QByteArray amount("amount=");
    amount.append(QString::number(Amount,'f',8));

    // Create signature from nonce, userid & apikey
    QByteArray nonce;
    createNonce(&nonce);
    QByteArray customerID(this->customerID.toUtf8());
    QByteArray APIkey(apiKey.toUtf8());

    QByteArray signatureMessage(nonce+customerID+APIkey);

    QByteArray signature = QMessageAuthenticationCode::hash(signatureMessage, apiSecret.toUtf8(), QCryptographicHash::Sha256).toHex().toUpper();

    // Add tags
    signature.prepend("signature=");
    nonce.prepend("nonce=");
    APIkey.prepend("key=");

    // Create the data string
    QByteArray data(APIkey +"&"+ signature +"&"+ nonce +"&"+ amount +"&"+ price);
    //QByteArray data(key +"&"+ signature +"&"+ nonce +"&"+ amount +"&"+ price +"&"+ limitPrice);

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://www.bitstamp.net/api/v2/"+type+"/"+Pair+"/"));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
    //downloader.addHeaderToRequest(&request, QByteArray("key"), apiKey.toUtf8());
    //downloader.addHeaderToRequest(&request, QByteArray("Sign"), sign);

    // Execute the download
    downloader.doPostDownload(request, createTradeDownloadManager, data, this, SLOT(CreateOrderReply(QNetworkReply*)));
}

void Exchange_bitstamp::cancelOrder(uint orderID) {
  (void) orderID;
  // TODO
}

void Exchange_bitstamp::updateActiveOrders(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_bitstamp::updateOrderInfo(uint OrderID) {

    QByteArray orderID("id=");
    orderID.append(QString::number(OrderID));

    // Create signature from nonce, userid & apikey
    QByteArray nonce;
    createNonce(&nonce);
    QByteArray customerID(this->customerID.toUtf8());
    QByteArray APIkey(apiKey.toUtf8());

    QByteArray signatureMessage(nonce+customerID+apiKey.toUtf8());

    // Sign the data
    QByteArray signature = QMessageAuthenticationCode::hash(signatureMessage, apiSecret.toUtf8(), QCryptographicHash::Sha256).toHex().toUpper();

    // Add tags
    signature.prepend("signature=");
    nonce.prepend("nonce=");
    APIkey.prepend("key=");

    // Create the data string
    QByteArray data(APIkey + "&" + signature + "&" + nonce + "&" + orderID);

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://www.bitstamp.net/api/order_status/"));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));

    // Execute the download
    downloader.doPostDownload(request, orderInfoDownloadManager, data, this, SLOT(UpdateOrderInfoReply(QNetworkReply*)));
}

//----------------------------------//
//           Task Replies           //
//----------------------------------//

void Exchange_bitstamp::UpdateMarketTickerReply(QNetworkReply *reply) {

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
      Ticker ticker = parseRawTickerData(&jsonObj);

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

void Exchange_bitstamp::UpdateMarketDepthReply(QNetworkReply *reply) {

    (void) reply;
    // TODO
}

void Exchange_bitstamp::UpdateMarketTradesReply(QNetworkReply *reply) {

    (void) reply;
    // TODO
}

void Exchange_bitstamp::UpdateBalancesReply(QNetworkReply *reply) {

    (void) reply;
    // TODO
}

void Exchange_bitstamp::CreateOrderReply(QNetworkReply *reply) {

    int orderID = -1;

    if(!reply->error()) {

        QJsonObject jsonObj;
        // Extract JSON object from network reply
        getObjectFromDocument(reply, &jsonObj);

        // Check if authentication was a succes
        if(checkCreateOrderSucces(&jsonObj)) {

            // Extract the info data we want
            orderID = jsonObj.value("id").toString("-1").toInt();

            qDebug() << "Trade created successfully, ID: " << orderID;

        } else {
            // TODO:
        }

    } else {
      QString errorString = reply->errorString();
      qDebug() << "CreateOrder Packet error: " << errorString;
    }

    // Connect & send order ID to the initiator
    connect(this, SIGNAL(sendOrderID(int)), currentTask.getSender(), SLOT(orderCreateReply(int)));
    emit sendOrderID(orderID);
    disconnect(this, SIGNAL(sendOrderID(int)), currentTask.getSender(), SLOT(orderCreateReply(int)));

    reply->deleteLater();

    disconnect(createTradeDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange_bitstamp::CancelOrderReply(QNetworkReply *reply) {

    (void) reply;
    // TODO
}

void Exchange_bitstamp::UpdateActiveOrdersReply(QNetworkReply *reply) {

    (void) reply;
    // TODO
}

void Exchange_bitstamp::UpdateOrderInfoReply(QNetworkReply *reply) {

    // TODO:!!!!!!!
    int status = -1;

    if(!reply->error()) {

      QJsonObject jsonObj;

      // Extract JSON object from network reply
      getObjectFromDocument(reply, &jsonObj);

      // Check if authentication was a succes
      if(checkUpdateOrderInfoSuccess(&jsonObj)) {

        // Parse new data
        QString statusString = jsonObj.value("status").toString("-1");

        if(statusString.compare("In Queue", Qt::CaseInsensitive) == 0) {
            status = 0;
        } else if(statusString.compare("Open", Qt::CaseInsensitive) == 0) {
            status = 0;
        } else if(statusString.compare("Finished", Qt::CaseInsensitive) == 0) {
            status = 1;
        } else {
            status = -1;
            qDebug() << "Received invalid Order Info state: " << statusString;
        }

      }
      else {
        qDebug() << "OrderInfo error: ";
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
bool Exchange_bitstamp::getObjectFromDocument(QNetworkReply *reply, QJsonObject *object) {

  QJsonDocument   jsonDoc;
  QJsonParseError error;

  jsonDoc = QJsonDocument().fromJson(reply->readAll(), &error);
  *object = jsonDoc.object();

  return true; // TODO: check json validity
}

Ticker Exchange_bitstamp::parseRawTickerData(QJsonObject *rawData) {

  // Retrieve ticker data from JSON object
  double high = rawData->value("high").toString().toDouble();
  double low  = rawData->value("low").toString().toDouble();
  double avg  = rawData->value("vwap").toString().toDouble();
  double last = rawData->value("last").toString().toDouble();
  double buy  = rawData->value("bid").toString().toDouble();
  double sell = rawData->value("ask").toString().toDouble();
  int    age  = rawData->value("timestamp").toString().toInt();

  return Ticker(high, low, avg, last, buy, sell, age);
}

bool Exchange_bitstamp::checkSuccess(QJsonObject *object) {

  bool result = false;

  if(!object->contains("success"))
    return result;

  if(object->value("success").toInt() == 1)
    result = true;

  return result;
}

bool Exchange_bitstamp::checkUpdateOrderInfoSuccess(QJsonObject *object) {

    bool result = false;

    if(object->contains("Missing id POST param"))
        return result;
    if(object->contains("Invalid order id"))
        return result;
    if(object->contains("Order not found"))
        return result;

    result = true;

    return result;
}

bool Exchange_bitstamp::checkCreateOrderSucces(QJsonObject *object) {

    bool result = false;

    if(object->contains("status")) {

        if(object->value("status") == "error") {

            qDebug() << "Failed to create order: " << object->value("reason");
        } else {
            result = true;
        }

    } else {
        result = true;
    }

    return result;
}

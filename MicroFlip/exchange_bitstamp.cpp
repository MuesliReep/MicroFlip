#include "exchange_bitstamp.h"
#include "json_helper.h"

Exchange_bitstamp::Exchange_bitstamp() {

  currentTask = ExchangeTask();

  fee = 0.25;

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
  downloader.doDownload(request, tickerDownloadManager, this, SLOT(updateMarketTickerReply(QNetworkReply*)));
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
    downloader.doPostDownload(request, createTradeDownloadManager, data, this, SLOT(createOrderReply(QNetworkReply*)));
}

void Exchange_bitstamp::cancelOrder(quint64 orderID) {
  (void) orderID;
  // TODO
}

void Exchange_bitstamp::updateActiveOrders(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_bitstamp::updateOrderInfo(quint64 OrderID) {

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
    downloader.doPostDownload(request, orderInfoDownloadManager, data, this, SLOT(updateOrderInfoReply(QNetworkReply*)));
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

Ticker Exchange_bitstamp::parseRawTickerData(QNetworkReply *reply) {

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

            // Extract the market data we want
            QString pair = currentTask.getAttributes().at(0);
            QJsonObject tickerData = jsonObj.value(pair).toObject();

            // Retrieve ticker data from JSON object
            double high = tickerData.value("high").toString().toDouble();
            double low  = tickerData.value("low").toString().toDouble();
            double avg  = tickerData.value("vwap").toString().toDouble();
            double last = tickerData.value("last").toString().toDouble();
            double buy  = tickerData.value("bid").toString().toDouble();
            double sell = tickerData.value("ask").toString().toDouble();
            int    age  = tickerData.value("timestamp").toString().toInt();

            ticker = Ticker(high, low, avg, last, buy, sell, age);
        }
    }

    return ticker;
}

void Exchange_bitstamp::parseRawDepthData(QNetworkReply *reply)
{
    (void) reply;
    // TODO
}

void Exchange_bitstamp::parseRawTradesData(QNetworkReply *reply)
{
    (void) reply;
    // TODO
}

void Exchange_bitstamp::parseRawBalancesData(QNetworkReply *reply)
{
    (void) reply;
    // TODO
}

quint64 Exchange_bitstamp::parseRawOrderCreationData(QNetworkReply *reply) {

    quint64 orderID = -1;

    if(reply->error()) {
        qDebug() << "Create Order Packet error: " << reply->errorString();

        return orderID;
    }

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Check if authentication was a succes
            if(jsonObj.contains("status")) {
                if(jsonObj.value("status") == "error") {
                    qDebug() << "Failed to create order: " << jsonObj.value("reason");
                    return orderID;
                }
            }

            // Extract the order ID
            orderID = jsonObj.value("id").toString("-1").toInt();
        }
    }

    return orderID;
}

void Exchange_bitstamp::parseRawOrderCancelationData(QNetworkReply *reply)
{
    (void) reply;
    // TODO
}

void Exchange_bitstamp::parseRawActiveOrdersData(QNetworkReply *reply)
{
    (void) reply;
    // TODO
}

int Exchange_bitstamp::parseRawOrderInfoData(QNetworkReply *reply) {

    // TODO

    int status = -1;

    if(reply->error()) {
        qDebug() << "Order Info Packet error: " << reply->errorString();

        return status;
    }

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    // Extract JSON object from network reply
    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Check if authentication was a succes
            if(jsonObj.contains("Missing id POST param") ||
                    jsonObj.contains("Invalid order id") ||
                    jsonObj.contains("Order not found")) {

                return status;
            }

            // Parse order status
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
    }

    return status;
}

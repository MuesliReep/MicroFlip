#include "exchange_bitfinex.h"

#include <QJsonArray>

#include "json_helper.h"

Exchange_bitfinex::Exchange_bitfinex() {

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

void Exchange_bitfinex::startWork() {

  this->apiKey    = config->getApiKey();
  this->apiSecret = config->getApiSecret();

  // 30 requests per min
  timer->start(2000);
  //timer2->start(1*1100); // TODO: determine correct amount
}

QString Exchange_bitfinex::createNonce() {

  uint now = QDateTime::currentDateTime().toMSecsSinceEpoch() / 250;

  if(lastNonce == now) {
    lastNonce+=2;
  }
  else
    lastNonce = now;

  return QString::number(lastNonce);
}

//----------------------------------//
//              Tasks               //
//----------------------------------//

void Exchange_bitfinex::updateMarketTicker(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = downloader.generateRequest(QUrl("https://api.bitfinex.com/v1/pubticker/"+pair));

  // Execute the download
  downloader.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketTickerReply(QNetworkReply*)));
}

void Exchange_bitfinex::updateMarketDepth(QString pair) {

    (void) pair;
  // TODO
}

void Exchange_bitfinex::updateMarketTrades(QString pair) {

  // Create the request to download new data
  // API v2 pair is in upper case and is preceded by a "t"
  //QNetworkRequest request = downloader.generateRequest(QUrl("https://api.bitfinex.com/v2/trades/t" + pair.toUpper() + "/hist?limit=999"));
  QNetworkRequest request = downloader.generateRequest(QUrl("https://api.bitfinex.com/v1/trades/" + pair));

  // Execute the download
  downloader.doDownload(request, updateMarketTradesDownloadManager, this, SLOT(updateMarketTradesReply(QNetworkReply*)));
}

void Exchange_bitfinex::updateBalances() {

    // Create JSON payload
    QJsonObject payload;
    payload.insert("request", "/v1/balances");
    payload.insert("nonce",   createNonce());

    QJsonDocument payloadDocument(payload);
    QByteArray payloadData   = payloadDocument.toJson(QJsonDocument::Compact);
    QByteArray payloadData64 = payloadData.toBase64();

    // Sign the data
    QByteArray signature = QMessageAuthenticationCode::hash(payloadData64, this->apiSecret.toUtf8(), QCryptographicHash::Sha384).toHex();

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.bitfinex.com/v1/order/new"));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-type"),    QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("Accept"),          QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-APIKEY"),    this->apiKey.toUtf8());
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-PAYLOAD"),   payloadData64);
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-SIGNATURE"), signature);

    // Execute the download
    downloader.doPostDownload(request, updateBalancesDownloadManager, payloadData, this, SLOT(updateBalancesReply(QNetworkReply*)));
}

void Exchange_bitfinex::createOrder(QString Pair, int Type, double Rate, double Amount) {

    // Create JSON payload
    QJsonObject payload;
    payload.insert("request",  "/v1/order/new");
    payload.insert("nonce",    createNonce());
    payload.insert("symbol",   Pair);
    payload.insert("amount",   QString::number(Amount));
    payload.insert("price",    QString::number(Rate));
    payload.insert("exchange", "bitfinex");
    payload.insert("side",     Type == 0 ? "buy" : "sell");
    payload.insert("type",     "exchange limit");

    QJsonDocument payloadDocument(payload);
    QByteArray payloadData   = payloadDocument.toJson(QJsonDocument::Compact);
    QByteArray payloadData64 = payloadData.toBase64();

    // Sign the data
    QByteArray signature = QMessageAuthenticationCode::hash(payloadData64, this->apiSecret.toUtf8(), QCryptographicHash::Sha384).toHex();

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.bitfinex.com/v1/order/new"));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-type"),    QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("Accept"),          QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-APIKEY"),    this->apiKey.toUtf8());
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-PAYLOAD"),   payloadData64);
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-SIGNATURE"), signature);

    // Execute the download
    downloader.doPostDownload(request, createTradeDownloadManager, payloadData, this, SLOT(createOrderReply(QNetworkReply*)));
}

void Exchange_bitfinex::cancelOrder(quint64 orderID) {

    (void) orderID;
  // TODO
}

void Exchange_bitfinex::updateActiveOrders(QString pair) {

    (void) pair;
  // TODO
}

void Exchange_bitfinex::updateOrderInfo(quint64 OrderID) {

    double orderIDd = OrderID;

    // Create JSON payload
    QJsonObject payload;
    payload.insert("request",  "/v1/order/status");
    payload.insert("nonce",    createNonce());
    payload.insert("order_id", orderIDd);

    // Create the payloads
    QJsonDocument payloadDocument(payload);
    QByteArray payloadData   = payloadDocument.toJson(QJsonDocument::Compact);
    QByteArray payloadData64 = payloadData.toBase64();

    // Sign the data
    QByteArray signature = QMessageAuthenticationCode::hash(payloadData64, this->apiSecret.toUtf8(), QCryptographicHash::Sha384).toHex();

    // Create request
    QNetworkRequest request = downloader.generateRequest(QUrl("https://api.bitfinex.com/v1/order/status"));

    // Add headers
    downloader.addHeaderToRequest(&request, QByteArray("Content-type"),    QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("Accept"),          QByteArray("application/json"));
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-APIKEY"),    this->apiKey.toUtf8());
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-PAYLOAD"),   payloadData64);
    downloader.addHeaderToRequest(&request, QByteArray("X-BFX-SIGNATURE"), signature);

    // Execute the download
    downloader.doPostDownload(request, orderInfoDownloadManager, payloadData, this, SLOT(updateOrderInfoReply(QNetworkReply*)));
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

Ticker Exchange_bitfinex::parseRawTickerData(QNetworkReply *reply) {

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
            double high = jsonObj.value("high").toString("-1.0").toDouble();
            double low  = jsonObj.value("low").toString("-1.0").toDouble();
            double avg  = jsonObj.value("avg").toString("-1.0").toDouble();
            double last = jsonObj.value("last_price").toString("-1.0").toDouble();
            double buy  = jsonObj.value("bid").toString("-1.0").toDouble();
            double sell = jsonObj.value("ask").toString("-1.0").toDouble();
            int    age  = jsonObj.value("timestamp").toString("-1").toInt();

            ticker = Ticker(high, low, avg, last, buy, sell, age);
        }
    }

    return ticker;
}

void Exchange_bitfinex::parseRawDepthData(QNetworkReply *reply)
{
    // TODO

    if(reply->error()) {
        qDebug() << "Market Depth Packet error: " << reply->errorString();
    }
}

void Exchange_bitfinex::parseRawTradesData(QNetworkReply *reply) {

    // TODO

    if(reply->error()) {
        qDebug() << "Market Trades Packet error: " << reply->errorString();
    }

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {

    }
}

quint64 Exchange_bitfinex::parseRawOrderCreationData(QNetworkReply *reply) {

    quint64 orderID = -1;

    if(reply->error()) {
        qDebug() << "Create Order Packet error: " << reply->errorString();

        return orderID;
    }

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // Check if authentication was a succes
            if(jsonObj.contains("order_id")) {

                // Parse new data
                double orderIDd = jsonObj.value("order_id").toDouble(-1.0);
                orderID = orderIDd;

                qDebug() << "Order created successfully, ID: " << orderID;
            }
            else {
                qDebug() << "Order creation error!" ;
            }
        }
    }

    return orderID;
}

int Exchange_bitfinex::parseRawOrderInfoData(QNetworkReply *reply) {

    int status = -1;

    if(reply->error()) {
        qDebug() << "Order Info Packet error: " << reply->errorString();

        return status;
    }

    QJsonDocument jsonDoc;
    QJsonObject   jsonObj;

    if(JSON_Helper::getDocumentFromNetworkReply(reply, &jsonDoc)) {
        if(JSON_Helper::getObjectFromDocument(&jsonDoc, &jsonObj)) {

            // First check if the "is_live" value is available
            if(jsonObj.contains("is_live")) {

                bool statusBool = jsonObj.value("is_live").toBool();

                if(statusBool) {

                    status = 0; // In process
                }
                else { // Order is no longer active

                    // Check if the order was cancelled
                    double remainingAmount = jsonObj.value("remaining_amount").toString("0.0").toDouble();

                    if(remainingAmount > 0.0) {

                        status = -2;
                    }
                    else { // Else the order was completed!

                        status = 1; //
                    }
                }
            } else {

                status = -2;
            }
        }
    }

    return status;
}

#include "exchange_bitfinex.h"

Exchange_bitfinex::Exchange_bitfinex() {

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

void Exchange_bitfinex::startWork() {

  this->apiKey    = c->getApiKey();
  this->apiSecret = c->getApiSecret();

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

    (void) pair;
  // TODO
}

void Exchange_bitfinex::updateBalances() {

  // TODO
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
    downloader.doPostDownload(request, createTradeDownloadManager, payloadData, this, SLOT(CreateOrderReply(QNetworkReply*)));
}

void Exchange_bitfinex::cancelOrder(uint orderID) {

    (void) orderID;
  // TODO
}

void Exchange_bitfinex::updateActiveOrders(QString pair) {

    (void) pair;
  // TODO
}

void Exchange_bitfinex::updateOrderInfo(uint OrderID) {

    // Create JSON payload
    QJsonObject payload;
    payload.insert("request", "/v1/order/status");
    payload.insert("nonce",   createNonce());
    payload.insert("id",      QString::number(OrderID));

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
    downloader.doPostDownload(request, orderInfoDownloadManager, payloadData, this, SLOT(UpdateOrderInfoReply(QNetworkReply*)));
}

void Exchange_bitfinex::executeExchangeTask(ExchangeTask *exchangeTask) {

  switch(exchangeTask->getTask()) {

    case 0: updateMarketTicker(exchangeTask->getAttributes().at(0)); break;
    case 1: updateMarketDepth(exchangeTask->getAttributes().at(0));  break;
    case 2: updateMarketDepth(exchangeTask->getAttributes().at(0));  break;
    case 3: updateBalances(); break;
    case 4:
      createOrder(exchangeTask->getAttributes().at(0),
                  exchangeTask->getAttributes().at(1).toInt(),
                  exchangeTask->getAttributes().at(2).toDouble(),
                  exchangeTask->getAttributes().at(3).toDouble());
      break;
    case 5: cancelOrder(exchangeTask->getAttributes().at(0).toUInt()); break;
    case 6: updateActiveOrders(exchangeTask->getAttributes().at(0));   break;
    case 7: updateOrderInfo(exchangeTask->getAttributes().at(0).toUInt()); break;
    default: qDebug() << "Bad exchange task received: " << exchangeTask->getTask(); break;
  }
}

//----------------------------------//
//          Public Slots            //
//----------------------------------//

void Exchange_bitfinex::receiveUpdateMarketTicker(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(0, sender, attr));
}
void Exchange_bitfinex::receiveUpdateMarketDepth(QString pair, QObject *sender) {
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(1, sender, attr));
}
void Exchange_bitfinex::receiveUpdateMarketTrades(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(2, sender, attr));
}
void Exchange_bitfinex::receiveUpdateBalances(QObject *sender){
  exchangeTasks.append(ExchangeTask(3, sender));
}
void Exchange_bitfinex::receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  attr.append(QString(QString::number(type)));
  attr.append(QString(QString::number(rate)));
  attr.append(QString(QString::number(amount)));
  exchangeTasks.append(ExchangeTask(4, sender, attr));
}
void Exchange_bitfinex::receiveCancelOrder(uint orderID, QObject *sender){
  QList<QString> attr; attr.append(QString::number(orderID));
  exchangeTasks.append(ExchangeTask(5, sender, attr));
}
void Exchange_bitfinex::receiveUpdateActiveOrders(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(6, sender, attr));
}
void Exchange_bitfinex::receiveUpdateOrderInfo(uint orderID, QObject *sender){

  // TODO: beter way of doing this
  // Check if task already exists in list
  for(int i = 0; i < exchangeTasks.size(); i++) {
    ExchangeTask task = exchangeTasks.at(i);
    if(task.getTask() == 7) {
      if(sender == task.getSender())
        return;
    }
  }

  QList<QString> attr; attr.append(QString::number(orderID));
  exchangeTasks.append(ExchangeTask(7, sender, attr));
}

//----------------------------------//
//           Task Replies           //
//----------------------------------//

void Exchange_bitfinex::UpdateMarketTickerReply(QNetworkReply *reply) {

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

void Exchange_bitfinex::UpdateMarketDepthReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_bitfinex::UpdateMarketTradesReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_bitfinex::UpdateBalancesReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_bitfinex::CreateOrderReply(QNetworkReply *reply) {

    int orderID = -1;

    if(!reply->error()) {

      QJsonObject jsonObj;

      // Extract JSON object from network reply
      getObjectFromDocument(reply, &jsonObj);

      // Check if authentication was a succes
      if(jsonObj.contains("order_id")) {

        // Parse new data
        orderID = jsonObj.value("order_id").toInt(-1);

        qDebug() << "Trade created successfully, ID: " << orderID;
      }
      else {
        qDebug() << "Trade error: " ;//<< getRequestErrorMessage(&jsonObj);
      }
    } else {
      QJsonObject jsonObj;
      bool result = getObjectFromDocument(reply, &jsonObj);
      QString errorString = reply->errorString();
      qDebug() << "Trade Packet error: " << errorString;
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

void Exchange_bitfinex::CancelOrderReply(QNetworkReply *reply) {

  int status = -1;

  if(!reply->error()) {

  }
}

void Exchange_bitfinex::UpdateActiveOrdersReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_bitfinex::UpdateOrderInfoReply(QNetworkReply *reply) {

    int status = -1;

    if(!reply->error()) {

        QJsonObject jsonObj;

        // Extract JSON object from network reply
        getObjectFromDocument(reply, &jsonObj);

        // First check if the "is_live" value is available
        if(jsonObj.contains("is_live")) {

            bool statusBool = jsonObj.value("is_live").toBool();

            if(statusBool) {
                status = 0;
            } else {
                status = 1;
            }
        } else {
            status = -2;
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
//          Private Slots           //
//----------------------------------//

void Exchange_bitfinex::updateTick() {

  // While currentTask is not complete, do nothing
  if(currentTask.getTask() != -1)
    return;

  // Get a new task and execute it
  if(exchangeTasks.size() > 0) {

    // Get a task and remove it from the list
    currentTask = exchangeTasks.takeFirst();

    // Execute the task
    executeExchangeTask(&currentTask);
  }
}

void Exchange_bitfinex::updateTick2() {

  // While currentTask is not complete, do nothing
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

// Grabs a JSON object from a Network reply
// Returns true if succesfull
bool Exchange_bitfinex::getObjectFromDocument(QNetworkReply *reply, QJsonObject *object) {

  QJsonDocument   jsonDoc;
  QJsonParseError error;

  jsonDoc = QJsonDocument().fromJson(reply->readAll(), &error);
  *object = jsonDoc.object();

  return true; // TODO: check json validity
}

Ticker Exchange_bitfinex::parseRawTickerData(QJsonObject *rawData) {

  // Values are stored as strings, so first convert to string then further
  double high = rawData->value("high").toString("-1.0").toDouble();
  double low  = rawData->value("low").toString("-1.0").toDouble();
  double avg  = rawData->value("avg").toString("-1.0").toDouble();
  double last = rawData->value("last_price").toString("-1.0").toDouble();
  double buy  = rawData->value("bid").toString("-1.0").toDouble();
  double sell = rawData->value("ask").toString("-1.0").toDouble();
  int    age  = rawData->value("timestamp").toString("-1").toInt();

  return Ticker(high, low, avg, last, buy, sell, age);
}

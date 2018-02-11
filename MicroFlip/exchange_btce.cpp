#include "exchange_btce.h"

Exchange_btce::Exchange_btce() {

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

void Exchange_btce::startWork() {

  apiKey = c->getApiKey();
  apiSecret = c->getApiSecret();

  timer->start(c->getCoolDownTime()*1100);
  //timer2->start(1*1100); // TODO: determine correct amount
}

void Exchange_btce::createNonce(QByteArray *nonce) {

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

void Exchange_btce::updateMarketTicker(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = downloader.generateRequest(QUrl("https://btc-e.com/api/3/ticker/"+pair));

  // Execute the download
  downloader.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketTickerReply(QNetworkReply*)));
}

void Exchange_btce::updateMarketDepth(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_btce::updateMarketTrades(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_btce::updateBalances() {

  // TODO
}

void Exchange_btce::createOrder(QString Pair, int Type, double Rate, double Amount) {

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
  //QString sPrice; sPrice.setNum(Price,'f',3);
  price.append(QString::number(Rate,'f',3));

  QByteArray amount("amount=");
  //QString sAmount; sAmount.setNum(Amount,'f',8);
  amount.append(QString::number(Amount,'f',8));

  QByteArray data(method +"&"+ nonce +"&"+ pair +"&"+ type +"&"+ price +"&"+ amount); //qDebug() << "data: " << data;

  // Sign the data
  QByteArray sign = QMessageAuthenticationCode::hash(data, apiSecret.toUtf8(), QCryptographicHash::Sha512).toHex();

  // Create request
  QNetworkRequest request = downloader.generateRequest(QUrl("https://btc-e.com/tapi/3/"));

  // Add headers
  downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
  downloader.addHeaderToRequest(&request, QByteArray("Key"), apiKey.toUtf8());
  downloader.addHeaderToRequest(&request, QByteArray("Sign"), sign);

  // Execute the download
  downloader.doPostDownload(request, createTradeDownloadManager, data, this, SLOT(CreateOrderReply(QNetworkReply*)));
}

void Exchange_btce::cancelOrder(uint orderID) {
  (void) orderID;
  // TODO
}

void Exchange_btce::updateActiveOrders(QString pair) {
  (void) pair;
  // TODO
}

void Exchange_btce::updateOrderInfo(uint OrderID) {

  // Create POST data from method and nonce
  QByteArray method("method=OrderInfo");
  QByteArray nonce;
  createNonce(&nonce);
  nonce.prepend("nonce=");

  QByteArray orderID("order_id=");
  orderID.append(QString::number(OrderID));

  QByteArray data(method +"&"+ nonce + "&" + orderID);  //qDebug() << "data: " << data;

  // Sign the data
  QByteArray sign = QMessageAuthenticationCode::hash(data, apiSecret.toUtf8(), QCryptographicHash::Sha512).toHex();

  // Create request
  QNetworkRequest request = downloader.generateRequest(QUrl("https://btc-e.com/tapi/3/"));

  // Add headers
  downloader.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
  downloader.addHeaderToRequest(&request, QByteArray("Key"), apiKey.toUtf8());
  downloader.addHeaderToRequest(&request, QByteArray("Sign"), sign);

  // Execute the download
  downloader.doPostDownload(request, orderInfoDownloadManager, data, this, SLOT(UpdateOrderInfoReply(QNetworkReply*)));
}

void Exchange_btce::executeExchangeTask(ExchangeTask *exchangeTask) {

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

void Exchange_btce::receiveUpdateMarketTicker(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(0, sender, attr));
}
void Exchange_btce::receiveUpdateMarketDepth(QString pair, QObject *sender) {
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(1, sender, attr));
}
void Exchange_btce::receiveUpdateMarketTrades(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(2, sender, attr));
}
void Exchange_btce::receiveUpdateBalances(QObject *sender){
  exchangeTasks.append(ExchangeTask(3, sender));
}
void Exchange_btce::receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  attr.append(QString(QString::number(type)));
  attr.append(QString(QString::number(rate)));
  attr.append(QString(QString::number(amount)));
  exchangeTasks.append(ExchangeTask(4, sender, attr));
}
void Exchange_btce::receiveCancelOrder(uint orderID, QObject *sender){
  QList<QString> attr; attr.append(QString::number(orderID));
  exchangeTasks.append(ExchangeTask(5, sender, attr));
}
void Exchange_btce::receiveUpdateActiveOrders(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(6, sender, attr));
}
void Exchange_btce::receiveUpdateOrderInfo(uint orderID, QObject *sender){

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

void Exchange_btce::UpdateMarketTickerReply(QNetworkReply *reply) {

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

    // Send signal to GUI to update
    //sendTicker(m.getTicker());
  }
  else
    qDebug() << "Ticker Packet error";

  reply->deleteLater();

  // Disconnect the download signal and release
  disconnect(tickerDownloadManager, 0, this, 0);

  // tickerDownloadManager->deleteLater();

  // Mark this task complete
  currentTask = ExchangeTask();
}

void Exchange_btce::UpdateMarketDepthReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_btce::UpdateMarketTradesReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_btce::UpdateBalancesReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_btce::CreateOrderReply(QNetworkReply *reply) {

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
  connect(this, SIGNAL(sendOrderID(int)), currentTask.getSender(), SLOT(orderCreateReply(int)));
  emit sendOrderID(orderID);
  disconnect(this, SIGNAL(sendOrderID(int)), currentTask.getSender(), SLOT(orderCreateReply(int)));

  reply->deleteLater();

  disconnect(createTradeDownloadManager, 0, this, 0);

  // Mark this task complete
  currentTask = ExchangeTask();
}

void Exchange_btce::CancelOrderReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_btce::UpdateActiveOrdersReply(QNetworkReply *reply) {

  (void) reply;
}

void Exchange_btce::UpdateOrderInfoReply(QNetworkReply *reply) {

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
//          Private Slots           //
//----------------------------------//

void Exchange_btce::updateTick() {

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

void Exchange_btce::updateTick2() {

  // While currentTask is not complete, do nothing
}

//----------------------------------//
//             Parsers              //
//----------------------------------//

// Grabs a JSON object from a Network reply
// Returns true if succesfull
bool Exchange_btce::getObjectFromDocument(QNetworkReply *reply, QJsonObject *object) {

  QJsonDocument   jsonDoc;
  QJsonParseError error;

  jsonDoc = QJsonDocument().fromJson(reply->readAll(), &error);
  *object = jsonDoc.object();

  return true; // TODO: check json validity
}

Ticker Exchange_btce::parseRawTickerData(QJsonObject *rawData) {

  QJsonObject jTicker;

  // Retrieve ticker object from JSON object
  //jTicker = rawData->value("btc_usd").toObject();

  //
  double high = rawData->value("high").toDouble();
  double low  = rawData->value("low").toDouble();
  double avg  = rawData->value("avg").toDouble();
  double last = rawData->value("last").toDouble();
  double buy  = rawData->value("buy").toDouble();
  double sell = rawData->value("sell").toDouble();
  int    age  = rawData->value("age").toInt();

  return Ticker(high, low, avg, last, buy, sell, age);
}

bool Exchange_btce::checkSuccess(QJsonObject *object) {

  bool result = false;

  if(!object->contains("success"))
    return result;

  if(object->value("success").toInt() == 1)
    result = true;

  return result;
}

QString Exchange_btce::getRequestErrorMessage(QJsonObject *object) {

  return object->value("error").toString();
}

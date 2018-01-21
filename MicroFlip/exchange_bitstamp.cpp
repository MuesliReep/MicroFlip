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

  apiKey    = c->getApiKey();
  apiSecret = c->getApiSecret();

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

  // TODO
}

void Exchange_bitstamp::updateMarketTrades(QString pair) {

  // TODO
}

void Exchange_bitstamp::updateBalances() {

  // TODO
}

void Exchange_bitstamp::createOrder(QString Pair, int Type, double Rate, double Amount) {

  // TODO
}

void Exchange_bitstamp::cancelOrder(uint orderID) {

  // TODO
}

void Exchange_bitstamp::updateActiveOrders(QString pair) {

  // TODO
}

void Exchange_bitstamp::updateOrderInfo(uint OrderID) {

  // TODO
}

void Exchange_bitstamp::executeExchangeTask(ExchangeTask *exchangeTask) {

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

void Exchange_bitstamp::receiveUpdateMarketTicker(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(0, sender, attr));
}
void Exchange_bitstamp::receiveUpdateMarketDepth(QString pair, QObject *sender) {
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(1, sender, attr));
}
void Exchange_bitstamp::receiveUpdateMarketTrades(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(2, sender, attr));
}
void Exchange_bitstamp::receiveUpdateBalances(QObject *sender){
  exchangeTasks.append(ExchangeTask(3, sender));
}
void Exchange_bitstamp::receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  attr.append(QString(QString::number(type)));
  attr.append(QString(QString::number(rate)));
  attr.append(QString(QString::number(amount)));
  exchangeTasks.append(ExchangeTask(4, sender, attr));
}
void Exchange_bitstamp::receiveCancelOrder(uint orderID, QObject *sender){
  QList<QString> attr; attr.append(QString::number(orderID));
  exchangeTasks.append(ExchangeTask(5, sender, attr));
}
void Exchange_bitstamp::receiveUpdateActiveOrders(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(6, sender, attr));
}
void Exchange_bitstamp::receiveUpdateOrderInfo(uint orderID, QObject *sender){

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
      Ticker ticker = parseRawTickerData(&tickerData);

      // Connect & send to the initiator
      connect(this, SIGNAL(sendTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));
      emit sendTicker(ticker);
      disconnect(this, SIGNAL(sendTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));

      // Send signal to GUI to update
      //sendTicker(m.getTicker());
  }
  else {
    qDebug() << "Ticker Packet error: " << reply->errorString();
  }

  reply->deleteLater();

  // Disconnect the download signal and release
  disconnect(tickerDownloadManager, 0, this, 0);

  // tickerDownloadManager->deleteLater();

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

    (void) reply;
    // TODO
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

    (void) reply;
    // TODO
}


//----------------------------------//
//          Private Slots           //
//----------------------------------//

void Exchange_bitstamp::updateTick() {

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

void Exchange_bitstamp::updateTick2() {

  // While currentTask is not complete, do nothing
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

  QJsonObject jTicker;

  // Retrieve ticker object from JSON object
  //jTicker = rawData->value("btc_usd").toObject();

  //
  double high = rawData->value("high").toDouble();
  double low  = rawData->value("low").toDouble();
  double avg  = rawData->value("vwap").toDouble();
  double last = rawData->value("last").toDouble();
  double buy  = rawData->value("bid").toDouble();
  double sell = rawData->value("ask").toDouble();
  int    age  = rawData->value("timestamp").toInt();

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

QString Exchange_bitstamp::getRequestErrorMessage(QJsonObject *object) {

  return object->value("error").toString();
}

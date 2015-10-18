#include "exchange_btce.h"

Exchange_btce::Exchange_btce() {

  currentTask = NULL;

  // Initiate download managers
  tickerDownloadManager = new QNetworkAccessManager(this);
  depthDownloadManager  = new QNetworkAccessManager(this);
}

//----------------------------------//
//              Tasks               //
//----------------------------------//

void Exchange_btce::updateMarketTicker(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = d.generateRequest(QUrl("https://btc-e.com/api/3/ticker/"+pair));

  // Execute the download
  d.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketDepthReply(QNetworkReply*)));
}

void Exchange_btce::updateMarketDepth(QString pair) {


}

void Exchange_btce::updateMarketTrades(QString pair) {


}

void Exchange_btce::updateBalances() {


}

void Exchange_btce::createOrder(QString pair, int type, double rate, double amount) {


}

void Exchange_btce::cancelOrder(uint orderID) {


}

void Exchange_btce::updateActiveOrders(QString pair) {


}

void Exchange_btce::updateOrderInfo(uint orderID) {


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
  QList<QString> attr; attr.append(QString(orderID));
  exchangeTasks.append(ExchangeTask(5, sender, attr));
}
void Exchange_btce::receiveUpdateActiveOrders(QString pair, QObject *sender){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(6, sender, attr));
}
void Exchange_btce::receiveUpdateOrderInfo(uint orderID, QObject *sender){
  QList<QString> attr; attr.append(QString(orderID));
  exchangeTasks.append(ExchangeTask(7, sender, attr));
}

//----------------------------------//
//           Task Replies           //
//----------------------------------//

void Exchange_btce::UpdateMarketTickerReply(QNetworkReply *reply) {


}

void Exchange_btce::UpdateMarketDepthReply(QNetworkReply *reply) {


}

void Exchange_btce::UpdateMarketTradesReply(QNetworkReply *reply) {


}

void Exchange_btce::UpdateBalancesReply(QNetworkReply *reply) {


}

void Exchange_btce::CreateOrderReply(QNetworkReply *reply) {


}

void Exchange_btce::CancelOrderReply(QNetworkReply *reply) {


}

void Exchange_btce::UpdateActiveOrdersReply(QNetworkReply *reply) {


}

void Exchange_btce::UpdateOrderInfoReply(QNetworkReply *reply) {


}


//----------------------------------//
//          Private Slots           //
//----------------------------------//

void Exchange_btce::updateTick() {

  // While currentTask is not complete, do nothing
  if(currentTask != NULL)
    return;

  // Get a new task and execute it
  if(exchangeTasks.size() > 0) {

    // Get a task and remove it from the list
    *currentTask = exchangeTasks.takeFirst();

    // Execute the task
    executeExchangeTask(currentTask);
  }
}

void Exchange_btce::updateTick2() {

  // While currentTask is not complete, do nothing
}

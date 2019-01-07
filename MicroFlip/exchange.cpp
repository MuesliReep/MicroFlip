#include "exchange.h"

#include "common.h"

#include <QDateTime>

Exchange::Exchange(QObject *parent) : QObject(parent)
{

}

void Exchange::setConfig(Config *config)
{
  this->config = config;
}

double Exchange::getFee() const
{
    return fee;
}

QList<Balance> Exchange::getBalances() const
{
    return balances;
}

double Exchange::getBalance(QString currency) const
{
    (void) currency;
    double amount = -1.0;
    /*

    for(int i = 0; i < this->balances.length(); i++) {

        if(QString::compare(this->balances.at(i).getCurrency(), currency,Qt::CaseInsensitive) == 0){
            amount = this->balances.at(i).getAmount();
        }
    }
    */
    return amount;
}

QString Exchange::getExchangeName() const
{
    return exchangeName;
}

void Exchange::executeExchangeTask(ExchangeTask *exchangeTask)
{
    switch(exchangeTask->getTask()) {

    case 0: updateMarketTicker(exchangeTask->getAttributes().at(0)); break;
    case 1: updateMarketDepth (exchangeTask->getAttributes().at(0)); break;
    case 2: updateMarketDepth (exchangeTask->getAttributes().at(0)); break;
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

void Exchange::receiveUpdateMarketTicker(QString pair, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(0, sender, SenderID, attr));
}
void Exchange::receiveUpdateMarketDepth(QString pair, QObject *sender, int SenderID) {
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(1, sender, SenderID, attr));
}
void Exchange::receiveUpdateMarketTrades(QString pair, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(2, sender, SenderID, attr));
}
void Exchange::receiveUpdateBalances(QObject *sender, int SenderID){
  exchangeTasks.append(ExchangeTask(3, sender, SenderID));
}
void Exchange::receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString(pair));
  attr.append(QString(QString::number(type)));
  attr.append(QString(QString::number(rate)));
  attr.append(QString(QString::number(amount)));
  exchangeTasks.append(ExchangeTask(4, sender, SenderID, attr));
}
void Exchange::receiveCancelOrder(qint64 orderID, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString::number(orderID));
  exchangeTasks.append(ExchangeTask(5, sender, SenderID, attr));
}
void Exchange::receiveUpdateActiveOrders(QString pair, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(6, sender, SenderID, attr));
}
void Exchange::receiveUpdateOrderInfo(qint64 orderID, QObject *sender, int SenderID){

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
  exchangeTasks.append(ExchangeTask(7, sender, SenderID, attr));
}

void Exchange::updateMarketTickerReply(QNetworkReply *reply)
{
    Ticker ticker;

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Ticker Packet error: " + reply->errorString(), logSeverity::LOG_CRITICAL);
    } else {
        // Parse the raw data
        ticker = parseRawTickerData(reply);
    }

    // Connect & send to the initiator
    connect(this, SIGNAL(sendNewMarketTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));
    emit sendNewMarketTicker(ticker);
    disconnect(this, SIGNAL(sendNewMarketTicker(Ticker)), currentTask.getSender(), SLOT(UpdateMarketTickerReply(Ticker)));

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(tickerDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::updateMarketDepthReply(QNetworkReply *reply)
{
    // TODO

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Market Depth Packet error: " + reply->errorString(), logSeverity::LOG_CRITICAL);
    } else {
        parseRawDepthData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(updateMarketDepthDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::updateMarketTradesReply(QNetworkReply *reply)
{
    // TODO

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Market Trades Update Packet error: " + reply->errorString(), logSeverity::LOG_CRITICAL);
    } else {
        parseRawTradesData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(updateMarketTradesDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::updateBalancesReply(QNetworkReply *reply)
{
    // TODO
    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Balance Update Packet error: " + reply->errorString(), logSeverity::LOG_CRITICAL);
    } else {
        parseRawBalancesData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(updateBalancesDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::createOrderReply(QNetworkReply *reply)
{
    qint64 orderID = -1;

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Create Order Packet error: " + reply->errorString() + " " + reply->readAll(), logSeverity::LOG_CRITICAL);
    } else {
        orderID = parseRawOrderCreationData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(createTradeDownloadManager, 0, this, 0);

    // Connect & send order ID to the initiator
    connect(this, SIGNAL(sendNewOrderID(qint64)), currentTask.getSender(), SLOT(orderCreateReply(qint64)));
    emit sendNewOrderID(orderID);
    disconnect(this, SIGNAL(sendNewOrderID(qint64)), currentTask.getSender(), SLOT(orderCreateReply(qint64)));

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::cancelOrderReply(QNetworkReply *reply)
{
    // TODO

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Order Cancelation Packet error: " + reply->errorString(), logSeverity::LOG_CRITICAL);
    } else {
        parseRawOrderCancelationData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(cancelOrderDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::updateActiveOrdersReply(QNetworkReply *reply)
{
    // TODO

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Order Cancelation Packet error: " + reply->errorString(), logSeverity::LOG_CRITICAL);
    } else {
        parseRawActiveOrdersData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(activeOrdersDownloadManager, 0, this, 0);

    // Mark this task complete
    currentTask = ExchangeTask();
}

void Exchange::updateOrderInfoReply(QNetworkReply *reply)
{
    int status = -1;

    if(reply->error()) {
        updateLog(currentTask.getSenderID(), className, "Order Info Packet error: " + reply->errorString() + " " + reply->readAll(), logSeverity::LOG_CRITICAL);
    } else {
        status = parseRawOrderInfoData(reply);
    }

    reply->deleteLater();

    // Disconnect the download signal and release
    disconnect(orderInfoDownloadManager, 0, this, 0);

    // Connect & send order status to the initiator
    connect(this, SIGNAL(sendNewOrderInfo(int)), currentTask.getSender(), SLOT(orderInfoReply(int)));
    emit sendNewOrderInfo(status);
    disconnect(this, SIGNAL(sendNewOrderInfo(int)), currentTask.getSender(), SLOT(orderInfoReply(int)));

    // Mark this task complete
    currentTask = ExchangeTask();
}

//----------------------------------//
//          Private Slots           //
//----------------------------------//

void Exchange::updateTick() {

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

void Exchange::updateTick2() {

  // While currentTask is not complete, do nothing
}

//----------------------------------//
//           ExchangeTask           //
//----------------------------------//

ExchangeTask::ExchangeTask(int Task, int senderID) {

  this->task     = Task;
  this->senderID = senderID;
}

ExchangeTask::ExchangeTask(int Task, QObject *Sender, int senderID) {

  this->task     = Task;
  this->sender   = Sender;
  this->senderID = senderID;
}

ExchangeTask::ExchangeTask(int Task, QObject *Sender, int senderID, QList<QString> Attributes) {

  this->task       = Task;
  this->sender     = Sender;
  this->senderID   = senderID;
  this->attributes = Attributes;
}

QObject *ExchangeTask::getSender() const
{
    return sender;
}

int ExchangeTask::getSenderID() const
{
    return senderID;
}
int ExchangeTask::getTask() const
{
  return task;
}
QList<QString> ExchangeTask::getAttributes() const
{
  return attributes;
}

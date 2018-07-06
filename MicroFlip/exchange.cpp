#include "exchange.h"

Exchange::Exchange(QObject *parent) : QObject(parent)
{

}

void Exchange::setConfig(Config *config)
{
  this->c = config;
}

double Exchange::getFee()
{
    return fee;
}

QList<Balance> Exchange::getBalances()
{
    return balances;
}

double Exchange::getBalance(QString currency)
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
void Exchange::receiveCancelOrder(quint64 orderID, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString::number(orderID));
  exchangeTasks.append(ExchangeTask(5, sender, SenderID, attr));
}
void Exchange::receiveUpdateActiveOrders(QString pair, QObject *sender, int SenderID){
  QList<QString> attr; attr.append(QString(pair));
  exchangeTasks.append(ExchangeTask(6, sender, SenderID, attr));
}
void Exchange::receiveUpdateOrderInfo(quint64 orderID, QObject *sender, int SenderID){

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

int ExchangeTask::getSenderID()
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

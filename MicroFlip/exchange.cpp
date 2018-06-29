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

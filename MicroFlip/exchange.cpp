#include "exchange.h"

Exchange::Exchange(QObject *parent) : QObject(parent)
{

}

void Exchange::setConfig(Config *C)
{
  this->c = C;
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

ExchangeTask::ExchangeTask(int Task, int SenderID) {

  this->task = Task;
  this->senderID;
}

ExchangeTask::ExchangeTask(int Task, QObject *Sender, int SenderID) {

  this->task     = Task;
  this->sender   = Sender;
  this->senderID = SenderID;
}

ExchangeTask::ExchangeTask(int Task, QObject *Sender, int SenderID, QList<QString> Attributes) {

  this->task       = Task;
  this->sender     = Sender;
  this->senderID   = SenderID;
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

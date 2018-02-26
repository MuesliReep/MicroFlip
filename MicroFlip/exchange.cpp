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

    for(int i = 0; i < balances.length(); i++) {

        if(QString::compare(balances.at(i).getCurrency(), currency,Qt::CaseInsensitive) == 0){
            amount = balances.at(i).getAmount();
        }
    }

    return amount;
}

ExchangeTask::ExchangeTask(int Task) {
 this->task = Task;
}

ExchangeTask::ExchangeTask(int Task, QObject *Sender) {

  this->task   = Task;
  this->sender = Sender;
}

ExchangeTask::ExchangeTask(int Task, QObject *Sender, QList<QString> Attributes) {

  this->task       = Task;
  this->sender     = Sender;
  this->attributes = Attributes;
}

QObject *ExchangeTask::getSender() const
{
  return sender;
}
int ExchangeTask::getTask() const
{
  return task;
}
QList<QString> ExchangeTask::getAttributes() const
{
  return attributes;
}

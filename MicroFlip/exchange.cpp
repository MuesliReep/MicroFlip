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

void Exchange::checkBalance(int currency, double amount, bool *result)
{

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

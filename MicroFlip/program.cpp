#include "program.h"

Program::Program(QObject *parent) : QObject(parent) {

  // Load configuration from file (if any)
  Config c;
  c.loadConfigFromFile();

  // Create a market bot
//  ExchangeBot *e = new ExchangeBot_btce();
//  e->setConfig(&c);
//  e->startBot();

  // Create an exchange interface
  Exchange *e = new Exchange_btce();
  e->setConfig(&c);
  e->startWork();

  // Create a work order
  double amount = 0.01;
  double profit = 0.00001;
  WorkOrder *wo = new WorkOrder(e,amount,profit);
  workOrders.append(wo);
}

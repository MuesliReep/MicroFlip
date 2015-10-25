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
  double minSell= 0.0;
  QString pair = "btc_usd";

  amount = 0.998;
  profit = 0.00001;
  pair   = "ltc_usd";
  minSell = 3.038;

  WorkOrder *wo = new WorkOrder(e,pair,amount,profit, minSell);
  workOrders.append(wo);
}

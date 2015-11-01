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

  amount = 0.1;
  profit = 0.00001;
  pair   = "ltc_usd";
  minSell = 3.038;

  int numWorkers = 5;

  for(int i = 0; i < numWorkers; i++) {

    std::cout << "Creating Work Order: " << (i+1) << std::endl;
    WorkOrder *wo = new WorkOrder(e,i+1,pair,amount,profit, minSell);
    workOrders.append(wo);
    QThread::sleep(5);
  }
}

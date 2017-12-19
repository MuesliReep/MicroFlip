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

  Display *d = new Display();
  connect(this, SIGNAL(updateLog(int,QString)), d, SLOT(logUpdate(int,QString)));

  // Create a work order
  double amount = 0.01;
  double profit = 0.00001;
  double minSell= 0.0;
  QString pair = "btc_usd";

  amount  = 0.1;
  profit  = 0.00001;
  pair    = "ltc_usd";
  minSell = 4.038;

  int numWorkers = 20;

  for(int i = 0; i < numWorkers; i++) {

    emit updateLog(99, "Creating Work Order: " + QString::number(i+1));
    WorkOrder *wo = new WorkOrder(e,i+1,pair,amount,profit, minSell);

    connect(wo, SIGNAL(updateLog(int,QString)),   d, SLOT(logUpdate(int,QString)));
    connect(wo, SIGNAL(updateState(int,QString)), d, SLOT(stateUpdate(int,QString)));

    connect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));
    wo->start();
    emit startOrder();
    disconnect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));

    workOrders.append(wo);
    QThread::sleep(5);
  }

  // OKCoin CNY workers
// Disabled for now 19-12-2017
//  e = new Exchange_OKCoin();
//  e->setConfig(&c);
//  e->startWork();

//  amount  = 0.01;
//  profit  = 0.00001;
//  pair    = "btc_cny";
//  minSell = 0.0;

//  numWorkers = 1;

//  bool highSpeed = true;

//  for(int i = 0; i < numWorkers; i++) {

//    emit updateLog(99, "Creating Work Order: " + QString::number(i+1));
//    WorkOrder *wo = new WorkOrder(e,i+1,pair,amount,profit, minSell, highSpeed);

//    connect(wo, SIGNAL(updateLog(int,QString)),   d, SLOT(logUpdate(int,QString)));
//    connect(wo, SIGNAL(updateState(int,QString)), d, SLOT(stateUpdate(int,QString)));

//    connect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));
//    wo->start();
//    emit startOrder();
//    disconnect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));

//    workOrders.append(wo);
//    QThread::sleep(5);
//  }
}

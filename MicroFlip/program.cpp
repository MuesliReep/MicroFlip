#include "program.h"

#include "config.h"

//#include "exchange_btce.h"
//#include "exchange_okcoin.h"
#include "exchange_wex.h"
#include "exchange_bitstamp.h"
#include "exchange_bitfinex.h"

Program::Program(QObject *parent) : QObject(parent) {

  // Load configuration from file (if any)
  Config c;
  c.loadConfigFromFile();

  // Create an exchange interface
  Exchange *exchange = new Exchange_wex();
  //Exchange *exchange = new Exchange_bitfinex();
  exchange->setConfig(&c);
  exchange->startWork();

  display = new Display();
  connect(this, SIGNAL(updateLog(int,QString)), display, SLOT(logUpdate(int,QString)));

  // Create work orders
  workOrderFactory(c.getNumWorkers(), exchange, c.getAmount(), c.getProfit(), c.getPair(), c.getShortInterval(), c.getLongInterval(), c.getMinSell());
}

///
/// \brief Program::workOrderFactory Creates a workorder and adds it to the list of workorders
/// \param numWorkers The amount of workers to create
/// \param exchange Pointer to the exchange interface
/// \param amount The amount of currency to trade with
/// \param profit The profit target this worker will aim for
/// \param pair The currenct pair, example: btc_usd
/// \param minSell Sets a static minimum sell price. To use a dynamic price, set to a negative number
/// \return
///
bool Program::workOrderFactory(int numWorkers, Exchange *exchange, double amount, double profit, QString pair, int shortInterval, int longInterval, double minSell) {

  emit updateLog(00, "User requested " + QString::number(numWorkers) + " work orders");

  for(int i = 0; i < numWorkers; i++) {

    emit updateLog(00, "Creating Work Order: " + QString::number(i+1) + " with currency: " + pair);
    WorkOrder *wo = new WorkOrder(exchange,i+1,pair,amount,profit, shortInterval, longInterval, minSell);

    connect(wo, SIGNAL(updateLog(int,QString)),   display, SLOT(logUpdate(int,QString)));
    connect(wo, SIGNAL(updateState(int,QString)), display, SLOT(stateUpdate(int,QString)));

    // Tell the newly created work order to start
    connect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));
    wo->start();
    emit startOrder();
    disconnect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));

    workOrders.append(wo);
    QThread::sleep(5);
  }

  return true;
}

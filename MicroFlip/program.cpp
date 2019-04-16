/*!
  \class Program
  \brief The Program class is the entry point for the whole program
*/

#include "program.h"

#include "common.h"

//#include "exchange_btce.h"
//#include "exchange_okcoin.h"
#include "exchange_wex.h"
#include "exchange_binance.h"
#include "exchange_bitstamp.h"
#include "exchange_bitfinex.h"
#include "exchange_sim.h"

Program::Program(QObject *parent) : QObject(parent) {

    // Load configuration from file (if any)
    config = new Config();
    config->loadConfigFromFile();

    // Create an exchange interface
//    exchange = new Exchange_wex();
//    exchange = new Exchange_bitfinex();
//    exchange = new Exchange_bitstamp();
    exchange = new Exchange_Binance();
//    exchange = new Exchange_Sim();
    exchange->setConfig(config);

    qRegisterMetaType<Ticker>();

    // Create & setup display
    display = new Display();
    display->setLogLevel(config->getLogLevel());
    display->setExchangeName(exchange->getExchangeName());
    connect(this,     &Program:: updateLog,            display, &Display::addToLog);
    connect(exchange, &Exchange::updateLog,            display, &Display::addToLog);
    connect(exchange, &Exchange::updateExchangePrices, display, &Display::updateExchangePrices);

    // Setup threads
    auto *exchangeThread = new QThread();
    auto *displayThread  = new QThread();
    exchangeThread->setObjectName("Exch. Thread");
    displayThread-> setObjectName("Disp. Thread");
    exchange->moveToThread(exchangeThread);
    display-> moveToThread(displayThread);
    exchangeThread->start();
    displayThread-> start();

    // Create start shot, this is called when the main event loop is triggered
    QTimer::singleShot(1, this,     &Program::startShotReceived);
    QTimer::singleShot(1, exchange, &Exchange::startWork);
}

void Program::startShotReceived() {

    // Create work orders
    workOrderFactory(config->getNumWorkers(),   exchange,          config->getAmount(),
                     config->getProfit(),       config->getPair(), config->getShortInterval(),
                     config->getLongInterval(), config->getMode(), config->getSingleShot(),
                     config->getMinSell());
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
bool Program::workOrderFactory(int    numWorkers,   Exchange *exchange,  double amount,
                               double profit,       const QString& pair, int    shortInterval,
                               int    longInterval, int mode,            bool   singleShot,
                               double minSell) {

    emit updateLog(00, className, "User requested " + QString::number(numWorkers) + " work orders", logSeverity::LOG_INFO);

    for(int i = 0; i < numWorkers; i++) {

        emit updateLog(00, className, "Creating Work Order: " + QString::number(i+1) + " with currency: " + pair, logSeverity::LOG_DEBUG);
        WorkOrder *wo = new WorkOrder(exchange,i+1,pair,amount,profit, shortInterval, longInterval, mode, singleShot, minSell);

        auto *workOrderThread = new QThread();
        wo->moveToThread(workOrderThread);

        connect(wo, SIGNAL(updateLog(int, QString, QString, int)), display, SLOT(addToLog(int, QString, QString, int)));
        connect(wo, SIGNAL(updateState(int,QString)),              display, SLOT(stateUpdate(int,QString)));

        // Tell the newly created work order to start
        connect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));
        workOrderThread->start();
        emit startOrder();
        disconnect(this,SIGNAL(startOrder()), wo, SLOT(startOrder()));

        workOrders.append(wo);
        workOrderThreads.append(workOrderThread);
        QThread::sleep(1);
    }

    return true;
}

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

#include "remotecontrol_tcp.h"

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
    connect(this,     &Program ::updateLog,            display, &Display::addToLog);
    connect(exchange, &Exchange::updateLog,            this,    &Program::updateLog);
    connect(exchange, &Exchange::updateExchangePrices, display, &Display::updateExchangePrices);

    // Setup WorkOrder Controller
    connect(&workOrderController, &WorkOrderController::updateLog,   this,    &Program::updateLog);
    connect(&workOrderController, &WorkOrderController::updateState, display, &Display::stateUpdate);

    // Create remote control
    if(config->getUseRemote()) {

        // Create new remote control
        remoteControl = new RemoteControl_TCP(config->getRemoteListenPort(),
                                              config->getRemoteServerKey(),
                                              config->getRemotePrivateKey());

        // Setup new connections
        connect(this,                 &Program            ::updateLog,            remoteControl, &RemoteControl::logUpdate);
//        connect(exchange,             &Exchange           ::updateLog,            remoteControl, &RemoteControl::logUpdate);
        connect(exchange,             &Exchange           ::updateExchangePrices, remoteControl, &RemoteControl::exchangePricesUpdate);
//        connect(&workOrderController, &WorkOrderController::updateLog,            remoteControl, &RemoteControl::logUpdate);
        connect(&workOrderController, &WorkOrderController::updateState,          remoteControl, &RemoteControl::workorderStateUpdate);
        connect(remoteControl,        &RemoteControl      ::createWorker,         this,          &Program      ::addWorker);
        connect(remoteControl,        &RemoteControl      ::removeWorker,         this,          &Program      ::removeWorker);
        connect(remoteControl,        &RemoteControl      ::updateLog,            this,          &Program      ::updateLog);

        // Create thread for remote control
        auto *remoteControlThread = new QThread();
        remoteControlThread->setObjectName("Remo. Thread");
        remoteControl->moveToThread(remoteControlThread);
        remoteControlThread->start();

        // Start after main event loop
        QTimer::singleShot(1, remoteControl, &RemoteControl::open);
    }

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
    workOrderController.factory(config->getNumWorkers(),   exchange,          config->getAmount(),
                                config->getProfit(),       config->getPair(), config->getShortInterval(),
                                config->getLongInterval(), config->getMode(), config->getSingleShot(),
                                config->getMinSell());
}

void Program::addWorker(int numWorkers, QString pair, double maxAmount, double profitTarget,
                        int shortInterval, int longInterval, int mode, bool singleShot, double minSellPrice) {

    // Long and short interval can be left blank, if they are use config values
    if(shortInterval == 0 || shortInterval < 0) {
        shortInterval = config->getShortInterval();
    }

    if(longInterval == 0 || longInterval < 0) {
        longInterval = config->getLongInterval();
    }

    if(numWorkers < 1) {
        emit updateLog(00, className, "Could not create worker, invalid number of workers requested: " + QString::number(numWorkers), logSeverity::LOG_WARNING);
        return;
    }

    workOrderController.factory(numWorkers, exchange, maxAmount, profitTarget, pair, shortInterval, longInterval, mode, singleShot, minSellPrice);
}

///
/// \brief Program::removeWorker Removes a workorder
/// \param workOrderID The ID of the workorder to be removed
/// \param force Set to true to be effective immediately, otherwise it wil wait until complete to remove
///
void Program::removeWorker(uint workOrderID, bool force) {

    if(workOrderID < 1) {
        emit updateLog(00, className, "Could not remove worker, invalid workOrderID: " + QString::number(workOrderID), logSeverity::LOG_WARNING);
        return;
    }

    workOrderController.remove(workOrderID, force);
}

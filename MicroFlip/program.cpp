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

    // Setup WorkOrder Controller
    connect(&workOrderController, &WorkOrderController::updateLog,   display, &Display::addToLog);
    connect(&workOrderController, &WorkOrderController::updateState, display, &Display::stateUpdate);

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


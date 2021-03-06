#include "exchange_sim.h"

Exchange_Sim::Exchange_Sim() {

    currentTask = ExchangeTask();

    fee = 0.2;
    this->exchangeName = "Simulation";

    // Start the interval timers
    timer  = new QTimer(this);
    //timer2 = new QTimer(this);

    connect(timer,  SIGNAL(timeout()), this, SLOT(updateTick()));
    //connect(timer2, SIGNAL(timeout()), this, SLOT(updateTick2()));
}

void Exchange_Sim::startWork() {

    this->apiKey    = config->getApiKey();
    this->apiSecret = config->getApiSecret();

    timer->start(2*1100);
    //timer2->start(1*1100); // TODO: determine correct amount
}

void Exchange_Sim::updateMarketTicker(QString pair) {
    (void) pair;
}

void Exchange_Sim::updateMarketDepth(QString pair) {
    (void) pair;
}

void Exchange_Sim::updateMarketTrades(QString pair) {
    (void) pair;
}

void Exchange_Sim::updateBalances() {

}

void Exchange_Sim::createOrder(QString pair, int type, double rate, double amount) {
    (void) pair; (void) type; (void) rate; (void) amount;
}

void Exchange_Sim::cancelOrder(qint64 orderID) {
    (void) orderID;
}

void Exchange_Sim::updateActiveOrders(QString pair) {
    (void) pair;
}

void Exchange_Sim::updateOrderInfo(qint64 orderID) {
    (void) orderID;
}

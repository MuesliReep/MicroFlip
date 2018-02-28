#include "worker.h"

Worker::Worker(Exchange *exchange, QString pair) {

    this->exchange = exchange;
    this->pair     = pair;

    intervalShort       = 10 * 1000;     // 10 seconds
    intervalLong        = 5 * 60 * 1000; // 5 minutes

    // TODO: this in a timer
    updateTick();
}

void Worker::updateTick() {

    connect(this, SIGNAL(sendUpdateMarketTrades(QString, QObject*)), exchange, SLOT(receiveUpdateMarketTrades(QString, QObject*)));
    emit sendUpdateMarketTrades(pair, this);
    disconnect(this, SIGNAL(sendUpdateMarketTrades(QString, QObject*)), exchange, SLOT(receiveUpdateMarketTrades(QString, QObject*)));
}

void Worker::updateMarketTradesReply(QList<Trade> marketTrades) {


}

void Worker::startWorker() {

    // Create timer & connect slot
    workThread = new QThread(this);
    timer = new QTimer(0);
    timer->setInterval(intervalShort);
    timer->moveToThread(workThread);

    // Connect timer to updateTick
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTick()), Qt::DirectConnection);

    // Connect thread to timer
    QObject::connect(workThread, SIGNAL(started()), timer, SLOT(start()));
    workThread->start();
}

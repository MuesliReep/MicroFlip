#include "program.h"

#include <QTimer>

#include <QQmlContext>
#include <QThread>
#include <utility>

#include "remotecontrol_tcp.h"
#include "config.h"
#include "logitemmodel.h"
#include "../../../MicroFlip/common.h"

Program::Program(QQmlApplicationEngine *engine, QObject *parent) : QObject(parent) {

    // Load config
    Config config;
    config.loadConfigFromFile();

    // Create Remote Control
    remoteControl = new RemoteControl_TCP(config);

    engine->rootContext()->setContextProperty("exchangeInfo", &exchangeInfo);
    engine->rootContext()->setContextProperty("remoteControl", this);

    // Dummy data
    logItemModel.addLogItem(LogItem(112, "classname", "TEST TEST TEST", 1));
    logItemModel.addLogItem(LogItem(113, "classname", "TEST TEST TEST", 1));
    workOrderModel.addWorkOrderItem(WorkOrderItem(112, "TEST_STATE"));
    workOrderModel.addWorkOrderItem(WorkOrderItem(113, "TEST_STATE"));

    // Bind data models
    engine->rootContext()->setContextProperty("workOrderModel", &workOrderModel);
    engine->rootContext()->setContextProperty("logItemModel",   &logItemModel);

    // Create thread for remote control
    auto *remoteControlThread = new QThread();
    remoteControlThread->setObjectName("Remo. Thread");
    remoteControl->moveToThread(remoteControlThread);
    remoteControlThread->start();

    // Create start shot, this is called when the main event loop is triggered
    QTimer::singleShot(1000, remoteControl, &RemoteControl::open);
//    QTimer::singleShot(1200, this,          &Program::loadDummyData);

    connect(remoteControl, &RemoteControl::newExchangeInformation,       this, &Program::onNewExchangeInformation);
    connect(remoteControl, &RemoteControl::newLogUpdate,                 this, &Program::onNewLogUpdate);
    connect(remoteControl, &RemoteControl::newWorkerStatus,              this, &Program::onNewWorkerStatus);
    connect(remoteControl, &RemoteControl::updateLog,                    this, &Program::onConsoleLog);
    connect(remoteControl, &RemoteControl::remoteConnectionStateChanged, this, &Program::remoteConnectionStateChanged);

    connect(this, &Program::sendCreateWorker, remoteControl, &RemoteControl::createWorkerMessage);
    connect(this, &Program::sendRemoveWorker, remoteControl, &RemoteControl::createRemoveWorkerMessage);
}

QString Program::remoteConnectionStateString() const {

    QString stateString;

    switch (remoteControl->getRmoteConnectionState()) {
    case REMOTE_ERROR:
        stateString = "ERROR";
        break;
    case REMOTE_IDLE:
        stateString = "IDLE";
        break;
    case REMOTE_CONNECTING:
        stateString = "CONNECTING";
        break;
    case REMOTE_CONNECTED:
        stateString = "CONNECTED";
        break;
    case REMOTE_VERIFIED:
        stateString = "VERIFIED";
        break;
    case REMOTE_DISCONNECTED:
        stateString = "DISCONNECTED";
        break;
    case REMOTE_REJECTED:
        stateString = "REJECTED";
        break;
    }

    return stateString;

}

void Program::onNewWorkerStatus(int workID, const QString& state) {

    workOrderModel.addWorkOrderItem(WorkOrderItem(workID, state));
}

void Program::onNewBalanceValues() {

}

void Program::onNewExchangeInformation(QString symbol, double lastPrice, double avgPrice) {
    exchangeInfo.setsymbol(std::move(symbol));
    exchangeInfo.setCurrentPrice(lastPrice);
    exchangeInfo.setAvgPrice(avgPrice);
}

void Program::onNewLogUpdate(int workID, const QString& className, const QString& log, int severity) {

    // Append new log item, gui will be notified through model binding
    logItemModel.addLogItem(LogItem(workID, className, log, severity));
}

void Program::onConsoleLog(int workID, const QString& className, const QString& log, int  /*severity*/) {

    QString message;

    message.append("[" + QString::number(workID) + "]");
    message.append("[" + className + "]");
    message.append(log);

    qDebug() << message;

}

void Program::onAddNewWorker(QString numWorkers,   QString pair,          QString amount,
                             QString profitTarget, QString shortInterval, QString longInterval,
                             QString mode,         bool singleShot,       QString minSellPrice) {

    bool ok = false;

    // Check each value
    int numWorkersValue = numWorkers.toInt(&ok);
    if(!ok) {
        return;
    }

    if(pair.isEmpty()) {
        return;
    }

    double amountValue = amount.toDouble(&ok);
    if(!ok) {
        return;
    }

    double profitTargetValue = profitTarget.toDouble(&ok);
    if(!ok) {
        return;
    }

    int shortIntervalValue = shortInterval.toInt(&ok);
    if(!ok) {
        return;
    }
    shortIntervalValue *= 1000; // Go from seconds to millis

    int longIntervalValue = longInterval.toInt(&ok);
    if(!ok) {
        return;
    }
    longIntervalValue *= 1000; // Go from seconds to millis

    if(mode.isEmpty()) {
        return;
    }
    int modeValue = 0;
    if(mode == "MINSELL") {
        modeValue = 0;
    } else if(mode == "TICKERAVG") {
        modeValue = 1;
    } else {
        return;
    }

    double minSellPriceValue = minSellPrice.toDouble(&ok);
    if(!ok) {
        return;
    }

    emit sendCreateWorker(numWorkersValue, pair, amountValue, profitTargetValue, shortIntervalValue, longIntervalValue, modeValue, singleShot, minSellPriceValue);
}

void Program::onRemoveWorker(QString workId, bool force) {

    bool ok = false;

    int workIdValue = workId.toInt(&ok);
    if(!ok) {
        return;
    }
    if(workId < 1) {
        return;
    }

    emit sendRemoveWorker(workIdValue, force);
}

void Program::loadDummyData()
{
    // Dummy data
    logItemModel.addLogItem(LogItem(110, "classname", "TEST TEST TEST", 1));
    logItemModel.addLogItem(LogItem(111, "classname", "TEST TEST TEST", 1));
}

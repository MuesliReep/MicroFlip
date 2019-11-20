#include "program.h"

#include <QTimer>

#include <QQmlContext>
#include <QThread>

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

void Program::onNewWorkerStatus(int workID, QString state) {

    workOrderModel.addWorkOrderItem(WorkOrderItem(workID, state));
}

void Program::onNewBalanceValues() {

}

void Program::onNewExchangeInformation(QString symbol, double lastPrice, double avgPrice) {
    exchangeInfo.setsymbol(symbol);
    exchangeInfo.setCurrentPrice(lastPrice);
    exchangeInfo.setAvgPrice(avgPrice);
}

void Program::onNewLogUpdate(int workID, QString className, QString log, int severity) {

    // Append new log item, gui will be notified through model binding
    logItemModel.addLogItem(LogItem(workID, className, log, severity));
}

void Program::onConsoleLog(int workID, QString className, QString log, int severity) {

    QString message;

    message.append("[" + QString::number(workID) + "]");
    message.append("[" + className + "]");
    message.append(log);

    qDebug() << message;

}

void Program::loadDummyData()
{
    // Dummy data
    logItemModel.addLogItem(LogItem(110, "classname", "TEST TEST TEST", 1));
    logItemModel.addLogItem(LogItem(111, "classname", "TEST TEST TEST", 1));
}

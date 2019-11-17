#include "program.h"

#include <QTimer>

#include <QQmlContext>

#include "remotecontrol_tcp.h"
#include "config.h"
#include "logitemmodel.h"

Program::Program(QQmlApplicationEngine *engine, QObject *parent) : QObject(parent) {

    // Load config
    Config config;
    config.loadConfigFromFile();

    // Create Remote Control
    remoteControl = new RemoteControl_TCP(config);


    engine->rootContext()->setContextProperty("exchangeInfo", &exchangeInfo);

    // Dummy data
    logItemModel.addLogItem(LogItem(112, "classname", "TEST TEST TEST", 1));
    logItemModel.addLogItem(LogItem(113, "classname", "TEST TEST TEST", 1));
    workOrderModel.addWorkOrderItem(WorkOrderItem(112, "TEST_STATE"));
    workOrderModel.addWorkOrderItem(WorkOrderItem(113, "TEST_STATE"));

    // Bind data models
    engine->rootContext()->setContextProperty("workOrderModel", &workOrderModel);
    engine->rootContext()->setContextProperty("logItemModel",   &logItemModel);


    // Create start shot, this is called when the main event loop is triggered
    QTimer::singleShot(100, remoteControl, &RemoteControl::open);
//    QTimer::singleShot(1200, this,          &Program::loadDummyData);

    connect(remoteControl, &RemoteControl::newExchangeInformation, this, &Program::onNewExchangeInformation);
    connect(remoteControl, &RemoteControl::newLogUpdate,           this, &Program::onNewLogUpdate);
    connect(remoteControl, &RemoteControl::newWorkerStatus,        this, &Program::onNewWorkerStatus);
    connect(remoteControl, &RemoteControl::updateLog,              this, &Program::onConsoleLog);
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

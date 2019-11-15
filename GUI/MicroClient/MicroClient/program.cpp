#include "program.h"

#include <QTimer>

#include <QQmlContext>

#include "remotecontrol_tcp.h"
#include "config.h"

Program::Program(QQmlApplicationEngine *engine, QObject *parent) : QObject(parent) {

    // Load config
    Config config;
    config.loadConfigFromFile();

    // Create Remote Control
    remoteControl = new RemoteControl_TCP(config);


    engine->rootContext()->setContextProperty("exchangeInfo", &exchangeInfo);

    // Create start shot, this is called when the main event loop is triggered
    QTimer::singleShot(100, remoteControl, &RemoteControl::open);

    connect(remoteControl, &RemoteControl::newExchangeInformation, this, &Program::onNewExchangeInformation);
    connect(remoteControl, &RemoteControl::updateLog,              this, &Program::onConsoleLog);
}

void Program::onNewWorkerStatus(int workID, QString state) {

}

void Program::onNewBalanceValues() {

}

void Program::onNewExchangeInformation(QString symbol, double lastPrice, double avgPrice) {
    exchangeInfo.setsymbol(symbol);
    exchangeInfo.setCurrentPrice(lastPrice);
    exchangeInfo.setAvgPrice(avgPrice);
}

void Program::onNewLogUpdate(int workID, QString className, QString log, int severity) {

    // Append new log
//    logItems.append(logItemController(workID, className, log, severity));

    // Notify GUI
    // TODO
}

void Program::onConsoleLog(int workID, QString className, QString log, int severity) {

    QString message;

    message.append("[" + QString::number(workID) + "]");
    message.append("[" + className + "]");
    message.append(log);

    qDebug() << message;

}

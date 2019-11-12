#include "program.h"

#include <QTimer>

#include "remotecontrol_tcp.h"
#include "config.h"

Program::Program(QObject *parent) : QObject(parent) {

    // Load config
    Config config;
    config.loadConfigFromFile();

    // Create Remote Control
    remoteControl = new RemoteControl_TCP(config);

    // Create start shot, this is called when the main event loop is triggered
//    QTimer::singleShot(100, remoteControl, &RemoteControl::open);
}

void Program::onNewWorkerStatus(int workID, QString state) {

}

void Program::onNewBalanceValues() {

}

void Program::onNewExchangeInformation(QString symbol, double lastPrice, double avgPrice) {

}

void Program::onNewLogUpdate(int workID, QString className, QString log, int severity) {

    // Append new log
//    logItems.append(logItemController(workID, className, log, severity));

    // Notify GUI
    // TODO
}

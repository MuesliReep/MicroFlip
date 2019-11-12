#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>

#include <QObject>
#include <QTimer>
#include <QList>
#include <QThread>

#include "display.h"
#include "config.h"
#include "exchange.h"
#include "workordercontroller.h"

class Program : public QObject {

  Q_OBJECT

public:
    explicit Program(QObject *parent = nullptr);

private:
    QString              className {"PROGRAM"};

    Display             *display;
    Config              *config;
    Exchange            *exchange;
    WorkOrderController  workOrderController;

signals:
    void updateLog(int workID, QString className, QString log, int severity);

private slots:
    void startShotReceived();

};

#endif // PROGRAM_H

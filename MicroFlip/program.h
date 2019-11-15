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
#include "remoteControl.h"
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
    RemoteControl       *remoteControl;

    QList<WorkOrder*>    workOrders;
    QList<QThread*>      workOrderThreads;
    uint                 workOrderIdIterator;

    bool workOrderFactory(int numWorkers, Exchange *exchange, double amount, double profit, const QString& pair, int shortInterval, int longInterval, int mode, bool singleShot, double minSell = -1.0);

signals:
    void updateLog(int workID, QString className, QString log, int severity);

public slots:
    void addWorker();
    void removeWorker(uint workOrderID, bool force = false);

private slots:
    void startShotReceived();

};

#endif // PROGRAM_H

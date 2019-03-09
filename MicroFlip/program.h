#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>

#include <QObject>
#include <QTimer>
#include <QList>
#include <QThread>

#include "display.h"
#include "config.h"
#include "workorder.h"
#include "exchange.h"

class Program : public QObject {

  Q_OBJECT

public:
    explicit Program(QObject *parent = nullptr);

private:
    QString className = "PROGRAM";

    Display  *display;
    Config   *config;
    Exchange *exchange;

    QList<WorkOrder*> workOrders;
    QList<QThread*>   workOrderThreads;

    bool workOrderFactory(int numWorkers, Exchange *exchange, double amount, double profit, const QString& pair, int shortInterval, int longInterval, int mode, bool singleShot, double minSell = -1.0);

signals:
    void updateLog(int workID, QString className, QString log, int severity);
    void startOrder();

private slots:
    void startShotReceived();

};

#endif // PROGRAM_H

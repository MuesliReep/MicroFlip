#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>

#include <QObject>
#include <QList>
#include <QThread>

#include "display.h"
#include "config.h"
#include "workorder.h"
#include "exchange.h"

class Program : public QObject
{
  Q_OBJECT
public:
  explicit Program(QObject *parent = 0);

private:

    QString className = "PROGRAM";

  Display *display;
  Config  *config;
  QList<WorkOrder*> workOrders;

  bool workOrderFactory(int numWorkers, Exchange *exchange, double amount, double profit, QString pair, int shortInterval, int longInterval, int mode, bool singleShot, double minSell = -1.0);

signals:
  void updateLog(int workID, QString className, QString log, int severity);
  void startOrder();
public slots:
};

#endif // PROGRAM_H

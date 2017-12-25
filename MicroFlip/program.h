#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>

#include <QObject>
#include <QList>
#include <QThread>

#include "workorder.h"
#include "exchange.h"

class Program : public QObject
{
  Q_OBJECT
public:
  explicit Program(QObject *parent = 0);

private:
  QList<WorkOrder*> workOrders;

signals:
  void updateLog(int workID, QString log);
  void startOrder();
public slots:
};

#endif // PROGRAM_H

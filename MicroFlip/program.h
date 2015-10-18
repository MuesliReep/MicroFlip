#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QList>

#include "config.h"
#include "workorder.h"

#include "exchange.h"
#include "exchange_btce.h"

class Program : public QObject
{
  Q_OBJECT
public:
  explicit Program(QObject *parent = 0);

private:
  QList<WorkOrder*> workOrders;

signals:

public slots:
};

#endif // PROGRAM_H

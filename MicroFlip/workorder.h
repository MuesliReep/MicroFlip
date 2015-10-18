#ifndef WORKORDER_H
#define WORKORDER_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include "exchange.h"

// START          : New workorder, setup sell order
// WAITINGFORSELL : Waiting for order to be processed
// SELLORDER      : Order processed, waiting for execution
// SOLD           : Order executed, set up buy order
// WAITINGFORBUY  : Waiting for order to be processed
// BUYORDER       : Order processed, waiting for execution
// COMPLETE       : Order executed, workorder complete

enum WorkState { ERROR = -1, START, WAITINGFORTICKER, CREATESELL, WAITINGFORSELL, SELLORDER, SOLD, WAITINGFORBUY, BUYORDER, COMPLETE };

class WorkOrder : public QObject
{
  Q_OBJECT
public:
  WorkOrder(Exchange *e, double maxAmount, double profitTarget);

private:
  Exchange *e;
  WorkState workState;
  QTimer *timer;

  double maxAmount;
  double profitTarget;
  QString orderID;

  double sellPrice;

  void calculateSellOrder(double *price);
  bool createSellOrder(double amount, double price);

  void calculateBuyOrder();
  bool createBuyOrder(double amount, double price);
  void calculateMinimumBuyTrade(double sellPrice, double sellAmount, double fee, double *buyPrice, double *buyAmount, double *buyTotal, double profit);

  void requestUpdateMarketTicker();
private slots:
  void updateTick();

public slots:


signals:
  sendCreateOrder(int type, double price, double amount);

  sendUpdateMarketTicker(QString pair, QObject *sender);
};

#endif // WORKORDER_H

#ifndef WORKORDER_H
#define WORKORDER_H

#include <iostream>

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QThread>

#include "exchange.h"

// START            : New workorder, setup sell order
// WAITINGFORTICKER :
// CREATESELL       :
// WAITINGFORSELL   : Waiting for order to be processed
// SELLORDER        : Order processed, waiting for execution
// SOLD             : Order executed, set up buy order
// CREATEBUY        :
// WAITINGFORBUY    : Waiting for order to be processed
// BUYORDER         : Order processed, waiting for execution
// COMPLETE         : Order executed, workorder complete

enum WorkState { ERROR = -1, START, WAITINGFORTICKER, CREATESELL, WAITINGFORSELL, SELLORDER, SOLD, CREATEBUY, WAITINGFORBUY, BUYORDER, COMPLETE };

class WorkOrder : public QThread
{
  Q_OBJECT
public:
  WorkOrder(Exchange *e, int workID, QString pair, double maxAmount, double profitTarget, double minSellPrice = 0.0, bool highSpeed = false);

  double getSellPrice() { return sellPrice; }
  int getWorkID() { return workID; }

private:
  Exchange *e;
  WorkState workState;
  QTimer   *timer;
  QThread  *workThread;

  int sellOrderID;
  int buyOrderID;
  int workID;

  double  maxAmount;
  double  profitTarget;
  double  sellPrice;
  double  buyPrice;
  QString pair;
  double  minSellPrice;
  bool    highSpeed;

  Ticker currentTicker;

  int  intervalShort;
  int  intervalLong;
  bool stdInterval;

  void createSellOrder(double amount);
  void createBuyOrder ();

  void calculateMinimumBuyTrade(double sellPrice, double sellAmount, double fee, double *buyPrice, double *buyAmount, double *buyTotal, double profit);

  void requestUpdateMarketTicker();
  void requestCreateOrder(int type, double rate, double amount);
  void requestOrderInfo(int orderID);

private slots:
  void updateTick();

public slots:
  void UpdateMarketTickerReply(Ticker ticker);

  void orderCreateReply(int orderID);
  void orderInfoReply(int status);

  void startOrder();

signals:
  void sendUpdateMarketTicker(QString pair, QObject *sender);
  void sendCreateOrder(QString pair, int type, double price, double amount, QObject *sender);
  void sendUpdateOrderInfo(uint orderID, QObject *sender);

  void updateLog(int workID, QString log);
  void updateState(int workID, QString state);
};

#endif // WORKORDER_H

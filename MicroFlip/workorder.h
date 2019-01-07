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
  WorkOrder(Exchange *exchange, int workID, QString pair, double maxAmount, double profitTarget,
            int shortInterval, int longInterval, int mode, double minSellPrice = 0.0, int sellTTL = 5, int buyTTL = 1440, bool highSpeed = false);

  double  getSellPrice() { return sellPrice; }
  int     getWorkID()    { return workID;    }
  QString getPair()      { return pair;      }
  int     getOrderSide() { return (workState == BUYORDER ? 0 : 1); }

private:

  QString className = "WORKORDER";

  Exchange  *exchange;
  WorkState  workState;
  QTimer    *timer;
  QThread   *workThread;

  qint64 sellOrderID;
  qint64 buyOrderID;
  int    workID;

  double  maxAmount;
  double  profitTarget;
  double  sellPrice;
  double  buyPrice;
  QString pair;
  double  minSellPrice;
  int     sellTTL;
  int     buyTTL;
  bool    highSpeed;
  int     mode;

  Ticker currentTicker;

  int  intervalShort;
  int  intervalLong;
  bool stdInterval;
  bool longIntervalRequest;

  QDateTime sellOrderTime;
  QDateTime buyOrderTime;

  void createSellOrder(double amount);
  void createBuyOrder ();

  void calculateMinimumBuyTrade(double sellPrice, double sellAmount, double fee, double *buyPrice, double *buyAmount, double *buyTotal, double profit);

  void requestUpdateMarketTicker();
  void requestCreateOrder(int type, double rate, double amount);
  void requestOrderInfo  (qint64 orderID);
  void requestCancelOrder(qint64 orderID);

private slots:
  void updateTick();

public slots:
  void UpdateMarketTickerReply(Ticker ticker);

  void orderCreateReply(qint64 orderID);
  void orderInfoReply  (int status);
  void orderCancelReply(bool succes);

  void startOrder();

signals:
  void sendUpdateMarketTicker(QString pair, QObject *sender, int senderID);
  void sendCreateOrder       (QString pair, int type, double price, double amount, QObject *sender, int senderID);
  void sendUpdateOrderInfo   (qint64 orderID, QObject *sender, int senderID);
  void sendCancelOrder       (qint64 orderID, QObject *sender, int senderID);

  void updateLog  (int workID, QString classID, QString logString, int severity);
  void updateState(int workID, QString state);

  void updateExchangePrices(double lastPrice, double avgPrice);
};

#endif // WORKORDER_H

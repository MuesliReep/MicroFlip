#ifndef WORKORDER_H
#define WORKORDER_H

#include <iostream>

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QThread>

#include "exchange.h"
#include "common.h"

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

class WorkOrder : public QObject {

    Q_OBJECT

public:
    WorkOrder(Exchange *exchange, uint workID, QString pair, double maxAmount, double profitTarget,
            int shortInterval, int longInterval, int mode, bool singleShot, double minSellPrice = 0.0, int sellTTL = 5, int buyTTL = 1440, bool highSpeed = false);

    double       getSellPrice() { return sellPrice; }
    int          getWorkID   () { return workID;    }
    QString      getPair     () { return pair;      }
    int          getOrderSide() { return (workerState == BUYORDER ? 0 : 1); }

private:
    QString      className   {"WORKORDER"};

    Exchange    *exchange    {};
    WorkerState  workerState {WorkerState::INITIALISE};
    QTimer      *timer       {};
    QThread     *workThread  {};

    qint64       sellOrderID {};
    qint64       buyOrderID  {};
    int          workID      ;

    double       maxAmount;
    double       profitTarget;
    double       sellPrice{};
    double       buyPrice{};
    QString      pair;
    double       minSellPrice;
    double       minimumPrice;
    int          sellTTL;
    int          buyTTL;
    bool         highSpeed;
    int          mode;
    bool         singleShot;

    Ticker       currentTicker;

    int          intervalShort;
    int          intervalLong;
    bool         stdInterval;
    bool         longIntervalRequest;

    QDateTime    sellOrderTime;
    QDateTime    buyOrderTime;

    void createSellOrder(double amount);
    void createBuyOrder ();

    void calculateMinimumBuyTrade(double sellPrice, double sellAmount, double fee, double *buyPrice, double *buyAmount, double *buyTotal, double profit);

    void initialiseSymbol(QString symbol);
    void requestUpdateMarketTicker();
    void requestCreateOrder(int type, double rate, double amount);
    void requestOrderInfo  (qint64 orderID);
    void requestCancelOrder(qint64 orderID);

private slots:
    void updateTick();

public slots:
    void UpdateMarketTickerReply(const Ticker& ticker);

    void orderCreateReply(qint64 orderID);
    void orderInfoReply  (int    status);
    void orderCancelReply(bool   succes);

    void startOrder();

signals:
    void sendInitialiseSymbol  (QString symbol);
    void sendRequestForTicker  (QString pair,   QObject *sender);
    void sendCreateOrder       (QString pair,   int type, double price, double amount, QObject *sender, int senderID);
    void sendUpdateOrderInfo   (qint64 orderID, QObject *sender, int senderID);
    void sendCancelOrder       (qint64 orderID, QObject *sender, int senderID);

    void updateLog  (int workID, QString classID, QString logString, int severity);
    void updateState(int workID, QString state);

    void updateExchangePrices(double lastPrice, double avgPrice);

};

#endif // WORKORDER_H

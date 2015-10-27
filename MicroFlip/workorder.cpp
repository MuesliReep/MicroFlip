#include "workorder.h"

WorkOrder::WorkOrder(Exchange *e, QString pair, double maxAmount, double profitTarget, double minSellPrice) {

  this->e = e;
  this->maxAmount = maxAmount;
  this->profitTarget = profitTarget;
  this->pair = pair;
  this->minSellPrice = minSellPrice;

  workState = START;

  interval = 5000;

  // Create timer & connect slot
  timer = new QTimer();

  connect(timer, SIGNAL(timeout()), this, SLOT(updateTick()));

  timer->start(interval);
  // Connect exchangebot signals & slots

}

void WorkOrder::updateTick() {

  QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
  //QString  time = now.toString("hh:mm:ss");

  switch(workState) {
    case START: {
      qDebug() << time + " State: START";

      // Get new data
      requestUpdateMarketTicker();

      workState = WAITINGFORTICKER;
      break;
      }
    case WAITINGFORTICKER:
      qDebug() << time + " State: WAITINGFORTICKER";
      break;
    case CREATESELL:
      qDebug() << time + " State: CREATESELL";
      // Create sell order
      createSellOrder(maxAmount);

      workState = WAITINGFORSELL;
      break;
    case WAITINGFORSELL:
      qDebug() << time + " State: WAITINGFORSELL";

      // if orderID = 0, order executed instantly, goto sold state.
      break;
    case SELLORDER:
      qDebug() << time + " State: SELLORDER";

      // Wait for order to be sold
      requestOrderInfo(sellOrderID);
      break;
    case SOLD:
      qDebug() << time + " State: SOLD";
      workState = CREATEBUY;
      break;
    case CREATEBUY:
      qDebug() << time + " State: CREATEBUY";
      // Calculate buy order & create order
      createBuyOrder();

      workState = WAITINGFORBUY;
      break;
    case WAITINGFORBUY:
      qDebug() << time + " State: WAITINGFORBUY";
      break;
    case BUYORDER:
      qDebug() << time + " State: BUYORDER";
      // Wait for order to be sold
      requestOrderInfo(buyOrderID);
      break;
    case COMPLETE:
      qDebug() << time + " State: COMPLETE";
      timer->stop();

      QThread::sleep(10*60); // Wait 10 min
      timer->start(interval);
      break;
    case ERROR:
    default:
      qDebug() << time + " State: ERROR";
      timer->stop();
      break;
  }
}

void WorkOrder::createSellOrder(double amount) {

  // Current price + 0.50 usd
  //sellPrice = currentTicker.getLast() + 0.5;

  // Match current sell order
  sellPrice = currentTicker.getBuy();

  // Check balance
  // TODO

  // Create order
  int type   = 1; // Sell

  qDebug() << "Creating Sell Order: " << amount << " BTC for " << sellPrice << " USD";

  // Connect & send order
  requestCreateOrder(type, sellPrice, maxAmount);
}

void WorkOrder::createBuyOrder() {

  double buyAmount;
  double buyTotal;
  double fee = e->getFee();

  calculateMinimumBuyTrade(sellPrice, maxAmount,fee, &buyPrice, &buyAmount, &buyTotal, profitTarget);

  // Create order
  int type   = 0; // Buy

  // Connect & send order
  requestCreateOrder(type, buyPrice, buyAmount);
}

void WorkOrder::calculateMinimumBuyTrade(double sellPrice, double sellAmount, double fee, double *buyPrice, double *buyAmount, double *buyTotal, double profit) {

  // First calculate sell netto
  double sellTotal = sellAmount * sellPrice;
  double sellFee   = sellTotal  * (fee / 100.0);
  double sellNetto = sellTotal - sellFee;

  // Buy netto + profit = total - fee
  // So if fee is 0.2%, buy netto + profit = 99.8%
  // To find find 100% and thus the fee we divide by 99.8 and multiply by 100
  double buyNetto  = sellAmount + profit;
  *buyAmount = (buyNetto / (100.0 - fee)) * 100.0;

  // Last step is to find the buy price
  // buy price = buyTotal / buyAmount
  // buyTotal = sellNetto
  *buyTotal = sellNetto;
  *buyPrice = *buyTotal / *buyAmount;

  qDebug() << "\t Buy Amount: \t" << *buyAmount << "\t BTC";
  qDebug() << "\t Buy Price: \t" << *buyPrice << "\t USD";
  qDebug() << "\t Buy Total: \t" << *buyTotal << "\t USD";
}

//----------------------------------//
//            Requests              //
//----------------------------------//

void WorkOrder::requestUpdateMarketTicker() {

  connect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*)), e, SLOT(receiveUpdateMarketTicker(QString,QObject*)));
  emit sendUpdateMarketTicker(pair, this);
  disconnect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*)), e, SLOT(receiveUpdateMarketTicker(QString,QObject*)));
}

void WorkOrder::requestCreateOrder(int type, double rate, double amount) {

  connect(this, SIGNAL(sendCreateOrder(QString,int,double,double,QObject*)), e, SLOT(receiveCreateOrder(QString,int,double,double,QObject*)));
  emit sendCreateOrder(pair, type, rate, amount, this);
  disconnect(this, SIGNAL(sendCreateOrder(QString,int,double,double,QObject*)), e, SLOT(receiveCreateOrder(QString,int,double,double,QObject*)));
}

void WorkOrder::requestOrderInfo(int orderID) {

  connect(this, SIGNAL(sendUpdateOrderInfo(uint,QObject*)), e, SLOT(receiveUpdateOrderInfo(uint,QObject*)));
  emit sendUpdateOrderInfo((uint)orderID, this);
  disconnect(this, SIGNAL(sendUpdateOrderInfo(uint,QObject*)), e, SLOT(receiveUpdateOrderInfo(uint,QObject*)));
}

//----------------------------------//
//             Replies              //
//----------------------------------//

void WorkOrder::UpdateMarketTickerReply(Ticker ticker) {

  currentTicker = ticker;
  qDebug() << "New ticker data: " << "Buy: " << currentTicker.getBuy() << "Sell: " << currentTicker.getSell() << "Last: " << currentTicker.getLast();

  // return to START state if buy price is too low
  if(currentTicker.getBuy() <= minSellPrice) {
    qDebug() << "Price " << currentTicker.getBuy() << " lower than minimum: " << minSellPrice << ". Reverting state!";
    workState = START;

    // Pause workorder for 5 minutes
    timer->stop();
    QThread::sleep(5*60);
    timer->start(interval);
  }

  // Only go to next state if we are in the correct state
  if(workState == WAITINGFORTICKER)
    workState = CREATESELL;
}

void WorkOrder::orderCreateReply(int orderID) {

  // Check if this is not an old create reply
  if(workState != WAITINGFORSELL) {
    if(workState != WAITINGFORBUY) {
      qDebug() << "Create reply received during wrong state";
      return;
    }
  }

  // Check if order went through ok
  if(orderID == -1) {
    workState = ERROR;
    return;
  }

  if(orderID != 0) { // Order executed immediatly

    switch(workState) {
      case WAITINGFORSELL:
        sellOrderID = orderID;
        workState = SELLORDER;
        break;
      case WAITINGFORBUY:
        buyOrderID = orderID;
        workState = BUYORDER;
        break;
    }
  } else {

    switch(workState) {
      case WAITINGFORSELL:
        sellOrderID = orderID;
        workState = SOLD;
        break;
      case WAITINGFORBUY:
        buyOrderID = orderID;
        workState = COMPLETE;
        break;
    }
  }
}

void WorkOrder::orderInfoReply(int status) {

  // Check if this is not an old info reply
  if(workState != SELLORDER) {
    if(workState != BUYORDER) {
      qDebug() << "Received old order info reply";
      return;
    }
  }

  switch(status) {
    case 0:
      // Order active, do nothing
      break;
    case 1:
      // Order executed, go to next state
      // If this is a sellorder goto sold state, if buy order goto complete state
      workState = (workState == SELLORDER) ? SOLD : COMPLETE;
      break;
    case 2:
    case 3:
    default:
      qDebug() << "Order status: " << status;
      workState = ERROR;
      break;
  }
}

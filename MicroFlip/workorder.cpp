#include "workorder.h"

#include "common.h"

///
/// \brief WorkOrder::WorkOrder
/// \param exchange Pointer to the exchange interface
/// \param workID This workers assigned ID
/// \param pair
/// \param maxAmount
/// \param profitTarget
/// \param minSellPrice Static minimum selling price. Set to a negative number to use a dynamic price
/// \param sellTTL Time in minutes a Sell order may live
/// \param buyTTL Time in minutes a Buy order may live
/// \param highSpeed
///
WorkOrder::WorkOrder(Exchange *exchange,  int workID,          QString pair,
                     double maxAmount,    double profitTarget, int shortInterval,
                     int longInterval,    double minSellPrice, int sellTTL,
                     int buyTTL,          bool highSpeed) {

  this->exchange      = exchange;
  this->workID        = workID;
  this->maxAmount     = maxAmount;
  this->profitTarget  = profitTarget;
  this->pair          = pair;
  this->minSellPrice  = minSellPrice;
  this->sellTTL       = sellTTL;
  this->buyTTL        = buyTTL;
  this->highSpeed     = highSpeed;
  this->intervalShort = shortInterval;
  this->intervalLong  = longInterval;

  dynamicMinSell = this->minSellPrice < 0 ? true : false;

  workState = START;

  stdInterval         = true;
  longIntervalRequest = false;
}

void WorkOrder::updateTick() {

  if(!stdInterval && longIntervalRequest) {
    timer->setInterval(intervalLong - intervalShort);
    stdInterval         = false;
    longIntervalRequest = false;
  } else if(!stdInterval && !longIntervalRequest) {
    timer->setInterval(intervalShort);
    stdInterval         = true;
    longIntervalRequest = false;
  }

  switch(workState) {
    case START: {
        emit updateState(workID, "START");

      // Get new data
      requestUpdateMarketTicker();

      workState = WAITINGFORTICKER;
      break;
      }
    case WAITINGFORTICKER:
      emit updateState(workID, "WAITINGFORTICKER");
      break;
    case CREATESELL:
      emit updateState(workID, "CREATESELL");
      // Create sell order
      createSellOrder(maxAmount);

      workState = WAITINGFORSELL;
      break;
    case WAITINGFORSELL:
      emit updateState(workID, "WAITINGFORSELL");

      // if orderID = 0, order executed instantly, goto sold state.
      break;
    case SELLORDER:
      emit updateState(workID, "SELLORDER");

      // Wait for order to be sold
      requestOrderInfo(sellOrderID);
      break;
    case SOLD:
      emit updateState(workID, "SOLD");
      workState = CREATEBUY;
      break;
    case CREATEBUY:
      emit updateState(workID, "CREATEBUY");
      // Calculate buy order & create order
      createBuyOrder();

      workState = WAITINGFORBUY;
      break;
    case WAITINGFORBUY:
      emit updateState(workID, "WAITINGFORBUY");
      break;
    case BUYORDER:
      emit updateState(workID, "BUYORDER");
      // Wait for order to be sold
      requestOrderInfo(buyOrderID);
      break;
    case COMPLETE:
      emit updateState(workID, "COMPLETE");

      stdInterval         = false;
      longIntervalRequest = true;

      workState = START;
      break;
    case ERROR:
    default:
      emit updateState(workID, "ERROR");
      timer->stop();
      break;
  }
}

void WorkOrder::createSellOrder(double amount) {

  // Current price + 0.50 usd
  //sellPrice = currentTicker.getLast() + 0.5;

  // Match current sell order
  sellPrice = currentTicker.getBuy();// * 1.19;

  // Check balance
  // TODO

  // Create order
  int type   = 1; // Sell

  emit updateLog(workID, "WORKORDER", "Creating Sell Order: " + QString::number(amount) + " BTC for " + QString::number(sellPrice) + " USD", logSeverity::LOG_INFO);

  // Connect & send order
  requestCreateOrder(type, sellPrice, maxAmount);
}

void WorkOrder::createBuyOrder() {

  double buyAmount;
  double buyTotal;
  double fee = exchange->getFee();

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

  QString pair1 = pair.mid(0,3).toUpper();
  QString pair2 = pair.mid(pair.length()-3-1,3).toUpper();

  emit updateLog(workID, className,
                         "Buying "   + QString::number(*buyAmount) + " " + pair1 +
                         " @ "       + QString::number(*buyPrice)  + " " + pair2 + "/" + pair1 +
                         ". Total: " + QString::number(*buyTotal)  + " " + pair2,
                         logSeverity::LOG_INFO);
}

//----------------------------------//
//            Requests              //
//----------------------------------//

void WorkOrder::requestUpdateMarketTicker() {

  connect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*,int)), exchange, SLOT(receiveUpdateMarketTicker(QString,QObject*,int)));
  emit sendUpdateMarketTicker(pair, this, this->workID);
  disconnect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*,int)), exchange, SLOT(receiveUpdateMarketTicker(QString,QObject*,int)));
}

void WorkOrder::requestCreateOrder(int type, double rate, double amount) {

  connect(this, SIGNAL(sendCreateOrder(QString,int,double,double,QObject*,int)), exchange, SLOT(receiveCreateOrder(QString,int,double,double,QObject*,int)));
  emit sendCreateOrder(pair, type, rate, amount, this, this->workID);
  disconnect(this, SIGNAL(sendCreateOrder(QString,int,double,double,QObject*,int)), exchange, SLOT(receiveCreateOrder(QString,int,double,double,QObject*,int)));
}

void WorkOrder::requestOrderInfo(qint64 orderID) {

  connect(this, SIGNAL(sendUpdateOrderInfo(qint64,QObject*,int)), exchange, SLOT(receiveUpdateOrderInfo(qint64,QObject*,int)));
  emit sendUpdateOrderInfo(orderID, this, this->workID);
  disconnect(this, SIGNAL(sendUpdateOrderInfo(qint64,QObject*,int)), exchange, SLOT(receiveUpdateOrderInfo(qint64,QObject*,int)));
}

void WorkOrder::requestCancelOrder(qint64 orderID) {

  connect(this, SIGNAL(sendCancelOrder(qint64,QObject*,int)), exchange, SLOT(receiveCancelOrder(qint64,QObject*,int)));
  emit sendCancelOrder(orderID, this, this->workID);
  disconnect(this, SIGNAL(sendCancelOrder(qint64,QObject*,int)), exchange, SLOT(receiveCancelOrder(qint64,QObject*,int)));
}

//----------------------------------//
//             Replies              //
//----------------------------------//

void WorkOrder::UpdateMarketTickerReply(Ticker ticker) {

  // Check ticker validity
  if (ticker.getAge() < 0) {

      emit updateLog(workID, className, "Received invalid ticker data, resetting", logSeverity::LOG_WARNING);

      // Something went wrong with the ticker, we need to revert to start state
      workState = START;

      stdInterval         = false;
      longIntervalRequest = true;

      return;
  }

  // Save the ticker data locally
  currentTicker = ticker;

  updateLog(workID, className, "New ticker data: Buy: " + QString::number(currentTicker.getBuy())
                             + " Sell: " + QString::number(currentTicker.getSell())
                             + " Last: " + QString::number(currentTicker.getLast()),
                               logSeverity::LOG_INFO);

  // If we are using a dynamic minimum sell price, calculate it here
  // TODO: Use something smarter than just using the 24h average
  if(dynamicMinSell) {
    minSellPrice = currentTicker.getAvg();
    emit updateLog(workID, className, "Using dynamic min. sell price, currently: " + QString::number(minSellPrice), logSeverity::LOG_INFO);
  }

//  // return to START state if buy price is too low
//  if(currentTicker.getBuy() <= minSellPrice) {
//    updateLog(workID, "Price " + QString::number(currentTicker.getBuy()) + " lower than minimum: " + QString::number(minSellPrice) + ". Reverting state!");
//    workState = START;

//    // Pause workorder for 5 minutes
//    stdInterval         = false;
//    longIntervalRequest = true;
//  }

  // return to START state if buy price is too low
  if(currentTicker.getSell() <= minSellPrice) {
    emit updateLog(workID, className, "Price " + QString::number(currentTicker.getSell())
                                               + " lower than minimum: " + QString::number(minSellPrice)
                                               + ". Reverting state!",
                                                 logSeverity::LOG_INFO);
    workState = START;

    // Pause workorder for 5 minutes
    stdInterval         = false;
    longIntervalRequest = true;
  }

  // Only go to next state if we are in the correct state
  if(workState == WAITINGFORTICKER)
    workState = CREATESELL;
}

void WorkOrder::orderCreateReply(qint64 orderID) {

  // Check if this is not an old create reply
  if(workState != WAITINGFORSELL) {
    if(workState != WAITINGFORBUY) {
      updateLog(workID, className, "Create order reply received during wrong state", logSeverity::LOG_WARNING);
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
        sellOrderID   = orderID;
        workState     = SELLORDER;
        sellOrderTime = QDateTime::currentDateTime();
        break;
      case WAITINGFORBUY:
        buyOrderID   = orderID;
        workState    = BUYORDER;
        buyOrderTime = QDateTime::currentDateTime();
        break;
      default:
        break;
    }
  } else {

    switch(workState) {
      case WAITINGFORSELL:
        sellOrderID = orderID;
        workState   = SOLD;
        break;
      case WAITINGFORBUY:
        buyOrderID = orderID;
        workState  = COMPLETE;
        break;
      default:
        break;
    }
  }
}

void WorkOrder::orderInfoReply(int status) {

  // Check if this is not an old info reply
  if(workState != SELLORDER) {
    if(workState != BUYORDER) {
      updateLog(workID, className, "Received old order info reply", logSeverity::LOG_WARNING);
      return;
    }
  }

//  if(status == -2) {
//    updateLog(workID, "Continuing with order");
//    return;
//  }

  switch(status) {
    case 0:
      // Order active, do nothing
      break;
    case 1:
      // Order executed, go to next state
      // If this is a sellorder goto sold state, if buy order goto complete state
      workState = (workState == SELLORDER) ? SOLD : COMPLETE;
      break;
    case -2:
      // Packet error, continue
    case 2:
    case 3:
    default:
      updateLog(workID, className, " Critical Order info status: " + QString::number(status), logSeverity::LOG_CRITICAL);
      workState = ERROR;
      break;
  }
}

void WorkOrder::orderCancelReply(bool succes) {

    if(!succes) {
        workState = ERROR;
        updateLog(workID, className, "Failed to cancel order!", logSeverity::LOG_CRITICAL);
        return;
    }

    workState = START;
    updateLog(workID, className, "Order cancelled", logSeverity::LOG_INFO);
}

void WorkOrder::startOrder() {

  // Create timer & connect slot
  workThread = new QThread(this);
  timer = new QTimer(0);
  timer->setInterval(intervalShort);
  timer->moveToThread(workThread);

  // Connect timer to updateTick
  connect(timer, SIGNAL(timeout()), this, SLOT(updateTick()), Qt::DirectConnection);

  // Connect thread to timer
  QObject::connect(workThread, SIGNAL(started()), timer, SLOT(start()));
  workThread->start();
}

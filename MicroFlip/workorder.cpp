#include "workorder.h"

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
WorkOrder::WorkOrder(Exchange *exchange, int workID, QString pair, double maxAmount,
                     double profitTarget, double minSellPrice, int sellTTL,
                     int buyTTL, bool highSpeed) {

  this->exchange     = exchange;
  this->workID       = workID;
  this->maxAmount    = maxAmount;
  this->profitTarget = profitTarget;
  this->pair         = pair;
  this->minSellPrice = minSellPrice;
  this->sellTTL      = sellTTL;
  this->buyTTL       = buyTTL;
  this->highSpeed    = highSpeed;

  dynamicMinSell = this->minSellPrice < 0 ? true : false;

  workState = START;

  intervalShort       = 10 * 1000;     // 10 seconds
  intervalLong        = 5 * 60 * 1000; // 5 minutes
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

//      this->sleep(10*60); // Wait 10 min
      //timer->setInterval(intervalLong);
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
  sellPrice = currentTicker.getBuy() * 1.19;

  // Check balance
  // TODO

  // Create order
  int type   = 1; // Sell

  emit updateLog(workID, " Creating Sell Order: " + QString::number(amount) + " BTC for " + QString::number(sellPrice) + " USD");

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

  emit updateLog(workID, "\t Buy Amount: \t" + QString::number(*buyAmount) + "\t BTC");
  emit updateLog(workID, "\t Buy Price: \t" + QString::number(*buyPrice) + "\t USD");
  emit updateLog(workID, "\t Buy Total: \t" + QString::number(*buyTotal) + "\t USD");
}

//----------------------------------//
//            Requests              //
//----------------------------------//

void WorkOrder::requestUpdateMarketTicker() {

  connect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*)), exchange, SLOT(receiveUpdateMarketTicker(QString,QObject*)));
  emit sendUpdateMarketTicker(pair, this);
  disconnect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*)), exchange, SLOT(receiveUpdateMarketTicker(QString,QObject*)));
}

void WorkOrder::requestCreateOrder(int type, double rate, double amount) {

  connect(this, SIGNAL(sendCreateOrder(QString,int,double,double,QObject*)), exchange, SLOT(receiveCreateOrder(QString,int,double,double,QObject*)));
  emit sendCreateOrder(pair, type, rate, amount, this);
  disconnect(this, SIGNAL(sendCreateOrder(QString,int,double,double,QObject*)), exchange, SLOT(receiveCreateOrder(QString,int,double,double,QObject*)));
}

void WorkOrder::requestOrderInfo(int orderID) {

  connect(this, SIGNAL(sendUpdateOrderInfo(uint,QObject*)), exchange, SLOT(receiveUpdateOrderInfo(uint,QObject*)));
  emit sendUpdateOrderInfo((uint)orderID, this);
  disconnect(this, SIGNAL(sendUpdateOrderInfo(uint,QObject*)), exchange, SLOT(receiveUpdateOrderInfo(uint,QObject*)));
}

void WorkOrder::requestCancelOrder(int orderID) {

    (void) orderID;
}

//----------------------------------//
//             Replies              //
//----------------------------------//

void WorkOrder::UpdateMarketTickerReply(Ticker ticker) {

  // Check ticker validity
  if (ticker.getAge() < 0) {

      updateLog(workID, "Received invalid ticker data, resetting");

      // Something went wrong with the ticker, we need to revert to start state
      workState = START;

      stdInterval         = false;
      longIntervalRequest = true;
  }

  // Save the ticker data locally
  currentTicker = ticker;

  updateLog(workID, "New ticker data: Buy: " + QString::number(currentTicker.getBuy())
                                 + " Sell: " + QString::number(currentTicker.getSell())
                                 + " Last: " + QString::number(currentTicker.getLast()));

  // If we are using a dynamic minimum sell price, calculate it here
  // TODO: Use something smarter than just using the 24h average
  if(dynamicMinSell) {
    minSellPrice = currentTicker.getAvg();
    updateLog(workID, "Using dynamic min. sell price, currently: " + QString::number(minSellPrice));
  }

  // return to START state if buy price is too low
  if(currentTicker.getBuy() <= minSellPrice) {
    updateLog(workID, "Price " + QString::number(currentTicker.getBuy()) + " lower than minimum: " + QString::number(minSellPrice) + ". Reverting state!");
    workState = START;

    // Pause workorder for 5 minutes

    stdInterval         = false;
    longIntervalRequest = true;
  }

  // Only go to next state if we are in the correct state
  if(workState == WAITINGFORTICKER)
    workState = CREATESELL;
}

void WorkOrder::orderCreateReply(int orderID) {

  // Check if this is not an old create reply
  if(workState != WAITINGFORSELL) {
    if(workState != WAITINGFORBUY) {
      updateLog(workID, "Create reply received during wrong state");
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
      updateLog(workID, "Received old order info reply");
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
      updateLog(workID, "Continuing with order");
    case 2:
    case 3:
    default:
      updateLog(workID, " Order status: " + QString::number(status));
      workState = ERROR;
      break;
  }
}

void WorkOrder::orderCancelReply(bool succes)
{
    (void) succes;
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

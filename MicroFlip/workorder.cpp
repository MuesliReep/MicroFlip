#include "workorder.h"

WorkOrder::WorkOrder(Exchange *e, int workID, QString pair, double maxAmount, double profitTarget, double minSellPrice) {

  this->e = e;
  this->workID = workID;
  this->maxAmount = maxAmount;
  this->profitTarget = profitTarget;
  this->pair = pair;
  this->minSellPrice = minSellPrice;

  workState = START;

  interval = 10*1000; // 10 seconds

  // Create timer & connect slot
  workThread = new QThread(this);
  timer = new QTimer(0);
  timer->setInterval(interval);
  timer->moveToThread(workThread);

  // Connect timer to updateTick
  connect(timer, SIGNAL(timeout()), this, SLOT(updateTick()), Qt::DirectConnection);

  // Connect thread to timer
  QObject::connect(workThread, SIGNAL(started()), timer, SLOT(start()));
  workThread->start();
}

void WorkOrder::updateTick() {

  QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
  //QString  time = now.toString("hh:mm:ss");

  switch(workState) {
    case START: {
      std::cout << time.toStdString() <<  " ID:" << workID << " State: START" << std::endl;

      // Get new data
      requestUpdateMarketTicker();

      workState = WAITINGFORTICKER;
      break;
      }
    case WAITINGFORTICKER:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: WAITINGFORTICKER" << std::endl;
      break;
    case CREATESELL:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: CREATESELL" << std::endl;
      // Create sell order
      createSellOrder(maxAmount);

      workState = WAITINGFORSELL;
      break;
    case WAITINGFORSELL:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: WAITINGFORSELL" << std::endl;

      // if orderID = 0, order executed instantly, goto sold state.
      break;
    case SELLORDER:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: SELLORDER" << std::endl;

      // Wait for order to be sold
      requestOrderInfo(sellOrderID);
      break;
    case SOLD:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: SOLD" << std::endl;
      workState = CREATEBUY;
      break;
    case CREATEBUY:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: CREATEBUY" << std::endl;
      // Calculate buy order & create order
      createBuyOrder();

      workState = WAITINGFORBUY;
      break;
    case WAITINGFORBUY:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: WAITINGFORBUY" << std::endl;
      break;
    case BUYORDER:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: BUYORDER" << std::endl;
      // Wait for order to be sold
      requestOrderInfo(buyOrderID);
      break;
    case COMPLETE:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: COMPLETE" << std::endl;
      timer->stop();

//      QThread::sleep(10*60); // Wait 10 min
      QThread::sleep(10*60);
      workState = START;
      timer->start(interval);
      break;
    case ERROR:
    default:
      std::cout << time.toStdString() <<  " ID:" << workID << " State: ERROR" << std::endl;
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

  std::cout << "ID:" << workID << " Creating Sell Order: " << amount << " BTC for " << sellPrice << " USD" << std::endl;

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

  std::cout << "ID:" << workID << "\t Buy Amount: \t" << *buyAmount << "\t BTC" << std::endl;
  std::cout << "ID:" << workID << "\t Buy Price: \t" << *buyPrice << "\t USD" << std::endl;
  std::cout << "ID:" << workID << "\t Buy Total: \t" << *buyTotal << "\t USD" << std::endl;
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
  std::cout << "ID:" << workID << " New ticker data: " << "Buy: " << currentTicker.getBuy() << "Sell: " << currentTicker.getSell() << "Last: " << currentTicker.getLast() << std::endl;

  // return to START state if buy price is too low
  if(currentTicker.getBuy() <= minSellPrice) {
    std::cout << "ID:" << workID << " Price " << currentTicker.getBuy() << " lower than minimum: " << minSellPrice << ". Reverting state!" << std::endl;
    workState = START;

    // Pause workorder for 5 minutes
    timer->stop();
//    QThread::sleep(5*60);
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
      std::cout << "ID:" << workID << " Create reply received during wrong state" << std::endl;
      return;
    }
  }

  // Check if order went through ok
  if(orderID == -1) {
    workState = ERROR;
    return;
  }

  if(orderID == -2) {
    std::cout << "ID:" << workID << " Continuing with order" << std::endl;
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
      std::cout << "ID:" << workID << " Received old order info reply" << std::endl;
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
      std::cout << "ID:" << workID << " Order status: " << status << std::endl;
      workState = ERROR;
      break;
  }
}

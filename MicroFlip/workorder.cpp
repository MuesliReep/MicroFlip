#include "workorder.h"

WorkOrder::WorkOrder(Exchange *e, double maxAmount, double profitTarget) {

  this->e = e;
  this->maxAmount = maxAmount;
  this->profitTarget = profitTarget;

  workState = START;

  // Create timer & connect slot
  timer = new QTimer();

  connect(timer, SIGNAL(timeout()), this, SLOT(updateTick()));

  timer->start(2500);
  // Connect exchangebot signals & slots

}

void WorkOrder::updateTick() {

  switch(workState) {
    case START: {

      // Get new data
      requestUpdateMarketTicker();

      workState = WAITINGFORTICKER;
      break;
      }
    case WAITINGFORTICKER:
      break;
    case CREATESELL:
      // Create sell order
      createSellOrder(maxAmount);

      workState = WAITINGFORSELL;
      break;
    case WAITINGFORSELL:


      // if orderID = 0, order executed instantly, goto sold state.
      break;
    case SELLORDER:

      // Wait for order to be sold
      requestOrderInfo(sellOrderID);
      break;
    case SOLD:

      // Calculate buy order & create order
      break;
    case WAITINGFORBUY:
      break;
    case BUYORDER:
      break;
    case COMPLETE:
      break;
    case ERROR:
    default:
      break;
  }
}

void WorkOrder::createSellOrder(double amount) {

  // Current price + 0.50 usd
  sellPrice = currentTicker.getLast() + 0.5;

  // Check balance

  // Create order
  qDebug() << "Creating Sell Order: " << amount << " BTC for " << sellPrice << " USD";
}

void WorkOrder::calculateBuyOrder() {


}

void WorkOrder::createBuyOrder(double amount, double price) {

  amount = 0;
  price  = 0;

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
  emit sendUpdateMarketTicker("btc_usd", this);
  disconnect(this, SIGNAL(sendUpdateMarketTicker(QString,QObject*)), e, SLOT(receiveUpdateMarketTicker(QString,QObject*)));
}

void WorkOrder::requestCreateOrder() {

}

void WorkOrder::requestOrderInfo(int orderID) {

}

//----------------------------------//
//             Replies              //
//----------------------------------//

void WorkOrder::UpdateMarketTickerReply(Ticker ticker) {

  currentTicker = ticker;
  qDebug() << "New ticker data: " << "High: " << currentTicker.getHigh() << "Low: " << currentTicker.getLow() << "Avg: " << currentTicker.getAvg() << "Last: " << currentTicker.getLast();
}

void WorkOrder::orderInfoReply() {

  int status = -20; // TODO: real status

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
      workState = ERROR;
      break;
  }
}

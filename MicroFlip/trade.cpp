#include "trade.h"

Trade::Trade() {

}

Trade::Trade(double Price, double Amount, uint TradeID, uint TimeStamp) {

  price     = Price;
  amount    = Amount;
  tradeID   = TradeID;
  timeStamp = TimeStamp;
}

Trade::~Trade() {

}

// Getters

double  Trade::getPrice()     { return price; }
double  Trade::getAmount()    { return amount; }
uint    Trade::getTradeID()   { return tradeID; }
uint    Trade::getTimeStamp() { return timeStamp; }

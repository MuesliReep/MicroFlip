#include "trade.h"

Trade::Trade()  = default;
Trade::~Trade() = default;

Trade::Trade(double Price, double Amount, uint TradeID, uint TimeStamp) {

    price     = Price;
    amount    = Amount;
    tradeID   = TradeID;
    timeStamp = TimeStamp;
}

// Getters
double  Trade::getPrice    () { return price;     }
double  Trade::getAmount   () { return amount;    }
uint    Trade::getTradeID  () { return tradeID;   }
uint    Trade::getTimeStamp() { return timeStamp; }

#ifndef TRADE_H
#define TRADE_H

#include <QtGlobal>

class Trade {

public:
    Trade();
    Trade(double Price, double Amount, uint TradeID, uint TimeStamp);
   ~Trade();

    double getPrice    ();
    double getAmount   ();
    uint   getTradeID  ();
    uint   getTimeStamp();

private:
    double price;
    double amount;
    uint   tradeID;
    uint   timeStamp;
};

#endif // TRADE_H

#ifndef ORDER_H
#define ORDER_H

#include <QString>

class Order
{

public:
  Order();
  Order(QString Name, double Pair1, double Pair2);
  Order(QString Name, double Amount, double Price, uint OrderID , uint Type , uint TimeStamp);
  ~Order();

  QString getOrderName();
  double getPair1();
  double getPair2();
  uint getOrderID();
  uint getType();
  uint getTimeStamp();

private:
  QString name;
  double pair1;
  double pair2;
  uint orderID;
  uint type;
  uint timeStamp;

};

#endif // ORDER_H

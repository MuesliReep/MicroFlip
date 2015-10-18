#include "order.h"

Order::Order(QString Name, double Pair1, double Pair2) {

  name  = Name;
  pair1 = Pair1;
  pair2 = Pair2;
}

Order::Order(QString Name, double Amount, double Price, uint OrderID , uint Type , uint TimeStamp) {

  name      = Name;
  pair1     = Amount;
  pair2     = Price;
  orderID   = OrderID;
  type      = Type;     // Buy = 0, Sell = 1;
  timeStamp = TimeStamp;
}

Order::~Order() {
}

// The pairs name
QString Order::getOrderName() {

  return name;
}

// The first pairs value
double Order::getPair1() {

  return pair1;
}

// The second pairs value
double Order::getPair2() {

  return pair2;
}

uint Order::getOrderID() {

  return orderID;
}

uint Order::getType() {

  return type;
}

uint Order::getTimeStamp() {

  return timeStamp;
}

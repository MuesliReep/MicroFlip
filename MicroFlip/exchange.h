#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

#include "downloader.h"
#include "config.h"
#include "order.h"

class ExchangeTask {
public:
  ExchangeTask(int Task = -1);
  ExchangeTask(int Task, QObject *Sender);
  ExchangeTask(int Task, QObject *Sender, QList<QString> Attributes);

  QObject *getSender() const;
  int getTask() const;
  QList<QString> getAttributes() const;

private:
  QObject *sender;
  int task;
  QList<QString> attributes;
};

class Balance {

public:
  Balance(QString Currency, double Amount) { currency = Currency; amount = Amount; }
  QString getCurrency() { return currency; }
  double getAmount() { return amount; }
private:
  QString currency;
  double amount;
};

///
/// \brief The Ticker class
///
class Ticker {

public:
  Ticker(){
    this->high = -1.0;
    this->low  = -1.0;
    this->avg  = -1.0;
    this->last = -1.0;
    this->buy  = -1.0;
    this->sell = -1.0;
    this->age  = -1;
  }
  Ticker(double high, double low, double avg, double last, double buy, double sell, double age) {
    this->high = high;
    this->low  = low;
    this->avg  = avg;
    this->last = last;
    this->buy  = buy;
    this->sell = sell;
    this->age  = age;
  }
  double getHigh() { return high; }
  double getLow()  { return low;  }
  double getAvg()  { return avg;  }
  double getLast() { return last; }
  double getBuy()  { return buy;  }
  double getSell() { return sell; }
  int    getAge()  { return age;  }
private:
  double high;
  double low;
  double avg;
  double last;
  double buy;
  double sell;
  int    age;
};

///
/// \brief The Exchange interface
///
class Exchange : public QObject
{
  Q_OBJECT
public:
  explicit Exchange(QObject *parent = 0);

  virtual void startWork() = 0;

  void setConfig(Config *C);

  double getFee();

private:
  virtual void updateMarketTicker(QString pair) = 0;
  virtual void updateMarketDepth(QString pair)  = 0;
  virtual void updateMarketTrades(QString pair) = 0;

  virtual void updateBalances() = 0;
  virtual void createOrder(QString pair, int type, double rate, double amount) = 0;
  virtual void cancelOrder(uint orderID) = 0;
  virtual void updateActiveOrders(QString pair) = 0;
  virtual void updateOrderInfo(uint orderID) = 0;

  virtual void executeExchangeTask(ExchangeTask *exchangeTask) = 0;

protected:
  Config *c;

  QTimer *timer;
  QTimer *timer2;

  uint lastNonce;
  ExchangeTask currentTask;

  double fee;

  QList<Order>    activeOrders;
  QList<Balance>  balances;
  QList<ExchangeTask> exchangeTasks;

  void createNonce(QByteArray *nonce);
  void createMilliNonce(QByteArray *nonce);

  bool getObjectFromDocument(QNetworkReply *reply, QJsonObject *object);
  bool checkCoolDownExpiration(bool reset);

signals:

public slots:
  virtual void receiveUpdateMarketTicker(QString pair, QObject *sender) = 0;
  virtual void receiveUpdateMarketDepth(QString pair, QObject *sender)  = 0;
  virtual void receiveUpdateMarketTrades(QString pair, QObject *sender) = 0;
  virtual void receiveUpdateBalances(QObject *sender) = 0;
  virtual void receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender) = 0;
  virtual void receiveCancelOrder(uint orderID, QObject *sender) = 0;
  virtual void receiveUpdateActiveOrders(QString pair, QObject *sender) = 0;
  virtual void receiveUpdateOrderInfo(uint orderID, QObject *sender) = 0;

  void checkBalance(int currency, double amount, bool *result);

private slots:
  virtual void updateTick()  = 0;
  virtual void updateTick2() = 0;

signals:
  void sendNewMarketTicker(Ticker ticker);
  void sendNewMarketDepth();
  void sendNewMarketTrades();
  void sendNewBalances();
  void sendNewCreateOrder();
  void sendNewCancelOrder();
  void sendNewActiveOrders();
  void sendNewOrderInfo();
};

#endif // EXCHANGE_H

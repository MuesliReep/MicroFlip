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

///
/// \brief The ExchangeTask class
///
class ExchangeTask {
public:
  ExchangeTask(int Task = -1, int SenderID = 0);
  ExchangeTask(int Task, QObject *Sender, int SenderID);
  ExchangeTask(int Task, QObject *Sender, int SenderID, QList<QString> Attributes);

  QObject *getSender() const;
  int getSenderID();
  int getTask() const;
  QList<QString> getAttributes() const;

private:
  QObject *sender;
  int task;
  QList<QString> attributes;
  int senderID;
};

///
/// \brief The Balance class
///
class Balance {

public:
  Balance(QString Currency, double Amount) { currency = Currency; amount = Amount; }
  QString getCurrency() { return currency; }
  double getAmount() { return amount; }
  void   setAmount(double Amount) { amount = Amount; }
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
  QList<Balance> getBalances();
  double getBalance(QString currency);

private:
  virtual void updateMarketTicker(QString pair) = 0;
  virtual void updateMarketDepth(QString pair)  = 0;
  virtual void updateMarketTrades(QString pair) = 0;

  virtual void updateBalances() = 0;
  virtual void createOrder(QString pair, int type, double rate, double amount) = 0;
  virtual void cancelOrder(quint64 orderID) = 0;
  virtual void updateActiveOrders(QString pair) = 0;
  virtual void updateOrderInfo(quint64 orderID) = 0;

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
  void updateLog(int workID, QString log);

public slots:
  virtual void receiveUpdateMarketTicker(QString pair, QObject *sender, int SenderID) = 0;
  virtual void receiveUpdateMarketDepth( QString pair, QObject *sender, int SenderID) = 0;
  virtual void receiveUpdateMarketTrades(QString pair, QObject *sender, int SenderID) = 0;
  virtual void receiveUpdateBalances(QObject *sender, int SenderID) = 0;
  virtual void receiveCreateOrder(QString pair, int type, double rate, double amount, QObject *sender, int SenderID) = 0;
  virtual void receiveCancelOrder(quint64 orderID,     QObject *sender, int SenderID) = 0;
  virtual void receiveUpdateActiveOrders(QString pair, QObject *sender, int SenderID) = 0;
  virtual void receiveUpdateOrderInfo(quint64 orderID, QObject *sender, int SenderID) = 0;

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

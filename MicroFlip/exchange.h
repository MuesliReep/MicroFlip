#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <QObject>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <utility>

#include "downloader.h"
#include "config.h"
#include "order.h"

///
/// \brief The ExchangeTask class
///
class ExchangeTask {

public:
  ExchangeTask(int Task = -1, int senderID = 0);
  ExchangeTask(int Task, QObject *Sender, int senderID);
  ExchangeTask(int Task, QObject *Sender, int senderID, QList<QString> Attributes);

  QObject       *getSender    () const;
  int            getSenderID  () const;
  int            getTask      () const;
  QList<QString> getAttributes() const;

private:
  QObject       *sender;
  int            senderID;
  int            task;
  QList<QString> attributes;

};

///
/// \brief The Balance class
///
class Balance {

public:
  Balance(QString Currency, double Amount) { currency = std::move(Currency); amount = Amount; }

  QString getCurrency() const { return currency; }
  double  getAmount  () const { return amount;   }

  void    setAmount  (double Amount) { amount = Amount; }

private:
  QString currency;
  double  amount;
};

///
/// \brief The Ticker class
///
class Ticker {

public:
  Ticker(){
    this->symbol = "";
    this->high = -1.0;
    this->low  = -1.0;
    this->avg  = -1.0;
    this->last = -1.0;
    this->buy  = -1.0;
    this->sell = -1.0;
    this->age  = -1;
  }
  Ticker(QString symbol, double high, double low, double avg, double last, double buy, double sell, double age) {
    this->symbol = std::move(symbol);
    this->high   = high;
    this->low    = low;
    this->avg    = avg;
    this->last   = last;
    this->buy    = buy;
    this->sell   = sell;
    this->age    = age;
  }

  QString getSymbol() const { return symbol; }
  double getHigh   () const { return high;   }
  double getLow    () const { return low;    }
  double getAvg    () const { return avg;    }
  double getLast   () const { return last;   }
  double getBuy    () const { return buy;    }
  double getSell   () const { return sell;   }
  qint64 getAge    () const { return age;    }

private:
  QString symbol;
  double  high;
  double  low;
  double  avg;
  double  last;
  double  buy;
  double  sell;
  qint64  age;
};

Q_DECLARE_METATYPE(Ticker)

///
/// \brief The Exchange interface
///
class Exchange : public QObject {

    Q_OBJECT

public:
  explicit Exchange(QObject *parent = nullptr);

  void setConfig(Config *config);

  double         getFee     ()                 const;
  QList<Balance> getBalances()                 const;
  double         getBalance (const QString& currency) const;

  QString getExchangeName() const;

private:

  virtual void updateMarketTicker(QString pair)    = 0;
  virtual void updateMarketDepth (QString pair)    = 0;
  virtual void updateMarketTrades(QString pair)    = 0;

  virtual void updateBalances    ()                = 0;
  virtual void createOrder       (QString pair, int type, double rate, double amount) = 0;
  virtual void cancelOrder       (qint64 orderID)  = 0;
  virtual void updateActiveOrders(QString pair)    = 0;
  virtual void updateOrderInfo   (qint64 orderID)  = 0;

  virtual Ticker parseRawTickerData(QNetworkReply *reply) = 0;
  virtual void   parseRawDepthData (QNetworkReply *reply) = 0;
  virtual void   parseRawTradesData(QNetworkReply *reply) = 0;

  virtual void   parseRawBalancesData        (QNetworkReply *reply) = 0;
  virtual qint64 parseRawOrderCreationData   (QNetworkReply *reply) = 0;
  virtual void   parseRawOrderCancelationData(QNetworkReply *reply) = 0;
  virtual void   parseRawActiveOrdersData    (QNetworkReply *reply) = 0;
  virtual int    parseRawOrderInfoData       (QNetworkReply *reply) = 0;

protected:
  Config *config {};

  QString exchangeName = {"NONE"};
  QString className    = {"EXCHANGE"};

  QNetworkAccessManager* tickerDownloadManager             {};
  QNetworkAccessManager* updateMarketDepthDownloadManager  {};
  QNetworkAccessManager* updateMarketTradesDownloadManager {};
  QNetworkAccessManager* updateBalancesDownloadManager     {};
  QNetworkAccessManager* createTradeDownloadManager        {};
  QNetworkAccessManager* orderInfoDownloadManager          {};
  QNetworkAccessManager* cancelOrderDownloadManager        {};
  QNetworkAccessManager* activeOrdersDownloadManager       {};

  QTimer *timer       {};
  QTimer *tickerTimer {};

  ExchangeTask currentTask;

  double fee{};

  QList<Order>        activeOrders;
  QList<Balance>      balances;
  QList<Ticker>       tickers;
  QList<QString>      symbols;
  QList<ExchangeTask> exchangeTasks;

  void executeExchangeTask(ExchangeTask *exchangeTask);

public slots:
  virtual void startWork() = 0;

  void receiveInitialiseSymbol  (const QString& symbol);
  void receiveRequestForTicker  (const QString& pair,    QObject *sender);
  void receiveUpdateMarketTicker(QString pair,    QObject *sender, int SenderID);
  void receiveUpdateMarketDepth (QString pair,    QObject *sender, int SenderID);
  void receiveUpdateMarketTrades(QString pair,    QObject *sender, int SenderID);
  void receiveUpdateBalances    (QObject *sender, int SenderID);
  void receiveCreateOrder       (QString pair,    int type, double rate, double amount, QObject *sender, int SenderID);
  void receiveCancelOrder       (qint64  orderID, QObject *sender, int SenderID);
  void receiveUpdateActiveOrders(QString pair,    QObject *sender, int SenderID);
  void receiveUpdateOrderInfo   (qint64  orderID, QObject *sender, int SenderID);

  void updateMarketTickerReply(QNetworkReply *reply);
  void updateMarketDepthReply (QNetworkReply *reply);
  void updateMarketTradesReply(QNetworkReply *reply);
  void updateBalancesReply    (QNetworkReply *reply);
  void createOrderReply       (QNetworkReply *reply);
  void cancelOrderReply       (QNetworkReply *reply);
  void updateActiveOrdersReply(QNetworkReply *reply);
  void updateOrderInfoReply   (QNetworkReply *reply);

protected slots:
  void updateTick   ();
  void updateTick2  ();
  void updateTickers();

signals:
  void sendNewMarketTicker(Ticker ticker);
  void sendNewMarketDepth ();
  void sendNewMarketTrades();
  void sendNewBalances    ();
  void sendNewOrderID     (qint64);
  void sendNewCancelOrder ();
  void sendNewActiveOrders();
  void sendNewOrderInfo   (int);

  void updateLog           (int workID, QString classID, QString logString, int severity);
  void updateExchangePrices(QString, double, double);
};

#endif // EXCHANGE_H

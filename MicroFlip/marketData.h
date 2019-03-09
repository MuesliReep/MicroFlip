#ifndef MARKETDATA_H
#define MARKETDATA_H

#include <QList>
#include <QString>
#include <QFile>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>

#include "config.h"
#include "order.h"
#include "trade.h"

class Ticker;
class TradeDepth;

class DataPoint {

public:
  DataPoint(uint TimeStamp, double Open, double Close, double High, double Low, double Average, double Volume) {

    timeStamp = TimeStamp;
    open      = Open;
    close     = Close;
    average   = Average;
    high      = High;
    low       = Low;
    volume    = Volume;
  }

  double  getOpen()       { return open; }
  double  getClose()      { return close; }
  uint    getTimeStamp()  { return timeStamp; }
  double  getAverage()    { return average; }
  double  getHigh()       { return high; }
  double  getLow()        { return low; }
  double  getVolume()     { return volume;}

private:
  double  open;
  double  close;
  double  high;
  double  low;
  uint    timeStamp;
  double  average;
  double  volume;

};

class TradeDepth {

public:
  TradeDepth() {}
  TradeDepth(QList<Order> Asks, QList<Order> Bids) {
    asks = Asks; bids = Bids;
  }
  QList<Order> getAsks() { return asks; }
  QList<Order> getBids() { return bids; }
private:
  QList<Order> asks;
  QList<Order> bids;
};

class Ticker {

public:
  Ticker( double high    = 0, double low  = 0,
          double avg     = 0, double vol  = 0,
          double vol_cur = 0, double last = 0,
          double buy     = 0, double sell = 0,
          uint updated   = 0 );

private:
  double high, low, avg, vol;
  double vol_cur, last, buy, sell;
  uint updated;
};

class MarketData {

public:
  MarketData();
  MarketData(Config *C, bool loadFromFile = false);
  ~MarketData();

  void parseRawTradeData (QJsonArray  *rawData);
  void parseRawDepthData (QJsonObject *rawData);
  void parseRawTickerData(QJsonObject *rawData);

  bool loadTradeDataFromFile(uint maxAge);
  void saveTradeDataToFile();

  uint getOldestTrade();

  uint findClosestBin();
  uint findClosestBin(uint desiredTime);

  TradeDepth           getTradeDepth();
  QList<Trade>         getTradeData();
  QList<DataPoint>     getBinnedTradeData();
  QList<QList<double>> getMAList();
  Ticker               getTicker();

private:
  Config *c{};
  // Ticker t;
  QString tradeDataFileName;

  TradeDepth tradeDepth;
  Ticker     ticker;

  QList<Trade>     tradeData;
  QList<DataPoint> binnedTradeData;

  QList<QList<double>> MAList;

  int dataPoints{};
  int binSize{};

  void binTradeData();
  QList<double> runEMA(int weight);
  QList<double> runSMA(int size);

};

#endif // MARKETDATA_H

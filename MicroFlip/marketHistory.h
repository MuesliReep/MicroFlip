#ifndef TRADEHISTORY_H
#define TRADEHISTORY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QDateTime>
#include <QList>

#include "config.h"
#include "downloader.h"
#include "trade.h"


class MarketHistory : public QObject
{
  Q_OBJECT

public:
  explicit MarketHistory(QObject *parent = 0);
  ~MarketHistory();

private:
  Config *c;
  Downloader d;
  int source;
  QList<Trade> newData;

  QNetworkAccessManager* historyDownloadManager;

  void setConfig(Config *C);
  void loadDataSince(uint timeStamp);
  void addTradeDataTo(QList<Trade> *tradeData);
  bool checkCoolDownExpiration(bool reset);

signals:

public slots:

  void historyDataReply(QNetworkReply *reply);

};

#endif // TRADEHISTORY_H

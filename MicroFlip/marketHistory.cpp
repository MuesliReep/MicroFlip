#include "marketHistory.h"

MarketHistory::MarketHistory(QObject *parent) :
QObject(parent) {

  source = -1;
}

MarketHistory::~MarketHistory() {

}

// Sets the source for the trade history data
void MarketHistory::setConfig(Config *C) {

  c = C;

  // Get the historical data source from the config file
  // 0 = private source (Custom server for later date)
  // 1 = bitcoincharts  (Less fun because no trade IDs are supplied)
  source = c->getHistorySource();

// TODO: set individual cooldown settings
// TODO: save config to config file
}

// Loads historical trade data from a set data source
void MarketHistory::loadDataSince(uint timeStamp) {
  (void) timeStamp;
  // TODO: if cooldown has not yet expired set a timer to when it will be expired

  // Create the request to download new data
  // QNetworkRequest request = d.generateRequest(QUrl("http://api.bitcoincharts.com/v1/trades.csv?symbol=SYMBOL&start="+timeStamp.toString()));
  QNetworkRequest request = d.generateRequest(QUrl("http://api.bitcoincharts.com/v1/trades.csv?symbol=btceUSD&start=1404172800")); //TODO: real url

  // Execute the download
  historyDownloadManager  = d.doDownload(request, this, SLOT(historyDataReply(QNetworkReply*)));
}

// Appends historical trade data to an existing data set
void MarketHistory::addTradeDataTo(QList<Trade> *tradeData) {
  (void) tradeData;
  // TODO: check which list is longer to determine fastest way of appending new data
}

// Checks if the current cool down timer has expired
// Can additionally reset the timer
bool MarketHistory::checkCoolDownExpiration(bool reset) {

  // The result to be returned
  bool result = false;

  // Check the current time and compare it to the saved timestamp
  QDateTime time(QDateTime::currentDateTime());
  if(time.toTime_t() > c->getHistoryLastLoadedTimeStamp()+c->getHistoryCoolDownTime()) {

    // If the timer needs to be reset
    if(reset) {

      c->setHistoryLastLoadedTimeStamp(source);
      c->saveConfigToFile();
    }

    result = true;
  }

  return result;
}

// Slots

void MarketHistory::historyDataReply(QNetworkReply *reply) {

  if(!reply->error()) {

    newData.clear();

    // Read the raw data from the reply and split it
    QString     replyData;
    QStringList dataList;

    replyData = reply->readAll();
    dataList  = replyData.split(" ", QString::SkipEmptyParts);

    // Create trades from the split data and add them to the newData list
    for(int i=0;i<dataList.size();i++) {

      QString     tradeString;
      QStringList splitTrade;

      // Split the individual values from the string
      splitTrade = tradeString.split(",");

      // Create a trade object from the data
      double  price       = splitTrade[1].toDouble();
      double  amount      = splitTrade[2].toDouble();
      uint    tradeID     = 0;
      uint    timeStamp   = (uint)splitTrade[0].toInt();

      Trade trade(price, amount, tradeID, timeStamp);

      // Prepend the new trade object to the list
      // Bitcoincharts data is in reverse order
      newData.prepend(trade);
    }

  }
  else
    qDebug() << "History Packet error";

    // Disconnect the signal and release
    reply->deleteLater();
    historyDownloadManager->deleteLater();
}

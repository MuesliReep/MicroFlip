#include "marketData.h"

#include <QDebug>
#include <QDateTime>

MarketData::MarketData() {

  tradeDataFileName = "btce_USD-BTC.json";
  dataPoints  = 250;
   binSize    = 1*60;
}

MarketData::MarketData(Config *C, bool loadFromFile) {
  c = C;

  if(loadFromFile) {

    // tradeDataFileName = "btce_USD-BTC.json"; // TODO: should be created by exchangeBot, according to market

    loadTradeDataFromFile(24*60*60); // TODO: maxAge should be defined in config
  }
}

MarketData::~MarketData() {

}

// Finds the current closest bin according to the clock
// For example: the time is 12:02:33 & binsize = 3min, closest bin would be 12:03:00
// Formula: ClosestBin = CurrentTime - (CurrentTime % binSize) + binSize
uint MarketData::findClosestBin() {

  uint currentTime = QDateTime::currentDateTime().toTime_t();
  return findClosestBin(currentTime);
}

uint MarketData::findClosestBin(uint desiredTime) {

  return desiredTime - (desiredTime % binSize) + binSize;
}

// Calculates the Exponential Moving Average over a data set
QList<double> MarketData::runEMA(int weight) {

    (void) weight;
  // TODO: get EMAX from old java project

    QList<double> EMAresult;

    return EMAresult;
}

// Calculates the Simple Moving Average over a data set
// SMA = (Pi + ... + Pi-(n-1)) / n
QList<double> MarketData::runSMA(int size) {

  QList<double> SMAList;

  for(int i=dataPoints-1;i >= 0; i--) {

    double accumulator = 0;

    for(int j = 0; j < size; j++) {

      // If we go outside dataset break so we can go to the next bin
      if(i+j > dataPoints-1)
        break;

      // Add the closing price
      accumulator += binnedTradeData[i+j].getClose();
    }

    SMAList.prepend(accumulator / (double)size);
  }

  return SMAList;
}

// Analyzes current trade data and creates a plottable format
void MarketData::binTradeData() {

  // TODO: Check if a list is already active, else create one
  binnedTradeData.clear();

  // Determine the oldest allowed trade according to the number of data points & bin size
  uint maxAge            = dataPoints * binSize;
  uint currentTimeStamp  = findClosestBin();
  uint maxTimeStamp      = currentTimeStamp - maxAge;

  // Work through the data list
  // Go from old to new

  // Find the oldest timestamp in the trade data
  int i;
  for(i = 0; i < tradeData.size(); i++) {

    if(tradeData[i].getTimeStamp() < maxTimeStamp) {

      break;  // Break out of the for loop, oldest timestamp has been found
    }
  }

  int x = i-1; // Keep track of where we are in the tradeData list so we dont look at the same values twice
  // 1 is subtracted from i because either a timestamp was found older than allowed and the previous one needs to be used
  // Or, the for loop went through all the data in which case the last timestamp needs to be used which is the list size -1

  uint minIntervalStamp = maxTimeStamp; // The youngest timestamp
  uint maxIntervalStamp = maxTimeStamp; // The oldest timestamp

  // For each bin calculate the opening and closing prices.
  // Also calculate the average price
  int counter = -1;
  for(int j = dataPoints-1; j >= 0; j--) {

    counter++; // Go to next bin

    // Determine the min and max timestamps for this time interval
    minIntervalStamp = maxTimeStamp + (binSize * (counter+1));
    maxIntervalStamp = maxTimeStamp + (binSize * counter);

    // Create a list with all the trades that occured in this time interval (bin)
    QList<Trade> intervalTrades;

    for( ; x >= 0; x--) {

      if(tradeData[x].getTimeStamp() < maxIntervalStamp)
        qDebug() << "ERROR timestamp is older than max allowed: " << tradeData[x].getTimeStamp() << "\tMax: " << maxIntervalStamp;

      // If a timestamp is found that is not part of our search area break out of the for loop
      if(!(tradeData[x].getTimeStamp() >= maxIntervalStamp && tradeData[x].getTimeStamp() <= minIntervalStamp)) {

        //x--;
        break;
      }

      intervalTrades.prepend(tradeData[x]);
    }

    // If no trades took place or no data is available for this time interval(bin)
    // use the previous value. If no previous value is available use zero.
    if(!intervalTrades.size()>0) {

      // Check if there is a previous value to use
      if(!binnedTradeData.size()>0) {

        binnedTradeData.prepend(DataPoint(maxIntervalStamp,0,0,0,0,0,0)); // The list is empty, create a zero value datapoint
      } else { // Use the previous value

        // Each point uses the previous average because no trades took place
        double average = binnedTradeData[0].getAverage(); // Zero is used because all items are prepended, so the last item will be at zero

        DataPoint previousPoint(maxIntervalStamp, average,  average,
                                                  average,  average,
                                                  average,  0); // Volume is zero because no trades took place
        binnedTradeData.prepend(previousPoint);
      }
    } else {

      // Initialise the dataPoint values
      double  open      = intervalTrades[intervalTrades.size()-1].getPrice();
      double  close     = intervalTrades[0].getPrice();
      uint    timeStamp = intervalTrades[0].getTimeStamp();
      double  high      = intervalTrades[0].getPrice();
      double  low       = intervalTrades[0].getPrice();
      double  volume    = 0;

      // Find High/Low values & calculate volume
      for(int k = 0; k < intervalTrades.size(); k++) {

        if(intervalTrades[k].getPrice() > high)
          high = intervalTrades[k].getPrice();

        if(intervalTrades[k].getPrice() < low)
          low = intervalTrades[k].getPrice();

        volume += intervalTrades[k].getAmount();
      }

      // Calculate average over bin // TODO: should use median values for average
      double  average;
      double  sum = 0;

      for(int k=0; k < intervalTrades.size(); k++)
        sum += intervalTrades[k].getPrice();

      average = sum / intervalTrades.size();

      // Create the dataPoint from the gathered values
      DataPoint dataPoint(timeStamp, open, close, high, low, average, volume);

      // Add the new dataPoint to the binnedTradeData
      binnedTradeData.prepend(dataPoint);
    }
  }
}

// Parses a new market trade data set and merges it with the existing set(if any)
void MarketData::parseRawTradeData(QJsonArray *rawData) {

  // QJsonArray jTrades;

  // Retrieve array form JSON object
  // jTrades = rawData->value("btc_usd").toArray();

  if(!rawData->size()>0)
    return;

  // Append new trades to the beginning of the trades list
  // Work through the list from oldest to newest
  for(int i = rawData->size(); i >= 0; i--) {

    uint timeStamp = (uint)rawData->at(i).toObject().value("timestamp").toInt();

    // Check if there is any existing data to begin with
    if(tradeData.size() > 0) {

      // Check if the timestamp is newer than the first in the list
      // otherwise skip it and go to the next
      if(timeStamp < tradeData[0].getTimeStamp())
        continue;

      // If timestamps are the same check the tradeIDs
      // Because more than two trades can happen in one second,
      // the number of trades in that second needs to be known
      if(timeStamp == tradeData[0].getTimeStamp()) {

        // Counter starts at 1 because the first trade has already been found to
        // be in the same timestamp
        int x = 1;

        while(true) {

          if(timeStamp == tradeData[x].getTimeStamp()) {

            x++;
            continue;
          }

          break;
        }

        // The number of trades with the same timestamp is now known.
        // Next check the trade IDs

        // Trade ID of the trade we want to insert
        uint tradeID = (uint)rawData->at(i).toObject().value("tid").toInt();

        bool duplicate = false;

        for(int j = 0; j < x; j++) {

          if(tradeID == tradeData[j].getTradeID()) { // TODO: Check if historical data, historical data will have tradeID = 0;

            duplicate = true;
            break;
          }
        }

        if(duplicate)
          continue;
      }
    }

    // Create a new trade object from the JSON values
    QJsonObject tradeObject = rawData->at(i).toObject();

    // QString tradePair = "btc_usd";
    // uint type         = (uint)tradeObject.value("type").toInt();
    double price      = tradeObject.value("price").toDouble();
    double amount     = tradeObject.value("amount").toDouble();
    uint tradeID      = (uint)tradeObject.value("tid").toInt();

    Trade trade(price, amount, tradeID, timeStamp);

    // Insert the new trade object into the trade data list
    tradeData.insert(0,trade);
  }

  // Save the new data to file // TODO: should this be run every update?
  saveTradeDataToFile();

  // Now bin the new data
  binTradeData(); // TODO: called from exchangeBot?

  // Calculate the MAs
  // Use 7 & 30
  MAList.clear();

  for(int i = 0; i < 2; i++) { // TODO: Number of MAs should be variable

      if(i==1)
        MAList.append(runSMA(7));
      else
        MAList.append(runSMA(30));
  }
}

// Parses new market depth data and overwrites current data
void MarketData::parseRawDepthData(QJsonObject *rawData) {

  QJsonArray jAsks; // Sell orders
  QJsonArray jBids; // Buy orders

  // Retrieve both arrays from JSON object
  jAsks = rawData->value("asks").toArray();
  jBids = rawData->value("bids").toArray();

  // Create new asks and bids lists
  QList<Order> asks;
  QList<Order> bids;

  // Populate ask orders list
  for(int i=0;i<jAsks.size();i++) {

    double pair1 = jAsks[i].toArray()[0].toDouble();
    double pair2 = jAsks[i].toArray()[1].toDouble();
    asks.append(Order("BTC/USD",pair1,pair2));
  }

  // Populate bid orders list
  for(int i=0;i<jBids.size();i++) { // TODO: order should be in reverse

    double pair1 = jBids[i].toArray()[0].toDouble();
    double pair2 = jBids[i].toArray()[1].toDouble();
    bids.append(Order("BTC/USD",pair1,pair2));
  }

  // Create new tradeDepth object
  tradeDepth = TradeDepth(asks, bids);
}

// Parses new market ticker data and overwrites current data
void MarketData::parseRawTickerData(QJsonObject *rawData) {
    (void) rawData;
}

// Saves the current market data set to a specified file
bool MarketData::loadTradeDataFromFile(uint maxAge) {

  bool result = false;

  tradeData.clear();

  // Open file and read the data
  QFile file(tradeDataFileName);
  QJsonDocument json;

  if(file.open(QFile::ReadOnly)) {

    qDebug() << "Loading trade data from " << tradeDataFileName;

    QJsonParseError error;

    json = QJsonDocument().fromJson(file.readAll(), &error);

    // Check if JSON was correctly parsed
    if (error.error == QJsonParseError::NoError)
      result = true;
    else {

      qDebug() << error.errorString();
      return result;
    }

    // Calculate oldest allowed timestamp
    uint maxTimeStamp = 0;

    if(maxAge != 0) {

      uint currentTimeStamp = findClosestBin();
      maxTimeStamp = currentTimeStamp - maxAge;
    }

    // Read JSON values
    QJsonObject object    = json.object();
    QJsonArray marketData = object.value("marketData").toArray();

    // Convert the JSON array to a Trade List
    for(int i = 0; i < marketData.size(); i++) {

      // Convert the JSON values
      QJsonObject tradeObject = marketData[i].toObject();

      double  price     = tradeObject.value("price").toDouble();
      double  amount    = tradeObject.value("amount").toDouble();
      uint    tradeID   = (uint)tradeObject.value("tradeID").toInt();
      uint    timeStamp = (uint)tradeObject.value("timeStamp").toInt();

      // Check if the timeStamp is younger than max allowed
      if(timeStamp < maxTimeStamp) {
        qDebug() << "timeStamp < maxTimeStamp " << timeStamp << " < " << maxTimeStamp;
        continue;
      }

      // Create a trade object from the converted values
      Trade trade(price, amount, tradeID, timeStamp);

      // Append the new trade object to the trade list
      tradeData.append(trade);
    }
  }

  return result;
}

// Loads a market data set from a specified file
void MarketData::saveTradeDataToFile() {

    if(!tradeData.size()>0)
        return;

  QJsonObject object;
  QJsonArray marketData;

  // Insert the currently stored trade data into the JSON array
  for(int i = 0; i < tradeData.size(); i++) {

    QJsonObject tradeObject;
    Trade trade = tradeData[i];

    tradeObject.insert("price", QJsonValue(trade.getPrice()));
    tradeObject.insert("amount", QJsonValue(trade.getAmount()));
    tradeObject.insert("tradeID", QJsonValue((int)trade.getTradeID()));
    tradeObject.insert("timeStamp", QJsonValue((int)trade.getTimeStamp()));

    marketData.append(tradeObject);
  }

  // Insert the marketData array to the JSON object
  object.insert("marketData", QJsonValue(marketData));

  // Create a JSON document from the newly created JSON object
  QJsonDocument document(object);

  // Save the JSON document to file
  QFile file(tradeDataFileName);
  file.open(QFile::WriteOnly);
  file.write(document.toJson(QJsonDocument::Indented));
}

// Returns the oldest stored trade
uint MarketData::getOldestTrade() {

  if(!tradeData.size()>0)
    return 0;

  return tradeData[tradeData.size()-1].getTimeStamp();
}

TradeDepth MarketData::getTradeDepth() {
  return tradeDepth;
}

QList<Trade> MarketData::getTradeData() {
  return tradeData;
}

QList<DataPoint>  MarketData::getBinnedTradeData() {
  return binnedTradeData;
}

QList<QList<double> > MarketData::getMAList() {
  return MAList;
}

Ticker MarketData::getTicker() {
  return ticker;
}

Ticker::Ticker( double High,    double Low,
                double Avg,     double Vol,
                double Vol_cur, double Last,
                double Buy,     double Sell,
                uint Updated ) {

  high    = High;
  avg     = Avg;
  vol_cur = Vol_cur;
  buy     = Buy;
  updated = Updated;
  low     = Low;
  vol     = Vol;
  last    = Last;
  sell    = Sell;

}

#include "config.h"

Config::Config() {

  fileName = "config.json";
  lastLoadedTimeStamp = 0;
  coolDownTime = 2;
}

Config::~Config() {

}

// Loads a configuration from a given file, overwriting the running config
bool Config::loadConfigFromFile() {

  bool result = false;

  QFile file(fileName);
  QJsonDocument json;

  if(file.open(QFile::ReadOnly)) {

    QJsonParseError error;

    json = QJsonDocument().fromJson(file.readAll(), &error);

    // Check if JSON was correctly parsed
    if (error.error == QJsonParseError::NoError)
      result = true;
    else {

      qDebug() << error.errorString();
      return result;
    }

    // Read JSON values
    QJsonObject object  = json.object();
    lastLoadedTimeStamp = (uint)object.value("lastLoadedTimeStamp").toInt();
    coolDownTime        = (uint)object.value("coolDownTime").toInt();
    apiKey              = object.value("apiKey").toString();
    apiSecret           = object.value("apiSecret").toString();

    // Read history values
    QJsonObject historyObject = object.value("historySources").toObject();
    historySourceID           = historyObject.value("historySourceID").toInt();
    QJsonArray  historyArray  = historyObject.value("sourcesList").toArray();

    // Read history source object from array and add it to the sources list
    historySources.clear();

    for(int i=0;i<historyArray.size();i++) {    //TODO: throw error if list empty

      QJsonObject sourceObject = historyArray[i].toObject();

      HistorySource source( sourceObject.value("sourceID").toInt(),
                            (uint)sourceObject.value("historyCoolDownTime").toInt(),
                            (uint)sourceObject.value("historyLastLoadedTimeStamp").toInt() );

      historySources.append(source);
    }

    // Set the current history cooldown and timestamp values
    if(historySources.size() > 0) {
      historyCoolDownTime         = historySources[historySourceID].getHistoryLastLoadedTimeStamp();
      historyLastLoadedTimeStamp  = historySources[historySourceID].getHistoryCoolDownTime();
    }
  }

  return result;
}

// Saves the running config to file
void Config::saveConfigToFile() {

  // Create JSON object from current configuration
  QJsonObject object;

  // Add generic values
  object.insert("lastLoadedTimeStamp", QJsonValue((int)lastLoadedTimeStamp));
  object.insert("coolDownTime", QJsonValue((int)coolDownTime));
  object.insert("apiKey", QJsonValue(apiKey));
  object.insert("apiSecret", QJsonValue(apiSecret));

  // Add history values to history object
  QJsonObject historyObject;
  QJsonArray  historyArray;

  historyObject.insert("historySourceID", QJsonValue(historySourceID));

  // Create an array for the historical data sources
  for(int i=0;i<historySources.size();i++) {

    QJsonObject source;

    source.insert("sourceID", QJsonValue(historySources[i].getSourceID()));
    source.insert("historyCoolDownTime", QJsonValue((int)historySources[i].getHistoryCoolDownTime()));
    source.insert("historyLastLoadedTimeStamp", QJsonValue((int)historySources[i].getHistoryLastLoadedTimeStamp()));

    historyArray.append(QJsonValue(source));
  }

  // Add the sources array to the history object
  historyObject.insert("sourcesList", QJsonValue(historyArray));

  // Add the history object to the JSON object
  object.insert("historySources", QJsonValue(historyObject));

  // Create a JSON document from the newly created JSON object
  QJsonDocument document(object);

  // Save the JSON document to file
  QFile file(fileName);
  file.open(QFile::WriteOnly);
  file.write(document.toJson(QJsonDocument::Indented));
}

void Config::setCoolDownTime(uint CoolDownTime) { coolDownTime = CoolDownTime; }
uint Config::getCoolDownTime() { return coolDownTime; }

uint Config::getLastLoadedTimeStamp() { return lastLoadedTimeStamp; }
void Config::setLastLoadedTimeStamp(uint LastLoadedTimeStamp) { lastLoadedTimeStamp = LastLoadedTimeStamp; }

QString Config::getApiKey() { return apiKey; }
QString Config::getApiSecret() { return apiSecret; }
void    Config::setApiKey(QString ApiKey){ apiKey = ApiKey; }
void    Config::setApiSecret(QString ApiSecret){ apiSecret = ApiSecret; }

int  Config::getHistorySource() { return historySourceID; }
uint Config::getHistoryCoolDownTime() { return historyCoolDownTime; }
void Config::setHistoryLastLoadedTimeStamp(uint timeStamp) { historyLastLoadedTimeStamp = timeStamp; }
uint Config::getHistoryLastLoadedTimeStamp() { return historyLastLoadedTimeStamp; }

// Sets the current time as the timeStamp for the same overloaded function
void Config::setLastLoadedTimeStamp() {

  QDateTime time(QDateTime::currentDateTime());
  setLastLoadedTimeStamp(time.toTime_t());
}

//
HistorySource::HistorySource(int SourceID, uint HistoryCoolDownTime, uint HistoryLastLoadedTimeStamp) {
    sourceID                    = SourceID;
    historyCoolDownTime         = HistoryCoolDownTime;
    historyLastLoadedTimeStamp  = HistoryLastLoadedTimeStamp;
}
uint HistorySource::getHistoryCoolDownTime() { return historyCoolDownTime; }
void HistorySource::setHistoryLastLoadedTimeStamp(int timeStamp) { historyLastLoadedTimeStamp = timeStamp; }
uint HistorySource::getHistoryLastLoadedTimeStamp() { return historyLastLoadedTimeStamp; }
int  HistorySource::getSourceID() { return sourceID; }

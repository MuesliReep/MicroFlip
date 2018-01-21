#ifndef CONFIG_H
#define CONFIG_H

#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonArray>

class HistorySource
{

public:
  HistorySource(int SourceID, uint HistoryCoolDownTIme, uint HistoryLastLoadedTimeStamp);

  uint getHistoryCoolDownTime();
  void setHistoryLastLoadedTimeStamp(int timeStamp);
  uint getHistoryLastLoadedTimeStamp();
  int  getSourceID();

private:
  int  sourceID;
  uint historyCoolDownTime;
  uint historyLastLoadedTimeStamp;
};

class Config
{

public:
  Config();
  ~Config();

  bool loadConfigFromFile();
  void saveConfigToFile();

  uint getLastLoadedTimeStamp();
  void setLastLoadedTimeStamp();
  void setLastLoadedTimeStamp(uint LastLoadedTimeStamp);

  QString getApiKey();
  QString getApiSecret();
  QString getCustomerID();

  void setApiKey(QString);
  void setApiSecret(QString);
  void setCustomerID(QString);

  void setCoolDownTime(uint CoolDownTime);
  uint getCoolDownTime();

  int  getHistorySource();
  uint getHistoryCoolDownTime();
  void setHistoryLastLoadedTimeStamp(uint timeStamp);
  uint getHistoryLastLoadedTimeStamp();

private:
  QList<HistorySource> historySources;
  QString fileName;
  uint lastLoadedTimeStamp;
  uint coolDownTime;
  QString apiKey;
  QString apiSecret;
  QString customerID;

  int  historySourceID;
  uint historyCoolDownTime;
  uint historyLastLoadedTimeStamp;

};

#endif // CONFIG_H

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config
{

public:
  Config();
  ~Config();

  bool loadConfigFromFile(QString fileName = "config.ini");
  void saveConfigToFile  (QString fileName = "config.ini");

  QString getApiKey();
  QString getApiSecret();
  QString getCustomerID();

  double  getAmount()     const;
  double  getProfit()     const;
  double  getMinSell()    const;
  QString getPair()       const;
  int     getNumWorkers() const;

  int getShortInterval() const;
  int getLongInterval()  const;

  int getLogLevel() const;

private:

  QString apiKey;
  QString apiSecret;
  QString customerID;

  double amount;
  double profit;
  double minSell;
  QString pair;
  int numWorkers;

  int shortInterval;
  int longInterval;

  int logLevel;
};

#endif // CONFIG_H

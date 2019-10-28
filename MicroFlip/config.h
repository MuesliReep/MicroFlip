#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config {

public:
  Config ();
  ~Config();

  bool loadConfigFromFile(const QString& fileName = "config.ini");
  void saveConfigToFile  (const QString& fileName = "config.ini");

  QString getApiKey                () const;
  QString getApiSecret             () const;
  QString getCustomerID            () const;

  double  getAmount                () const;
  double  getProfit                () const;
  double  getMinSell               () const;
  QString getPair                  () const;
  int     getNumWorkers            () const;
  int     getMode                  () const;

  int     getShortInterval         () const;
  int     getLongInterval          () const;

  int     getLogLevel              () const;
  bool    getSingleShot            () const;

  bool    getUseRemote             () const;
  QString getRemoteServerKey       () const;
  QString getRemotePrivateKey      () const;
  quint16 getRemoteListenPort      () const;

private:

  QString apiKey                {};
  QString apiSecret             {};
  QString customerID            {};

  double  amount                {};
  double  profit                {};
  double  minSell               {};
  QString pair                  {};
  int     numWorkers            {};
  int     mode                  {};

  bool    singleShot            {};
  int     logLevel              {};

  int     shortInterval         {};
  int     longInterval          {};

  bool    useRemote             {};
  QString remoteServerKey       {};
  QString remotePrivateKey      {};
  QString remoteVerificationKey {};
  quint16 remoteListenPort      {};

};

#endif // CONFIG_H

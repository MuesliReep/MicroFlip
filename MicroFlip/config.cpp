#include "config.h"

#include <QSettings>
#include <QFile>

#include "common.h"

Config::Config() {

}

Config::~Config() {

}

bool Config::loadConfigFromFile(QString fileName) {

    static const QString DEFAULT_API_KEY     = "";
    static const QString DEFAULT_API_SECRET  = "";
    static const QString DEFAULT_CUSTOMER_ID = "";

    static const double  DEFAULT_AMOUNT  = 0;
    static const double  DEFAULT_PROFIT  = 0;
    static const double  DEFAULT_MINSELL = 0;
    static const int     DEFAULT_WORKERS = 0;
    static const QString DEFAULT_PAIR    = "btc_usd";
    static const int     DEFAULT_MODE    = workerMode::MINSELL;

    static const int     DEFAULT_INTERVAL_SHORT = 10000;
    static const int     DEFAULT_INTERVAL_LONG  = 60*1*1000;

    static const int     DEFAULT_LOG_LEVEL = logSeverity::LOG_INFO;

    bool result = false;

    QFile file(fileName);

    // If config file does not exist, create a default file and exit
    if(!file.exists()) {
        // TODO: log line instead of qdebug

        // Create file
        QSettings settings(fileName, QSettings::IniFormat);

        // Set default values
        settings.setValue("Exchange/ApiKey",        DEFAULT_API_KEY);
        settings.setValue("Exchange/ApiSecret",     DEFAULT_API_SECRET);
        settings.setValue("Exchange/CustomerID",    DEFAULT_CUSTOMER_ID);

        settings.setValue("Worker/Amount",          DEFAULT_AMOUNT);
        settings.setValue("Worker/Profit",          DEFAULT_PROFIT);
        settings.setValue("Worker/MinimumSell",     DEFAULT_MINSELL);
        settings.setValue("Worker/NumberOfWorkers", DEFAULT_WORKERS);
        settings.setValue("Worker/CurrencyPair",    DEFAULT_PAIR);
        settings.setValue("Worker/Mode",            DEFAULT_MODE);

        settings.setValue("Worker/Interval/Short",  DEFAULT_INTERVAL_SHORT);
        settings.setValue("Worker/Interval/Long",   DEFAULT_INTERVAL_LONG);

        settings.setValue("System/LogLevel",        DEFAULT_LOG_LEVEL);

        // Save settings
        settings.sync();

        return result;
    }

    QSettings settings(fileName, QSettings::IniFormat);

    this->apiKey        = settings.value("Exchange/ApiKey",        DEFAULT_API_KEY        ).toString();
    this->apiSecret     = settings.value("Exchange/ApiSecret",     DEFAULT_API_SECRET     ).toString();
    this->customerID    = settings.value("Exchange/CustomerID",    DEFAULT_CUSTOMER_ID    ).toString();

    this->amount        = settings.value("Worker/Amount",          DEFAULT_AMOUNT         ).toDouble();
    this->profit        = settings.value("Worker/Profit",          DEFAULT_PROFIT         ).toDouble();
    this->minSell       = settings.value("Worker/MinimumSell",     DEFAULT_MINSELL        ).toDouble();
    this->numWorkers    = settings.value("Worker/NumberOfWorkers", DEFAULT_WORKERS        ).toInt();
    this->pair          = settings.value("Worker/CurrencyPair",    DEFAULT_PAIR           ).toString();
    this->mode          = settings.value("Worker/Mode",            DEFAULT_MODE           ).toInt();

    this->shortInterval = settings.value("Worker/Interval/Short",  DEFAULT_INTERVAL_SHORT ).toInt();
    this->longInterval  = settings.value("Worker/Interval/Long",   DEFAULT_INTERVAL_LONG  ).toInt();

    this->logLevel      = settings.value("System/LogLevel",        DEFAULT_LOG_LEVEL      ).toInt();

    result = true;

    settings.sync();

    return result;
}

void Config::saveConfigToFile(QString fileName) {

    QSettings settings(fileName, QSettings::IniFormat);

    settings.setValue("Exchange/ApiKey",        this->apiKey        );
    settings.setValue("Exchange/ApiSecret",     this->apiSecret     );
    settings.setValue("Exchange/CustomerID",    this->customerID    );

    settings.setValue("Worker/Amount",          this->amount        );
    settings.setValue("Worker/Profit",          this->profit        );
    settings.setValue("Worker/MinimumSell",     this->minSell       );
    settings.setValue("Worker/NumberOfWorkers", this->numWorkers    );
    settings.setValue("Worker/CurrencyPair",    this->pair          );
    settings.setValue("Worker/Mode",            this->mode          );

    settings.setValue("Worker/Interval/Short",  this->shortInterval );
    settings.setValue("Worker/Interval/Long",   this->longInterval  );

    settings.setValue("System/LogLevel",        this->logLevel      );

    settings.sync();
}

QString Config::getApiKey() { return apiKey; }
QString Config::getApiSecret() { return apiSecret; }
QString Config::getCustomerID() { return customerID; }

double Config::getAmount() const
{
    return amount;
}

double Config::getProfit() const
{
    return profit;
}

double Config::getMinSell() const
{
    return minSell;
}

QString Config::getPair() const
{
    return pair;
}

int Config::getNumWorkers() const
{
    return numWorkers;
}

int Config::getShortInterval() const
{
    return shortInterval;
}

int Config::getLongInterval() const
{
    return longInterval;
}

int Config::getLogLevel() const
{
    return logLevel;
}

int Config::getMode() const
{
    return mode;
}

/* Example:
  
{
    "apiKey": "QN1M3IS6-1KVW0SKR-NY3VAY9J-S4O1OL7T-70370K8I",
    "apiSecret": "a1e313937830a75a810b4409e707f0ee7eda016177a608f4c7f3b6c3c0508f6c",
    "customerID":"123456",
    "coolDownTime": 2,
    "historySources": {
        "historySourceID": 0,
        "sourcesList": [
            {
                "historyCoolDownTime": 2,
                "historyLastLoadedTimeStamp": 0,
                "sourceID": 0
            },
            {
                "historyCoolDownTime": 900,
                "historyLastLoadedTimeStamp": 0,
                "sourceID": 1
            }
        ]
    },
    "lastLoadedTimeStamp": 1422385480
}

 */

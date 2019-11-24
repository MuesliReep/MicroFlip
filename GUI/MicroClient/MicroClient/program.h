#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <utility>

#include "remotecontrol.h"
#include "logitemmodel.h"
#include "workordermodel.h"

class ExchangeInfo : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString symbol   READ symbol   WRITE  setsymbol NOTIFY symbolChanged)
    Q_PROPERTY(QString price    READ price    NOTIFY priceChanged)
    Q_PROPERTY(QString avgPrice READ avgPrice NOTIFY avgPriceChanged)

public:
    QString symbol   () const { return currentSymbol;   }
    QString price    () const { return currentPrice;    }
    QString avgPrice () const { return currentAvgPrice; }

    void setsymbol       (QString symbol ) { currentSymbol   = std::move(symbol);         emit symbolChanged  (); }
    void setCurrentPrice (double price   ) { currentPrice    = QString::number(price);    emit priceChanged   (); }
    void setAvgPrice     (double avgPrice) { currentAvgPrice = QString::number(avgPrice); emit avgPriceChanged(); }

private:
    QString currentSymbol   {"CRYPTO"};
    QString currentPrice    {"0.0"};
    QString currentAvgPrice {"0.0"};

signals:
    void symbolChanged   ();
    void priceChanged    ();
    void avgPriceChanged ();
};


class Program : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString remoteConnectionState READ remoteConnectionStateString NOTIFY remoteConnectionStateChanged)

public:
    explicit Program(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    QString      remoteConnectionStateString     () const;

private:
    RemoteControl *remoteControl;
    ExchangeInfo   exchangeInfo;

    WorkOrderModel workOrderModel;
    LogItemModel   logItemModel;

    void startUp();

signals:
    void remoteConnectionStateChanged ();
    void sendCreateWorker(int numWorkers, const QString& pair, double amount, double profitTarget, int shortInterval, int longInterval, int mode, bool singleShot, double minSellPrice);
    void sendRemoveWorker(int workId, bool force);

public slots:
    void onNewWorkerStatus        (int workID, const QString& state);
    void onNewBalanceValues       ();
    void onNewExchangeInformation (QString symbol, double lastPrice, double avgPrice);
    void onNewLogUpdate           (int workID, const QString& className, const QString& log, int severity);
    void onConsoleLog             (int workID, const QString& className, const QString& log, int severity);
    void onAddNewWorker(QString numWorkers, QString pair, QString amount, QString profitTarget, QString shortInterval, QString longInterval, QString mode, bool singleShot, QString minSellPrice);
    void onRemoveWorker(QString workId, bool force);

private slots:

    void loadDummyData();
};

#endif // PROGRAM_H

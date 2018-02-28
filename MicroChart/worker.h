#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "../MicroFlip/trade.h"
#include "../MicroFlip/exchange.h"

class Worker : public QThread
{
    Q_OBJECT
public:
    Worker(Exchange *exchange, QString pair);

private:
    Exchange *exchange;
    QString   pair;
    QTimer   *timer;
    QThread  *workThread;

    int  intervalShort;
    int  intervalLong;

public slots:
  void updateMarketTradesReply(QList<Trade> marketTrades);

  void startWorker();

private slots:
  void updateTick();

signals:
  void sendUpdateMarketTrades(QString pair, QObject *sender);
};

#endif // WORKER_H

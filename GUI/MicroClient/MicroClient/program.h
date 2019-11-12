#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>

#include "remotecontrol.h"
#include "logitemcontroller.h"


class Program : public QObject
{
    Q_OBJECT
public:
    explicit Program(QObject *parent = nullptr);

private:
    RemoteControl            *remoteControl;
//    QList<logItemController>  logItems;

    void startUp();

signals:

public slots:
    void         onNewWorkerStatus        (int workID, QString state);
    void         onNewBalanceValues       ();
    void         onNewExchangeInformation (QString symbol, double lastPrice, double avgPrice);
    void         onNewLogUpdate           (int workID, QString className, QString log, int severity);

private slots:
};

#endif // PROGRAM_H

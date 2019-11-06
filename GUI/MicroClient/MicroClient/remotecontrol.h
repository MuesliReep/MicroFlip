#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

#include "config.h"

class RemoteControl : public QObject {

    Q_OBJECT

public:
    explicit     RemoteControl               (Config config);

private:
    uint64_t     lastNonce                   {0};
    bool         parseNewMessage             ();
    bool         parseHelloMessage           ();
    bool         parseLogUpdateMessage       (QString message);
    bool         parseWorkorderUpdateMessage (QString message);
    bool         parseExchangePriceUpdateMessage (QString message);
    bool         verifySignature             (QString message, QString nonce, QString signature);
    QByteArray   createSignature             (QString message, QString key);

protected:
    QString      className                   {"REMOTECONTROL"};
    QString      serverKey                   {};
    QString      privateKey                  {};
    bool         connected                   {false};
    bool         authenticated               {false};
    virtual bool sendMessage                 (QString message) = 0;
    void         createHelloMessage          ();
    bool         parseNewMessage             (QString message, bool *verified);

public slots:
    virtual void open                        () = 0;
    void         createWorkerMessage         ();
    bool         createRemoveWorkerMessage   (uint workerID);

signals:
    void         newWorkerStatus             (int workID, QString state);
    void         newBalanceValues            ();
    void         newExchangeInformation      (QString symbol, double lastPrice, double avgPrice);
    void         logUpdate                   (int workID, QString className, QString log, int severity);
    void         isConnected                 (bool state);
    void         isAuthenticated             (bool state);
    void         updateLog                   (int workID, QString className, QString log, int severity);
};

#endif // REMOTECONTROL_H

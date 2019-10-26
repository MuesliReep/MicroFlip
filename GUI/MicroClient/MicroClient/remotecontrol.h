#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

#include "config.h"

class RemoteControl : public QObject {

    Q_OBJECT

public:
    explicit     RemoteControl             (Config config);

private:
    uint64_t     lastReceivedNonce         {0};
    bool         parseNewMessage           ();
    bool         parseHelloMessage         ();
    uint64_t     createNonce               ();
    bool         verifySignature           (QString message, uint64_t nonce, QString signature, QString key);
    QByteArray   createSignature           (QString message, QString key);

protected:
    QString      serverKey                 {};
    QString      privateKey                {};
    bool         connected                 {false};
    bool         authenticated             {false};
    virtual bool sendMessage               (QString message) = 0;
    void         createHelloMessage        ();
    bool         parseNewMessage           (QString message, bool *verified);

public slots:
    virtual void open                      () = 0;
    void         createWorkerMessage       ();
    bool         createRemoveWorkerMessage (uint workerID);

signals:
    void         newWorkerStatus           ();
    void         newBalanceValues          ();
    void         newExchangeInformation    ();
    void         logUpdate                 ();
    void         isConnected               (bool state);
    void         isAuthenticated           (bool state);

};

#endif // REMOTECONTROL_H

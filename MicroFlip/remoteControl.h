#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

class RemoteControl : public QObject {

    Q_OBJECT

public:
    RemoteControl();

private:
    uint64_t     lastNonce                {0};

protected:
    QString      className                {"REMOTECONTROL"};
    QString      serverKey                {};
    QString      privateKey               {};

    //TODO: server should send verification key so client can check if server is valid
    bool         verifySignature          (QString message, QString nonce, QString signature);

    bool         parseNewMessage          (QString message, bool *verified);
    bool         parseHelloMessage        ();
    bool         parseCreateWorkerMessage (QString message);
    bool         parseRemoveWorkerMessage (QString message);
    void         createHelloMessage       ();
    virtual bool sendMessage              (QString message) = 0;

public slots:
    virtual bool open                     () = 0;
    void         logUpdate                (int workID, QString className, QString log, int severity);
    void         workorderStateUpdate     (int workID, QString state);
    void         exchangePricesUpdate     (QString symbol, double lastPrice, double avgPrice);

signals:
    void         createWorker             ();
    void         removeWorker             (uint workOrderID, bool force = false);
    void         updateLog                (int workID, QString className, QString log, int severity);
};

#endif // REMOTECONTROL_H

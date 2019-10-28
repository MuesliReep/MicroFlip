#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

class RemoteControl : public QObject {

    Q_OBJECT

public:
    RemoteControl();

private:
    uint64_t   lastNonce              {0};

    QByteArray createSignature        (QString message, QString key);

protected:
    QString className                 {"REMOTECONTROL"};
    QString serverKey                 {};
    QString privateKey                {};

    bool verifySignature              (QString message, QString nonce, QString signature); //TODO: server should send verification key so client can check if server is valid

    bool parseNewMessage              (QString message, bool *verified);
    bool parseHelloMessage            ();
    bool parseCreateWorkerMessage     ();
    bool parseRemoveWorkerMessage     ();
    void createHelloMessage           ();

public slots:
    virtual bool open                 () = 0;
    virtual void logUpdate            (int workID, QString className, QString log, int severity) = 0;
    virtual void workorderStateUpdate (int workID, QString state) = 0;
    virtual void exchangePricesUpdate (QString symbol, double lastPrice, double avgPrice) = 0;

signals:
    void createWorker                 ();
    void removeWorker                 (uint workOrderID, bool force = false);
    void updateLog                    (int workID, QString className, QString log, int severity);
};

#endif // REMOTECONTROL_H

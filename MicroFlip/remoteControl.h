#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

class RemoteControl : public QObject {

    Q_OBJECT

public:
    RemoteControl();

protected:
    QString className = "REMOTECONTROL";

    bool verifySignature(QString message, QString nonce, QString signature); //TODO: server should send verification key so client can check if server is valid

    bool parseNewMessage(QString message, bool *verified);
    bool parseHelloMessage();
    bool parseCreateWorkerMessage();
    bool parseRemoveWorkerMessage();

public slots:
    virtual bool open()                 = 0;
    virtual void logUpdate()            = 0;
    virtual void workorderStateUpdate() = 0;
    virtual void exchangePricesUpdate() = 0;

signals:
    void createWorker();
    void removeWorker();
};

#endif // REMOTECONTROL_H

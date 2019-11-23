#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

class RemoteControl : public QObject {

    Q_OBJECT

public:
    RemoteControl();

private:
    uint64_t     lastNonce                {0};
    uint64_t     ourLastNonce             {0};
    uint64_t     createNonce              ();

protected:
    QString      className                {"REMOTECONTROL"};
    QString      serverKey                {};
    QString      privateKey               {};

    bool         verifySignature          (QString message, const QString& nonce, const QString& signature);

    bool         parseNewMessage          (const QString& message, bool *verified);
    bool         parseHelloMessage        ();
    bool         parseCreateWorkerMessage (const QString& message);
    bool         parseRemoveWorkerMessage (const QString& message);
    void         createHelloMessage       ();
    virtual bool sendMessage              (QString message) = 0;

public slots:
    virtual bool open                     () = 0;
    void         logUpdate                (int workID, const QString& className, const QString& log, int severity);
    void         workorderStateUpdate     (int workID, const QString& state);
    void         exchangePricesUpdate     (const QString& symbol, double lastPrice, double avgPrice);

signals:
    void         createWorker             (int numWorkers, QString pair, double maxAmount, double profitTarget, int shortInterval, int longInterval, int mode, bool singleShot, double minSellPrice);
    void         removeWorker             (uint workOrderID, bool force = false);
    void         updateLog                (int workID, QString className, QString log, int severity);
};

#endif // REMOTECONTROL_H

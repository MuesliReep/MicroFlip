#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>

#include "config.h"

class RemoteControl : public QObject {

    Q_OBJECT

public:
    explicit     RemoteControl                   (const Config& config);
    int          getRmoteConnectionState         () const;
    void         setRemoteConnectionState        (int state);

private:
    int          m_remoteConnectionState         {0};
    uint64_t     lastNonce                       {0};
    uint64_t     ourLastNonce                    {0};
    uint64_t     createNonce                     ();
    bool         parseNewMessage                 ();
    bool         parseHelloMessage               ();
    bool         parseLogUpdateMessage           (const QString& message);
    bool         parseWorkorderUpdateMessage     (const QString& message);
    bool         parseExchangePriceUpdateMessage (const QString& message);
    bool         verifySignature                 (const QString& message, const QString& nonce, const QString& signature);
    QByteArray   createSignature                 (const QString& message, const QString& key);

protected:
    QString      className                       {"REMOTECONTROL"};
    QString      serverKey                       {};
    QString      privateKey                      {};
    bool         connected                       {false};
    bool         authenticated                   {false};
    virtual bool sendMessage                     (QString message) = 0;
    void         createHelloMessage              ();
    bool         parseNewMessage                 (const QString& message, bool *verified);

public slots:
    virtual void open                            () = 0;
    void         createWorkerMessage             (int numWorkers, const QString& pair, double amount, double profitTarget, int shortInterval, int longInterval, int mode, bool singleShot, double minSellPrice);
    bool         createRemoveWorkerMessage       (uint workerID, bool force);

signals:
    void         newWorkerStatus                 (int workID, QString state);
    void         newBalanceValues                ();
    void         newExchangeInformation          (QString exchangeName, double lastPrice, double avgPrice);
    void         newLogUpdate                    (int workID, QString className, QString log, int severity);
    void         isConnected                     (bool state);
    void         isAuthenticated                 (bool state);
    void         updateLog                       (int workID, QString className, QString log, int severity);
    void         remoteConnectionStateChanged    ();
};

#endif // REMOTECONTROL_H

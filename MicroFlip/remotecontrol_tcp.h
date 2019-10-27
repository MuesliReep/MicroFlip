#ifndef REMOTECONTROL_TCP_H
#define REMOTECONTROL_TCP_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "common.h"
#include "remoteControl.h"
#include "tcpauthsocket.h"

class RemoteControl_TCP : public RemoteControl {

    Q_OBJECT

public:
    RemoteControl_TCP(quint16 listenPort);

private:    
    QTcpServer            *server;
    QList<TcpAuthSocket*>  sockets;
    quint16                listenPort;

    void parseRawMessage(QByteArray rawData);

private slots:
    void onNewConnection      ();
    void onSocketStateChanged (QAbstractSocket::SocketState socketState);
    void onReadyRead          ();

signals:
    void updateLog            (int workID, QString className, QString log, int severity);


    // RemoteControl interface
public slots:
    bool open                 ();
    void logUpdate            (int workID, QString className, QString log, int severity);
    void workorderStateUpdate (int workID, QString state);
    void exchangePricesUpdate (QString symbol, double lastPrice, double avgPrice);
};

#endif // REMOTECONTROL_TCP_H

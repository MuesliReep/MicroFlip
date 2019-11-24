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
    RemoteControl_TCP(quint16 listenPort, QString serverKey, QString privateKey);

private:    
    QTcpServer            *server{};
    QList<TcpAuthSocket*>  sockets;
    quint16                listenPort;
    QByteArray             readBuffer;

    bool parseRawMessage(const QByteArray& rawData, TcpAuthSocket *sender);

private slots:
    void onNewConnection      ();
    void onSocketStateChanged (QAbstractSocket::SocketState socketState);
    void onReadyRead          ();

    // RemoteControl interface
public slots:
    bool open                 ();

protected:
    bool sendMessage(QString message);
};

#endif // REMOTECONTROL_TCP_H

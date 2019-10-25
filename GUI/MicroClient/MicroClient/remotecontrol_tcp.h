#ifndef REMOTECONTROL_TCP_H
#define REMOTECONTROL_TCP_H

#include <QTcpSocket>

#include "remotecontrol.h"


class RemoteControl_TCP : public RemoteControl {

    Q_OBJECT

public:
    RemoteControl_TCP(Config config);

private:
    QString     serverAddress   {};
    uint16_t    serverPort      {};
    QTcpSocket  socket          {};
    QByteArray  readBuffer      {};

    void parseRawMessage(QByteArray rawData);

protected:
    bool sendMessage(QString message);

public slots:
    void open();
    void onReadyRead();




public slots:

};

#endif // REMOTECONTROL_TCP_H

#include "remotecontrol_tcp.h"

RemoteControl_TCP::RemoteControl_TCP() {

    listenPort = 10000;
}

bool RemoteControl_TCP::open() {

    bool openResult = false;

    openResult = server.listen(QHostAddress::Any, listenPort);

    if(openResult) {
        connect(&server, &QTcpServer::newConnection, this, &RemoteControl_TCP::onNewConnection);
    }

    return openResult;
}

void RemoteControl_TCP::parseRawMessage(QByteArray rawData) {

}

void RemoteControl_TCP::onNewConnection() {

    TcpAuthSocket *clientSocket = static_cast<TcpAuthSocket *>(server.nextPendingConnection());
    connect(clientSocket, &QTcpSocket::readyRead,    this, &RemoteControl_TCP::onReadyRead);
    connect(clientSocket, &QTcpSocket::stateChanged, this, &RemoteControl_TCP::onSocketStateChanged);

    sockets.push_back(clientSocket);

    emit updateLog(00, className, "Remote client " + clientSocket->peerAddress().toString() + " connected", logSeverity::LOG_INFO);
}

void RemoteControl_TCP::onSocketStateChanged(QAbstractSocket::SocketState socketState) {

    if (socketState == QAbstractSocket::UnconnectedState)
    {
        TcpAuthSocket* sender = static_cast<TcpAuthSocket*>(QObject::sender());

        emit updateLog(00, className, "Remote client " + sender->peerAddress().toString() + " disconnected", logSeverity::LOG_INFO);

        sockets.removeOne(sender);
    }
}

void RemoteControl_TCP::onReadyRead() {

    TcpAuthSocket* sender = static_cast<TcpAuthSocket*>(QObject::sender());

    parseRawMessage(sender->readAll());

}

void RemoteControl_TCP::logUpdate() {

    for (TcpAuthSocket* socket : sockets) {
        socket->write(QByteArray::fromStdString("log update"));
    }
}

void RemoteControl_TCP::workorderStateUpdate() {

    for (TcpAuthSocket* socket : sockets) {
        socket->write(QByteArray::fromStdString("log update"));
    }
}

void RemoteControl_TCP::exchangePricesUpdate() {

    for (TcpAuthSocket* socket : sockets) {
        socket->write(QByteArray::fromStdString("log update"));
    }
}

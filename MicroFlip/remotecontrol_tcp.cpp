#include "remotecontrol_tcp.h"

RemoteControl_TCP::RemoteControl_TCP(quint16 listenPort, QString serverKey, QString privateKey) {

    this->listenPort = listenPort;
    this->serverKey  = serverKey;
    this->privateKey = privateKey;
}

bool RemoteControl_TCP::open() {

    bool openResult = false;

    server = new QTcpServer();

    openResult = server->listen(QHostAddress::Any, listenPort);

    if(openResult) {
        connect(server, &QTcpServer::newConnection, this, &RemoteControl_TCP::onNewConnection);
    }

    return openResult;
}

void RemoteControl_TCP::parseRawMessage(QByteArray rawData, TcpAuthSocket *sender) {

    // Send the raw data message to be parsed
    // If message is valid but verification failed, disconnect this client

    bool verified     = false;
    bool messageValid = parseNewMessage(QString(rawData), &verified);

    if(messageValid && !verified) {

        emit updateLog(00, className, "Could not verify remote client " + sender->peerAddress().toString(), logSeverity::LOG_INFO);

        sender->disconnectFromHost();
    } else if(messageValid && verified) {

        sender->setAuthenticated(true);

        emit updateLog(00, className, "Verified remote client " + sender->peerAddress().toString(), logSeverity::LOG_INFO);
    }
}

void RemoteControl_TCP::onNewConnection() {

    TcpAuthSocket *clientSocket = static_cast<TcpAuthSocket *>(server->nextPendingConnection());
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

    parseRawMessage(sender->readAll(), sender);

}

bool RemoteControl_TCP::sendMessage(QString message) {

    for (TcpAuthSocket* socket : sockets) {
        socket->write(message.toUtf8());
    }

    return true;
}

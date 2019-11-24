#include "remotecontrol_tcp.h"

#include <utility>

RemoteControl_TCP::RemoteControl_TCP(quint16 listenPort, QString serverKey, QString privateKey) {

    this->listenPort = listenPort;
    this->serverKey  = std::move(serverKey);
    this->privateKey = std::move(privateKey);
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

bool RemoteControl_TCP::parseRawMessage(const QByteArray& rawData, TcpAuthSocket *sender) {

    // Send the raw data message to be parsed
    // If message is valid but verification failed, disconnect this client

    bool verified     = false;
    bool messageValid = parseNewMessage(QString(rawData), &verified);

    if(messageValid && !verified) {

        emit updateLog(00, className, "Could not verify remote client " + sender->peerAddress().toString(), logSeverity::LOG_INFO);

        sender->disconnectFromHost();

        return false;
    } else if(messageValid && verified) {

        sender->setAuthenticated(true);

        emit updateLog(00, className, "Verified remote client " + sender->peerAddress().toString(), logSeverity::LOG_INFO);
    }

    return true;
}

void RemoteControl_TCP::onNewConnection() {

    auto *clientSocket = static_cast<TcpAuthSocket *>(server->nextPendingConnection());
    connect(clientSocket, &QTcpSocket::readyRead,    this, &RemoteControl_TCP::onReadyRead);
    connect(clientSocket, &QTcpSocket::stateChanged, this, &RemoteControl_TCP::onSocketStateChanged);

    sockets.push_back(clientSocket);

    emit updateLog(00, className, "Remote client " + clientSocket->peerAddress().toString() + " connected", logSeverity::LOG_INFO);
}

void RemoteControl_TCP::onSocketStateChanged(QAbstractSocket::SocketState socketState) {

    if (socketState == QAbstractSocket::UnconnectedState)
    {
        auto* sender = static_cast<TcpAuthSocket*>(QObject::sender());

        emit updateLog(00, className, "Remote client " + sender->peerAddress().toString() + " disconnected", logSeverity::LOG_INFO);

        sockets.removeOne(sender);
    }
}

void RemoteControl_TCP::onReadyRead() {

    auto* sender = static_cast<TcpAuthSocket*>(QObject::sender());

//    parseRawMessage(sender->readAll(), sender);

    readBuffer.append(sender->readAll());

    QByteArray prefix = MESSAGE_PREFIX.toUtf8();
    QByteArray suffix = MESSAGE_SUFFIX.toUtf8();

    bool continueParsing = true;

    while (continueParsing) {

        int prefixIndex = readBuffer.indexOf(prefix);
        if(prefixIndex == -1) { return; } // No message
        if(prefixIndex > 0) { // If there is a partial message at the beginning, remove it
            readBuffer = readBuffer.remove(0, prefixIndex);
        }

        int suffixIndex = readBuffer.indexOf(suffix);
        if(suffixIndex == -1) { return; } // Not a full message, retry later
        else {

            // Grab new message
            QByteArray message = readBuffer.mid(0, suffixIndex + suffix.length());

            // Remove it from the buffer
            readBuffer = readBuffer.remove(0, message.length());

            // Parse message, continue if parse was verified
            continueParsing = parseRawMessage(message, sender);

            // Check if we can continue with parsing
            if(readBuffer.length() <= suffix.length()) {
                continueParsing = false;
            }
        }
    }
}

bool RemoteControl_TCP::sendMessage(QString message) {

    for (TcpAuthSocket* socket : sockets) {
        socket->write(message.toUtf8());
    }

    return true;
}

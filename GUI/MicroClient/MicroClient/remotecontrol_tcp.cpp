#include "remotecontrol_tcp.h"

#include <QHostAddress>
#include "../../../MicroFlip/common.h"

RemoteControl_TCP::RemoteControl_TCP(Config config) : RemoteControl (config) {

    this->serverKey     = config.getServerKey    ();
    this->privateKey    = config.getPrivateKey   ();
    this->serverAddress = config.getServerAdress ();
    this->serverPort    = config.getServerPort   ();

    readBuffer.clear();
}

void RemoteControl_TCP::open() {

    this->setRemoteConnectionState(REMOTE_CONNECTING);

    qDebug() << "Opening socket " << serverAddress << ":" << QString::number(serverPort);

    // Connect to server
    socket = new QTcpSocket;
    socket->connectToHost(QHostAddress(this->serverAddress), this->serverPort);

    // Wait for connection
    if(!socket->waitForConnected(5000)) {

        this->setRemoteConnectionState(REMOTE_ERROR);
        qDebug() << "Could not connect to server, timeout!";
        return;
    }

    this->setRemoteConnectionState(REMOTE_CONNECTED);
    qDebug() << "Connected to server!";

// Does nothing:    connect(&socket, &QTcpSocket::connected,    this, &RemoteControl_TCP::onConnect    );
    connect(socket, &QTcpSocket::disconnected, this, &RemoteControl_TCP::onDisconnect );
    connect(socket, &QTcpSocket::readyRead,    this, &RemoteControl_TCP::onReadyRead  );

    // Send hello message to authenticate
    createHelloMessage();
}

void RemoteControl_TCP::parseRawMessage(QByteArray rawData) {

    // Send the raw data message to be parsed
    // If message is valid but verification failed, disconnect this client

    bool verified     = false;
    bool messageValid = parseNewMessage(QString(rawData), &verified);

    if(messageValid && !verified) {

        this->setRemoteConnectionState(REMOTE_REJECTED);
        emit updateLog(00, className, "Could not verify Server", logSeverity::LOG_CRITICAL);

        socket->disconnectFromHost();
    } else if(messageValid && verified) {

        this->authenticated = true;
        this->setRemoteConnectionState(REMOTE_VERIFIED);

        emit updateLog(00, className, "Verified Server", logSeverity::LOG_INFO);
    }
}

bool RemoteControl_TCP::sendMessage(QString message) {

    if(socket->write(message.toUtf8()) != -1) {
        return true;
    }

    return false;
}

void RemoteControl_TCP::onReadyRead() {

    parseRawMessage(socket->readAll());
}

void RemoteControl_TCP::onConnect() {

    this->setRemoteConnectionState(REMOTE_CONNECTED);

    qDebug() << "Connected to server!";

    this->connected = true;

    emit isConnected(connected);

    // Once connected send a hello message, to authenticate
    createHelloMessage();
}

void RemoteControl_TCP::onDisconnect() {

    this->setRemoteConnectionState(REMOTE_DISCONNECTED);
    this->connected = false;

    emit isConnected(connected);

    qDebug() << "Disconnected from server";
}

#include "remotecontrol_tcp.h"

#include <QHostAddress>

RemoteControl_TCP::RemoteControl_TCP(Config config) : RemoteControl (config) {

    this->serverKey     = config.getServerKey    ();
    this->privateKey    = config.getPrivateKey   ();
    this->serverAddress = config.getServerAdress ();
    this->serverPort    = config.getServerPort   ();

    readBuffer.clear();
}

void RemoteControl_TCP::open() {

    qDebug() << "Opening socket " << serverAddress << ":" << QString::number(serverPort);

    // Connect to server
    socket.connectToHost(QHostAddress(this->serverAddress), this->serverPort);

    // Wait for connection
    if(!socket.waitForConnected(5000)) {

        qDebug() << "Could not connect to server, timeout!";
        return;
    }

    qDebug() << "Connected to server!";

// Does nothing:    connect(&socket, &QTcpSocket::connected,    this, &RemoteControl_TCP::onConnect    );
    connect(&socket, &QTcpSocket::disconnected, this, &RemoteControl_TCP::onDisconnect );
    connect(&socket, &QTcpSocket::readyRead,    this, &RemoteControl_TCP::onReadyRead  );

    // Send hello message to authenticate
    createHelloMessage();
}

void RemoteControl_TCP::parseRawMessage(QByteArray rawData) {

    readBuffer.append(rawData);

    // Search for message prefix in buffer
}

bool RemoteControl_TCP::sendMessage(QString message) {

    if(socket.write(message.toUtf8()) != -1) {
        return true;
    }

    return false;
}

void RemoteControl_TCP::onReadyRead() {

    parseRawMessage(socket.readAll());
}

void RemoteControl_TCP::onConnect() {

    qDebug() << "Connected to server!";

    this->connected = true;

    emit isConnected(connected);

    // Once connected send a hello message, to authenticate
    createHelloMessage();
}

void RemoteControl_TCP::onDisconnect() {

    this->connected = false;

    emit isConnected(connected);
}

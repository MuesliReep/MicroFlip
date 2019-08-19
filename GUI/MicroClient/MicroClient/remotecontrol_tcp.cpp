#include "remotecontrol_tcp.h"

#include <QHostAddress>

RemoteControl_TCP::RemoteControl_TCP(Config config) : RemoteControl (config) {

    this->serverKey     = config.getServerKey    ();
    this->privateKey    = config.getPrivateKey   ();
    this->serverAddress = config.getServerAdress ();
    this->serverPort    = config.getServerPort   ();
}

void RemoteControl_TCP::open() {

    socket.connectToHost(QHostAddress(serverAddress), serverPort);
}

void RemoteControl_TCP::parseRawMessage(QByteArray rawData) {

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

#include "remotecontrol_tcp.h"

#include <QHostAddress>
#include "../../../MicroFlip/common.h"

RemoteControl_TCP::RemoteControl_TCP(const Config& config) : RemoteControl (config) {

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

bool RemoteControl_TCP::parseRawMessage(const QByteArray& rawData) {

    // Send the raw data message to be parsed
    // If message is valid but verification failed, disconnect this client

    bool verified     = false;
    bool messageValid = parseNewMessage(QString(rawData), &verified);

    if(messageValid && !verified) {

        this->setRemoteConnectionState(REMOTE_REJECTED);
        emit updateLog(00, className, "Could not verify Server", logSeverity::LOG_CRITICAL);

        socket->disconnectFromHost();

        return false;

    } else if(messageValid && verified) {

        this->authenticated = true;
        this->setRemoteConnectionState(REMOTE_VERIFIED);

        emit updateLog(00, className, "Verified Server", logSeverity::LOG_INFO);
    }

    return true;
}

bool RemoteControl_TCP::sendMessage(QString message) {

    return socket->write(message.toUtf8()) != -1;
}

void RemoteControl_TCP::onReadyRead() {

    readBuffer.append(socket->readAll());

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
            continueParsing = parseRawMessage(message);

            // Check if we can continue with parsing
            if(readBuffer.length() <= suffix.length()) {
                continueParsing = false;
            }
        }
    }
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

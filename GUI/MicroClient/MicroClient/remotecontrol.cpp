#include "remotecontrol.h"

#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

#include "../../../MicroFlip/common.h"

RemoteControl::RemoteControl(Config config) { (void) config; }

bool RemoteControl::verifySignature(QString message, uint64_t nonce, QString signature, QString key) {

    // First check if nonce is valid
    if (nonce <= lastReceivedNonce) {
        return false;
    }

    lastReceivedNonce = nonce;

    // TODO: verify nonce is higher than the one we sent

    // Create a signature from the message and compare it to the received signature
    if(signature.toUtf8().compare(createSignature(message, key), Qt::CaseInsensitive) == 0) {
        return true;
    }

    return false;
}

QByteArray RemoteControl::createSignature(QString message, QString key) {

    return QMessageAuthenticationCode::hash(message.toUtf8(), key.toUtf8(), QCryptographicHash::Sha256).toHex();
}

// Returns true if message is valid
// Verified is true when signiture is valid
// Message can be valid but signature invalid
// If message is invalid, verified should be ignored
bool RemoteControl::parseNewMessage(QString message, bool *verified) {

    // Message components:
    // PREFIX:COMMAND:COMMAND_COMPONENTS:NONCE:SIGNATURE:SUFFIX

    // Remove suffix
    //TODO: message.remove(message.length() - 1 - MESSAGE_SUFFIX.length(), MESSAGE_SUFFIX.length());

    // Split message into components
    QStringList messageComponents = message.split(MESSAGE_SPLITTER);
    // TODO: check number of components

    // First verify signature
    uint64_t nonce = static_cast<uint64_t>(messageComponents.at(messageComponents.length() -2).toUInt());
    QString signature = messageComponents.last();
    if(verifySignature(message, nonce, signature, serverKey)) {
        *verified = true;
    } else {
        *verified = false;
        return true;
    }

    // Remove prefix
    messageComponents.removeFirst();

    // Next send message to correct parser
    bool parseResult = false;

    QString messageCommand = messageComponents.first();

    if (messageCommand == HELLO_MESSAGE) {

        return parseHelloMessage();
    }

    return parseResult;
}

void RemoteControl::createHelloMessage() {

    QByteArray message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(HELLO_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Create nonce
    uint64_t nonce = createNonce();
    message.append(QString::number(nonce));
    message.append(MESSAGE_SPLITTER);

    // Create signature
    message.append(createSignature(message, privateKey));
    message.append(MESSAGE_SPLITTER);

    // Add suffix
    message.append(MESSAGE_SUFFIX);

    // Send
    sendMessage(message);
}

void RemoteControl::createWorkerMessage() {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(CREATE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    // TODO: payload
    message.append(MESSAGE_SPLITTER);

    // Create nonce
    uint64_t nonce = createNonce();
    message.append(QString::number(nonce));
    message.append(MESSAGE_SPLITTER);

    // Create signature
    message.append(createSignature(message, privateKey));
    message.append(MESSAGE_SPLITTER);

    // Add suffix
    message.append(MESSAGE_SUFFIX);

    // Send
    sendMessage(message);
}

bool RemoteControl::createRemoveWorkerMessage(uint workerID) {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(REMOVE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    message.append(QString::number(workerID));
    message.append(MESSAGE_SPLITTER);

    // Create nonce
    uint64_t nonce = createNonce();
    message.append(QString::number(nonce));
    message.append(MESSAGE_SPLITTER);

    // Create signature
    message.append(createSignature(message, privateKey));
    message.append(MESSAGE_SPLITTER);

    // Add suffix
    message.append(MESSAGE_SUFFIX);

    // Send
    sendMessage(message);

    return true; // TODO: when is this false?
}

bool RemoteControl::parseHelloMessage() {

    // After a hello message, the sender can be authenticated and will receive updates from the server

    // TODO
    if(true) {
        this->authenticated = true;
        emit isAuthenticated(authenticated);
        return true;
    }

    return false;
}

uint64_t RemoteControl::createNonce() {

    return static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
}

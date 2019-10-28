#include "remoteControl.h"

#include "common.h"

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

RemoteControl::RemoteControl() = default;

bool RemoteControl::verifySignature(QString message, QString nonce, QString signature) {

    bool ok = false;
    uint64_t currentNonce = static_cast<uint64_t>(nonce.toULongLong(&ok));

    // Check if nonce is valid
    if(!ok) { return false; }
    if(currentNonce > lastNonce) {
        lastNonce = currentNonce;
    } else {
        return false;
    }

    QString createdSignature = createSignature(message, privateKey);

    if(createdSignature != signature) {
        return false;
    }

    return true;
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

    // Remove prefix & suffix

    // Split message into components
    QStringList messageComponents = message.split(MESSAGE_SPLITTER);

    // Check number of message components is correct
    if(messageComponents.length() != 6) {
        return false;
    }

    // First verify signature
    QString nonce     = messageComponents.at(3);
    QString signature = messageComponents.at(4);
    if(verifySignature(messageComponents.mid(0,4).join(MESSAGE_SPLITTER).append(MESSAGE_SPLITTER), nonce, signature)) {
        *verified = true;
    } else {
        *verified = false;
        return true;
    }

    // Next send message to correct parser
    bool parseResult = false;

    QString messageCommand = messageComponents.first();

    if (messageCommand == HELLO_MESSAGE) {

        parseResult = parseHelloMessage();
    } else if (messageCommand == CREATE_WORKER_MESSAGE) {

        parseResult = parseCreateWorkerMessage();
    } else if (messageCommand == REMOVE_WORKER_MESSAGE) {

        parseResult = parseRemoveWorkerMessage();
    }

    return parseResult;
}

bool RemoteControl::parseHelloMessage() {

    // After a hello message, the sender can be authenticated and will receive updates from the server

    // Send response to client
    createHelloMessage();

    return true;
}

bool RemoteControl::parseCreateWorkerMessage() {

    return false;
}

bool RemoteControl::parseRemoveWorkerMessage() {

    return false;
}

void RemoteControl::createHelloMessage() {
}

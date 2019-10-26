#include "remoteControl.h"

#include "common.h"

RemoteControl::RemoteControl() = default;

bool RemoteControl::open() {

    return false;
}

bool RemoteControl::verifySignature(QString message, QString nonce, QString signature) {

    (void) message;
    (void) nonce;
    (void) signature;

    return false;
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
    // TODO: check number of components

    // First verify signature
    QString nonce = messageComponents.at(messageComponents.length() -2);
    QString signature = messageComponents.last();
    if(verifySignature(message, nonce, signature)) {
        *verified = true;
    } else {
        *verified = false;
        return true;
    }

    // Next send message to correct parser
    bool parseResult = false;

    QString messageCommand = messageComponents.first();

    if (messageCommand == HELLO_MESSAGE) {

        parseHelloMessage();
    } else if (messageCommand == CREATE_WORKER_MESSAGE) {

        parseCreateWorkerMessage();
    } else if (messageCommand == REMOVE_WORKER_MESSAGE) {

        parseRemoveWorkerMessage();
    }

    return parseResult;
}

bool RemoteControl::parseHelloMessage() {

    // After a hello message, the sender can be authenticated and will receive updates from the server

    // TODO: send response to client
    return false;
}

bool RemoteControl::parseCreateWorkerMessage() {

    return false;
}

bool RemoteControl::parseRemoveWorkerMessage() {

    return false;
}

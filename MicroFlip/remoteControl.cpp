#include "remoteControl.h"

#include "common.h"

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

// Returns true if message is valid
// Verified is true when signiture is valid
// Message can be valid but signature invalid
// If message is invalid, verified should be ignored
bool RemoteControl::parseNewMessage(QString message, bool *verified) {

    // Message components:
    // PREFIX:COMMAND:COMMAND_COMPONENTS:NONCE:SIGNATURE:SUFFIX

    // Split message into components
    QStringList messageComponents = message.split(MESSAGE_SPLITTER);

    // Check number of message components is correct
    if(messageComponents.length() != MESSAGE_COMPONENTS) {
        return false;
    }

    // First verify signature
    QString nonce     = messageComponents.at(NONCE_POSITION);
    QString signature = messageComponents.at(SIGNATURE_POSITION);
    if(verifySignature(messageComponents.mid(0,SIGNATURE_POSITION).join(MESSAGE_SPLITTER).append(MESSAGE_SPLITTER), nonce, signature)) {
        *verified = true;
    } else {
        *verified = false;
        return true;
    }

    // Extract payload
    QString payload = messageComponents.at(PAYLOAD_POSITION);

    // Next send payload to correct parser
    bool parseResult = false;

    QString messageCommand = messageComponents.first();

    if (messageCommand == HELLO_MESSAGE) {

        parseResult = parseHelloMessage();
    } else if (messageCommand == CREATE_WORKER_MESSAGE) {

        parseResult = parseCreateWorkerMessage(payload);
    } else if (messageCommand == REMOVE_WORKER_MESSAGE) {

        parseResult = parseRemoveWorkerMessage(payload);
    }

    return parseResult;
}

bool RemoteControl::parseHelloMessage() {

    // After a hello message, the sender can be authenticated and will receive updates from the server

    // Send response to client
    createHelloMessage();

    return true;
}

bool RemoteControl::parseCreateWorkerMessage(QString message) {

    (void) message;

    return false;
}

bool RemoteControl::parseRemoveWorkerMessage(QString message) {

    (void) message;

    return false;
}

void RemoteControl::createHelloMessage() {

    QByteArray message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(HELLO_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Create payload
    message.append(MESSAGE_SPLITTER);

    // Create nonce
    uint64_t nonce = createNonce();
    message.append(QString::number(nonce));
    message.append(MESSAGE_SPLITTER);

    // Create signature
    message.append(createSignature(message, serverKey));
    message.append(MESSAGE_SPLITTER);

    // Add suffix
    message.append(MESSAGE_SUFFIX);

    // Send
    sendMessage(message);
}

void RemoteControl::logUpdate(int workID, QString className, QString log, int severity) {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(REMOVE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    message.append(QString::number(workID));
    message.append(PAYLOAD_SPLITTER);
    message.append(className);
    message.append(PAYLOAD_SPLITTER);
    message.append(log);
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(severity));

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

void RemoteControl::workorderStateUpdate(int workID, QString state) {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(REMOVE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    message.append(QString::number(workID));
    message.append(PAYLOAD_SPLITTER);
    message.append(state);

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

void RemoteControl::exchangePricesUpdate(QString symbol, double lastPrice, double avgPrice) {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(REMOVE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    message.append(symbol);
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(lastPrice));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(avgPrice));

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

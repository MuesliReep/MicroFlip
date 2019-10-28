#include "remotecontrol.h"

#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

#include "../../../MicroFlip/common.h"

RemoteControl::RemoteControl(Config config) { (void) config; }

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
    } else if (messageCommand == LOG_UPDATE_MESSAGE) {
        parseResult = parseLogUpdateMessage(payload);
    } else if (messageCommand == WORKORDER_UPDATE_MESSAGE) {
        parseResult = parseWorkorderUpdateMessage(payload);
    } else if (messageCommand == EXCHANGE_PRICE_UPDATE_MESSAGE) {
        parseResult = parseExchangePriceUpdateMessage(payload);
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

    // Create payload
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

bool RemoteControl::parseLogUpdateMessage(QString message) {

    // Split payload into components
    QStringList payloadComponents = message.split(MESSAGE_SPLITTER);

    return false;
}

bool RemoteControl::parseWorkorderUpdateMessage(QString message) {

    // Split payload into components
    QStringList payloadComponents = message.split(MESSAGE_SPLITTER);

    return false;
}

bool RemoteControl::parseExchangePriceUpdateMessage(QString message) {

    // Split payload into components
    QStringList payloadComponents = message.split(MESSAGE_SPLITTER);

    return false;
}

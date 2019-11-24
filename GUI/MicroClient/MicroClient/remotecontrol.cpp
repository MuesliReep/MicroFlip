#include "remotecontrol.h"

#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <utility>

#include "../../../MicroFlip/common.h"

RemoteControl::RemoteControl(const Config &config) { (void) config; }

int RemoteControl::getRmoteConnectionState() const
{
    return m_remoteConnectionState;
}

void RemoteControl::setRemoteConnectionState(int state) {

    this->m_remoteConnectionState = state;

    emit remoteConnectionStateChanged();
}

bool RemoteControl::verifySignature(const QString& message, const QString& nonce, const QString& signature) {

    bool ok = false;
    auto currentNonce = static_cast<uint64_t>(nonce.toULongLong(&ok));

    // Check if nonce is valid
    if(!ok) {

        emit updateLog(00, className, "Failed to parse Nonce", logSeverity::LOG_INFO);
        return false; }
    if(currentNonce > lastNonce) {
        lastNonce = currentNonce;
    } else {
        emit updateLog(00, className, "Invalid nonce received", logSeverity::LOG_INFO);
        return false;
    }

    QString createdSignature = createSignature(std::move(message), serverKey);

    if(createdSignature != signature) {
        emit updateLog(00, className, "Signature invalid!", logSeverity::LOG_INFO);
        return false;
    }

    return true;
}

uint64_t RemoteControl::createNonce() {

    auto newNonce = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());

    // If alot of messages are sent at once, there wont be enough time between nonces
    // To work around this, just one up the last nonce
    if(newNonce <= ourLastNonce) {
        newNonce = ourLastNonce + 1;
    }

    ourLastNonce = newNonce;

    return newNonce;
}

QByteArray RemoteControl::createSignature(const QString& message, const QString& key) {

    return QMessageAuthenticationCode::hash(message.toUtf8(), key.toUtf8(), QCryptographicHash::Sha256).toHex();
}

// Returns true if message is valid
// Verified is true when signiture is valid
// Message can be valid but signature invalid
// If message is invalid, verified should be ignored
bool RemoteControl::parseNewMessage(const QString& message, bool *verified) {

    // Message components:
    // PREFIX:COMMAND:COMMAND_COMPONENTS:NONCE:SIGNATURE:SUFFIX

    // Split message into components
    QStringList messageComponents = message.split(MESSAGE_SPLITTER);

    // Check number of message components is correct
    if(messageComponents.length() != MESSAGE_COMPONENTS) {
        return false;
    }

    // First verify signature
    const QString& nonce     = messageComponents.at(NONCE_POSITION);
    const QString& signature = messageComponents.at(SIGNATURE_POSITION);
    if(verifySignature(messageComponents.mid(0,SIGNATURE_POSITION).join(MESSAGE_SPLITTER).append(MESSAGE_SPLITTER), nonce, signature)) {
        *verified = true;
    } else {
        *verified = false;
        return true;
    }

    // Extract payload
    const QString& payload = messageComponents.at(PAYLOAD_POSITION);

    // Next send payload to correct parser
    bool parseResult = false;

    const QString& messageCommand = messageComponents.at(COMMAND_POSITION);

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

void RemoteControl::createWorkerMessage(int    numWorkers,   const QString& pair,          double amount,
                                        double profitTarget, int     shortInterval, int    longInterval,
                                        int    mode,         bool    singleShot,    double minSellPrice) {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(CREATE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    message.append(QString::number(numWorkers));
    message.append(PAYLOAD_SPLITTER);
    message.append(pair);
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(amount));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(profitTarget));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(shortInterval));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(longInterval));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(mode));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(singleShot));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(minSellPrice));
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

bool RemoteControl::createRemoveWorkerMessage(uint workerID, bool force) {

    QString message;

    // Create header
    message.append(MESSAGE_PREFIX);
    message.append(MESSAGE_SPLITTER);
    message.append(REMOVE_WORKER_MESSAGE);
    message.append(MESSAGE_SPLITTER);

    // Add payload
    message.append(QString::number(workerID));
    message.append(PAYLOAD_SPLITTER);
    message.append(QString::number(force));
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

bool RemoteControl::parseLogUpdateMessage(const QString& message) {

    int     workID;
    QString className;
    QString log;
    int     severity;

    // Split payload into components
    QStringList payloadComponents = message.split(PAYLOAD_SPLITTER);

    int numPayloadComponents = 4;

    // Check number of components is correct
    if (payloadComponents.size() != numPayloadComponents) {
        return false;
    }

    workID    = payloadComponents.at(0).toInt();
    className = payloadComponents.at(1);
    log       = payloadComponents.at(2);
    severity  = payloadComponents.at(3).toInt();

    emit newLogUpdate(workID, className, log, severity);

    return true;
}

bool RemoteControl::parseWorkorderUpdateMessage(const QString& message) {

    int     workID;
    QString state;

    // Split payload into components
    QStringList payloadComponents = message.split(PAYLOAD_SPLITTER);

    int numPayloadComponents = 2;

    // Check number of components is correct
    if (payloadComponents.size() != numPayloadComponents) {
        return false;
    }

    workID = payloadComponents.at(0).toInt();
    state  = payloadComponents.at(1);

    emit newWorkerStatus(workID, state);

    return true;
}

bool RemoteControl::parseExchangePriceUpdateMessage(const QString& message) {

    QString symbol;
    double  lastPrice;
    double  avgPrice;

    // Split payload into components
    QStringList payloadComponents = message.split(PAYLOAD_SPLITTER);

    int numPayloadComponents = 3;

    // Check number of components is correct
    if (payloadComponents.size() != numPayloadComponents) {
        return false;
    }

    symbol    = payloadComponents.at(0);
    avgPrice  = payloadComponents.at(1).toDouble();
    lastPrice = payloadComponents.at(2).toDouble();

    emit newExchangeInformation(symbol, avgPrice, lastPrice);

    return true;
}

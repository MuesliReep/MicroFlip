#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

enum logSeverity { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_CRITICAL, LOG_FATAL };
enum workerMode  { MINSELL, TICKERAVG };
enum WorkerState { ERROR = -1, INITIALISE, START, WAITINGFORTICKER, CREATESELL, WAITINGFORSELL, SELLORDER, SOLD, CREATEBUY, WAITINGFORBUY, BUYORDER, COMPLETE };

// Message components:
// PREFIX:COMMAND:COMMAND_COMPONENTS:NONCE:SIGNATURE:SUFFIX

static const QString MESSAGE_PREFIX   = "FLIP";
static const QString MESSAGE_SUFFIX   = "FLOP";
static const QString MESSAGE_SPLITTER = "|";

static const QString HELLO_MESSAGE         = "HELLO";
static const QString CREATE_WORKER_MESSAGE = "CREATE";
static const QString REMOVE_WORKER_MESSAGE = "REMOVE";

static const QString PAYLOAD_SPLITTER              = ";";
static const QString LOG_UPDATE_MESSAGE            = "LOG";
static const QString WORKORDER_UPDATE_MESSAGE      = "WORKORDER";
static const QString EXCHANGE_PRICE_UPDATE_MESSAGE = "EXCHPRICE";

static const uint8_t COMMAND_POSITION   = 1;
static const uint8_t PAYLOAD_POSITION   = 2;
static const uint8_t NONCE_POSITION     = 3;
static const uint8_t SIGNATURE_POSITION = 4;
static const uint8_t MESSAGE_COMPONENTS = 6;

static uint64_t createNonce() {

    return static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
}

static QByteArray createSignature(QString message, QString key) {

    return QMessageAuthenticationCode::hash(message.toUtf8(), key.toUtf8(), QCryptographicHash::Sha256).toHex();
}

#endif // COMMON_H

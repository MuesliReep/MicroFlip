#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

enum logSeverity { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_CRITICAL, LOG_FATAL };
enum workerMode  { MINSELL, TICKERAVG };
enum WorkerState { ERROR = -1, INITIALISE, START, WAITINGFORTICKER, CREATESELL, WAITINGFORSELL, SELLORDER, SOLD, CREATEBUY, WAITINGFORBUY, BUYORDER, COMPLETE , REMOVING, REMOVED};

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

//static uint64_t createNonce() {

//    return static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
//}

static QByteArray createSignature(QString message, QString key) {

    return QMessageAuthenticationCode::hash(message.toUtf8(), key.toUtf8(), QCryptographicHash::Sha256).toHex();
}

static const QString ERROR_STRING            = "ERROR";
static const QString INITIALISE_STRING       = "INITIALISE";
static const QString START_STRING            = "START";
static const QString WAITINGFORTICKER_STRING = "WAITINGFORTICKER";
static const QString CREATESELL_STRING       = "CREATESELL";
static const QString WAITINGFORSELL_STRING   = "WAITINGFORSELL";
static const QString SELLORDER_STRING        = "SELLORDER";
static const QString SOLD_STRING             = "SOLD";
static const QString CREATEBUY_STRING        = "CREATEBUY";
static const QString WAITINGFORBUY_STRING    = "WAITINGFORBUY";
static const QString BUYORDER_STRING         = "BUYORDER";
static const QString COMPLETE_STRING         = "COMPLETE";
static const QString REMOVING_STRING         = "REMOVING";
static const QString REMOVED_STRING          = "REMOVED";

#endif // COMMON_H

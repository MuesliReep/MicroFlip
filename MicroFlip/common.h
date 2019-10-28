#ifndef COMMON_H
#define COMMON_H

#include <QString>

enum logSeverity { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_CRITICAL, LOG_FATAL };
enum workerMode  { MINSELL, TICKERAVG };

static const QString MESSAGE_PREFIX   = "FLIP";
static const QString MESSAGE_SUFFIX   = "FLOP";
static const QString MESSAGE_SPLITTER = ":";

static const QString HELLO_MESSAGE         = "HELLO";
static const QString CREATE_WORKER_MESSAGE = "CREATE";
static const QString REMOVE_WORKER_MESSAGE = "REMOVE";

#endif // COMMON_H

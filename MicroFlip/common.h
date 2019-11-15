#ifndef COMMON_H
#define COMMON_H

#include <QString>

enum logSeverity { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_CRITICAL, LOG_FATAL };
enum workerMode  { MINSELL, TICKERAVG };

enum WorkState { ERROR = -1, INITIALISE, START, WAITINGFORTICKER, CREATESELL, WAITINGFORSELL, SELLORDER, SOLD, CREATEBUY, WAITINGFORBUY, BUYORDER, COMPLETE , REMOVING, REMOVED};

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

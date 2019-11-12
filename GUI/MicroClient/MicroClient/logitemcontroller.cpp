#include "logitemcontroller.h"

logItemController::logItemController(int workID, QString className, QString log,
                                     int severity, QObject *parent) : QObject(parent) {

    this->workID    = workID;
    this->className = className;
    this->log       = log;
    this->severity  = severity;
}

void logItemController::setLogTime(QString logTime) {

    this->logTime = logTime;
}

void logItemController::setWorkID(int workID) {

    this->workID = workID;
}

void logItemController::setClassName(QString className) {

    this->className = className;
}

void logItemController::setLog(QString log) {

    this->log = log;
}

void logItemController::setSeverity(int severity) {

    this->severity = severity;
}

QString logItemController::LogTime   () { return logTime;   }
int     logItemController::WorkID    () { return workID;    }
QString logItemController::ClassName () { return className; }
QString logItemController::Log       () { return log;       }
int     logItemController::Severity  () { return severity;  }

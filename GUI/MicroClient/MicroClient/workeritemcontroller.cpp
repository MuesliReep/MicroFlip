#include "workeritemcontroller.h"

workerItemController::workerItemController(int workerID, QString workerStatus, QObject *parent) : QObject(parent) {

    this->id     = workerID;
    this->status = workerStatus;
}

void workerItemController::setWorkerID(int workerID) {

    this->id = workerID;
}

void workerItemController::setWorkerStatus(QString workerStatus) {

    this->status = workerStatus;
}

int     workerItemController::workerID     () { return id;     }
QString workerItemController::workerStatus () { return status; }

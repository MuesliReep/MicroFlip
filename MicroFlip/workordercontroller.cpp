#include "workordercontroller.h"

WorkOrderController::WorkOrderController(QObject *parent) : QObject(parent) {

}

///
/// \brief Program::workOrderFactory Creates a workorder and adds it to the list of workorders
/// \param numWorkers The amount of workers to create
/// \param exchange Pointer to the exchange interface
/// \param amount The amount of currency to trade with
/// \param profit The profit target this worker will aim for
/// \param pair The currenct pair, example: btc_usd
/// \param minSell Sets a static minimum sell price. To use a dynamic price, set to a negative number
/// \return
///
bool WorkOrderController::factory(int    numWorkers,   Exchange *exchange,  double amount,
                                  double profit,       const QString& pair, int    shortInterval,
                                  int    longInterval, int mode,            bool   singleShot,
                                  double minSell) {

    emit updateLog(00, className, "User requested " + QString::number(numWorkers) + " work order(s)", logSeverity::LOG_INFO);

    for(int i = 0; i < numWorkers; i++) {

        workOrderIterator++;

        emit updateLog(00, className, "Creating Work Order: " + QString::number(i+1) + " with currency: " + pair, logSeverity::LOG_DEBUG);
        WorkOrder *newWorkOrder = new WorkOrder(exchange, workOrderIterator,pair,amount,profit, shortInterval, longInterval, mode, singleShot, minSell);

        // Create thread for each workorder
        auto *workOrderThread = new QThread();
        newWorkOrder->moveToThread(workOrderThread);

        //
        connect(newWorkOrder, &WorkOrder::updateLog  , this, &WorkOrderController::updateLog);
        connect(newWorkOrder, &WorkOrder::updateState, this, &WorkOrderController::updateState);

        // Tell the newly created work order to start
        connect(this, &WorkOrderController::startOrder, newWorkOrder, &WorkOrder::startOrder);
        workOrderThread->start();
        emit startOrder();
        disconnect(this, &WorkOrderController::startOrder, newWorkOrder, &WorkOrder::startOrder);

        // Store the workorder and its thread
        workOrders.append(newWorkOrder);
        workOrderThreads.append(workOrderThread);
        QThread::sleep(1);
    }

    return true;
}

bool WorkOrderController::remove(int workID, bool force) {

    bool result = false;

    for (int i = 0; i < workOrders.length(); i++) {

        if(workOrders.at(i)->getWorkID() == workID) {

            if(force) {

//                if(workOrderThreads.at(i)->exit()) { }
            } else {

                QMetaObject::invokeMethod(workOrders.at(i), "stopOrder");
                result = true;
            }
        }
    }

    if(!result) {
        emit updateLog(00, className, "Could not remove WorkOrder " + QString::number(workID) + ", it was not found!", logSeverity::LOG_INFO);
    }

    return result;
}

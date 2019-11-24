#ifndef WORKORDERCONTROLLER_H
#define WORKORDERCONTROLLER_H

#include <QObject>

#include "workorder.h"

class WorkOrderController : public QObject {

    Q_OBJECT

public:
    explicit WorkOrderController(QObject *parent = nullptr);

    bool factory(int numWorkers,      Exchange *exchange, double amount,    double profit,
                 const QString& pair, int shortInterval,  int longInterval, int mode,
                 bool singleShot,     double minSell = -1.0);

    bool remove(int workID, bool force = false);

private:
    QString           className         {"PROGRAM"};
    uint64_t          workOrderIterator {0};
    QList<WorkOrder*> workOrders;
    QList<QThread*>   workOrderThreads;

signals:
    void updateLog  (int workID, QString className, QString log, int severity);
    void updateState(int workID, QString state);

    void startOrder();
};

#endif // WORKORDERCONTROLLER_H

#ifndef WORKERITEMCONTROLLER_H
#define WORKERITEMCONTROLLER_H

#include <QObject>

class workerItemController : public QObject {

    Q_OBJECT

    Q_PROPERTY(int     workerID     READ workerID     WRITE setWorkerID     NOTIFY workerIDChanged)
    Q_PROPERTY(QString workerStatus READ workerStatus WRITE setWorkerStatus NOTIFY workerStatusChanged)

public:
    explicit workerItemController (int workerID, QString workerStatus, QObject *parent = nullptr);

    int      workerID             ();
    QString  workerStatus         ();

    void     setWorkerID          (int     workerID);
    void     setWorkerStatus      (QString workerStatus);

private:
    int      id                   {};
    QString  status               {};

signals:
    void     workerIDChanged      ();
    void     workerStatusChanged  ();

};

#endif // WORKERITEMCONTROLLER_H

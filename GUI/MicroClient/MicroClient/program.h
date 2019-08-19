#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>

#include "remotecontrol.h"


class Program : public QObject
{
    Q_OBJECT
public:
    explicit Program(QObject *parent = nullptr);

private:
    RemoteControl *remoteControl;

    void startUp();

signals:

public slots:

private slots:
};

#endif // PROGRAM_H

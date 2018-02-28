#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>

#include "../MicroFlip/config.h"

class Program : public QObject
{
    Q_OBJECT
public:
    explicit Program(QObject *parent = nullptr);
    ~Program();

private:
    Config *config;

signals:

public slots:
};

#endif // PROGRAM_H

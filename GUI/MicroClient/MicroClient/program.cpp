#include "program.h"

#include <QTimer>

#include "remotecontrol_tcp.h"
#include "config.h"

Program::Program(QObject *parent) : QObject(parent) {

    // Load config
    Config config;
    config.loadConfigFromFile();

    // Create Remote Control
    remoteControl = new RemoteControl_TCP(config);

    // Create start shot, this is called when the main event loop is triggered
    QTimer::singleShot(1, remoteControl, &RemoteControl::open);
}

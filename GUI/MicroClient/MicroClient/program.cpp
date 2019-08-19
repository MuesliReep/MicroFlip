#include "program.h"

#include "remotecontrol_tcp.h"
#include "config.h"

Program::Program(QObject *parent) : QObject(parent) {

    // Load config
    Config config;
    config.loadConfigFromFile();

    // Create Remote Control
    remoteControl = new RemoteControl_TCP();

    // Create start shot
}

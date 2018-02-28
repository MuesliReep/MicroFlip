#include "program.h"

#include "../MicroFlip/exchange.h"
#include "../MicroFlip/exchange_bitfinex.h"

#include "worker.h"

Program::Program(QObject *parent) : QObject(parent)
{

    // Load config
    config = new Config();
    config->loadConfigFromFile();

    // Create an exchange interface
    Exchange *exchange = new Exchange_bitfinex();
    exchange->setConfig(config);
    exchange->startWork();

    Worker *w = new Worker(exchange, "xrpusd");
}

Program::~Program()
{
    delete this->config;
}

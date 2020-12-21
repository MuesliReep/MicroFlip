#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T15:00:37
#
#-------------------------------------------------

QT       += core
QT       -= gui
QT       += network

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

lessThan(QT_MAJOR_VERSION, 6): error("requires Qt 6")

TARGET = MicroFlip

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp\
    program.cpp \
    workorder.cpp \
    downloader.cpp \
    marketData.cpp \
#    marketHistory.cpp \
    config.cpp \
    order.cpp \
    trade.cpp \
    display.cpp \
    exchange.cpp \
#    exchange_btce.cpp \
    control.cpp \
#    exchange_okcoin.cpp \
    exchange_wex.cpp \
    exchange_bitstamp.cpp \
    exchange_bitfinex.cpp \
    exchange_sim.cpp \
    exchange_binance.cpp \
    json_helper.cpp \
    workordercontroller.cpp

HEADERS  += program.h \
    workorder.h \
    downloader.h \
    marketData.h \
#    marketHistory.h \
    config.h \
    order.h \
    trade.h \
    display.h \
    exchange.h \
#    exchange_btce.h \
    control.h \
#    exchange_okcoin.h \
    exchange_wex.h \
    exchange_bitstamp.h \
    exchange_bitfinex.h \
    exchange_sim.h \
    exchange_binance.h \
    json_helper.h \
    common.h \
    workordercontroller.h

CONFIG += console

win32:DEFINES+=ISWIN
win32:LIBS+=-LC:\Qt\Tools\mingw730_64\opt\bin

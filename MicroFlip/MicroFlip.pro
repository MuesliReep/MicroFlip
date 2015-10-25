#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T15:00:37
#
#-------------------------------------------------

QT       += core #gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MicroFlip
TEMPLATE = app


SOURCES += main.cpp\
       # mainwindow.cpp \
    program.cpp \
    workorder.cpp \
    downloader.cpp \
    marketData.cpp \
    marketHistory.cpp \
    config.cpp \
    order.cpp \
    trade.cpp \
    display.cpp \
    exchange.cpp \
    exchange_btce.cpp

HEADERS  += program.h \
    #mainwindow.h \
    workorder.h \
    downloader.h \
    marketData.h \
    marketHistory.h \
    config.h \
    order.h \
    trade.h \
    display.h \
    exchange.h \
    exchange_btce.h

#FORMS    += mainwindow.ui

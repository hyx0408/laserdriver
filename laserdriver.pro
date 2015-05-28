#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T14:39:54
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = laserdriver
TEMPLATE = app


SOURCES += main.cpp\
        laserserialport.cpp

HEADERS  += laserserialport.h

FORMS    += laserserialport.ui

RESOURCES += \
    icon.qrc

RC_ICONS = topic.ico

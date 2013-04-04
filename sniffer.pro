#-------------------------------------------------
#
# Project created by QtCreator 2013-03-22T16:18:46
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sniffer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qcustomplot.cpp

HEADERS  += mainwindow.h qcustomplot.h

FORMS    += mainwindow.ui
include(qextserialport/src/qextserialport.pri)

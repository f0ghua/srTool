#-------------------------------------------------
#
# Project created by QtCreator 2016-09-09T13:59:02
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = srTool
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    SrecFile.cpp \
    Utility.cpp \
    HeaderFile.cpp

HEADERS  += mainwindow.h \
    SrecFile.h \
    Utility.h \
    HeaderFile.h

FORMS    += mainwindow.ui

RESOURCES += \
    sctool.qrc

RC_FILE = scTool.rc

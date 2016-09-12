#-------------------------------------------------
#
# Project created by QtCreator 2016-09-09T13:59:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = srTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SrecFile.cpp \
    AppHeaderFile.cpp \
    Utility.cpp

HEADERS  += mainwindow.h \
    SrecFile.h \
    AppHeaderFile.h \
    Utility.h

FORMS    += mainwindow.ui

RESOURCES += \
    sctool.qrc

RC_FILE = scTool.rc

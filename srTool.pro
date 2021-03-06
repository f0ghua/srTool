#-------------------------------------------------
#
# Project created by QtCreator 2016-09-09T13:59:02
#
#-------------------------------------------------

QT       += core gui
#CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(debug, debug|release) {
	DEFINES +=
} else {
	DEFINES += F_NO_DEBUG
}

TARGET = srTool
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    SrecFile.cpp \
    Utility.cpp \
    HeaderFile.cpp \
    BinFile.cpp \
    IHexFile.cpp \
    AboutDialog.cpp \
    ConfigDialog.cpp \
    SrConfig.cpp

HEADERS  += mainwindow.h \
    SrecFile.h \
    Utility.h \
    HeaderFile.h \
    BinFile.h \
    IHexFile.h \
    AboutDialog.h \
    ConfigDialog.h \
    SrConfig.h

FORMS    += mainwindow.ui \
    ConfigDialog.ui

RESOURCES += \
    sctool.qrc

RC_FILE = scTool.rc

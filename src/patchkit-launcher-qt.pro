#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T15:22:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = patchkit-launcher-qt
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    launcher.cpp \
    launcherdata.cpp \
    launcherexception.cpp \
    patchkitremotepatcher.cpp \
    launcherthread.cpp \
    patchkitlocalpatcher.cpp \
    launcherconfiguration.cpp

HEADERS  += mainwindow.h \
    launcher.h \
    launcherconfiguration.h \
    launcherdata.h \
    launcherexception.h \
    patchkitremotepatcher.h \
    remotepatcher.h \
    launcherthread.h \
    localpatcher.h \
    patchkitlocalpatcher.h

FORMS    += mainwindow.ui

#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T15:22:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = patchkit-launcher-qt
TEMPLATE = app

LIBS += -LC:\Qt\quazip-0.7.2\vs_proj_64\Release -lquazip_static

INCLUDEPATH += C:\Qt\quazip-0.7.2\quazip

SOURCES += main.cpp\
        mainwindow.cpp \
    launcher.cpp \
    launcherdata.cpp \
    launcherexception.cpp \
    launchercancelledexception.cpp \
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
    patchkitlocalpatcher.h \
    launchercancelledexception.h

FORMS    += mainwindow.ui

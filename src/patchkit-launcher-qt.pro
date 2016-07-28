#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T15:22:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = patchkit-launcher-qt
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    launcher.cpp \
    launcherdata.cpp

HEADERS  += mainwindow.h \
    ilauncherwindow.h \
    launcher.h \
    launcherconfiguration.h \
    launcherdata.h

FORMS    += mainwindow.ui

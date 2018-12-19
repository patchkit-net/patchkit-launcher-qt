QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launcher
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES     += \
    $$PWD/src/launcher.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/mainwindow.cpp \
    src/interface.cpp

HEADERS     += \
    $$PWD/src/launcher.h \
    $$PWD/src/mainwindow.h \
    src/interface.h

FORMS       += $$PWD/src/mainwindow.ui

include(../default.pri)
include(../link_libz_and_quazip.pri)
include(../version.pri)
include(../link_core.pri)

QT += core gui network
include(../default.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launcher
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES     += \
    $$PWD/src/launcher.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/mainwindow.cpp \

HEADERS     += \
    $$PWD/src/launcher.h \
    $$PWD/src/mainwindow.h \

FORMS       += $$PWD/src/mainwindow.ui

include(../link_static.pri)

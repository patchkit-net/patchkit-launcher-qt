QT += core gui network
include(../default.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launcher
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES     += $$PWD/src/*.cpp
HEADERS     += $$PWD/src/*.h

FORMS       += $$PWD/src/*.ui

include(../link_static.pri)

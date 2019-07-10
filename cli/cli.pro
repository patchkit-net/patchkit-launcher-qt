TARGET = LauncherCli

QT += core network widgets
QT -= gui

CONFIG += console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/interface.cpp

HEADERS += \
    interface.h

include($$PWD/../default.pri)
include($$PWD/../version.pri)
include($$PWD/../link_core.pri)
include($$PWD/../link_libz_and_quazip.pri)

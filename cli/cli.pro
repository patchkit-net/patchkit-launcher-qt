QT -= gui
include(../default.pri)

TARGET = LauncherCli

CONFIG += console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        $$PWD/main.cpp

include($$PWD/../version.pri)
include($$PWD/../link_libz_and_quazip.pri)

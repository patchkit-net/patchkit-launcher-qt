QT -= gui
include(../default.pri)

TARGET = LauncherCli

CONFIG += console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp

include(../version.pri)
include(../link_libz_and_quazip.pri)

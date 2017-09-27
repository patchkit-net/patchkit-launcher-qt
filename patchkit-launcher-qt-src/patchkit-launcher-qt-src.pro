QT += core network
include(../default.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LauncherLib
TEMPLATE = lib
CONFIG += staticlib

DEFINES += GIT_VERSION=\\\"$$system(git describe --always --tags)\\\"

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/chunkeddownloadstrategy.cpp \
    $$PWD/src/tempdir.cpp \
    $$PWD/src/hashingstrategy.cpp \
    $$PWD/src/defaultdownloadstrategy.cpp \
    $$PWD/src/chunkeddownloader.cpp \
    $$PWD/src/remotepatcherdata.cpp \
    $$PWD/src/downloader.cpp \
    $$PWD/src/config.cpp \
    $$PWD/src/data.cpp \
    $$PWD/src/downloaderoperator.cpp \
    $$PWD/src/logger.cpp \
    $$PWD/src/ioutils.cpp \
    $$PWD/src/basedownloadstrategy.cpp \
    $$PWD/src/localpatcherdata.cpp \
    $$PWD/src/launcherworker.cpp \
    $$PWD/src/api.cpp \
    $$PWD/src/contentsummary.cpp \
    $$PWD/src/locations.cpp \
    $$PWD/src/executableresources.cpp \
    $$PWD/src/urlprovider.cpp \
    $$PWD/src/launcherstate.cpp \
    $$PWD/src/xxhash.c \

HEADERS += \
    $$PWD/src/launcherstate.h \
    $$PWD/src/api.h \
    $$PWD/src/idownloadstrategy.h \
    $$PWD/src/defaultdownloadstrategy.h \
    $$PWD/src/downloader.h \
    $$PWD/src/data.h \
    $$PWD/src/downloaderoperator.h \
    $$PWD/src/basedownloadstrategy.h \
    $$PWD/src/urlprovider.h \
    $$PWD/src/xxhash.h \
    $$PWD/src/tempdir.h \
    $$PWD/src/cancellationtokensource.h \
    $$PWD/src/locations.h \
    $$PWD/src/chunkeddownloader.h \
    $$PWD/src/ioutils.h \
    $$PWD/src/iapi.h \
    $$PWD/src/launcherworker.h \
    $$PWD/src/logger.h \
    $$PWD/src/chunkeddownloadstrategy.h \
    $$PWD/src/cancellationtoken.h \
    $$PWD/src/contentsummary.h \
    $$PWD/src/hashingstrategy.h \
    $$PWD/src/localpatcherdata.h \
    $$PWD/src/config.h \
    $$PWD/src/remotepatcherdata.h \
    $$PWD/src/customexceptions.h \
    $$PWD/src/executableresources.h \

QT += core network
include(../default.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LauncherLib
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/src/

include(../version.pri)

HEADERS += \
    $$PWD/src/api.h \
    $$PWD/src/basedownloadstrategy.h \
    $$PWD/src/cancellationtoken.h \
    $$PWD/src/cancellationtokensource.h \
    $$PWD/src/chunkeddownloader.h \
    $$PWD/src/chunkeddownloadstrategy.h \
    $$PWD/src/config.h \
    $$PWD/src/contentsummary.h \
    $$PWD/src/customexceptions.h \
    $$PWD/src/data.h \
    $$PWD/src/defaultdownloadstrategy.h \
    $$PWD/src/downloader.h \
    $$PWD/src/downloaderoperator.h \
    $$PWD/src/executableresources.h \
    $$PWD/src/hashingstrategy.h \
    $$PWD/src/iapi.h \
    $$PWD/src/idownloaderpool.h \
    $$PWD/src/idownloadstrategy.h \
    $$PWD/src/ioutils.h \
    $$PWD/src/launcherworker.h \
    $$PWD/src/localpatcherdata.h \
    $$PWD/src/locations.h \
    $$PWD/src/lockfile.h \
    $$PWD/src/logger.h \
    $$PWD/src/patchermanifest.h \
    $$PWD/src/remoteappdata.h \
    $$PWD/src/remotepatcherdata.h \
    $$PWD/src/tempdir.h \
    $$PWD/src/urlprovider.h \
    $$PWD/src/utilities.h \
    $$PWD/src/xxhash.h

SOURCES += \
    $$PWD/src/api.cpp \
    $$PWD/src/basedownloadstrategy.cpp \
    $$PWD/src/chunkeddownloader.cpp \
    $$PWD/src/chunkeddownloadstrategy.cpp \
    $$PWD/src/config.cpp \
    $$PWD/src/contentsummary.cpp \
    $$PWD/src/data.cpp \
    $$PWD/src/defaultdownloadstrategy.cpp \
    $$PWD/src/downloader.cpp \
    $$PWD/src/downloaderoperator.cpp \
    $$PWD/src/executableresources.cpp \
    $$PWD/src/hashingstrategy.cpp \
    $$PWD/src/idownloaderpool.cpp \
    $$PWD/src/ioutils.cpp \
    $$PWD/src/launcherworker.cpp \
    $$PWD/src/localpatcherdata.cpp \
    $$PWD/src/locations.cpp \
    $$PWD/src/lockfile.cpp \
    $$PWD/src/logger.cpp \
    $$PWD/src/patchermanifest.cpp \
    $$PWD/src/remoteappdata.cpp \
    $$PWD/src/remotepatcherdata.cpp \
    $$PWD/src/tempdir.cpp \
    $$PWD/src/urlprovider.cpp \
    $$PWD/src/utilities.cpp \
    $$PWD/src/xxhash.c


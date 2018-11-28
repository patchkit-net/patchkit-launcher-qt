QT += core network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = core
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += src

SOURCES += \
    src/tempdir.cpp \
    src/hashingstrategy.cpp \
    src/config.cpp \
    src/logger.cpp \
    src/ioutils.cpp \
    src/localpatcherdata.cpp \
    src/launcherworker.cpp \
    src/locations.cpp \
    src/executableresources.cpp \
    src/lockfile.cpp \
    src/xxhash.c \
    src/utilities.cpp \
    src/remote/api/api.cpp \
    src/remote/api/apiconnectionsetting.cpp \
    src/remote/downloading/downloadingabstractions.cpp \
    src/remote/downloading/chunked/chunkedbuffer.cpp \
    src/data/contentsummary.cpp \
    src/data/data.cpp \
    src/data/patchermanifest.cpp \
    src/data/installationinfo.cpp \
    src/data/launchersettings.cpp \
    src/data/secretencoding.cpp \
    src/data/downloadrange.cpp \
    src/remote/downloading/chunked/downloader.cpp \
    src/remote/downloading/progressdevice.cpp \
    src/remote/downloading/apiconnectionstrategy.cpp

HEADERS += \
    src/xxhash.h \
    src/tempdir.h \
    src/locations.h \
    src/ioutils.h \
    src/launcherworker.h \
    src/logger.h \
    src/hashingstrategy.h \
    src/localpatcherdata.h \
    src/config.h \
    src/customexceptions.h \
    src/executableresources.h \
    src/lockfile.h \
    src/utilities.h \
    src/data/networkstatus.hpp \
    src/remote/api/iapi.h \
    src/remote/api/api.h \
    src/remote/api/apiconnectionsetting.h \
    src/remote/downloading/downloadingabstractions.h \
    src/remote/downloading/chunked/chunkedbuffer.h \
    src/cancellation/cancellationtoken.h \
    src/cancellation/cancellationtokensource.h \
    src/data/contentsummary.h \
    src/data/data.h \
    src/data/patchermanifest.h \
    src/ilauncher.h \
    src/data/installationinfo.h \
    src/data/launchersettings.h \
    src/data/secretencoding.h \
    src/data/downloadrange.h \
    src/data/launcherresult.h \
    src/remote/downloading/chunked/downloader.h \
    src/remote/downloading/progressdevice.h \
    src/remote/downloading/apiconnectionstrategy.h

include(../default.pri)
include(../version.pri)
include(../link_libz_and_quazip.pri)

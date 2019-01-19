QT += core network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = core
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/tempdir.cpp \
    $$PWD/src/hashingstrategy.cpp \
    $$PWD/src/config.cpp \
    $$PWD/src/logger.cpp \
    $$PWD/src/ioutils.cpp \
    $$PWD/src/localpatcherdata.cpp \
    $$PWD/src/launcherworker.cpp \
    $$PWD/src/locations.cpp \
    $$PWD/src/executableresources.cpp \
    $$PWD/src/lockfile.cpp \
    $$PWD/src/xxhash.c \
    $$PWD/src/utilities.cpp \
    $$PWD/src/remote/api/api.cpp \
    $$PWD/src/remote/api/apiconnectionsetting.cpp \
    $$PWD/src/remote/downloading/downloadingabstractions.cpp \
    $$PWD/src/remote/downloading/chunked/chunkedbuffer.cpp \
    $$PWD/src/data/contentsummary.cpp \
    $$PWD/src/data/data.cpp \
    $$PWD/src/data/patchermanifest.cpp \
    $$PWD/src/data/installationinfo.cpp \
    $$PWD/src/data/launchersettings.cpp \
    $$PWD/src/data/secretencoding.cpp \
    $$PWD/src/data/downloadrange.cpp \
    $$PWD/src/data/networkstatus.cpp \
    $$PWD/src/remote/downloading/chunked/downloader.cpp \
    $$PWD/src/remote/downloading/progressdevice.cpp \
    $$PWD/src/remote/downloading/apiconnectionstrategy.cpp

HEADERS += \
    $$PWD/src/xxhash.h \
    $$PWD/src/tempdir.h \
    $$PWD/src/locations.h \
    $$PWD/src/ioutils.h \
    $$PWD/src/launcherworker.h \
    $$PWD/src/logger.h \
    $$PWD/src/hashingstrategy.h \
    $$PWD/src/localpatcherdata.h \
    $$PWD/src/config.h \
    $$PWD/src/customexceptions.h \
    $$PWD/src/executableresources.h \
    $$PWD/src/lockfile.h \
    $$PWD/src/utilities.h \
    $$PWD/src/data/networkstatus.hpp \
    $$PWD/src/remote/api/iapi.h \
    $$PWD/src/remote/api/api.h \
    $$PWD/src/remote/api/apiconnectionsetting.h \
    $$PWD/src/remote/downloading/downloadingabstractions.h \
    $$PWD/src/remote/downloading/chunked/chunkedbuffer.h \
    $$PWD/src/cancellation/cancellationtoken.h \
    $$PWD/src/cancellation/cancellationtokensource.h \
    $$PWD/src/data/contentsummary.h \
    $$PWD/src/data/data.h \
    $$PWD/src/data/patchermanifest.h \
    $$PWD/src/ilauncher.h \
    $$PWD/src/data/installationinfo.h \
    $$PWD/src/data/launchersettings.h \
    $$PWD/src/data/secretencoding.h \
    $$PWD/src/data/downloadrange.h \
    $$PWD/src/data/launcherresult.h \
    $$PWD/src/remote/downloading/chunked/downloader.h \
    $$PWD/src/remote/downloading/progressdevice.h \
    $$PWD/src/remote/downloading/apiconnectionstrategy.h

include($$PWD/../default.pri)
include($$PWD/../version.pri)
include($$PWD/../link_libz_and_quazip.pri)

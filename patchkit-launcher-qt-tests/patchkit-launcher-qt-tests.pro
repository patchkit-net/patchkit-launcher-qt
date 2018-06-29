QT += core network testlib
include(../default.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LauncherTests
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/chunkeddownloadertests.cpp \
    $$PWD/src/contentsummarytests.cpp \
    $$PWD/src/downloaderpooltests.cpp \
    $$PWD/src/downloadertests.cpp \
    $$PWD/src/downloadoperatortests.cpp \
    $$PWD/src/hashingstrategiestests.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/patchermanifesttests.cpp \
    $$PWD/src/mocks/nam.cpp \
    $$PWD/src/mocks/reply.cpp\
    $$PWD/src/mocks/api.cpp \
    src/apitests.cpp

HEADERS += \
    $$PWD/src/catch.h \
    $$PWD/src/custommacros.h \
    $$PWD/src/mocks/nam.h \
    $$PWD/src/mocks/reply.h \
    $$PWD/src/mocks/api.h

include(../link_static.pri)

include($$PWD/../link_core.pri)
include($$PWD/../default.pri)
include($$PWD/../link_libz_and_quazip.pri)
include($$PWD/../version.pri)

QT += core network testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LauncherTests
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/contentsummarytests.cpp \
    $$PWD/src/hashingstrategiestests.cpp \
    $$PWD/src/mockednam.cpp \
    $$PWD/src/mockedreply.cpp\
    $$PWD/src/main.cpp \
    $$PWD/src/patchermanifesttests.cpp \
    $$PWD/src/chunkedbuffertests.cpp \
    $$PWD/src/downloadingabstractionstests.cpp \
    $$PWD/src/secretencodingtests.cpp \
    $$PWD/src/installationinfotests.cpp \
    $$PWD/src/sink.cpp

HEADERS += \
    $$PWD/src/catch.h \
    $$PWD/src/custommacros.h \
    $$PWD/src/mockednam.h \
    $$PWD/src/mockedreply.h \
    $$PWD/src/sink.h

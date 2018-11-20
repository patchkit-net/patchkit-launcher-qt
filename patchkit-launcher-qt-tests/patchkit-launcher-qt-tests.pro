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
    $$PWD/src/mockednam.cpp \
    $$PWD/src/mockedreply.cpp\
    $$PWD/src/main.cpp \
    $$PWD/src/patchermanifesttests.cpp

HEADERS += \
    $$PWD/src/catch.h \
    $$PWD/src/custommacros.h \
    $$PWD/src/mockednam.h \
    $$PWD/src/mockedreply.h \

include(../link_static.pri)

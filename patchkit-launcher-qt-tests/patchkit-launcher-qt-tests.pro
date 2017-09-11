QT += core network testlib
include(../default.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LauncherTests
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES     += $$PWD/src/*.cpp
HEADERS     += $$PWD/src/*.h

include(../link_static.pri)

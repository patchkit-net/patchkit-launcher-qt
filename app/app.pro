QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launcher
TEMPLATE = app

INCLUDEPATH += $$PWD/src

SOURCES     += \
    $$PWD/src/launcher.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/interface.cpp

HEADERS     += \
    $$PWD/src/launcher.h \
    $$PWD/src/mainwindow.h \
    $$PWD/src/interface.h

FORMS       += $$PWD/src/mainwindow.ui

include($$PWD/../default.pri)
include($$PWD/../version.pri)
include($$PWD/../link_core.pri)
include($$PWD/../link_libz_and_quazip.pri)

#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T15:22:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launcher
TEMPLATE = app

DEFINES += QUAZIP_STATIC

INCLUDEPATH += $$PWD/include/zlib
INCLUDEPATH += $$PWD/include/quazip

win32 {

    !contains(QMAKE_TARGET.arch, x86_64) {
        Debug:LIBS += $$PWD/lib/zlib/win_i386/zlibstaticd.lib
        Release:LIBS += $$PWD/lib/zlib/win_i386/zlibstatic.lib

        Debug:LIBS += $$PWD/lib/quazip/win_i386/quazip_staticd.lib
        Release:LIBS += $$PWD/lib/quazip/win_i386/quazip_static.lib

    } else {
        Debug:LIBS += $$PWD/lib/zlib/win_x86_64/zlibstaticd.lib
        Release:LIBS += $$PWD/lib/zlib/win_x86_64/zlibstatic.lib

        Debug:LIBS += $$PWD/lib/quazip/win_x86_64/quazip_staticd.lib
        Release:LIBS += $$PWD/lib/quazip/win_x86_64/quazip_static.lib

    }
}

unix:!macx {
    QMAKE_CXXFLAGS += -std=c++0x
    debug:LIBS += $$PWD/lib/quazip/linux_x86_64/libquazipd.a -lz
    release:LIBS += $$PWD/lib/quazip/linux_x86_64/libquazip.a -lz
}

macx {
    QMAKE_CXXFLAGS += -std=c++0x
    debug:LIBS += $$PWD/lib/quazip/macosx_x86_64/libquazipd.a -lz
    release:LIBS += $$PWD/lib/quazip/macosx_x86_64/libquazip.a -lz
}

SOURCES += launcherconfiguration.cpp \
    launcherdata.cpp \
    launcherthread.cpp \
    main.cpp\
    mainwindow.cpp \
    patchkitlocalpatcher.cpp \
    patchkitremotepatcher.cpp \
    
HEADERS  += launchercancelledexception.h \
    launcherconfiguration.h \
    launcherdata.h \
    launcherexception.h \
    launcherlog.h \
    launcherthread.h \
    localpatcher.h \
    mainwindow.h \
    patchkitlocalpatcher.h \
    patchkitremotepatcher.h \
    remotepatcher.h \
    

FORMS    += mainwindow.ui

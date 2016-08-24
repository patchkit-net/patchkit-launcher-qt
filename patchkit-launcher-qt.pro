QT += core gui network

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
    CONFIG += c++14
    !contains(QMAKE_HOST.arch, x86_64) {
        debug:LIBS += $$PWD/lib/quazip/linux_i386/libquazipd.a -lz
        release:LIBS += $$PWD/lib/quazip/linux_i386/libquazip.a -lz
    } else {
        debug:LIBS += $$PWD/lib/quazip/linux_x86_64/libquazipd.a -lz
        release:LIBS += $$PWD/lib/quazip/linux_x86_64/libquazip.a -lz
    }
}

macx {
    CONFIG += c++14
    debug:LIBS += $$PWD/lib/quazip/macosx_x86_64/libquazipd.a -lz
    release:LIBS += $$PWD/lib/quazip/macosx_x86_64/libquazip.a -lz
}

SOURCES     += $$PWD/src/*.cpp

HEADERS     += $$PWD/src/*.h

INCLUDEPATH += $$PWD/src

FORMS       += $$PWD/src/*.ui

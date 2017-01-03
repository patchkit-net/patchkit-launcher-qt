QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launcher
TEMPLATE = app

DEFINES += QUAZIP_STATIC

INCLUDEPATH += $$PWD/include/zlib
INCLUDEPATH += $$PWD/include/quazip

CONFIG += c++14

win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        Debug:LIBS += $$PWD/lib/zlib/win_i386/zlibstaticd_md.lib
        Release:LIBS += $$PWD/lib/zlib/win_i386/zlibstatic_mt.lib

        Debug:LIBS += $$PWD/lib/quazip/win_i386/quazip_staticd_md.lib
        Release:LIBS += $$PWD/lib/quazip/win_i386/quazip_static_mt.lib

    } else {
        Debug:LIBS += $$PWD/lib/zlib/win_x86_64/zlibstaticd_md.lib
        Release:LIBS += $$PWD/lib/zlib/win_x86_64/zlibstatic_mt.lib

        Debug:LIBS += $$PWD/lib/quazip/win_x86_64/quazip_staticd_md.lib
        Release:LIBS += $$PWD/lib/quazip/win_x86_64/quazip_static_mt.lib

    }
}

unix:!macx {
    !contains(QMAKE_HOST.arch, x86_64) {
        debug:LIBS += $$PWD/lib/quazip/linux_i386/libquazipd.a -lz
        release:LIBS += $$PWD/lib/quazip/linux_i386/libquazip.a -lz
    } else {
        debug:LIBS += $$PWD/lib/quazip/linux_x86_64/libquazipd.a -lz
        release:LIBS += $$PWD/lib/quazip/linux_x86_64/libquazip.a -lz
    }
}

macx {
    debug:LIBS += $$PWD/lib/quazip/macosx_x86_64/libquazipd.a -lz
    release:LIBS += $$PWD/lib/quazip/macosx_x86_64/libquazip.a -lz
}

SOURCES     += $$PWD/src/*.cpp
SOURCES     += $$PWD/src/*.c

HEADERS     += $$PWD/src/*.h

INCLUDEPATH += $$PWD/src

FORMS       += $$PWD/src/*.ui

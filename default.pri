DEFINES += QUAZIP_STATIC

CONFIG += c++14

INCLUDEPATH += $$PWD/include/quazip
INCLUDEPATH += $$PWD/include/zlib
INCLUDEPATH += $$(PK_LAUNCHER_BOOST_INCLUDEDIR)

LIBS += -L$$(PK_LAUNCHER_BOOST_LIBDIR)

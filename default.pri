DEFINES += QUAZIP_STATIC
DEFINES += "BOOST_AUTO_LINK_TAGGED"

CONFIG += c++14

INCLUDEPATH += $$PWD/include/quazip
INCLUDEPATH += $$PWD/include/zlib
INCLUDEPATH += $$(PK_LAUNCHER_BOOST_INCLUDEDIR)

LIBS += -L$$(PK_LAUNCHER_BOOST_LIBDIR)

for(var, $$list($$enumerate_vars())) {
    message($$var)
    message($$eval($$var))
}

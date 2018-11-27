TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    core \
    app \
    tests \
    cli

OTHER_FILES += \
    link_libz_and_quazip.pri \
    default.pri \
    version.pri

DISTFILES += \
    link_core.pri

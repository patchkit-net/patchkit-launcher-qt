TEMPLATE = subdirs

SUBDIRS += \
    core \
    app \
    tests \
    cli

app.depends = core
tests.depends = core
cli.depends = core

OTHER_FILES += \
    link_libz_and_quazip.pri \
    default.pri \
    version.pri \
    link_core.pri

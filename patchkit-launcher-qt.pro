TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    patchkit-launcher-qt-src \
    patchkit-launcher-qt-app \
    patchkit-launcher-qt-tests \

patchkit-launcher-qt-app.depends = patchkit-launcher-qt-src
patchkit-launcher-qt-tests.depends = patchkit-launcher-qt-src

OTHER_FILES += default.pri link_static.pri

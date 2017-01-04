win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../patchkit-launcher-qt-src/release/ -lLauncherLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../patchkit-launcher-qt-src/debug/ -lLauncherLib
else:unix: LIBS += -L$$OUT_PWD/../patchkit-launcher-qt-src/ -lLauncherLib

INCLUDEPATH += $$PWD/../patchkit-launcher-qt-src
DEPENDPATH += $$PWD/../patchkit-launcher-qt-src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../patchkit-launcher-qt-src/release/libLauncherLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../patchkit-launcher-qt-src/debug/libLauncherLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../patchkit-launcher-qt-src/release/LauncherLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../patchkit-launcher-qt-src/debug/LauncherLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../patchkit-launcher-qt-src/libLauncherLib.a

INCLUDEPATH += ../core
DEPENDPATH += ../core

win32 {
    Debug:LIBS += -L$$OUT_PWD/../core/debug/ -lcore
}

unix {
    LIBS += -L$$OUT_PWD/../core/ -lcore
}

win32 {
    Debug:PRE_TARGETDEPS += $$OUT_PWD/../core/debug/core.lib
}

QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = spgraphicview
TEMPLATE = lib

DEFINES += GRAPHICVIEW_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/src/cspgraphicstructmodel.h \
    $$PWD/src/cspgraphicview.h \
    $$PWD/src/cspgraphicviewitem.h \
    $$PWD/src/cspgraphicviewmouseeventinfo.h \
    $$PWD/src/spgraphicview_global.h \
    $$PWD/src/cspgraviewbtn.h \
    $$PWD/src/csprealslider.h
SOURCES += \
    $$PWD/src/cspgraphicstructmodel.cpp \
    $$PWD/src/cspgraphicview.cpp \
    $$PWD/src/cspgraphicviewitem.cpp \
    $$PWD/src/cspgraphicviewmouseeventinfo.cpp \
    $$PWD/src/cspgraviewbtn.cpp \
    $$PWD/src/csprealslider.cpp

include($$PWD/src/cspgaphicviewAnimation/spgraphicView_animation.pri)
include($$PWD/src/cspgaphicviewNotAnimation/spgraphicView_notanimation.pri)


#export pdb on release version
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS += /NODEFAULTLIB:library
QMAKE_CFLAGS += /wd"4819" /wd"4566"
QMAKE_CXXFLAGS += /wd"4819" /wd"4566"

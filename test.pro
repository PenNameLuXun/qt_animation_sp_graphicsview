QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = spgraphicview
TEMPLATE = app

DEFINES += GRAPHICVIEW_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/csptestitem.h \
    $$PWD/spgraphicView/src/cspgraphicstructmodel.h \
    $$PWD/spgraphicView/src/cspgraphicview.h \
    $$PWD/spgraphicView/src/cspgraphicviewitem.h \
    $$PWD/spgraphicView/src/cspgraphicviewmouseeventinfo.h \
    $$PWD/spgraphicView/src/spgraphicview_global.h \
    $$PWD/spgraphicView/src/cspgraviewbtn.h \
    $$PWD/spgraphicView/src/csprealslider.h \
    mainwindow.h
SOURCES += \
    $$PWD/main.cpp \
    $$PWD/csptestitem.cpp \
    $$PWD/spgraphicView/src/cspgraphicstructmodel.cpp \
    $$PWD/spgraphicView/src/cspgraphicview.cpp \
    $$PWD/spgraphicView/src/cspgraphicviewitem.cpp \
    $$PWD/spgraphicView/src/cspgraphicviewmouseeventinfo.cpp \
    $$PWD/spgraphicView/src/cspgraviewbtn.cpp \
    $$PWD/spgraphicView/src/csprealslider.cpp \
    mainwindow.cpp


include($$PWD/spgraphicView/src/cspgaphicviewAnimation/spgraphicView_animation.pri)

INCLUDEPATH += $$PWD/spgraphicView/src
INCLUDEPATH += $$PWD/spgraphicView/src/cspgaphicviewAnimation

FORMS += \
    mainwindow.ui


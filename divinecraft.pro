#-------------------------------------------------
#
# Project created by QtCreator 2015-04-11T17:05:46
#
#-------------------------------------------------

# QT       += core gui

QT += opengl widgets

contains(QT_CONFIG, opengles.) {
    contains(QT_CONFIG, angle): \
        warning("Qt was built with ANGLE, which provides only OpenGL ES 2.0 on top of DirectX 9.0c")
    error("This example requires Qt to be configured with -opengl desktop")
}


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = divinecraft
TEMPLATE = app

# LIBS += -lglut -lGLU

SOURCES += main.cpp\
        dcview.cpp \
    glkernel/glextensions.cpp \
    glkernel/glbuffers.cpp \
    gamescene.cpp \
    camera.cpp \
    block.cpp \
    glkernel/chunkmesh.cpp \
    chunkmap.cpp \
    world.cpp \
    gmath.cpp \
    module/panels.cpp \
    module/optionswidget.cpp

HEADERS  += dcview.h \
    glkernel/glextensions.h \
    glkernel/glbuffers.h \
    gamescene.h \
    camera.h \
    block.h \
    glkernel/glmeshs.h \
    glkernel/chunkmesh.h \
    chunkmap.h \
    world.h \
    gmath.h \
    module/panels.h \
    module/optionswidget.h


wince*: {
    DEPLOYMENT_PLUGIN += qjpeg
}

win32-msvc* {
    QMAKE_CXXFLAGS -= -Zm200
    QMAKE_CFLAGS -= -Zm200
    QMAKE_CXXFLAGS += -Zm500
    QMAKE_CFLAGS += -Zm500
}

RESOURCES += \
    dces.qrc

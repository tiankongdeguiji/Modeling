#-------------------------------------------------
#
# Project created by QtCreator 2015-10-08T14:14:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Modeling

TEMPLATE = app


SOURCES += main.cpp \
    EulerOperation.cpp \
    GLWidget.cpp \
    RenderEngine.cpp \
    polypartition.cpp

HEADERS += \
    BRep.h \
    EulerOperation.h \
    GLWidget.h \
    RenderEngine.h \
    polypartition.h

RESOURCES += shaders.qrc

#INCLUDEPATH += /usr/local/include/eigen3

#-------------------------------------------------
#
# Project created by QtCreator 2018-11-30T12:19:15
#
#-------------------------------------------------

QT       += core gui
QT += opengl
LIBS += -lopengl32 -lglu32 -lglut
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GraphicProject
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    gl3dwidget.cpp

HEADERS  += window.h \
    figureinfo.h \
    gl3dwidget.h

FORMS    += window.ui

RESOURCES += \
    images.qrc

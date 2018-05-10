#-------------------------------------------------
#
# Project created by QtCreator 2017-10-18T16:45:28
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LiveView
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QCUSTOMPLOT_USE_OPENGL

QMAKE_CXXFLAGS += -Ofast -std=c++0x -Wno-inconsistent-missing-override

OBJECTS_DIR = ./obj
MOC_DIR = ./obj

INCLUDEPATH += ./include \
               ./include/qcustomplot

VPATH += ./include \
         ./src \
         ./kernel

SOURCES += \
        main.cpp \
        lvmainwindow.cpp \
        frameview_widget.cpp \
        frameworker.cpp \
        qcustomplot.cpp \
        debugcamera.cpp \
        ssdcamera.cpp \
        controlsbox.cpp \
        darksubfilter.cpp \
        ctkrangeslider.cpp \
        osutils.cpp \
        stddevfilter.cpp
#       clcamera.cpp

HEADERS += \
        lvmainwindow.h \
        frameview_widget.h \
        image_type.h \
        lvframe.h \
        frameworker.h \
        qcustomplot/qcustomplot.h \
        cameramodel.h \
        debugcamera.h \
        constants.h \
        ssdcamera.h \
        osutils.h \
        framethread.h \
        controlsbox.h \
        alphanum.hpp \
        darksubfilter.h \
        ctkrangeslider.h \
        lvtabapplication.h \
        stddevfilter.h
#       clcamera.h

RESOURCES += \
    images/images.qrc \
    kernel/kernel.qrc

DISTFILES += \
    kernel/stddev.cl

mac: LIBS += -framework OpenCL
else:unix|win32: LIBS += -lOpenCL

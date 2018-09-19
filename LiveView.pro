#-------------------------------------------------
#
# Project created by QtCreator 2017-10-18T16:45:28
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LiveView
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS -= -std=gnu++11
QMAKE_CXXFLAGS += -O3 -std=c++0x -Wno-inconsistent-missing-override -Wno-ignored-attributes -Wno-date-time
DEFINES += \
    HOST=\\\"`hostname`\\\" UNAME=\\\"`whoami`\\\" \
    GIT_CURRENT_SHA1="\\\"$(shell git -C $$PWD rev-parse HEAD)\\\"" \
    GIT_CURRENT_SHA1_SHORT="\\\"$(shell git -C $$PWD rev-parse --short HEAD)\\\""

OBJECTS_DIR = ./obj
MOC_DIR = ./obj

RC_FILE = liveview.rc
ICON = liveview.icns

INCLUDEPATH += ./include \
               ./include/qcustomplot

VPATH += ./include \
         ./src \
         ./kernel \
         ./util \
         ./EDT_include

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
        stddevfilter.cpp \
        histogram_widget.cpp \
        line_widget.cpp \
        meanfilter.cpp \
        fft_widget.cpp \
        saveserver.cpp \
        twoscomplimentfilter.cpp
unix:!macx {
   SOURCES += clcamera.cpp
}

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
        controlsbox.h \
        alphanum.hpp \
        darksubfilter.h \
        ctkrangeslider.h \
        lvtabapplication.h \
        stddevfilter.h \
        histogram_widget.h \
        line_widget.h \
        meanfilter.h \
        fft_widget.h \
        sliding_dft.h \
        computedevdialog.h \
        saveserver.h \
        saveclient.h \
        dsfprefdialog.h \
        cameraselectdialog.h \
        twoscomplimentfilter.h
unix:!macx {
    HEADERS += clcamera.h
}

RESOURCES += \
    images/images.qrc \
    kernel/kernel.qrc \
    qdarkstyle/style.qrc

DISTFILES += \
    kernel/stddev.cl

macx: LIBS += -framework OpenCL
else:unix|win32: LIBS += -lOpenCL
unix:!macx: LIBS += -L$$PWD/lib -lm -lpdv -ldl

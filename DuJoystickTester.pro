#-------------------------------------------------
#
# Project created by QtCreator 2015-06-14T10:59:02
#
#-------------------------------------------------

QT       += core gui serialport printsupport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DuJoystickTester
TEMPLATE = app

INCLUDEPATH += $$PWD/3rdParty/SDL2-2.0.3/i686-w64-mingw32/include
INCLUDEPATH += C://OpenCV//opencv//release//install//include

LIBS += -L$$PWD/3rdParty/SDL2-2.0.3/i686-w64-mingw32/lib

LIBS += -lSDL2

DEFINES += SDL_MAIN_HANDLED

SOURCES += main.cpp\
        Widget.cpp \
    DuJoystickManager.cpp \
    QcGaugeWidget/source/qcgaugewidget.cpp

HEADERS  += Widget.h \
    DuJoystickManager.h \
    QcGaugeWidget/source/qcgaugewidget.h

FORMS    += Widget.ui

LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_calib3d320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_core320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_features2d320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_flann320.dll.a
#LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_gpu320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_highgui320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_imgproc320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_imgcodecs320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_ml320.dll.a
#LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_nonfree320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_objdetect320.dll.a
#LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_ocl320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_photo320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_stitching320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_superres320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_shape320.dll.a
#LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_ts320.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_video320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_videoio320.dll.a
LIBS += C://OpenCV//opencv//release//install//x86//mingw//lib//libopencv_videostab320.dll.a

QMAKE_CXXFLAGS += -std=gnu++14

RESOURCES += \
    img.qrc

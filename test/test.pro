QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt
INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2
LIBS += /usr/local/lib/libopencv_calib3d.so //usr/local/lib/libopencv_core.so //usr/local/lib/libopencv_features2d.so //usr/local/lib/libopencv_flann.so //usr/local/lib/libopencv_highgui.so //usr/local/lib/libopencv_imgcodecs.so //usr/local/lib/libopencv_imgproc.so //usr/local/lib/libopencv_ml.so //usr/local/lib/libopencv_objdetect.so //usr/local/lib/libopencv_photo.so //usr/local/lib/libopencv_shape.so //usr/local/lib/libopencv_stitching.so //usr/local/lib/libopencv_superres.so //usr/local/lib/libopencv_videoio.so //usr/local/lib/libopencv_video.so //usr/local/lib/libopencv_videostab.so

LIBS += -l wiringPi

SOURCES += \
    main.cpp \
    hardwarecontrol.cpp \
    imageprocess.cpp \
    mg955.cpp \
    solution.cpp \
    machine_state.cpp

HEADERS += \
    hardwarecontrol.h \
    imageprocess.h \
    mg955.h \
    solution.h \
    machine_state.h



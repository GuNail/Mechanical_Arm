QT      +=  multimedia multimediawidgets

INCLUDEPATH += $$PWD/include/opencv4
INCLUDEPATH += $$PWD

contains(QT_ARCH, arm){
    LIBS += -L $$PWD/armlib -lopencv_world
}else{
    LIBS += -L $$PWD/linuxlib -lopencv_world
}


HEADERS += \
    $$PWD/opencv_imagetools.h \
    $$PWD/opencvhead.h \
    $$PWD/opencvwidget.h \
    $$PWD/opencvthread.h

SOURCES += \
    $$PWD/opencv_imagetools.cpp \
    $$PWD/opencvwidget.cpp \
    $$PWD/opencvthread.cpp


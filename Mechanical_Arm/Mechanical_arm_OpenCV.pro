QT       += core gui
QT      +=  serialport



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    automechanicalarm.cpp \
    calculateangles.cpp \
    imageproc.cpp \
    main.cpp \
    mainwindow.cpp \
    register.cpp \
    typewrite.cpp \
    customlineedit.cpp

HEADERS += \
    automechanicalarm.h \
    calculateangles.h \
    imageproc.h \
    logInfo.h \
    mainwindow.h \
    register.h \
    typewrite.h \
    customlineedit.h

FORMS += \
    mainwindow.ui \
    typewrite.ui

include(./OpenCV/OpenCV.pri)
include(./Serial/Serial.pri)
include(./Socket/Socket.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/ubuntu/Qt/$${TARGET}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resourcesImages.qrc

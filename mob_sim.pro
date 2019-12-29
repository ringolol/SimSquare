#-------------------------------------------------
#
# Project created by QtCreator 2019-02-19T01:14:08
#
#-------------------------------------------------

QT       += core gui
#QT += quick quickcontrols2 svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl

TARGET = mob_sim
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

CONFIG += c++11

SOURCES += \
    core/gr_core.cpp \
    core/gr_core_fun.cpp \
    core/gr_coreobj.cpp \
    interface/gr_comfun.cpp \
    interface/gr_const.cpp \
    interface/gr_drawable.cpp \
    interface/gr_line.cpp \
    interface/gr_mainwin.cpp \
    interface/main.cpp \
    interface/vibrate.cpp \
    logger/gr_logger.cpp

HEADERS += \
    core/gr_core.h \
    core/gr_core_const.h \
    core/gr_core_fun.h \
    core/gr_coreobj.h \
    interface/gr_comfun.h \
    interface/gr_const.h \
    interface/gr_drawable.h \
    interface/gr_line.h \
    interface/gr_mainwin.h \
    interface/vibrate.h \
    logger/gr_logger.h

CONFIG += mobility
MOBILITY = 


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    interface/Gr_vibrate.java \
    android2/AndroidManifest.xml \
    android2/gradle/wrapper/gradle-wrapper.jar \
    android2/gradlew \
    android2/res/values/libs.xml \
    android2/build.gradle \
    android2/gradle/wrapper/gradle-wrapper.properties \
    android2/gradlew.bat

contains(ANDROID_TARGET_ARCH,x86) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android2
}
android: QT += androidextras
android: QT += qml

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android2
}

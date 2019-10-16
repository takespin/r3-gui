#-------------------------------------------------
#
# Project created by QtCreator 2019-09-11T11:43:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = r3gui
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
        main.cpp \
        mainwindow.cpp \
        fid.cpp \
        metricPrefix.cpp \
    calcr3.cpp

HEADERS += \
        mainwindow.h \
        fid.h \
        metricPrefix.h \
    calcr3.h

mac:INCLUDEPATH += /usr/local/include/gamma-4.3.3/
mac:LIBS += -L/usr/local/lib/gamma-4.3.3/ -lgamma -lm

win32:INCLUDEPATH += C:/dev/gamma/src
win32:LIBS += -L"C:\dev\gamma\i686-pc-cygwin" -llibgamma.dll -lm

QMAKE_CXXFLAGS += -O3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

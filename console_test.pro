QT += core serialbus network
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = console_test

SOURCES += \
    console_test.cpp \
    modbusmanager.cpp

HEADERS += \
    modbusmanager.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
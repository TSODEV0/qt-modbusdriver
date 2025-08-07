QT += core serialbus network
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = connection_test
TEMPLATE = app

SOURCES += \
    connection_test.cpp \
    modbusmanager.cpp

HEADERS += \
    modbusmanager.h

# Build configuration
DESTDIR = .
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
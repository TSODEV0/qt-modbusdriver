QT += core widgets serialbus network sql
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = ScadaVerification
TEMPLATE = app

# Build directories
DESTDIR = build-release
OBJECTS_DIR = build-release/obj
MOC_DIR = build-release/moc
UI_DIR = build-release/ui

# Source files
SOURCES += \
    src/scada_verification.cpp \
    src/modbusmanager.cpp

# Header files
HEADERS += \
    include/modbusmanager.h

# Include paths
INCLUDEPATH += include src
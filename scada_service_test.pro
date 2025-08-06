QT += core serialbus network
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = scada_service_test
TEMPLATE = app

# Source files
SOURCES += \
    scada_service_test.cpp \
    scada_core_service.cpp \
    modbusmanager.cpp

# Header files
HEADERS += \
    scada_core_service.h \
    modbusmanager.h

# Compiler flags
CXXFLAGS += -std=c++17

# Define application information
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Output directory
DESTDIR = .

# Object files directory
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Installation
target.path = .
INSTALLS += target
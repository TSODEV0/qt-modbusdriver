QT += core serialbus network sql
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = scada_service_test
TEMPLATE = app

# Source files
SOURCES += \
    src/scada_service_test.cpp \
    src/scada_core_service.cpp \
    src/modbusmanager.cpp \
    src/connection_resilience_manager.cpp \
    src/database_manager.cpp \
    src/influxdb_line_protocol.cpp

# Header files
HEADERS += \
    include/modbusmanager.h \
    include/scada_core_service.h \
    include/connection_resilience_manager.h \
    include/database_manager.h \
    include/influxdb_line_protocol.h

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
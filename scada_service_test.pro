QT += core serialbus network sql concurrent testlib
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = scada_service_test
TEMPLATE = app

# Source files
SOURCES += \
    src/scada_service_test.cpp \
    src/modbusmanager.cpp \
    src/scada_core_service.cpp \
    src/modbus_worker.cpp \
    src/modbus_worker_manager.cpp \
    src/connection_resilience_manager.cpp \
    src/database_manager.cpp

# Header files
HEADERS += \
    include/modbusmanager.h \
    include/scada_core_service.h \
    include/modbus_worker.h \
    include/modbus_worker_manager.h \
    include/connection_resilience_manager.h \
    include/database_manager.h

# Compiler flags for multithreading support
QMAKE_CXXFLAGS += -pthread
QMAKE_LFLAGS += -pthread

# Define application information
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Include directories
INCLUDEPATH += include src

# Output directory
DESTDIR = .

# Object files directory
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Libraries (pthread already handled by QMAKE_LFLAGS)

# Installation
target.path = .
INSTALLS += target
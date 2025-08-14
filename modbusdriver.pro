# ModbusDriver - SCADA Modbus Communication System
# Qt Project Configuration File

TEMPLATE = app
TARGET = ModbusDriver

# Qt modules
QT += core gui widgets serialbus network sql

# C++ standard
CONFIG += c++17

# Compiler flags
CONFIG += warn_on
CONFIG += debug_and_release

# Debug configuration for Modbus register tracking
CONFIG(debug, debug|release) {
    DEFINES += MODBUS_DEBUG_ENABLED
    DEFINES += BLOCK_OPTIMIZATION_DEBUG
    DEFINES += REGISTER_ADDRESS_DEBUG
    DEFINES += FLOAT32_DECODE_DEBUG
    message("Debug build: Modbus debugging enabled")
}

# Disable deprecated APIs
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Include paths
INCLUDEPATH += include
INCLUDEPATH += src

# Source files
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/modbusmanager.cpp \
    src/scada_core_service.cpp \
    src/connection_resilience_manager.cpp

# Header files
HEADERS += \
    include/mainwindow.h \
    include/modbusmanager.h \
    include/scada_core_service.h \
    include/connection_resilience_manager.h

# UI forms
FORMS += \
    ui/mainwindow.ui

# Build directories
CONFIG(debug, debug|release) {
    DESTDIR = build-debug
    OBJECTS_DIR = build-debug/obj
    MOC_DIR = build-debug/moc
    UI_DIR = build-debug/ui
    RCC_DIR = build-debug/rcc
}

CONFIG(release, debug|release) {
    DESTDIR = build-release
    OBJECTS_DIR = build-release/obj
    MOC_DIR = build-release/moc
    UI_DIR = build-release/ui
    RCC_DIR = build-release/rcc
}

# Installation paths
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Additional clean targets
CLEAN_FILES += build-debug/* build-release/*

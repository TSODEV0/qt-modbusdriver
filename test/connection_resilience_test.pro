# Connection Resilience Test - SCADA Poor Connection Handling Test
# Qt Project Configuration File

TEMPLATE = app
TARGET = connection_resilience_test

# Qt modules
QT += core serialbus network
QT -= gui

# C++ standard
CONFIG += c++17

# Compiler flags
CONFIG += warn_on
CONFIG += debug_and_release
CONFIG += console

# Debug configuration for connection testing
CONFIG(debug, debug|release) {
    DEFINES += CONNECTION_DEBUG_ENABLED
    DEFINES += RESILIENCE_DEBUG_ENABLED
    message("Debug build: Connection resilience debugging enabled")
}

# Disable deprecated APIs
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Include paths
INCLUDEPATH += include
INCLUDEPATH += src

# Source files
SOURCES += \
    src/connection_resilience_test.cpp \
    src/modbusmanager.cpp \
    src/connection_resilience_manager.cpp

# Header files
HEADERS += \
    include/modbusmanager.h \
    include/connection_resilience_manager.h

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
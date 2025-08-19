QT += testlib serialbus network sql core concurrent
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++17

TEMPLATE = app
TARGET = ModbusDriverTests

# Test source files
SOURCES += \
    main.cpp \
    test_modbus_worker.cpp \
    test_modbus_worker_manager.cpp

# Test header files (these will generate MOC files automatically)
HEADERS += \
    test_modbus_worker.h \
    test_modbus_worker_manager.h

# Include the main project source files for testing
SOURCES += \
    ../src/modbusmanager.cpp \
    ../src/modbus_worker.cpp \
    ../src/modbus_worker_manager.cpp \
    ../src/scada_core_service.cpp \
    ../src/connection_resilience_manager.cpp

# Include the main project header files
HEADERS += \
    ../include/modbusmanager.h \
    ../include/modbus_worker.h \
    ../include/scada_core_service.h \
    ../include/connection_resilience_manager.h

# Include paths
INCLUDEPATH += \
    ../include \
    ../src

# Compiler flags
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += DEBUG_MODBUS_REGISTERS

# Build directories
DESTDIR = ../build/tests
OBJECTS_DIR = ../build/tests/obj
MOC_DIR = ../build/tests/moc
RCC_DIR = ../build/tests/rcc
UI_DIR = ../build/tests/ui

# Create build directories
system(mkdir -p $$DESTDIR)
system(mkdir -p $$OBJECTS_DIR)
system(mkdir -p $$MOC_DIR)
system(mkdir -p $$RCC_DIR)
system(mkdir -p $$UI_DIR)

# Clean target
CLEAN_DEPS += clean_test_output
clean_test_output.commands = rm -rf test_output
QMAKE_EXTRA_TARGETS += clean_test_output

# Test execution target
test.depends = $(TARGET)
test.commands = ./$(TARGET)
QMAKE_EXTRA_TARGETS += test

# Coverage target (if gcov is available)
coverage.depends = test
coverage.commands = \
    gcov *.gcno && \
    lcov --capture --directory . --output-file coverage.info && \
    genhtml coverage.info --output-directory coverage_html
QMAKE_EXTRA_TARGETS += coverage

# Memory check target (if valgrind is available)
memcheck.depends = $(TARGET)
memcheck.commands = valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(TARGET)
QMAKE_EXTRA_TARGETS += memcheck

# Installation
target.path = /opt/modbusdriver/tests
INSTALLS += target
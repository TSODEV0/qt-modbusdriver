#ifndef TEST_MODBUS_MANAGER_H
#define TEST_MODBUS_MANAGER_H

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QThread>
#include <QtConcurrent>
#include <QFuture>
#include <QAtomicInt>
#include "../include/modbus_manager.h"

class TestModbusManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Constructor and basic functionality tests
    void testConstructor();
    void testManagerInitialization();
    
    // Connection management tests
    void testConnectionCreation();
    void testConnectionDestruction();
    void testMultipleConnections();
    
    // Request handling tests
    void testRequestExecution();
    void testRequestQueuing();
    void testRequestTimeout();
    
    // Error handling tests
    void testConnectionErrors();
    void testRequestErrors();
    void testErrorRecovery();
    
    // Performance and resource tests
    void testResourceManagement();
    void testConnectionPooling();
    
    // Multithreading tests
    void testConcurrentConnections();
    void testConcurrentRequests();
    void testThreadSafeOperations();
    void testConcurrentErrorHandling();
    
private:
    ModbusManager* m_manager;
    QString m_testHost1;
    QString m_testHost2;
    int m_testPort;
    int m_testUnitId1;
    int m_testUnitId2;
};

#endif // TEST_MODBUS_MANAGER_H
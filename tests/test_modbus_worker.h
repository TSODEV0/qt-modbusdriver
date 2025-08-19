#ifndef TEST_MODBUS_WORKER_H
#define TEST_MODBUS_WORKER_H

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTimer>
#include "../include/modbus_worker.h"
#include "../include/modbusmanager.h"

class TestModbusWorker : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Constructor and basic properties tests
    void testConstructor();
    void testDeviceProperties();
    void testGetDeviceKey();
    
    // Connection management tests
    void testConnectionState();
    void testStartStopWorker();
    
    // Request queuing tests
    void testQueueReadRequest();
    void testQueueWriteRequest();
    void testRequestPriority();
    void testClearRequestQueue();
    
    // Statistics tests
    void testStatisticsTracking();
    void testResetStatistics();
    
    // Polling tests
    void testPollInterval();
    void testPollTimer();
    
    // Error handling tests
    void testErrorHandling();
    void testRequestInterruption();
    
    // Signal emission tests
    void testSignalEmission();
    
    // Multithreading tests
    void testConcurrentRequestQueuing();
    void testConcurrentStatisticsAccess();
    void testThreadSafeRequestInterruption();
    void testConcurrentPollIntervalChanges();
    
private:
    ModbusWorker* m_worker;
    QString m_testHost;
    int m_testPort;
    int m_testUnitId;
};

#endif // TEST_MODBUS_WORKER_H
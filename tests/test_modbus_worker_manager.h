#ifndef TEST_MODBUS_WORKER_MANAGER_H
#define TEST_MODBUS_WORKER_MANAGER_H

#include <QtTest/QtTest>
#include <QSignalSpy>
#include "../include/modbus_worker.h"

class TestModbusWorkerManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Constructor and basic functionality tests
    void testConstructor();
    void testCreateDeviceKey();
    
    // Worker management tests
    void testGetOrCreateWorker();
    void testGetWorker();
    void testRemoveWorker();
    void testRemoveAllWorkers();
    void testGetWorkerCount();
    
    // Device management tests
    void testGetActiveDevices();
    void testGetConnectedDevices();
    
    // Statistics tests
    void testGlobalStatistics();
    void testResetGlobalStatistics();
    void testStatisticsUpdates();
    
    // Configuration tests
    void testDefaultPollInterval();
    void testSetWorkerPollInterval();
    
    // Worker lifecycle tests
    void testStartAllWorkers();
    void testStopAllWorkers();
    void testConnectAllDevices();
    void testDisconnectAllDevices();
    
    // Signal emission tests
    void testWorkerCreatedSignal();
    void testWorkerRemovedSignal();
    void testGlobalStatisticsSignal();
    
    // Thread safety tests
    void testConcurrentWorkerAccess();
    void testConcurrentStatisticsAccess();
    void testConcurrentWorkerRemoval();
    void testConcurrentPollIntervalChanges();
    void testConcurrentWorkerLifecycle();
    
private:
    ModbusWorkerManager* m_manager;
    QString m_testHost1;
    QString m_testHost2;
    int m_testPort;
    int m_testUnitId1;
    int m_testUnitId2;
};

#endif // TEST_MODBUS_WORKER_MANAGER_H
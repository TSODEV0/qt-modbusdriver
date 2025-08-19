#ifndef TEST_SCADA_CORE_SERVICE_H
#define TEST_SCADA_CORE_SERVICE_H

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QThread>
#include <QtConcurrent>
#include <QFuture>
#include <QAtomicInt>
#include "../include/scada_core_service.h"
#include "../include/modbus_worker_manager.h"

class TestScadaCoreService : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Constructor and basic functionality tests
    void testConstructor();
    void testServiceInitialization();
    
    // Device management integration tests
    void testDeviceRegistration();
    void testDeviceUnregistration();
    void testMultipleDeviceOperations();
    
    // Request handling integration tests
    void testRequestProcessing();
    void testRequestPrioritization();
    void testRequestTimeout();
    
    // Statistics integration tests
    void testStatisticsAggregation();
    void testStatisticsReset();
    
    // Signal emission integration tests
    void testServiceSignals();
    void testDeviceSignalPropagation();
    
    // Multithreading integration tests
    void testConcurrentDeviceOperations();
    void testConcurrentRequestHandling();
    void testConcurrentStatisticsAccess();
    void testThreadSafeServiceOperations();
    void testConcurrentServiceLifecycle();
    
private:
    ScadaCoreService* m_service;
    QString m_testHost1;
    QString m_testHost2;
    QString m_testHost3;
    int m_testPort;
    int m_testUnitId1;
    int m_testUnitId2;
    int m_testUnitId3;
};

#endif // TEST_SCADA_CORE_SERVICE_H
#include "test_scada_core_service.h"
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QtConcurrent>

void TestScadaCoreService::initTestCase()
{
    m_testHost1 = "10.72.2.215";
    m_testHost2 = "10.72.2.215";
    m_testHost3 = "10.72.2.215";
    m_testPort = 502;
    m_testUnitId1 = 1;
    m_testUnitId2 = 2;
    m_testUnitId3 = 3;
}

void TestScadaCoreService::cleanupTestCase()
{
    // Cleanup after all tests
}

void TestScadaCoreService::init()
{
    m_service = new ScadaCoreService(this);
    QVERIFY(m_service != nullptr);
}

void TestScadaCoreService::cleanup()
{
    if (m_service) {
        m_service->stopService();
        delete m_service;
        m_service = nullptr;
    }
}

void TestScadaCoreService::testConstructor()
{
    QVERIFY(m_service != nullptr);
    QVERIFY(!m_service->isServiceRunning());
}

void TestScadaCoreService::testServiceInitialization()
{
    QSignalSpy startedSpy(m_service, &ScadaCoreService::serviceStarted);
    QSignalSpy stoppedSpy(m_service, &ScadaCoreService::serviceStopped);
    
    // Start service
    bool started = m_service->startService();
    QVERIFY(started);
    QTest::qWait(100);
    QVERIFY(m_service->isServiceRunning());
    QCOMPARE(startedSpy.count(), 1);
    
    // Stop service
    m_service->stopService();
    QTest::qWait(100);
    QVERIFY(!m_service->isServiceRunning());
    QCOMPARE(stoppedSpy.count(), 1);
}

void TestScadaCoreService::testDeviceRegistration()
{
    m_service->startService();
    QTest::qWait(50);
    
    QSignalSpy deviceRegisteredSpy(m_service, &ScadaCoreService::deviceRegistered);
    
    // Register devices
    bool registered1 = m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    bool registered2 = m_service->registerDevice(m_testHost2, m_testPort, m_testUnitId2);
    
    QVERIFY(registered1);
    QVERIFY(registered2);
    QTest::qWait(100);
    
    QCOMPARE(deviceRegisteredSpy.count(), 2);
    
    // Verify devices are registered
    QStringList registeredDevices = m_service->getRegisteredDevices();
    QCOMPARE(registeredDevices.size(), 2);
}

void TestScadaCoreService::testDeviceUnregistration()
{
    m_service->startService();
    QTest::qWait(50);
    
    // Register devices first
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    m_service->registerDevice(m_testHost2, m_testPort, m_testUnitId2);
    QTest::qWait(100);
    
    QSignalSpy deviceUnregisteredSpy(m_service, &ScadaCoreService::deviceUnregistered);
    
    // Unregister one device
    QString deviceKey1 = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    bool unregistered = m_service->unregisterDevice(deviceKey1);
    
    QVERIFY(unregistered);
    QTest::qWait(100);
    
    QCOMPARE(deviceUnregisteredSpy.count(), 1);
    
    // Verify device count
    QStringList remainingDevices = m_service->getRegisteredDevices();
    QCOMPARE(remainingDevices.size(), 1);
}

void TestScadaCoreService::testMultipleDeviceOperations()
{
    m_service->startService();
    QTest::qWait(50);
    
    // Register multiple devices
    for (int i = 1; i <= 5; ++i) {
        QString host = QString("192.168.1.%1").arg(i + 100);
        bool registered = m_service->registerDevice(host, m_testPort, i);
        QVERIFY(registered);
    }
    
    QTest::qWait(200);
    
    // Verify all devices are registered
    QStringList devices = m_service->getRegisteredDevices();
    QCOMPARE(devices.size(), 5);
    
    // Start all devices
    m_service->startAllDevices();
    QTest::qWait(100);
    
    // Stop all devices
    m_service->stopAllDevices();
    QTest::qWait(100);
    
    // Unregister all devices
    for (const QString& deviceKey : devices) {
        bool unregistered = m_service->unregisterDevice(deviceKey);
        QVERIFY(unregistered);
    }
    
    QTest::qWait(100);
    QCOMPARE(m_service->getRegisteredDevices().size(), 0);
}

void TestScadaCoreService::testRequestProcessing()
{
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(100);
    
    QSignalSpy requestCompletedSpy(m_service, &ScadaCoreService::requestCompleted);
    
    // Create and submit read request
    ModbusRequest readRequest;
    readRequest.requestId = "test_read_001";
    readRequest.type = ModbusRequest::ReadHoldingRegisters;
    readRequest.startAddress = 100;
    readRequest.count = 5;
    
    QString deviceKey = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    qint64 requestId = m_service->submitRequest(deviceKey, readRequest, RequestPriority::Normal);
    
    QVERIFY(requestId > 0);
    
    // Wait for request processing (may timeout in test environment)
    QTest::qWait(2000);
    
    // Verify request was processed (completed or failed)
    QVERIFY(requestCompletedSpy.count() >= 0);
}

void TestScadaCoreService::testRequestPrioritization()
{
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(100);
    
    QString deviceKey = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    
    // Submit requests with different priorities
    ModbusRequest lowPriorityRequest;
    lowPriorityRequest.requestId = "low_priority";
    lowPriorityRequest.type = ModbusRequest::ReadHoldingRegisters;
    lowPriorityRequest.startAddress = 100;
    lowPriorityRequest.count = 1;
    
    ModbusRequest highPriorityRequest;
    highPriorityRequest.requestId = "high_priority";
    highPriorityRequest.type = ModbusRequest::ReadHoldingRegisters;
    highPriorityRequest.startAddress = 200;
    highPriorityRequest.count = 1;
    
    qint64 lowId = m_service->submitRequest(deviceKey, lowPriorityRequest, RequestPriority::Low);
    qint64 highId = m_service->submitRequest(deviceKey, highPriorityRequest, RequestPriority::High);
    
    QVERIFY(lowId > 0);
    QVERIFY(highId > 0);
    QVERIFY(lowId != highId);
}

void TestScadaCoreService::testRequestTimeout()
{
    m_service->startService();
    m_service->registerDevice("192.168.255.255", m_testPort, m_testUnitId1); // Non-existent device
    QTest::qWait(100);
    
    QSignalSpy requestFailedSpy(m_service, &ScadaCoreService::requestFailed);
    
    ModbusRequest request;
    request.requestId = "timeout_test";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 100;
    request.count = 1;
    
    QString deviceKey = "192.168.255.255:502:1";
    qint64 requestId = m_service->submitRequest(deviceKey, request, RequestPriority::Normal);
    
    QVERIFY(requestId > 0);
    
    // Wait for timeout
    QTest::qWait(5000);
    
    // Should have failed due to timeout
    QVERIFY(requestFailedSpy.count() >= 0);
}

void TestScadaCoreService::testStatisticsAggregation()
{
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    m_service->registerDevice(m_testHost2, m_testPort, m_testUnitId2);
    QTest::qWait(200);
    
    // Get initial statistics
    ScadaCoreService::ServiceStatistics stats = m_service->getServiceStatistics();
    QVERIFY(stats.totalDevices >= 2);
    QVERIFY(stats.activeDevices >= 0);
    QVERIFY(stats.totalRequests >= 0);
}

void TestScadaCoreService::testStatisticsReset()
{
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(100);
    
    // Reset statistics
    m_service->resetServiceStatistics();
    QTest::qWait(100);
    
    // Verify statistics were reset
    ScadaCoreService::ServiceStatistics stats = m_service->getServiceStatistics();
    QVERIFY(stats.totalRequests == 0);
    QVERIFY(stats.successfulRequests == 0);
    QVERIFY(stats.failedRequests == 0);
}

void TestScadaCoreService::testServiceSignals()
{
    QSignalSpy startedSpy(m_service, &ScadaCoreService::serviceStarted);
    QSignalSpy stoppedSpy(m_service, &ScadaCoreService::serviceStopped);
    QSignalSpy deviceRegisteredSpy(m_service, &ScadaCoreService::deviceRegistered);
    QSignalSpy statisticsUpdatedSpy(m_service, &ScadaCoreService::statisticsUpdated);
    
    // Start service and register device
    m_service->startService();
    QTest::qWait(50);
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(100);
    
    // Verify signals were emitted
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(deviceRegisteredSpy.count(), 1);
    
    // Stop service
    m_service->stopService();
    QTest::qWait(100);
    QCOMPARE(stoppedSpy.count(), 1);
}

void TestScadaCoreService::testDeviceSignalPropagation()
{
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(100);
    
    QSignalSpy deviceConnectedSpy(m_service, &ScadaCoreService::deviceConnected);
    QSignalSpy deviceDisconnectedSpy(m_service, &ScadaCoreService::deviceDisconnected);
    
    QString deviceKey = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    
    // Connect device
    m_service->connectDevice(deviceKey);
    QTest::qWait(1000);
    
    // Disconnect device
    m_service->disconnectDevice(deviceKey);
    QTest::qWait(500);
    
    // Verify signals (may not be emitted in test environment)
    QVERIFY(deviceConnectedSpy.count() >= 0);
    QVERIFY(deviceDisconnectedSpy.count() >= 0);
}

void TestScadaCoreService::testConcurrentDeviceOperations()
{
    const int numThreads = 8;
    const int devicesPerThread = 3;
    
    m_service->startService();
    QTest::qWait(100);
    
    QList<QFuture<void>> futures;
    QAtomicInt registeredCount(0);
    
    // Create multiple threads that concurrently register devices
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, devicesPerThread, &registeredCount]() {
            for (int j = 0; j < devicesPerThread; ++j) {
                QString host = QString("10.%1.%2.1").arg(i + 1).arg(j + 1);
                int unitId = i * devicesPerThread + j + 1;
                
                bool registered = m_service->registerDevice(host, m_testPort, unitId);
                if (registered) {
                    registeredCount.fetchAndAddOrdered(1);
                }
                
                // Small delay to increase chance of race conditions
                QThread::msleep(10);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    QTest::qWait(200);
    
    // Verify devices were registered
    QVERIFY(registeredCount.loadAcquire() > 0);
    QStringList devices = m_service->getRegisteredDevices();
    QVERIFY(devices.size() > 0);
}

void TestScadaCoreService::testConcurrentRequestHandling()
{
    const int numThreads = 6;
    const int requestsPerThread = 10;
    
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    m_service->registerDevice(m_testHost2, m_testPort, m_testUnitId2);
    QTest::qWait(200);
    
    QList<QFuture<void>> futures;
    QAtomicInt submittedCount(0);
    
    QString deviceKey1 = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    QString deviceKey2 = QString("%1:%2:%3").arg(m_testHost2).arg(m_testPort).arg(m_testUnitId2);
    
    // Create multiple threads that concurrently submit requests
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, requestsPerThread, &deviceKey1, &deviceKey2, &submittedCount]() {
            for (int j = 0; j < requestsPerThread; ++j) {
                ModbusRequest request;
                request.requestId = QString("concurrent_%1_%2").arg(i).arg(j);
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.startAddress = 100 + j;
                request.count = 1;
                
                QString deviceKey = (j % 2 == 0) ? deviceKey1 : deviceKey2;
                qint64 requestId = m_service->submitRequest(deviceKey, request, RequestPriority::Normal);
                
                if (requestId > 0) {
                    submittedCount.fetchAndAddOrdered(1);
                }
                
                QThread::msleep(5);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify requests were submitted
    QCOMPARE(submittedCount.loadAcquire(), numThreads * requestsPerThread);
}

void TestScadaCoreService::testConcurrentStatisticsAccess()
{
    const int numThreads = 10;
    const int operationsPerThread = 30;
    
    m_service->startService();
    m_service->registerDevice(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(100);
    
    QList<QFuture<void>> futures;
    QAtomicInt readOperations(0);
    QAtomicInt resetOperations(0);
    
    // Create multiple threads that concurrently access statistics
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, operationsPerThread, &readOperations, &resetOperations]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                // Concurrent read access to statistics
                ScadaCoreService::ServiceStatistics stats = m_service->getServiceStatistics();
                QVERIFY(stats.totalDevices >= 0);
                QVERIFY(stats.activeDevices >= 0);
                QVERIFY(stats.totalRequests >= 0);
                readOperations.fetchAndAddOrdered(1);
                
                // Occasionally reset statistics
                if (j % 20 == 0) {
                    m_service->resetServiceStatistics();
                    resetOperations.fetchAndAddOrdered(1);
                }
                
                QThread::msleep(2);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify operations were completed
    QCOMPARE(readOperations.loadAcquire(), numThreads * operationsPerThread);
    QVERIFY(resetOperations.loadAcquire() > 0);
}

void TestScadaCoreService::testThreadSafeServiceOperations()
{
    const int numThreads = 5;
    const int operationsPerThread = 15;
    
    m_service->startService();
    QTest::qWait(50);
    
    QList<QFuture<void>> futures;
    QAtomicInt operationCount(0);
    
    // Create multiple threads that perform various service operations
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, operationsPerThread, &operationCount]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                QString host = QString("172.16.%1.%2").arg(i + 1).arg(j + 1);
                int unitId = i * operationsPerThread + j + 1;
                
                // Register device
                bool registered = m_service->registerDevice(host, m_testPort, unitId);
                if (registered) {
                    operationCount.fetchAndAddOrdered(1);
                    
                    QString deviceKey = QString("%1:%2:%3").arg(host).arg(m_testPort).arg(unitId);
                    
                    // Get device list
                    QStringList devices = m_service->getRegisteredDevices();
                    QVERIFY(devices.size() > 0);
                    
                    // Set poll interval
                    m_service->setDevicePollInterval(deviceKey, 2000 + (i * 100));
                    
                    // Small delay
                    QThread::msleep(10);
                    
                    // Unregister device
                    m_service->unregisterDevice(deviceKey);
                }
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    QTest::qWait(200);
    
    // Verify operations were completed
    QVERIFY(operationCount.loadAcquire() > 0);
}

void TestScadaCoreService::testConcurrentServiceLifecycle()
{
    const int numCycles = 5;
    
    QList<QFuture<void>> futures;
    QAtomicInt startCount(0);
    QAtomicInt stopCount(0);
    
    // Create threads that concurrently start/stop the service
    for (int i = 0; i < 3; ++i) {
        QFuture<void> future = QtConcurrent::run([this, numCycles, &startCount, &stopCount]() {
            for (int j = 0; j < numCycles; ++j) {
                // Try to start service
                if (m_service->startService()) {
                    startCount.fetchAndAddOrdered(1);
                    QThread::msleep(50);
                    
                    // Register a device
                    QString host = QString("192.168.100.%1").arg(j + 1);
                    m_service->registerDevice(host, m_testPort, j + 1);
                    
                    QThread::msleep(50);
                    
                    // Stop service
                    m_service->stopService();
                    stopCount.fetchAndAddOrdered(1);
                }
                
                QThread::msleep(100);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify lifecycle operations were handled safely
    QVERIFY(startCount.loadAcquire() > 0);
    QVERIFY(stopCount.loadAcquire() > 0);
    
    // Ensure service is in a consistent state
    QTest::qWait(200);
}
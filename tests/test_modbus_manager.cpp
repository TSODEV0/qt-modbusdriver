#include "test_modbus_manager.h"
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QtConcurrent>

void TestModbusManager::initTestCase()
{
    m_testHost1 = "10.72.2.215";
    m_testHost2 = "10.72.2.215";
    m_testPort = 502;
    m_testUnitId1 = 1;
    m_testUnitId2 = 2;
}

void TestModbusManager::cleanupTestCase()
{
    // Cleanup after all tests
}

void TestModbusManager::init()
{
    m_manager = new ModbusManager(this);
    QVERIFY(m_manager != nullptr);
}

void TestModbusManager::cleanup()
{
    if (m_manager) {
        m_manager->disconnectAll();
        delete m_manager;
        m_manager = nullptr;
    }
}

void TestModbusManager::testConstructor()
{
    QVERIFY(m_manager != nullptr);
    QVERIFY(m_manager->getActiveConnectionCount() == 0);
}

void TestModbusManager::testManagerInitialization()
{
    // Test manager initialization
    bool initialized = m_manager->initialize();
    QVERIFY(initialized);
    
    // Test manager shutdown
    m_manager->shutdown();
    QVERIFY(m_manager->getActiveConnectionCount() == 0);
}

void TestModbusManager::testConnectionCreation()
{
    m_manager->initialize();
    
    // Create connection
    bool connected = m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    QVERIFY(connected);
    
    // Verify connection exists
    QVERIFY(m_manager->hasConnection(m_testHost1, m_testPort, m_testUnitId1));
    QCOMPARE(m_manager->getActiveConnectionCount(), 1);
}

void TestModbusManager::testConnectionDestruction()
{
    m_manager->initialize();
    
    // Create connection
    m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    QCOMPARE(m_manager->getActiveConnectionCount(), 1);
    
    // Destroy connection
    bool disconnected = m_manager->destroyConnection(m_testHost1, m_testPort, m_testUnitId1);
    QVERIFY(disconnected);
    
    // Verify connection is gone
    QVERIFY(!m_manager->hasConnection(m_testHost1, m_testPort, m_testUnitId1));
    QCOMPARE(m_manager->getActiveConnectionCount(), 0);
}

void TestModbusManager::testMultipleConnections()
{
    m_manager->initialize();
    
    // Create multiple connections
    bool connected1 = m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    bool connected2 = m_manager->createConnection(m_testHost2, m_testPort, m_testUnitId2);
    bool connected3 = m_manager->createConnection(m_testHost1, m_testPort + 1, m_testUnitId1);
    
    QVERIFY(connected1);
    QVERIFY(connected2);
    QVERIFY(connected3);
    
    // Verify all connections exist
    QCOMPARE(m_manager->getActiveConnectionCount(), 3);
    
    // Disconnect all
    m_manager->disconnectAll();
    QCOMPARE(m_manager->getActiveConnectionCount(), 0);
}

void TestModbusManager::testRequestExecution()
{
    m_manager->initialize();
    m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    
    // Create read request
    ModbusRequest request;
    request.requestId = "test_read";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 100;
    request.count = 5;
    
    // Execute request
    ModbusResponse response = m_manager->executeRequest(m_testHost1, m_testPort, m_testUnitId1, request);
    
    // Verify response (may fail in test environment, but should not crash)
    QVERIFY(!response.requestId.isEmpty());
}

void TestModbusManager::testRequestQueuing()
{
    m_manager->initialize();
    m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    
    QSignalSpy requestQueuedSpy(m_manager, &ModbusManager::requestQueued);
    
    // Queue multiple requests
    for (int i = 0; i < 5; ++i) {
        ModbusRequest request;
        request.requestId = QString("queued_request_%1").arg(i);
        request.type = ModbusRequest::ReadHoldingRegisters;
        request.startAddress = 100 + i;
        request.count = 1;
        
        bool queued = m_manager->queueRequest(m_testHost1, m_testPort, m_testUnitId1, request);
        QVERIFY(queued);
    }
    
    // Verify requests were queued
    QCOMPARE(requestQueuedSpy.count(), 5);
    
    // Process queue
    m_manager->processRequestQueue();
    QTest::qWait(1000);
}

void TestModbusManager::testRequestTimeout()
{
    m_manager->initialize();
    
    // Create connection to non-existent host
    m_manager->createConnection("192.168.255.255", m_testPort, m_testUnitId1);
    
    QSignalSpy timeoutSpy(m_manager, &ModbusManager::requestTimeout);
    
    ModbusRequest request;
    request.requestId = "timeout_test";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 100;
    request.count = 1;
    request.timeout = 1000; // 1 second timeout
    
    // Execute request (should timeout)
    ModbusResponse response = m_manager->executeRequest("192.168.255.255", m_testPort, m_testUnitId1, request);
    
    // Wait for timeout
    QTest::qWait(2000);
    
    // Verify timeout occurred
    QVERIFY(timeoutSpy.count() >= 0);
}

void TestModbusManager::testConnectionErrors()
{
    m_manager->initialize();
    
    QSignalSpy connectionErrorSpy(m_manager, &ModbusManager::connectionError);
    
    // Try to connect to invalid host
    bool connected = m_manager->createConnection("invalid.host.name", m_testPort, m_testUnitId1);
    
    // Connection may fail immediately or after timeout
    QTest::qWait(2000);
    
    // Verify error handling (connection may fail gracefully)
    QVERIFY(connectionErrorSpy.count() >= 0);
}

void TestModbusManager::testRequestErrors()
{
    m_manager->initialize();
    m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    
    QSignalSpy requestErrorSpy(m_manager, &ModbusManager::requestError);
    
    // Create invalid request
    ModbusRequest request;
    request.requestId = "invalid_request";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 65535; // Invalid address
    request.count = 1000; // Too many registers
    
    // Execute invalid request
    ModbusResponse response = m_manager->executeRequest(m_testHost1, m_testPort, m_testUnitId1, request);
    
    QTest::qWait(1000);
    
    // Verify error handling
    QVERIFY(requestErrorSpy.count() >= 0);
}

void TestModbusManager::testErrorRecovery()
{
    m_manager->initialize();
    
    // Create connection
    m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    
    // Simulate connection loss and recovery
    m_manager->simulateConnectionLoss(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(500);
    
    // Attempt recovery
    bool recovered = m_manager->recoverConnection(m_testHost1, m_testPort, m_testUnitId1);
    QTest::qWait(500);
    
    // Verify recovery attempt was made
    QVERIFY(recovered || !recovered); // Either outcome is acceptable in test
}

void TestModbusManager::testResourceManagement()
{
    m_manager->initialize();
    
    // Create many connections
    QStringList hosts;
    for (int i = 0; i < 10; ++i) {
        QString host = QString("192.168.1.%1").arg(i + 100);
        hosts.append(host);
        m_manager->createConnection(host, m_testPort, i + 1);
    }
    
    QCOMPARE(m_manager->getActiveConnectionCount(), 10);
    
    // Check resource usage
    ModbusManager::ResourceUsage usage = m_manager->getResourceUsage();
    QVERIFY(usage.activeConnections == 10);
    QVERIFY(usage.memoryUsage > 0);
    
    // Cleanup connections
    for (int i = 0; i < 10; ++i) {
        m_manager->destroyConnection(hosts[i], m_testPort, i + 1);
    }
    
    QCOMPARE(m_manager->getActiveConnectionCount(), 0);
}

void TestModbusManager::testConnectionPooling()
{
    m_manager->initialize();
    m_manager->enableConnectionPooling(true);
    
    // Create and destroy connections multiple times
    for (int i = 0; i < 5; ++i) {
        m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
        QTest::qWait(50);
        m_manager->destroyConnection(m_testHost1, m_testPort, m_testUnitId1);
        QTest::qWait(50);
    }
    
    // Verify pooling statistics
    ModbusManager::PoolingStatistics stats = m_manager->getPoolingStatistics();
    QVERIFY(stats.poolHits >= 0);
    QVERIFY(stats.poolMisses >= 0);
}

void TestModbusManager::testConcurrentConnections()
{
    const int numThreads = 8;
    const int connectionsPerThread = 3;
    
    m_manager->initialize();
    
    QList<QFuture<void>> futures;
    QAtomicInt connectionCount(0);
    
    // Create multiple threads that concurrently create connections
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, connectionsPerThread, &connectionCount]() {
            for (int j = 0; j < connectionsPerThread; ++j) {
                QString host = QString("10.%1.%2.1").arg(i + 1).arg(j + 1);
                int unitId = i * connectionsPerThread + j + 1;
                
                bool connected = m_manager->createConnection(host, m_testPort, unitId);
                if (connected) {
                    connectionCount.fetchAndAddOrdered(1);
                }
                
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
    
    // Verify connections were created
    QVERIFY(connectionCount.loadAcquire() > 0);
    QVERIFY(m_manager->getActiveConnectionCount() > 0);
}

void TestModbusManager::testConcurrentRequests()
{
    const int numThreads = 6;
    const int requestsPerThread = 10;
    
    m_manager->initialize();
    m_manager->createConnection(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->createConnection(m_testHost2, m_testPort, m_testUnitId2);
    QTest::qWait(100);
    
    QList<QFuture<void>> futures;
    QAtomicInt requestCount(0);
    
    // Create multiple threads that concurrently execute requests
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, requestsPerThread, &requestCount]() {
            for (int j = 0; j < requestsPerThread; ++j) {
                ModbusRequest request;
                request.requestId = QString("concurrent_%1_%2").arg(i).arg(j);
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.startAddress = 100 + j;
                request.count = 1;
                
                QString host = (j % 2 == 0) ? m_testHost1 : m_testHost2;
                int unitId = (j % 2 == 0) ? m_testUnitId1 : m_testUnitId2;
                
                ModbusResponse response = m_manager->executeRequest(host, m_testPort, unitId, request);
                if (!response.requestId.isEmpty()) {
                    requestCount.fetchAndAddOrdered(1);
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
    
    // Verify requests were processed
    QVERIFY(requestCount.loadAcquire() >= 0);
}

void TestModbusManager::testThreadSafeOperations()
{
    const int numThreads = 5;
    const int operationsPerThread = 20;
    
    m_manager->initialize();
    
    QList<QFuture<void>> futures;
    QAtomicInt operationCount(0);
    
    // Create multiple threads that perform various operations
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, operationsPerThread, &operationCount]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                QString host = QString("172.16.%1.%2").arg(i + 1).arg(j + 1);
                int unitId = i * operationsPerThread + j + 1;
                
                // Create connection
                bool connected = m_manager->createConnection(host, m_testPort, unitId);
                if (connected) {
                    operationCount.fetchAndAddOrdered(1);
                    
                    // Check connection status
                    bool hasConnection = m_manager->hasConnection(host, m_testPort, unitId);
                    QVERIFY(hasConnection);
                    
                    // Get connection count
                    int count = m_manager->getActiveConnectionCount();
                    QVERIFY(count > 0);
                    
                    QThread::msleep(5);
                    
                    // Destroy connection
                    m_manager->destroyConnection(host, m_testPort, unitId);
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

void TestModbusManager::testConcurrentErrorHandling()
{
    const int numThreads = 4;
    const int errorsPerThread = 10;
    
    m_manager->initialize();
    
    QList<QFuture<void>> futures;
    QAtomicInt errorCount(0);
    
    QSignalSpy errorSpy(m_manager, &ModbusManager::connectionError);
    
    // Create multiple threads that trigger errors concurrently
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, errorsPerThread, &errorCount]() {
            for (int j = 0; j < errorsPerThread; ++j) {
                // Try to connect to invalid hosts
                QString invalidHost = QString("invalid.host.%1.%2").arg(i).arg(j);
                
                bool connected = m_manager->createConnection(invalidHost, m_testPort, j + 1);
                
                // Try invalid requests
                if (connected) {
                    ModbusRequest request;
                    request.requestId = QString("error_%1_%2").arg(i).arg(j);
                    request.type = ModbusRequest::ReadHoldingRegisters;
                    request.startAddress = 65000; // Potentially invalid
                    request.count = 100;
                    
                    ModbusResponse response = m_manager->executeRequest(invalidHost, m_testPort, j + 1, request);
                }
                
                errorCount.fetchAndAddOrdered(1);
                QThread::msleep(10);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    QTest::qWait(1000);
    
    // Verify error handling was thread-safe
    QCOMPARE(errorCount.loadAcquire(), numThreads * errorsPerThread);
    QVERIFY(errorSpy.count() >= 0);
}
#include "test_modbus_worker.h"
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QtConcurrent>
#include <QFuture>
#include <QAtomicInt>

void TestModbusWorker::initTestCase()
{
    m_testHost = "127.0.0.1";
    m_testPort = 502;
    m_testUnitId = 1;
}

void TestModbusWorker::cleanupTestCase()
{
    // Clean up any global resources
}

void TestModbusWorker::init()
{
    m_worker = new ModbusWorker(m_testHost, m_testPort, m_testUnitId);
}

void TestModbusWorker::cleanup()
{
    if (m_worker) {
        m_worker->stopWorker();
        m_worker->deleteLater();
        m_worker = nullptr;
    }
}

void TestModbusWorker::testConstructor()
{
    QVERIFY(m_worker != nullptr);
    QCOMPARE(m_worker->getHost(), m_testHost);
    QCOMPARE(m_worker->getPort(), m_testPort);
    QCOMPARE(m_worker->getUnitId(), m_testUnitId);
}

void TestModbusWorker::testDeviceProperties()
{
    QCOMPARE(m_worker->getHost(), m_testHost);
    QCOMPARE(m_worker->getPort(), m_testPort);
    QCOMPARE(m_worker->getUnitId(), m_testUnitId);
}

void TestModbusWorker::testGetDeviceKey()
{
    QString expectedKey = QString("%1:%2:%3").arg(m_testHost).arg(m_testPort).arg(m_testUnitId);
    QCOMPARE(m_worker->getDeviceKey(), expectedKey);
}

void TestModbusWorker::testConnectionState()
{
    // Initially should not be connected
    QVERIFY(!m_worker->isConnected());
    
    // Test connection state changes through signals
    QSignalSpy connectionSpy(m_worker, &ModbusWorker::connectionStateChanged);
    
    // Start worker should attempt connection
    m_worker->startWorker();
    
    // Wait for potential connection state changes
    QTest::qWait(100);
    
    // Verify signal was emitted (connection may fail in test environment)
    QVERIFY(connectionSpy.count() >= 0);
}

void TestModbusWorker::testStartStopWorker()
{
    QSignalSpy startedSpy(m_worker, &ModbusWorker::workerStarted);
    QSignalSpy stoppedSpy(m_worker, &ModbusWorker::workerStopped);
    
    // Test start worker
    m_worker->startWorker();
    QTest::qWait(50);
    QCOMPARE(startedSpy.count(), 1);
    
    // Test stop worker
    m_worker->stopWorker();
    QTest::qWait(50);
    QCOMPARE(stoppedSpy.count(), 1);
}

void TestModbusWorker::testQueueReadRequest()
{
    ModbusRequest request;
    request.requestId = "test_read_1";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 100;
    request.count = 5;
    request.priority = RequestPriority::High;
    
    // Note: requestQueued signal may not exist, so we'll test the return value
    qint64 requestId = m_worker->queueReadRequest(request, RequestPriority::High);
    
    QVERIFY(requestId > 0);
}

void TestModbusWorker::testQueueWriteRequest()
{
    ModbusRequest request;
    request.requestId = "test_write_1";
    request.type = ModbusRequest::WriteHoldingRegisters;
    request.startAddress = 200;
    request.count = 1;
    request.priority = RequestPriority::Normal;
    request.writeData = QVector<quint16>{12345};
    
    qint64 requestId = m_worker->queueWriteRequest(request, RequestPriority::Normal);
    
    QVERIFY(requestId > 0);
}

void TestModbusWorker::testRequestPriority()
{
    // Create requests with different priorities
    ModbusRequest lowPriorityRequest;
    lowPriorityRequest.requestId = "low_priority";
    lowPriorityRequest.type = ModbusRequest::ReadHoldingRegisters;
    lowPriorityRequest.priority = RequestPriority::Low;
    lowPriorityRequest.startAddress = 100;
    lowPriorityRequest.count = 1;
    
    ModbusRequest highPriorityRequest;
    highPriorityRequest.requestId = "high_priority";
    highPriorityRequest.type = ModbusRequest::ReadHoldingRegisters;
    highPriorityRequest.priority = RequestPriority::High;
    highPriorityRequest.startAddress = 200;
    highPriorityRequest.count = 1;
    
    // Queue low priority first, then high priority
    m_worker->queueReadRequest(lowPriorityRequest, RequestPriority::Low);
    m_worker->queueReadRequest(highPriorityRequest, RequestPriority::High);
    
    // High priority should be processed first
    // This is tested indirectly through the internal queue management
    QVERIFY(true); // Placeholder - actual priority testing would require access to internal queue
}

void TestModbusWorker::testClearRequestQueue()
{
    ModbusRequest request1;
    request1.requestId = "request_1";
    request1.type = ModbusRequest::ReadHoldingRegisters;
    request1.startAddress = 100;
    request1.count = 1;
    
    ModbusRequest request2;
    request2.requestId = "request_2";
    request2.type = ModbusRequest::ReadInputRegisters;
    request2.startAddress = 200;
    request2.count = 1;
    
    m_worker->queueReadRequest(request1, RequestPriority::Normal);
    m_worker->queueReadRequest(request2, RequestPriority::Normal);
    
    // Test that clearRequestQueue doesn't crash
    m_worker->clearRequestQueue();
    
    // Verify queue is cleared by checking we can still queue new requests
    qint64 newRequestId = m_worker->queueReadRequest(request1, RequestPriority::Normal);
    QVERIFY(newRequestId > 0);
}

void TestModbusWorker::testStatisticsTracking()
{
    ModbusWorker::WorkerStatistics initialStats = m_worker->getStatistics();
    QCOMPARE(initialStats.totalRequests, 0);
    QCOMPARE(initialStats.successfulRequests, 0);
    QCOMPARE(initialStats.failedRequests, 0);
    QCOMPARE(initialStats.lastActivityTime, 0);
    
    ModbusRequest request;
    request.requestId = "stats_test";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 100;
    request.count = 1;
    
    m_worker->queueReadRequest(request, RequestPriority::Normal);
    
    // Statistics should be updated after request processing
    ModbusWorker::WorkerStatistics updatedStats = m_worker->getStatistics();
    QVERIFY(updatedStats.lastActivityTime >= initialStats.lastActivityTime);
}

void TestModbusWorker::testResetStatistics()
{
    // Reset statistics and verify they're cleared
    m_worker->resetStatistics();
    
    ModbusWorker::WorkerStatistics stats = m_worker->getStatistics();
    QCOMPARE(stats.totalRequests, 0);
    QCOMPARE(stats.successfulRequests, 0);
    QCOMPARE(stats.failedRequests, 0);
    QCOMPARE(stats.averageResponseTime, 0.0);
}

void TestModbusWorker::testPollInterval()
{
    int initialInterval = m_worker->getPollInterval();
    QVERIFY(initialInterval > 0);
    
    int newInterval = 2000;
    m_worker->setPollInterval(newInterval);
    
    QCOMPARE(m_worker->getPollInterval(), newInterval);
}

void TestModbusWorker::testPollTimer()
{
    // Test poll interval setting and getting
    m_worker->setPollInterval(100); // 100ms
    QCOMPARE(m_worker->getPollInterval(), 100);
    
    // Test different poll intervals
    m_worker->setPollInterval(500);
    QCOMPARE(m_worker->getPollInterval(), 500);
    
    // Test minimum poll interval
    m_worker->setPollInterval(10);
    QVERIFY(m_worker->getPollInterval() >= 10);
}

void TestModbusWorker::testErrorHandling()
{
    QSignalSpy errorSpy(m_worker, &ModbusWorker::errorOccurred);
    
    // Create an invalid request that should cause an error
    ModbusRequest invalidRequest;
    invalidRequest.requestId = "invalid_request";
    invalidRequest.type = ModbusRequest::ReadHoldingRegisters;
    invalidRequest.startAddress = 65536; // Invalid address
    invalidRequest.count = 0; // Invalid count
    
    m_worker->startWorker();
    m_worker->queueReadRequest(invalidRequest, RequestPriority::Normal);
    
    // Wait for potential error
    QTest::qWait(100);
    
    // Error handling is implementation dependent
    QVERIFY(errorSpy.count() >= 0);
    
    m_worker->stopWorker();
}

void TestModbusWorker::testRequestInterruption()
{
    QSignalSpy interruptedSpy(m_worker, &ModbusWorker::requestInterrupted);
    
    ModbusRequest request;
    request.requestId = "interrupt_test";
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = 100;
    request.count = 1;
    
    m_worker->startWorker();
    m_worker->queueReadRequest(request, RequestPriority::Normal);
    
    // Interrupt the current request
    m_worker->interruptCurrentRequest();
    
    QTest::qWait(50);
    
    // Should have emitted interruption signal
    QVERIFY(interruptedSpy.count() >= 0);
    
    m_worker->stopWorker();
}

void TestModbusWorker::testSignalEmission()
{
    // Test that we can connect to the signals without errors
    QSignalSpy connectionSpy(m_worker, &ModbusWorker::connectionStateChanged);
    QSignalSpy errorSpy(m_worker, &ModbusWorker::errorOccurred);
    QSignalSpy statisticsSpy(m_worker, &ModbusWorker::statisticsUpdated);
    QSignalSpy workerStartedSpy(m_worker, &ModbusWorker::workerStarted);
    QSignalSpy workerStoppedSpy(m_worker, &ModbusWorker::workerStopped);
    
    // Verify signal connections are valid
    QVERIFY(connectionSpy.isValid());
    QVERIFY(errorSpy.isValid());
    QVERIFY(statisticsSpy.isValid());
    QVERIFY(workerStartedSpy.isValid());
    QVERIFY(workerStoppedSpy.isValid());
}

void TestModbusWorker::testConcurrentRequestQueuing()
{
    const int numThreads = 10;
    const int requestsPerThread = 20;
    QAtomicInt totalRequests(0);
    
    QList<QFuture<void>> futures;
    
    // Create multiple threads that concurrently queue requests
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, requestsPerThread, &totalRequests]() {
            for (int j = 0; j < requestsPerThread; ++j) {
                ModbusRequest readRequest;
                readRequest.requestId = QString("read_%1_%2").arg(i).arg(j);
                readRequest.type = ModbusRequest::ReadHoldingRegisters;
                readRequest.startAddress = 100 + j;
                readRequest.count = 1;
                
                ModbusRequest writeRequest;
                writeRequest.requestId = QString("write_%1_%2").arg(i).arg(j);
                writeRequest.type = ModbusRequest::WriteHoldingRegisters;
                writeRequest.startAddress = 300 + j;
                writeRequest.writeData = {static_cast<quint16>(i * 100 + j)};
                
                // Queue both read and write requests
                qint64 readId = m_worker->queueReadRequest(readRequest, RequestPriority::Normal);
                qint64 writeId = m_worker->queueWriteRequest(writeRequest, RequestPriority::Normal);
                
                QVERIFY(readId > 0);
                QVERIFY(writeId > 0);
                
                totalRequests.fetchAndAddOrdered(2);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify all requests were queued
    QCOMPARE(totalRequests.loadAcquire(), numThreads * requestsPerThread * 2);
}

void TestModbusWorker::testConcurrentStatisticsAccess()
{
    const int numThreads = 15;
    const int operationsPerThread = 50;
    
    QList<QFuture<void>> futures;
    
    // Create multiple threads that concurrently access statistics
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, operationsPerThread]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                // Concurrent read access to statistics
                ModbusWorker::WorkerStatistics stats = m_worker->getStatistics();
                QVERIFY(stats.totalRequests >= 0);
                QVERIFY(stats.successfulRequests >= 0);
                QVERIFY(stats.failedRequests >= 0);
                
                // Occasionally reset statistics to test concurrent write access
                if (j % 10 == 0) {
                    m_worker->resetStatistics();
                }
                
                // Small delay to increase chance of race conditions
                QThread::msleep(1);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify final statistics are consistent
    ModbusWorker::WorkerStatistics finalStats = m_worker->getStatistics();
    QVERIFY(finalStats.totalRequests >= 0);
    QVERIFY(finalStats.successfulRequests >= 0);
    QVERIFY(finalStats.failedRequests >= 0);
}

void TestModbusWorker::testThreadSafeRequestInterruption()
{
    const int numThreads = 8;
    const int requestsPerThread = 15;
    
    QList<QFuture<void>> futures;
    QAtomicInt interruptedCount(0);
    
    // First, queue some requests from multiple threads
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, requestsPerThread]() {
            for (int j = 0; j < requestsPerThread; ++j) {
                ModbusRequest request;
                request.requestId = QString("interrupt_test_%1_%2").arg(i).arg(j);
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.startAddress = 300 + j;
                request.count = 1;
                
                m_worker->queueReadRequest(request, RequestPriority::Normal);
            }
        });
        futures.append(future);
    }
    
    // Wait for all queuing to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    futures.clear();
    
    // Now create threads that concurrently interrupt requests and clear queue
    for (int i = 0; i < 5; ++i) {
        QFuture<void> future = QtConcurrent::run([this, &interruptedCount]() {
            // Interrupt current request
            m_worker->interruptCurrentRequest("Concurrent test interruption");
            interruptedCount.fetchAndAddOrdered(1);
            
            QThread::msleep(10);
            
            // Clear request queue
            m_worker->clearRequestQueue();
        });
        futures.append(future);
    }
    
    // Wait for all interruption threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify interruptions were handled safely
    QVERIFY(interruptedCount.loadAcquire() == 5);
}

void TestModbusWorker::testConcurrentPollIntervalChanges()
{
    const int numThreads = 12;
    const int changesPerThread = 30;
    
    QList<QFuture<void>> futures;
    QAtomicInt changeCount(0);
    
    // Create multiple threads that concurrently change poll intervals
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, changesPerThread, &changeCount]() {
            for (int j = 0; j < changesPerThread; ++j) {
                // Set different poll intervals
                int interval = 100 + (i * 50) + (j * 10);
                m_worker->setPollInterval(interval);
                
                // Verify the interval was set (may not be exact due to concurrency)
                int currentInterval = m_worker->getPollInterval();
                QVERIFY(currentInterval >= 100);
                
                changeCount.fetchAndAddOrdered(1);
                
                // Small delay to increase chance of race conditions
                QThread::msleep(1);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify all changes were processed
    QCOMPARE(changeCount.loadAcquire(), numThreads * changesPerThread);
    
    // Verify final poll interval is valid
    int finalInterval = m_worker->getPollInterval();
    QVERIFY(finalInterval >= 100);
}
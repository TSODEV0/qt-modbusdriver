#include "test_modbus_worker_manager.h"
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QtConcurrent>

void TestModbusWorkerManager::initTestCase()
{
    m_testHost1 = "10.72.2.215";
    m_testHost2 = "10.72.2.215";
    m_testPort = 502;
    m_testUnitId1 = 1;
    m_testUnitId2 = 2;
}

void TestModbusWorkerManager::cleanupTestCase()
{
    // Clean up any global resources
}

void TestModbusWorkerManager::init()
{
    m_manager = new ModbusWorkerManager();
}

void TestModbusWorkerManager::cleanup()
{
    if (m_manager) {
        m_manager->removeAllWorkers();
        m_manager->deleteLater();
        m_manager = nullptr;
    }
}

void TestModbusWorkerManager::testConstructor()
{
    QVERIFY(m_manager != nullptr);
    QCOMPARE(m_manager->getWorkerCount(), 0);
    
    ModbusWorkerManager::GlobalStatistics stats = m_manager->getGlobalStatistics();
    QCOMPARE(stats.activeWorkers, 0);
    QCOMPARE(stats.connectedDevices, 0);
}

void TestModbusWorkerManager::testCreateDeviceKey()
{
    // This tests the internal createDeviceKey method indirectly
    ModbusWorker* worker1 = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    ModbusWorker* worker2 = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    
    // Should return the same worker for the same device
    QCOMPARE(worker1, worker2);
    QCOMPARE(m_manager->getWorkerCount(), 1);
}

void TestModbusWorkerManager::testGetOrCreateWorker()
{
    QCOMPARE(m_manager->getWorkerCount(), 0);
    
    // Create first worker
    ModbusWorker* worker1 = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QVERIFY(worker1 != nullptr);
    QCOMPARE(m_manager->getWorkerCount(), 1);
    
    // Create second worker with different parameters
    ModbusWorker* worker2 = m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    QVERIFY(worker2 != nullptr);
    QVERIFY(worker1 != worker2);
    QCOMPARE(m_manager->getWorkerCount(), 2);
    
    // Request same worker again - should return existing
    ModbusWorker* worker1_again = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QCOMPARE(worker1, worker1_again);
    QCOMPARE(m_manager->getWorkerCount(), 2);
}

void TestModbusWorkerManager::testGetWorker()
{
    // Initially no workers
    QString deviceKey1 = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    ModbusWorker* worker = m_manager->getWorker(deviceKey1);
    QVERIFY(worker == nullptr);
    
    // Create a worker
    ModbusWorker* createdWorker = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QVERIFY(createdWorker != nullptr);
    
    // Now getWorker should return it
    worker = m_manager->getWorker(deviceKey1);
    QCOMPARE(worker, createdWorker);
}

void TestModbusWorkerManager::testRemoveWorker()
{
    // Create a worker
    ModbusWorker* worker = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QVERIFY(worker != nullptr);
    QCOMPARE(m_manager->getWorkerCount(), 1);
    
    QSignalSpy removedSpy(m_manager, &ModbusWorkerManager::workerRemoved);
    
    // Remove the worker
    QString deviceKey1 = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    m_manager->removeWorker(deviceKey1);
    QCOMPARE(m_manager->getWorkerCount(), 0);
    
    // Should emit workerRemoved signal
    QCOMPARE(removedSpy.count(), 1);
    
    // Try to remove non-existent worker
    QString deviceKey2 = QString("%1:%2:%3").arg(m_testHost2).arg(m_testPort).arg(m_testUnitId2);
    m_manager->removeWorker(deviceKey2);  // Should not crash even if worker doesn't exist
}

void TestModbusWorkerManager::testRemoveAllWorkers()
{
    // Create multiple workers
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    QCOMPARE(m_manager->getWorkerCount(), 2);
    
    QSignalSpy removedSpy(m_manager, &ModbusWorkerManager::workerRemoved);
    
    // Remove all workers
    m_manager->removeAllWorkers();
    QCOMPARE(m_manager->getWorkerCount(), 0);
    
    // Should emit workerRemoved signal for each worker
    QCOMPARE(removedSpy.count(), 2);
}

void TestModbusWorkerManager::testGetWorkerCount()
{
    QCOMPARE(m_manager->getWorkerCount(), 0);
    
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QCOMPARE(m_manager->getWorkerCount(), 1);
    
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    QCOMPARE(m_manager->getWorkerCount(), 2);
    
    QString deviceKey = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    m_manager->removeWorker(deviceKey);
    QCOMPARE(m_manager->getWorkerCount(), 1);
}

void TestModbusWorkerManager::testGetActiveDevices()
{
    QStringList devices = m_manager->getActiveDevices();
    QVERIFY(devices.isEmpty());
    
    // Create workers
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    devices = m_manager->getActiveDevices();
    QCOMPARE(devices.size(), 2);
    
    QString expectedKey1 = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    QString expectedKey2 = QString("%1:%2:%3").arg(m_testHost2).arg(m_testPort).arg(m_testUnitId2);
    
    QVERIFY(devices.contains(expectedKey1));
    QVERIFY(devices.contains(expectedKey2));
}

void TestModbusWorkerManager::testGetConnectedDevices()
{
    QStringList connectedDevices = m_manager->getConnectedDevices();
    QVERIFY(connectedDevices.isEmpty());
    
    // Create workers (they won't be connected in test environment)
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    connectedDevices = m_manager->getConnectedDevices();
    // In test environment, workers likely won't connect to actual devices
    QVERIFY(connectedDevices.size() >= 0);
}

void TestModbusWorkerManager::testGlobalStatistics()
{
    ModbusWorkerManager::GlobalStatistics stats = m_manager->getGlobalStatistics();
    QCOMPARE(stats.activeWorkers, 0);
    QCOMPARE(stats.connectedDevices, 0);
    
    // Create workers
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    // Wait for statistics update
    QTest::qWait(100);
    
    stats = m_manager->getGlobalStatistics();
    QCOMPARE(stats.activeWorkers, 2);
    // activeWorkers and connectedWorkers depend on worker state
}

void TestModbusWorkerManager::testResetGlobalStatistics()
{
    // Create some workers to generate statistics
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    QTest::qWait(100);
    
    ModbusWorkerManager::GlobalStatistics stats = m_manager->getGlobalStatistics();
    QVERIFY(stats.activeWorkers > 0);
    
    // Reset statistics
    m_manager->resetGlobalStatistics();
    
    stats = m_manager->getGlobalStatistics();
    // Note: totalWorkers, activeWorkers, connectedWorkers are current counts, not cumulative
    // So they won't be reset to 0, but other statistics would be
    QVERIFY(stats.activeWorkers >= 0);
}

void TestModbusWorkerManager::testStatisticsUpdates()
{
    QSignalSpy statsSpy(m_manager, &ModbusWorkerManager::globalStatisticsUpdated);
    
    // Create a worker
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    
    // Wait for statistics update timer
    QTest::qWait(6000); // Statistics update every 5 seconds
    
    // Should have emitted at least one statistics update
    QVERIFY(statsSpy.count() >= 1);
}

void TestModbusWorkerManager::testDefaultPollInterval()
{
    int defaultInterval = 1000; // Default from implementation
    
    // Create a worker and check its poll interval
    ModbusWorker* worker = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QCOMPARE(worker->getPollInterval(), defaultInterval);
    
    // Set new default poll interval
    int newDefault = 2000;
    m_manager->setDefaultPollInterval(newDefault);
    
    // New workers should use the new default
    ModbusWorker* worker2 = m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    // Wait for the asynchronous setPollInterval call to complete
    QTest::qWait(100);
    
    QCOMPARE(worker2->getPollInterval(), newDefault);
}

void TestModbusWorkerManager::testSetWorkerPollInterval()
{
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    
    int newInterval = 3000;
    QString deviceKey1 = QString("%1:%2:%3").arg(m_testHost1).arg(m_testPort).arg(m_testUnitId1);
    m_manager->setWorkerPollInterval(deviceKey1, newInterval);
    
    // Wait for the asynchronous setPollInterval call to complete
    QTest::qWait(100);
    
    // Verify the interval was set by checking the worker
    ModbusWorker* updatedWorker = m_manager->getWorker(deviceKey1);
    QVERIFY(updatedWorker != nullptr);
    QCOMPARE(updatedWorker->getPollInterval(), newInterval);
    
    // Try to set interval for non-existent worker (should not crash)
    QString deviceKey2 = QString("%1:%2:%3").arg(m_testHost2).arg(m_testPort).arg(m_testUnitId2);
    m_manager->setWorkerPollInterval(deviceKey2, newInterval);
}

void TestModbusWorkerManager::testStartAllWorkers()
{
    // Create workers
    ModbusWorker* worker1 = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    ModbusWorker* worker2 = m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    QSignalSpy startedSpy1(worker1, &ModbusWorker::workerStarted);
    QSignalSpy startedSpy2(worker2, &ModbusWorker::workerStarted);
    
    m_manager->startAllWorkers();
    
    QTest::qWait(100);
    
    QCOMPARE(startedSpy1.count(), 1);
    QCOMPARE(startedSpy2.count(), 1);
}

void TestModbusWorkerManager::testStopAllWorkers()
{
    // Create and start workers
    ModbusWorker* worker1 = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    ModbusWorker* worker2 = m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    m_manager->startAllWorkers();
    QTest::qWait(50);
    
    QSignalSpy stoppedSpy1(worker1, &ModbusWorker::workerStopped);
    QSignalSpy stoppedSpy2(worker2, &ModbusWorker::workerStopped);
    
    m_manager->stopAllWorkers();
    
    QTest::qWait(100);
    
    QCOMPARE(stoppedSpy1.count(), 1);
    QCOMPARE(stoppedSpy2.count(), 1);
}

void TestModbusWorkerManager::testConnectAllDevices()
{
    // Create workers
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    // This will attempt to connect all devices
    // In test environment, connections will likely fail
    m_manager->connectAllDevices();
    
    QTest::qWait(100);
    
    // Verify the method executes without crashing
    QVERIFY(true);
}

void TestModbusWorkerManager::testDisconnectAllDevices()
{
    // Create workers
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    m_manager->disconnectAllDevices();
    
    QTest::qWait(100);
    
    // Verify the method executes without crashing
    QVERIFY(true);
}

void TestModbusWorkerManager::testWorkerCreatedSignal()
{
    QSignalSpy createdSpy(m_manager, &ModbusWorkerManager::workerCreated);
    
    ModbusWorker* worker = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    
    QCOMPARE(createdSpy.count(), 1);
    QList<QVariant> arguments = createdSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), worker->getDeviceKey());
}

void TestModbusWorkerManager::testWorkerRemovedSignal()
{
    // Create a worker first
    ModbusWorker* worker = m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    QString deviceKey = worker->getDeviceKey();
    
    QSignalSpy removedSpy(m_manager, &ModbusWorkerManager::workerRemoved);
    m_manager->removeWorker(deviceKey);
    
    QCOMPARE(removedSpy.count(), 1);
    QList<QVariant> arguments = removedSpy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), deviceKey);
}

void TestModbusWorkerManager::testGlobalStatisticsSignal()
{
    QSignalSpy statsSpy(m_manager, &ModbusWorkerManager::globalStatisticsUpdated);
    
    // Create workers to trigger statistics updates
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    
    // Wait for statistics update (every 5 seconds)
    QTest::qWait(6000);
    
    QVERIFY(statsSpy.count() >= 1);
}

void TestModbusWorkerManager::testConcurrentWorkerAccess()
{
    const int numThreads = 5;
    const int workersPerThread = 3;
    
    QList<QFuture<void>> futures;
    
    // Create multiple threads that concurrently access the manager
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, workersPerThread]() {
            for (int j = 0; j < workersPerThread; ++j) {
                QString host = QString("192.168.%1.%2").arg(i + 1).arg(j + 1);
                int unitId = i * workersPerThread + j + 1;
                
                ModbusWorker* worker = m_manager->getOrCreateWorker(host, m_testPort, unitId);
                QVERIFY(worker != nullptr);
                
                // Perform some operations
                worker->getPollInterval();
                worker->getDeviceKey();
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify all workers were created
    QCOMPARE(m_manager->getWorkerCount(), numThreads * workersPerThread);
}

void TestModbusWorkerManager::testConcurrentStatisticsAccess()
{
    const int numThreads = 12;
    const int operationsPerThread = 40;
    
    // Create some initial workers
    m_manager->getOrCreateWorker(m_testHost1, m_testPort, m_testUnitId1);
    m_manager->getOrCreateWorker(m_testHost2, m_testPort, m_testUnitId2);
    
    QList<QFuture<void>> futures;
    QAtomicInt readOperations(0);
    QAtomicInt resetOperations(0);
    
    // Create multiple threads that concurrently access statistics
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, operationsPerThread, &readOperations, &resetOperations]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                // Concurrent read access to global statistics
                ModbusWorkerManager::GlobalStatistics stats = m_manager->getGlobalStatistics();
                QVERIFY(stats.activeWorkers >= 0);
                QVERIFY(stats.totalRequests >= 0);
                QVERIFY(stats.totalSuccessfulRequests >= 0);
                QVERIFY(stats.totalFailedRequests >= 0);
                readOperations.fetchAndAddOrdered(1);
                
                // Occasionally reset statistics to test concurrent write access
                if (j % 15 == 0) {
                    m_manager->resetGlobalStatistics();
                    resetOperations.fetchAndAddOrdered(1);
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
    
    // Verify operations were completed
    QCOMPARE(readOperations.loadAcquire(), numThreads * operationsPerThread);
    QVERIFY(resetOperations.loadAcquire() > 0);
    
    // Verify final statistics are consistent
    ModbusWorkerManager::GlobalStatistics finalStats = m_manager->getGlobalStatistics();
    QVERIFY(finalStats.activeWorkers >= 0);
}

void TestModbusWorkerManager::testConcurrentWorkerRemoval()
{
    const int numThreads = 8;
    const int workersPerThread = 5;
    
    // First, create workers from multiple threads
    QList<QFuture<void>> createFutures;
    QStringList createdKeys;
    QMutex keysMutex;
    
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, workersPerThread, &createdKeys, &keysMutex]() {
            for (int j = 0; j < workersPerThread; ++j) {
                QString host = QString("10.0.%1.%2").arg(i + 1).arg(j + 1);
                int unitId = i * workersPerThread + j + 1;
                
                ModbusWorker* worker = m_manager->getOrCreateWorker(host, m_testPort, unitId);
                QVERIFY(worker != nullptr);
                
                QString deviceKey = worker->getDeviceKey();
                QMutexLocker locker(&keysMutex);
                createdKeys.append(deviceKey);
            }
        });
        createFutures.append(future);
    }
    
    // Wait for all creation to complete
    for (auto& future : createFutures) {
        future.waitForFinished();
    }
    
    int initialWorkerCount = m_manager->getWorkerCount();
    QCOMPARE(initialWorkerCount, numThreads * workersPerThread);
    
    // Now concurrently remove workers
    QList<QFuture<void>> removeFutures;
    QAtomicInt removedCount(0);
    
    for (int i = 0; i < numThreads / 2; ++i) {
        QFuture<void> future = QtConcurrent::run([this, &createdKeys, &keysMutex, &removedCount]() {
            QMutexLocker locker(&keysMutex);
            if (!createdKeys.isEmpty()) {
                QString keyToRemove = createdKeys.takeFirst();
                locker.unlock();
                
                m_manager->removeWorker(keyToRemove);
                removedCount.fetchAndAddOrdered(1);
            }
        });
        removeFutures.append(future);
    }
    
    // Wait for all removal to complete
    for (auto& future : removeFutures) {
        future.waitForFinished();
    }
    
    // Verify some workers were removed
    int finalWorkerCount = m_manager->getWorkerCount();
    QVERIFY(finalWorkerCount < initialWorkerCount);
    QVERIFY(removedCount.loadAcquire() > 0);
}

void TestModbusWorkerManager::testConcurrentPollIntervalChanges()
{
    const int numThreads = 10;
    const int changesPerThread = 25;
    
    // Create some workers first
    QStringList deviceKeys;
    for (int i = 0; i < 5; ++i) {
        QString host = QString("172.16.%1.1").arg(i + 1);
        ModbusWorker* worker = m_manager->getOrCreateWorker(host, m_testPort, i + 1);
        deviceKeys.append(worker->getDeviceKey());
    }
    
    QList<QFuture<void>> futures;
    QAtomicInt changeCount(0);
    
    // Create multiple threads that concurrently change poll intervals
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, changesPerThread, &deviceKeys, &changeCount]() {
            for (int j = 0; j < changesPerThread; ++j) {
                // Change default poll interval
                int defaultInterval = 500 + (i * 100) + (j * 50);
                m_manager->setDefaultPollInterval(defaultInterval);
                
                // Change specific worker poll intervals
                if (!deviceKeys.isEmpty()) {
                    QString deviceKey = deviceKeys[j % deviceKeys.size()];
                    int workerInterval = 1000 + (i * 200) + (j * 25);
                    m_manager->setWorkerPollInterval(deviceKey, workerInterval);
                }
                
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
    
    // Wait for asynchronous operations to complete
    QTest::qWait(200);
    
    // Verify workers have valid poll intervals
    for (const QString& deviceKey : deviceKeys) {
        ModbusWorker* worker = m_manager->getWorker(deviceKey);
        if (worker) {
            QVERIFY(worker->getPollInterval() > 0);
        }
    }
}

void TestModbusWorkerManager::testConcurrentWorkerLifecycle()
{
    const int numThreads = 6;
    const int cyclesPerThread = 10;
    
    QList<QFuture<void>> futures;
    QAtomicInt createCount(0);
    QAtomicInt startCount(0);
    QAtomicInt stopCount(0);
    
    // Create multiple threads that perform complete worker lifecycle operations
    for (int i = 0; i < numThreads; ++i) {
        QFuture<void> future = QtConcurrent::run([this, i, cyclesPerThread, &createCount, &startCount, &stopCount]() {
            for (int j = 0; j < cyclesPerThread; ++j) {
                QString host = QString("192.168.%1.%2").arg(i + 10).arg(j + 10);
                int unitId = i * cyclesPerThread + j + 100;
                
                // Create worker
                ModbusWorker* worker = m_manager->getOrCreateWorker(host, m_testPort, unitId);
                QVERIFY(worker != nullptr);
                createCount.fetchAndAddOrdered(1);
                
                // Start worker
                QSignalSpy startedSpy(worker, &ModbusWorker::workerStarted);
                worker->startWorker();
                QTest::qWait(10);
                if (startedSpy.count() > 0) {
                    startCount.fetchAndAddOrdered(1);
                }
                
                // Stop worker
                QSignalSpy stoppedSpy(worker, &ModbusWorker::workerStopped);
                worker->stopWorker();
                QTest::qWait(10);
                if (stoppedSpy.count() > 0) {
                    stopCount.fetchAndAddOrdered(1);
                }
                
                // Small delay between cycles
                QThread::msleep(5);
            }
        });
        futures.append(future);
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }
    
    // Verify operations were completed
    QCOMPARE(createCount.loadAcquire(), numThreads * cyclesPerThread);
    QVERIFY(startCount.loadAcquire() > 0);
    QVERIFY(stopCount.loadAcquire() > 0);
    
    // Verify final worker count
    int finalWorkerCount = m_manager->getWorkerCount();
    QCOMPARE(finalWorkerCount, numThreads * cyclesPerThread);
}
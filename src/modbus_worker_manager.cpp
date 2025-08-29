#include "modbus_worker_manager.h"
#include "modbus_worker.h"
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QMetaObject>
#include <QDateTime>
#include <limits>

// ModbusWorkerManager Implementation
ModbusWorkerManager::ModbusWorkerManager(QObject *parent)
    : QObject(parent)
    , m_defaultPollInterval(2000)  // Increased from 1000ms to reduce connection drops
    , m_statsUpdateTimer(nullptr)
    , m_loadBalancingEnabled(true)
    , m_loadBalancingTimer(nullptr)
{
    // Initialize global statistics
    m_globalStats.activeWorkers = 0;
    m_globalStats.connectedDevices = 0;
    m_globalStats.totalRequests = 0;
    m_globalStats.totalSuccessfulRequests = 0;
    m_globalStats.totalFailedRequests = 0;
    m_globalStats.totalInterruptedRequests = 0;
    m_globalStats.globalAverageResponseTime = 0.0;
    m_globalStats.lastUpdateTime = 0;
    
    // Set up statistics update timer
    m_statsUpdateTimer = new QTimer(this);
    connect(m_statsUpdateTimer, &QTimer::timeout, this, &ModbusWorkerManager::updateGlobalStatistics);
    m_statsUpdateTimer->start(5000); // Update every 5 seconds
    
    // Set up load balancing timer
    m_loadBalancingTimer = new QTimer(this);
    connect(m_loadBalancingTimer, &QTimer::timeout, this, &ModbusWorkerManager::distributeLoad);
    m_loadBalancingTimer->start(10000); // Rebalance every 10 seconds
}

ModbusWorkerManager::~ModbusWorkerManager()
{
    // Clean up all workers
    removeAllWorkers();
}

ModbusWorker* ModbusWorkerManager::getOrCreateWorker(const QString& host, int port, int unitId)
{
    QString deviceKey = createDeviceKey(host, port, unitId);
    
    ModbusWorker* worker = nullptr;
    
    {
        QMutexLocker locker(&m_workersMutex);
        
        auto it = m_workers.find(deviceKey);
        if (it != m_workers.end()) {
            qDebug() << "🔄 ModbusWorkerManager::getOrCreateWorker() - Reusing existing worker for device:" << deviceKey;
            return it.value().worker;
        }
        
        qDebug() << "🆕 ModbusWorkerManager::getOrCreateWorker() - Creating new worker for device:" << deviceKey;
        
        // Create new worker but defer initialization until delayed startup
        worker = new ModbusWorker(host, port, unitId);
        
        QThread* thread = new QThread(this);
        
        WorkerInfo info;
        info.worker = worker;
        info.thread = thread;
        info.isConnected = false;
        
        // Store worker info but don't initialize yet
        m_workers[deviceKey] = info;
        
        qDebug() << "🆕 ModbusWorkerManager::getOrCreateWorker() - Worker created but not initialized for device:" << deviceKey;
        
        // Stagger worker startup to prevent resource contention
        // Each new worker gets a slightly delayed start with proper synchronization
        int workerCount = m_workers.size();
        int startupDelay = qMax(200, workerCount * 100); // Minimum 200ms, 100ms delay per existing worker
        
        qDebug() << "⏰ ModbusWorkerManager setting up delayed startup timer for device:" << deviceKey << "delay:" << startupDelay << "ms";
        
        // Store pending startup info
        m_pendingStartups[deviceKey] = {worker, deviceKey};
        
        // Use QTimer::singleShot with slot method instead of lambda
        QTimer::singleShot(startupDelay, this, [this, deviceKey]() {
            qDebug() << "🚀 TIMER SLOT - ModbusWorkerManager executing delayed startup for device:" << deviceKey;
            handleDelayedStartup(deviceKey);
        });
    }
    
    // Update statistics for existing workers
    if (worker != nullptr) {
        updateGlobalStatistics();
    }
    
    return worker;
}

ModbusWorker* ModbusWorkerManager::getWorker(const QString& deviceKey) const
{
    QMutexLocker locker(&m_workersMutex);
    auto it = m_workers.find(deviceKey);
    if (it != m_workers.end()) {
        return it.value().worker;
    }
    return nullptr;
}

void ModbusWorkerManager::removeWorker(const QString& deviceKey)
{
    QMutexLocker locker(&m_workersMutex);
    
    if (!m_workers.contains(deviceKey)) {
        return;
    }
    
    WorkerInfo& info = m_workers[deviceKey];
    
    // Stop the worker thread
    info.thread->quit();
    info.thread->wait();
    
    // Clean up
    info.worker->deleteLater();
    info.thread->deleteLater();
    
    m_workers.remove(deviceKey);
    
    emit workerRemoved(deviceKey);
}

void ModbusWorkerManager::removeAllWorkers()
{
    QMutexLocker locker(&m_workersMutex);
    
    for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
        WorkerInfo& info = it.value();
        
        // Stop the worker thread
        info.thread->quit();
        info.thread->wait();
        
        // Clean up
        info.worker->deleteLater();
        info.thread->deleteLater();
        
        emit workerRemoved(it.key());
    }
    
    m_workers.clear();
    emit globalStatisticsUpdated(m_globalStats);
}



















int ModbusWorkerManager::getWorkerCount() const
{
    QMutexLocker locker(&m_workersMutex);
    return m_workers.size();
}

void ModbusWorkerManager::updateGlobalStatistics()
{
    // Create a separate scope for workers mutex to avoid deadlock
    int activeWorkers = 0;
    int connectedCount = 0;
    
    {
        QMutexLocker workersLocker(&m_workersMutex);
        activeWorkers = m_workers.size();
        
        // Count connected workers
        for (auto it = m_workers.constBegin(); it != m_workers.constEnd(); ++it) {
            // For now, just count all workers as potentially connected
            // In a full implementation, you would check actual connection status
            connectedCount++;
        }
    }
    
    // Update global statistics with separate mutex
    {
        QMutexLocker statsLocker(&m_globalStatsMutex);
        m_globalStats.activeWorkers = activeWorkers;
        m_globalStats.connectedDevices = connectedCount;
        m_globalStats.lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    }
    
    emit globalStatisticsUpdated(m_globalStats);
}

QString ModbusWorkerManager::createDeviceKey(const QString& host, int port, int unitId) const
{
    return QString("%1:%2:%3").arg(host).arg(port).arg(unitId);
}

void ModbusWorkerManager::connectWorkerSignals(ModbusWorker* worker)
{
    connect(worker, &ModbusWorker::connectionStateChanged,
            this, &ModbusWorkerManager::onWorkerConnectionStateChanged, Qt::QueuedConnection);
    
    // Convert ModbusWorker::WorkerStatistics to ModbusWorkerTypes::WorkerStatistics
    connect(worker, &ModbusWorker::statisticsUpdated,
            this, [this](const QString& deviceKey, const ModbusWorker::WorkerStatistics& stats) {
                ModbusWorkerTypes::WorkerStatistics convertedStats;
                convertedStats.totalRequests = stats.totalRequests;
                convertedStats.successfulRequests = stats.successfulRequests;
                convertedStats.failedRequests = stats.failedRequests;
                convertedStats.interruptedRequests = stats.interruptedRequests;
                convertedStats.highPriorityRequests = stats.highPriorityRequests;
                convertedStats.averageResponseTime = stats.averageResponseTime;
                convertedStats.lastActivityTime = stats.lastActivityTime;
                convertedStats.isConnected = stats.isConnected;
                this->onWorkerStatisticsUpdated(deviceKey, convertedStats);
            }, Qt::QueuedConnection);
    
    connect(worker, &ModbusWorker::workerStarted,
            this, &ModbusWorkerManager::onWorkerStarted, Qt::QueuedConnection);
    connect(worker, &ModbusWorker::workerStopped,
            this, &ModbusWorkerManager::onWorkerStopped, Qt::QueuedConnection);
}

void ModbusWorkerManager::cleanupWorker(const QString& deviceKey)
{
    // This method can be used for additional cleanup if needed
    Q_UNUSED(deviceKey);
}

QStringList ModbusWorkerManager::getActiveDevices() const
{
    QMutexLocker locker(&m_workersMutex);
    return m_workers.keys();
}

QStringList ModbusWorkerManager::getConnectedDevices() const
{
    QMutexLocker locker(&m_workersMutex);
    QStringList connectedDevices;
    
    for (auto it = m_workers.constBegin(); it != m_workers.constEnd(); ++it) {
        if (it.value().isConnected) {
            connectedDevices.append(it.key());
        }
    }
    
    return connectedDevices;
}

ModbusWorkerManager::GlobalStatistics ModbusWorkerManager::getGlobalStatistics() const
{
    QMutexLocker locker(&m_globalStatsMutex);
    return m_globalStats;
}

void ModbusWorkerManager::resetGlobalStatistics()
{
    QMutexLocker locker(&m_globalStatsMutex);
    m_globalStats = GlobalStatistics();
    m_globalStats.lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
}

void ModbusWorkerManager::setDefaultPollInterval(int intervalMs)
{
    m_defaultPollInterval = intervalMs;
}

void ModbusWorkerManager::setWorkerPollInterval(const QString& deviceKey, int intervalMs)
{
    ModbusWorker* worker = getWorker(deviceKey);
    if (worker) {
        QMetaObject::invokeMethod(worker, "setPollInterval", Qt::QueuedConnection,
                                  Q_ARG(int, intervalMs));
    }
}

void ModbusWorkerManager::startAllWorkers()
{
    QMutexLocker locker(&m_workersMutex);
    for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
        QMetaObject::invokeMethod(it.value().worker, "startWorker", Qt::QueuedConnection);
    }
}

void ModbusWorkerManager::stopAllWorkers()
{
    QMutexLocker locker(&m_workersMutex);
    for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
        QMetaObject::invokeMethod(it.value().worker, "stopWorker", Qt::QueuedConnection);
    }
}

void ModbusWorkerManager::connectAllDevices()
{
    QMutexLocker locker(&m_workersMutex);
    for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
        QMetaObject::invokeMethod(it.value().worker, "connectToDevice", Qt::QueuedConnection);
    }
}

void ModbusWorkerManager::disconnectAllDevices()
{
    QMutexLocker locker(&m_workersMutex);
    for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
        QMetaObject::invokeMethod(it.value().worker, "disconnectFromDevice", Qt::QueuedConnection);
    }
}

void ModbusWorkerManager::onWorkerStarted(const QString& deviceKey)
{
    qDebug() << "Worker started:" << deviceKey;
    updateGlobalStatistics();
}

void ModbusWorkerManager::onWorkerStopped(const QString& deviceKey)
{
    updateGlobalStatistics();
}

void ModbusWorkerManager::onWorkerStatisticsUpdated(const QString& deviceKey, const ModbusWorkerTypes::WorkerStatistics& stats)
{
    QMutexLocker locker(&m_workersMutex);
    auto it = m_workers.find(deviceKey);
    if (it != m_workers.end()) {
        it.value().lastStats = stats;
        it.value().isConnected = stats.isConnected;
    }
    updateGlobalStatistics();
}





void ModbusWorkerManager::onWorkerConnectionStateChanged(const QString& deviceKey, bool connected)
{
    QMutexLocker locker(&m_workersMutex);
    if (m_workers.contains(deviceKey)) {
        m_workers[deviceKey].isConnected = connected;
    }
    updateGlobalStatistics();
}

// Load balancing implementation
QString ModbusWorkerManager::getLeastLoadedWorker() const
{
    QMutexLocker locker(&m_workersMutex);
    
    if (m_workers.isEmpty()) {
        return QString();
    }
    
    QString leastLoadedDevice;
    double minLoad = std::numeric_limits<double>::max();
    
    for (auto it = m_workers.constBegin(); it != m_workers.constEnd(); ++it) {
        if (it.value().isConnected) {
            double load = getWorkerLoad(it.key());
            if (load < minLoad) {
                minLoad = load;
                leastLoadedDevice = it.key();
            }
        }
    }
    
    return leastLoadedDevice;
}

void ModbusWorkerManager::distributeLoad()
{
    if (!m_loadBalancingEnabled || m_workers.size() <= 1) {
        return;
    }
    
    QMutexLocker loadLocker(&m_loadBalancingMutex);
    
    // Check if any workers are still initializing to prevent interference
    QMutexLocker workersLocker(&m_workersMutex);
    int initializingWorkers = 0;
    for (auto it = m_workers.constBegin(); it != m_workers.constEnd(); ++it) {
        if (!it.value().isConnected && it.value().worker) {
            // Check if worker thread is running but not yet connected (likely initializing)
            if (it.value().thread && it.value().thread->isRunning()) {
                initializingWorkers++;
            }
        }
    }
    
    // Defer load balancing if workers are still initializing
    if (initializingWorkers > 0) {
        QTimer::singleShot(5000, this, &ModbusWorkerManager::distributeLoad);
        return;
    }
    
    workersLocker.unlock();
    
    calculateWorkerLoads();
    rebalanceWorkerLoads();
}

void ModbusWorkerManager::setLoadBalancingEnabled(bool enabled)
{
    QMutexLocker locker(&m_loadBalancingMutex);
    m_loadBalancingEnabled = enabled;
    
    if (enabled && m_loadBalancingTimer) {
        m_loadBalancingTimer->start(10000);
    } else if (m_loadBalancingTimer) {
        m_loadBalancingTimer->stop();
    }
}

bool ModbusWorkerManager::isLoadBalancingEnabled() const
{
    QMutexLocker locker(&m_loadBalancingMutex);
    return m_loadBalancingEnabled;
}

double ModbusWorkerManager::getWorkerLoad(const QString& deviceKey) const
{
    QMutexLocker locker(&m_workersMutex);
    
    auto it = m_workers.find(deviceKey);
    if (it == m_workers.end() || !it.value().isConnected) {
        return 0.0;
    }
    
    const auto& stats = it.value().lastStats;
    
    // Calculate load based on request rate and response time
    double requestRate = stats.totalRequests > 0 ? 
        static_cast<double>(stats.totalRequests) / (QDateTime::currentMSecsSinceEpoch() - stats.lastActivityTime + 1) : 0.0;
    double responseTimeFactor = stats.averageResponseTime / 1000.0; // Convert to seconds
    double failureRate = stats.totalRequests > 0 ? 
        static_cast<double>(stats.failedRequests) / stats.totalRequests : 0.0;
    
    // Combined load metric (higher values indicate higher load)
    return (requestRate * 0.4) + (responseTimeFactor * 0.4) + (failureRate * 0.2);
}

void ModbusWorkerManager::optimizeWorkerDistribution()
{
    if (!m_loadBalancingEnabled) {
        return;
    }
    
    QMutexLocker locker(&m_workersMutex);
    
    // Adjust poll intervals based on worker load
    for (auto it = m_workers.begin(); it != m_workers.end(); ++it) {
        if (it.value().isConnected) {
            double load = getWorkerLoad(it.key());
            
            // Adjust poll interval: higher load = longer interval
            int adjustedInterval = m_defaultPollInterval;
            if (load > 0.8) {
                adjustedInterval = static_cast<int>(m_defaultPollInterval * 1.5);
            } else if (load > 0.5) {
                adjustedInterval = static_cast<int>(m_defaultPollInterval * 1.2);
            } else if (load < 0.2) {
                adjustedInterval = static_cast<int>(m_defaultPollInterval * 0.8);
            }
            
            setWorkerPollInterval(it.key(), adjustedInterval);
        }
    }
    
    qDebug() << "Worker distribution optimized for" << m_workers.size() << "workers";
}

void ModbusWorkerManager::calculateWorkerLoads() const
{
    // This method is called internally to update load calculations
    // The actual load calculation is done in getWorkerLoad()
    for (auto it = m_workers.constBegin(); it != m_workers.constEnd(); ++it) {
        if (it.value().isConnected) {
            double load = getWorkerLoad(it.key());
            Q_UNUSED(load); // Load is calculated and can be used for logging if needed
        }
    }
}

void ModbusWorkerManager::rebalanceWorkerLoads()
{
    if (m_workers.size() <= 1) {
        return;
    }
    
    // Find overloaded and underloaded workers
    QStringList overloadedWorkers;
    QStringList underloadedWorkers;
    
    const double loadThresholdHigh = 0.8;
    const double loadThresholdLow = 0.3;
    
    for (auto it = m_workers.constBegin(); it != m_workers.constEnd(); ++it) {
        if (it.value().isConnected) {
            double load = getWorkerLoad(it.key());
            if (load > loadThresholdHigh) {
                overloadedWorkers.append(it.key());
            } else if (load < loadThresholdLow) {
                underloadedWorkers.append(it.key());
            }
        }
    }
    
    // Adjust poll intervals for load balancing
    for (const QString& deviceKey : overloadedWorkers) {
        int currentInterval = m_workers[deviceKey].worker->getPollInterval();
        int newInterval = static_cast<int>(currentInterval * 1.3);
        setWorkerPollInterval(deviceKey, newInterval);
        qDebug() << "Increased poll interval for overloaded worker:" << deviceKey;
    }
    
    for (const QString& deviceKey : underloadedWorkers) {
        int currentInterval = m_workers[deviceKey].worker->getPollInterval();
        int newInterval = static_cast<int>(currentInterval * 0.8);
        if (newInterval < 500) newInterval = 500; // Minimum interval
        setWorkerPollInterval(deviceKey, newInterval);
        qDebug() << "Decreased poll interval for underloaded worker:" << deviceKey;
    }
}

void ModbusWorkerManager::handleDelayedStartup(const QString &deviceKey)
{
    qDebug() << "🚀 ModbusWorkerManager executing delayed startup for device:" << deviceKey;
    
    QMutexLocker pendingLocker(&m_pendingStartupsMutex);
    if (!m_pendingStartups.contains(deviceKey)) {
        qDebug() << "⚠️ No pending startup found for device:" << deviceKey;
        return;
    }
    
    PendingStartup startup = m_pendingStartups.take(deviceKey);
    pendingLocker.unlock();
    
    QMutexLocker locker(&m_workersMutex);
    if (m_workers.contains(deviceKey) && m_workers[deviceKey].worker == startup.worker) {
        ModbusWorker* worker = startup.worker;
        QThread* thread = m_workers[deviceKey].thread;
        
        // Now perform the actual worker initialization that was deferred
        qDebug() << "🔧 ModbusWorkerManager initializing worker for device:" << deviceKey;
        
        // Move worker to thread
        worker->moveToThread(thread);
        
        // Connect signals
        connectWorkerSignals(worker);
        
        // Start thread
        thread->start();
        
        // Initialize timers in the worker thread (must be done after moveToThread)
        // Use BlockingQueuedConnection to ensure proper initialization sequence
        QMetaObject::invokeMethod(worker, "initializeTimers", Qt::BlockingQueuedConnection);
        
        // Set default poll interval for new worker
        QMetaObject::invokeMethod(worker, "setPollInterval", Qt::BlockingQueuedConnection,
                                  Q_ARG(int, m_defaultPollInterval));
        
        // Now start the worker
        QMetaObject::invokeMethod(worker, "startWorker", Qt::QueuedConnection);
        
        // Emit workerCreated signal after worker is actually started
        locker.unlock();
        qDebug() << "📡 ModbusWorkerManager emitting workerCreated signal for device:" << deviceKey;
        emit workerCreated(deviceKey);
        updateGlobalStatistics();
    } else {
        qDebug() << "⚠️ Worker validation failed for device:" << deviceKey;
    }
}
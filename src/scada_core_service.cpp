#include "../include/scada_core_service.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QThread>
#include <QMetaType>
#include <QCoreApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QThreadPool>
#include <QTime>
#include <QRandomGenerator>
#include <cstring>
#include <algorithm>

ScadaCoreService::ScadaCoreService(QObject *parent)
    : QObject(parent)
    , m_pollTimer(nullptr)
    , m_workerManager(nullptr)
    , m_telegrafSocketPath("/tmp/telegraf.sock")
    , m_serviceRunning(false)
    , m_currentPointIndex(0)
    , m_dataPointsMutex()
    , m_statisticsMutex()
    , m_requestTrackingMutex()
    , m_threadingMode(ThreadingMode::Auto)
    , m_useSingleThreadedMode(false)
    , m_singleThreadModbusManager(nullptr)
    , m_performanceMonitoringEnabled(false)
    , m_deploymentConfig()
{
    // Register metatypes for thread-safe signal/slot connections
    qRegisterMetaType<ModbusReadResult>("ModbusReadResult");
    qRegisterMetaType<ModbusWriteResult>("ModbusWriteResult");
    qRegisterMetaType<ModbusWorker::WorkerStatistics>("ModbusWorker::WorkerStatistics");
    qRegisterMetaType<ModbusWorkerManager::GlobalStatistics>("ModbusWorkerManager::GlobalStatistics");
    qRegisterMetaType<DataAcquisitionPoint>("DataAcquisitionPoint");
    qRegisterMetaType<AcquiredDataPoint>("AcquiredDataPoint");
    qRegisterMetaType<RequestPriority>("RequestPriority");
    
    // Initialize components
    m_pollTimer = new QTimer(this);
    m_workerManager = new ModbusWorkerManager(this);
    
    // Connect worker manager signals with thread-safe queued connections
    connect(m_pollTimer, &QTimer::timeout, this, &ScadaCoreService::onPollTimer, Qt::QueuedConnection);
    
    // Connect worker signals for request completion
    connect(m_workerManager, &ModbusWorkerManager::workerCreated,
            this, &ScadaCoreService::onWorkerCreated, Qt::QueuedConnection);
    connect(m_workerManager, &ModbusWorkerManager::workerRemoved,
            this, &ScadaCoreService::workerRemoved, Qt::QueuedConnection);
    connect(m_workerManager, &ModbusWorkerManager::globalStatisticsUpdated,
            this, &ScadaCoreService::onGlobalStatisticsUpdated, Qt::QueuedConnection);
    
    // Set up thread pool for concurrent data processing
    QThreadPool::globalInstance()->setMaxThreadCount(qMax(4, QThread::idealThreadCount()));
    
    // Initialize statistics
    resetStatistics();
    
    qDebug() << "ScadaCoreService initialized with multithreaded worker architecture";
}

ScadaCoreService::~ScadaCoreService()
{
    stopService();
    
    if (m_singleThreadModbusManager) {
        delete m_singleThreadModbusManager;
        m_singleThreadModbusManager = nullptr;
    }
}

bool ScadaCoreService::startService()
{
    if (m_serviceRunning) {
        qDebug() << "SCADA Core Service is already running";
        return true;
    }
    
    if (m_dataPoints.isEmpty()) {
        emit errorOccurred("No data points configured for acquisition");
        return false;
    }
    
    qDebug() << "Starting SCADA Core Service...";
    qDebug() << "Configured data points:" << m_dataPoints.size();
    qDebug() << "Telegraf socket path:" << m_telegrafSocketPath;
    
    // Log device summary for multi-device visibility
    QMap<QString, int> devicePointCounts;
    for (const auto &point : m_dataPoints) {
        QString unitIdStr = point.tags.value("unit_id", "1");
        QString deviceKey = QString("%1:%2:%3").arg(point.host).arg(point.port).arg(unitIdStr);
        devicePointCounts[deviceKey]++;
    }
    
    qDebug() << "📊 Multi-device configuration summary:";
    for (auto it = devicePointCounts.begin(); it != devicePointCounts.end(); ++it) {
        qDebug() << "   Device" << it.key() << "has" << it.value() << "data points";
    }
    
    // Determine threading strategy based on device count and configuration
    int deviceCount = devicePointCounts.size();
    switch (m_threadingMode) {
        case ThreadingMode::Auto:
            m_useSingleThreadedMode = (deviceCount == 1);
            break;
        case ThreadingMode::SingleThreaded:
            m_useSingleThreadedMode = true;
            break;
        case ThreadingMode::MultiThreaded:
            m_useSingleThreadedMode = false;
            break;
    }
    
    qDebug() << "🔧 Threading Strategy:" << (m_useSingleThreadedMode ? "Single-threaded" : "Multi-threaded")
             << "(" << deviceCount << "devices," << "mode:" << (int)m_threadingMode << ")";
    
    // Note: Using direct Unix socket calls for each data transmission
    
    // Initialize service state
    m_serviceRunning = true;
    m_currentPointIndex = 0;
    m_statistics.serviceStartTime = QDateTime::currentMSecsSinceEpoch();
    
    // Initialize based on threading mode
    if (m_useSingleThreadedMode) {
        // Single-threaded mode: create one ModbusManager for direct communication
        qDebug() << "🔧 Initializing single-threaded mode...";
        
        if (!m_singleThreadModbusManager) {
            // Get the first device configuration for single-threaded mode
            if (!m_dataPoints.isEmpty()) {
                const auto &firstPoint = m_dataPoints.first();
                QString unitIdStr = firstPoint.tags.value("unit_id", "1");
                int unitId = unitIdStr.toInt();
                
                m_singleThreadModbusManager = new ModbusManager(this);
                m_singleThreadModbusManager->initializeClient(); // Initialize the QModbusTcpClient
                m_singleThreadModbusManager->connectToServer(firstPoint.host, firstPoint.port);
                
                // Connect signals for single-threaded mode
                connect(m_singleThreadModbusManager, &ModbusManager::readCompleted,
                        this, [this](const ModbusReadResult &result) {
                            // Handle read completion in single-threaded mode
                            this->onSingleThreadReadCompleted(result);
                        });
                
                connect(m_singleThreadModbusManager, &ModbusManager::connectionStateChanged,
                        this, [this](bool connected) {
                            qDebug() << "Single-threaded Modbus connection state:" << connected;
                        });
                
                connect(m_singleThreadModbusManager, &ModbusManager::errorOccurred,
                        this, [this](const QString &error) {
                            qDebug() << "Single-threaded Modbus error:" << error;
                            emit errorOccurred(error);
                        });
                
                qDebug() << "✅ Single-threaded ModbusManager created for" << firstPoint.host << ":" << firstPoint.port;
            }
        }
    } else {
        // Multi-threaded mode: create workers for all configured devices
        QSet<QString> uniqueDevices;
        for (const auto &point : m_dataPoints) {
            QString unitIdStr = point.tags.value("unit_id", "1");
            int unitId = unitIdStr.toInt();
            QString deviceKey = QString("%1:%2:%3").arg(point.host).arg(point.port).arg(unitId);
            uniqueDevices.insert(deviceKey);
        }
        
        qDebug() << "🔧 Pre-creating workers for" << uniqueDevices.size() << "unique devices...";
        for (const QString &deviceKey : uniqueDevices) {
            QStringList parts = deviceKey.split(":");
            if (parts.size() >= 3) {
                QString host = parts[0];
                int port = parts[1].toInt();
                int unitId = parts[2].toInt();
                
                qDebug() << "Creating worker for device:" << deviceKey;
                ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, unitId);
                if (worker) {
                    connectWorkerSignals(worker);
                    qDebug() << "✅ Pre-created worker for device:" << deviceKey;
                } else {
                    qDebug() << "❌ Failed to create worker for device:" << deviceKey;
                }
            }
        }
        
        qDebug() << "Starting all workers...";
        // Start all workers for configured devices
        m_workerManager->startAllWorkers();
        qDebug() << "✅ All workers started";
        
        // Enable load balancing for multi-device scenarios
        if (uniqueDevices.size() > 1) {
            m_workerManager->setLoadBalancingEnabled(true);
            qDebug() << "🔄 Load balancing enabled for" << uniqueDevices.size() << "devices";
        } else {
            m_workerManager->setLoadBalancingEnabled(false);
            qDebug() << "Load balancing disabled for single device";
        }
    }
    
    // Allow time for connections to establish before starting polling
    qDebug() << "Allowing 2 seconds for worker connections to establish...";
    QTimer::singleShot(2000, this, [this]() {
        // Optimize and start polling timer based on threading mode
        optimizePollInterval();
        
        // Add debug output for timer interval
        qDebug() << "Poll timer interval before start:" << m_pollTimer->interval() << "ms";
        m_pollTimer->start();
        qDebug() << "Poll timer started. Active:" << m_pollTimer->isActive() << "Interval:" << m_pollTimer->interval() << "ms";
        
        if (m_useSingleThreadedMode) {
            qDebug() << "Started optimized single-threaded polling timer";
        } else {
            qDebug() << "Started polling timer with" << m_workerManager->getActiveDevices().size() << "active workers";
            
            // Add service-level polling backup mechanism for multi-device mode
            QTimer::singleShot(5000, this, [this]() {
                qDebug() << "ScadaCoreService - Activating polling backup mechanism for multi-device mode";
                
                // Ensure all workers have polling enabled
                QStringList activeDevices = m_workerManager->getActiveDevices();
                for (const QString& deviceKey : activeDevices) {
                    ModbusWorker* worker = m_workerManager->getWorker(deviceKey);
                    if (worker) {
                        worker->setPollingEnabled(true);
                        qDebug() << "ScadaCoreService - Backup polling activation for device:" << deviceKey;
                    }
                }
                
                // Schedule periodic polling health checks
                QTimer* healthCheckTimer = new QTimer(this);
                connect(healthCheckTimer, &QTimer::timeout, this, [this]() {
                    QStringList devices = m_workerManager->getActiveDevices();
                    for (const QString& deviceKey : devices) {
                        ModbusWorker* worker = m_workerManager->getWorker(deviceKey);
                        if (worker && !worker->isConnected()) {
                            qDebug() << "ScadaCoreService - Health check: Re-enabling polling for disconnected device:" << deviceKey;
                            worker->setPollingEnabled(true);
                        }
                    }
                });
                healthCheckTimer->start(10000); // Check every 10 seconds
                qDebug() << "ScadaCoreService - Polling health check timer started (10s interval)";
            });
        }
    });
    
    emit serviceStarted();
    qDebug() << "SCADA Core Service started successfully with" << m_workerManager->getActiveDevices().size() << "workers";
    return true;
}

void ScadaCoreService::stopService()
{
    if (!m_serviceRunning) {
        return;
    }
    
    qDebug() << "Stopping SCADA Core Service...";
    
    m_serviceRunning = false;
    m_pollTimer->stop();
    
    if (m_useSingleThreadedMode) {
        // Single-threaded mode cleanup
        if (m_singleThreadModbusManager) {
            m_singleThreadModbusManager->disconnectFromServer();
            qDebug() << "🔧 Single-threaded ModbusManager disconnected";
        }
    } else {
        // Multi-threaded mode cleanup
        if (m_workerManager) {
            m_workerManager->stopAllWorkers();
            qDebug() << "🔧 All workers stopped";
        }
    }
    
    // Clear pending requests
    m_pendingReadRequests.clear();
    m_pendingWriteRequests.clear();
    
    // Note: Using direct Unix socket calls, no persistent connection needed
    
    emit serviceStopped();
    qDebug() << "SCADA Core Service stopped";
}

bool ScadaCoreService::isRunning() const
{
    return m_serviceRunning;
}

void ScadaCoreService::setThreadingMode(ThreadingMode mode)
{
    if (m_serviceRunning) {
        qWarning() << "Cannot change threading mode while service is running";
        return;
    }
    
    m_threadingMode = mode;
    qDebug() << "Threading mode set to:" << (int)mode;
}

ScadaCoreService::ThreadingMode ScadaCoreService::getThreadingMode() const
{
    return m_threadingMode;
}

bool ScadaCoreService::isSingleThreadedMode() const
{
    return m_useSingleThreadedMode;
}

void ScadaCoreService::optimizePollInterval()
{
    if (!m_pollTimer) {
        return;
    }
    
    int optimalInterval;
    
    if (m_useSingleThreadedMode) {
        // Single-threaded mode: faster polling for better responsiveness
        // Base interval on number of data points to ensure all points get polled efficiently
        int dataPointCount = m_dataPoints.size();
        if (dataPointCount <= 5) {
            optimalInterval = 100; // Very fast for few points
        } else if (dataPointCount <= 20) {
            optimalInterval = 200; // Fast for moderate number of points
        } else {
            optimalInterval = 500; // Moderate for many points
        }
        
        qDebug() << "🔧 Single-threaded mode: optimized poll interval to" << optimalInterval << "ms for" << dataPointCount << "data points";
    } else {
        // Multi-threaded mode: standard interval since workers handle concurrency
        optimalInterval = 1000; // Standard 1 second interval
        qDebug() << "🔧 Multi-threaded mode: using standard poll interval of" << optimalInterval << "ms";
    }
    
    m_pollTimer->setInterval(optimalInterval);
}

void ScadaCoreService::enableLoadBalancing(bool enabled)
{
    if (m_workerManager) {
        m_workerManager->setLoadBalancingEnabled(enabled);
        qDebug() << "🔄 Load balancing" << (enabled ? "enabled" : "disabled") << "via SCADA core service";
    }
}

bool ScadaCoreService::isLoadBalancingEnabled() const
{
    return m_workerManager ? m_workerManager->isLoadBalancingEnabled() : false;
}

QString ScadaCoreService::getLeastLoadedDevice() const
{
    return m_workerManager ? m_workerManager->getLeastLoadedWorker() : QString();
}

void ScadaCoreService::optimizeWorkerDistribution()
{
    if (m_workerManager) {
        m_workerManager->optimizeWorkerDistribution();
        qDebug() << "🔧 Worker distribution optimization triggered";
    }
}

double ScadaCoreService::getDeviceLoad(const QString &deviceKey) const
{
    return m_workerManager ? m_workerManager->getWorkerLoad(deviceKey) : 0.0;
}

ScadaCoreService::PerformanceMetrics ScadaCoreService::getPerformanceMetrics() const
{
    QMutexLocker locker(&m_performanceMetricsMutex);
    
    PerformanceMetrics metrics = m_performanceMetrics;
    
    // Calculate derived metrics
    if (metrics.singleThreadedOperations > 0) {
        metrics.singleThreadedAvgResponseTime = static_cast<double>(metrics.singleThreadedTotalTime) / metrics.singleThreadedOperations;
        metrics.throughputSingleThreaded = metrics.singleThreadedOperations * 1000.0 / metrics.singleThreadedTotalTime; // ops per second
    }
    
    if (metrics.multiThreadedOperations > 0) {
        metrics.multiThreadedAvgResponseTime = static_cast<double>(metrics.multiThreadedTotalTime) / metrics.multiThreadedOperations;
        metrics.throughputMultiThreaded = metrics.multiThreadedOperations * 1000.0 / metrics.multiThreadedTotalTime; // ops per second
    }
    
    // Calculate efficiency ratio
    if (metrics.throughputSingleThreaded > 0 && metrics.throughputMultiThreaded > 0) {
        metrics.efficiencyRatio = metrics.throughputMultiThreaded / metrics.throughputSingleThreaded;
    }
    
    return metrics;
}

void ScadaCoreService::resetPerformanceMetrics()
{
    QMutexLocker locker(&m_performanceMetricsMutex);
    m_performanceMetrics = PerformanceMetrics();
    m_operationStartTimes.clear();
    qDebug() << "Performance metrics reset";
}

QString ScadaCoreService::getPerformanceReport() const
{
    PerformanceMetrics metrics = getPerformanceMetrics();
    
    QString report;
    report += "=== SCADA Performance Report ===\n";
    report += QString("Threading Mode: %1\n").arg(m_useSingleThreadedMode ? "Single-threaded" : "Multi-threaded");
    report += QString("Performance Monitoring: %1\n\n").arg(m_performanceMonitoringEnabled ? "Enabled" : "Disabled");
    
    // Operation counts
    report += "Operation Counts:\n";
    report += QString("  Single-threaded: %1\n").arg(metrics.singleThreadedOperations);
    report += QString("  Multi-threaded: %1\n\n").arg(metrics.multiThreadedOperations);
    
    // Response times
    report += "Average Response Times:\n";
    report += QString("  Single-threaded: %1 ms\n").arg(metrics.singleThreadedAvgResponseTime, 0, 'f', 2);
    report += QString("  Multi-threaded: %1 ms\n\n").arg(metrics.multiThreadedAvgResponseTime, 0, 'f', 2);
    
    // Throughput
    report += "Throughput (ops/sec):\n";
    report += QString("  Single-threaded: %1\n").arg(metrics.throughputSingleThreaded, 0, 'f', 2);
    report += QString("  Multi-threaded: %1\n").arg(metrics.throughputMultiThreaded, 0, 'f', 2);
    report += QString("  Efficiency Ratio: %1\n\n").arg(metrics.efficiencyRatio, 0, 'f', 2);
    
    // Resource usage
    report += "Resource Usage:\n";
    report += QString("  Active Workers: %1 / %2 max\n").arg(metrics.currentActiveWorkers).arg(metrics.maxConcurrentWorkers);
    report += QString("  CPU Usage: %1%\n").arg(metrics.cpuUsagePercent, 0, 'f', 1);
    report += QString("  Memory Usage: %1 KB\n\n").arg(metrics.memoryUsageBytes / 1024);
    
    // Error rates
    report += "Error Rates:\n";
    report += QString("  Single-threaded: %1%\n").arg(metrics.singleThreadedErrorRate * 100, 0, 'f', 2);
    report += QString("  Multi-threaded: %1%\n").arg(metrics.multiThreadedErrorRate * 100, 0, 'f', 2);
    
    return report;
}

void ScadaCoreService::enablePerformanceMonitoring(bool enabled)
{
    QMutexLocker locker(&m_performanceMetricsMutex);
    m_performanceMonitoringEnabled = enabled;
    
    if (enabled) {
        m_operationTimer.start();
        qDebug() << "Performance monitoring enabled";
    } else {
        qDebug() << "Performance monitoring disabled";
    }
}

bool ScadaCoreService::isPerformanceMonitoringEnabled() const
{
    QMutexLocker locker(&m_performanceMetricsMutex);
    return m_performanceMonitoringEnabled;
}

void ScadaCoreService::setDeploymentConfig(const DeploymentConfig &config)
{
    m_deploymentConfig = config;
    
    // Apply configuration settings
    setThreadingMode(config.threadingMode);
    enableLoadBalancing(config.enableLoadBalancing);
    enablePerformanceMonitoring(config.enablePerformanceMonitoring);
    
    // Note: maxWorkerThreads will be applied when creating new worker manager
    // Other settings like connectionTimeoutMs and maxRetryAttempts can be used
    // by individual components as needed
}

ScadaCoreService::DeploymentConfig ScadaCoreService::getDeploymentConfig() const
{
    return m_deploymentConfig;
}

bool ScadaCoreService::loadConfigFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config JSON:" << error.errorString();
        return false;
    }
    
    QJsonObject obj = doc.object();
    DeploymentConfig config;
    
    // Parse threading mode
    QString threadingModeStr = obj["threadingMode"].toString("Auto");
    if (threadingModeStr == "SingleThreaded") {
        config.threadingMode = ThreadingMode::SingleThreaded;
    } else if (threadingModeStr == "MultiThreaded") {
        config.threadingMode = ThreadingMode::MultiThreaded;
    } else {
        config.threadingMode = ThreadingMode::Auto;
    }
    
    config.maxWorkerThreads = obj["maxWorkerThreads"].toInt(10);
    config.deviceCountThreshold = obj["deviceCountThreshold"].toInt(1);
    config.pollIntervalMs = obj["pollIntervalMs"].toInt(1000);
    config.enableLoadBalancing = obj["enableLoadBalancing"].toBool(true);
    config.enablePerformanceMonitoring = obj["enablePerformanceMonitoring"].toBool(false);
    config.connectionTimeoutMs = obj["connectionTimeoutMs"].toInt(5000);
    config.maxRetryAttempts = obj["maxRetryAttempts"].toInt(3);
    config.configFilePath = obj["configFilePath"].toString("scada_config.json");
    
    setDeploymentConfig(config);
    return true;
}

bool ScadaCoreService::saveConfigToFile(const QString &filePath) const
{
    QJsonObject obj;
    
    // Convert threading mode to string
    QString threadingModeStr;
    switch (m_deploymentConfig.threadingMode) {
        case ThreadingMode::SingleThreaded:
            threadingModeStr = "SingleThreaded";
            break;
        case ThreadingMode::MultiThreaded:
            threadingModeStr = "MultiThreaded";
            break;
        default:
            threadingModeStr = "Auto";
            break;
    }
    
    obj["threadingMode"] = threadingModeStr;
    obj["maxWorkerThreads"] = m_deploymentConfig.maxWorkerThreads;
    obj["deviceCountThreshold"] = m_deploymentConfig.deviceCountThreshold;
    obj["pollIntervalMs"] = m_deploymentConfig.pollIntervalMs;
    obj["enableLoadBalancing"] = m_deploymentConfig.enableLoadBalancing;
    obj["enablePerformanceMonitoring"] = m_deploymentConfig.enablePerformanceMonitoring;
    obj["connectionTimeoutMs"] = m_deploymentConfig.connectionTimeoutMs;
    obj["maxRetryAttempts"] = m_deploymentConfig.maxRetryAttempts;
    obj["configFilePath"] = m_deploymentConfig.configFilePath;
    
    QJsonDocument doc(obj);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << filePath;
        return false;
    }
    
    file.write(doc.toJson());
    return true;
}

void ScadaCoreService::resetConfigToDefaults()
{
    m_deploymentConfig = DeploymentConfig();
    setDeploymentConfig(m_deploymentConfig);
}

void ScadaCoreService::addDataPoint(const DataAcquisitionPoint &point)
{
    QMutexLocker locker(&m_dataPointsMutex);
    
    // Check if point already exists
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == point.name) {
            m_dataPoints[i] = point;
            qDebug() << "Updated existing data point:" << point.name;
            return;
        }
    }
    
    m_dataPoints.append(point);
    m_lastPollTimes[point.name] = 0; // Initialize last poll time
    qDebug() << "Added new data point:" << point.name << "at" << point.host << ":" << point.port;
}

void ScadaCoreService::removeDataPoint(const QString &pointName)
{
    QMutexLocker locker(&m_dataPointsMutex);
    
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == pointName) {
            m_dataPoints.removeAt(i);
            m_lastPollTimes.remove(pointName);
            qDebug() << "Removed data point:" << pointName;
            return;
        }
    }
}

void ScadaCoreService::updateDataPoint(const QString &pointName, const DataAcquisitionPoint &point)
{
    QMutexLocker locker(&m_dataPointsMutex);
    
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == pointName) {
            m_dataPoints[i] = point;
            qDebug() << "Updated data point:" << pointName;
            return;
        }
    }
}

QVector<DataAcquisitionPoint> ScadaCoreService::getDataPoints() const
{
    QMutexLocker locker(&m_dataPointsMutex);
    return m_dataPoints;
}

void ScadaCoreService::clearDataPoints()
{
    QMutexLocker locker(&m_dataPointsMutex);
    
    m_dataPoints.clear();
    m_lastPollTimes.clear();
    qDebug() << "Cleared all data points";
}

void ScadaCoreService::setTelegrafSocketPath(const QString &socketPath)
{
    m_telegrafSocketPath = socketPath;
    qDebug() << "Telegraf socket path set to:" << socketPath;
}

QString ScadaCoreService::getTelegrafSocketPath() const
{
    return m_telegrafSocketPath;
}

ScadaCoreService::ServiceStatistics ScadaCoreService::getStatistics() const
{
    return m_statistics;
}

void ScadaCoreService::resetStatistics()
{
    m_statistics = ServiceStatistics();
    m_responseTimers.clear();
}

// Modbus write operations with priority support
qint64 ScadaCoreService::writeHoldingRegister(const QString &host, int port, int address, quint16 value, RequestPriority priority)
{
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, 1);
    if (!worker) {
        emit writeCompleted(0, QString("WriteHoldingRegister[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to create worker");
        return 0;
    }
    
    // Create Modbus request
    ModbusRequest request;
    request.type = ModbusRequest::WriteHoldingRegisters;
    request.startAddress = address;
    request.count = 1;
    request.writeData = QVector<quint16>() << value;
    request.dataType = ModbusDataType::HoldingRegister;
    request.unitId = 1;
    
    qint64 requestId = worker->queueWriteRequest(request, priority, true);
    
    // Track the request with thread safety
    {
        QMutexLocker locker(&m_requestTrackingMutex);
        m_pendingWriteRequests[requestId] = QString("WriteHoldingRegister[%1:%2@%3]").arg(host).arg(port).arg(address);
    }
    
    // Connect worker signals if not already connected
    connectWorkerSignals(worker);
    
    qDebug() << "Queued holding register write:" << host << ":" << port << "address" << address << "value" << value << "priority" << static_cast<int>(priority) << "requestId" << requestId;
    return requestId;
}

qint64 ScadaCoreService::writeHoldingRegisterFloat32(const QString &host, int port, int address, float value, RequestPriority priority)
{
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, 1);
    if (!worker) {
        emit writeCompleted(0, QString("WriteHoldingRegisterFloat32[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to create worker");
        return 0;
    }
    
    // Convert float to two 16-bit registers
    union { float f; quint32 i; } converter;
    converter.f = value;
    QVector<quint16> data;
    data << static_cast<quint16>(converter.i & 0xFFFF);
    data << static_cast<quint16>((converter.i >> 16) & 0xFFFF);
    
    ModbusRequest request;
    request.type = ModbusRequest::WriteHoldingRegisters;
    request.startAddress = address;
    request.count = 2;
    request.writeData = data;
    request.dataType = ModbusDataType::Float32;
    request.unitId = 1;
    
    qint64 requestId = worker->queueWriteRequest(request, priority, true);
    m_pendingWriteRequests[requestId] = QString("WriteHoldingRegisterFloat32[%1:%2@%3]").arg(host).arg(port).arg(address);
    connectWorkerSignals(worker);
    
    qDebug() << "Queued Float32 register write:" << host << ":" << port << "address" << address << "value" << value << "priority" << static_cast<int>(priority) << "requestId" << requestId;
    return requestId;
}

qint64 ScadaCoreService::writeHoldingRegisterDouble64(const QString &host, int port, int address, double value, RequestPriority priority)
{
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, 1);
    if (!worker) {
        emit writeCompleted(0, QString("WriteHoldingRegisterDouble64[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to create worker");
        return 0;
    }
    
    // Convert double to four 16-bit registers
    union { double d; quint64 i; } converter;
    converter.d = value;
    QVector<quint16> data;
    data << static_cast<quint16>(converter.i & 0xFFFF);
    data << static_cast<quint16>((converter.i >> 16) & 0xFFFF);
    data << static_cast<quint16>((converter.i >> 32) & 0xFFFF);
    data << static_cast<quint16>((converter.i >> 48) & 0xFFFF);
    
    ModbusRequest request;
    request.type = ModbusRequest::WriteHoldingRegisters;
    request.startAddress = address;
    request.count = 4;
    request.writeData = data;
    request.dataType = ModbusDataType::Double64;
    request.unitId = 1;
    
    qint64 requestId = worker->queueWriteRequest(request, priority, true);
    m_pendingWriteRequests[requestId] = QString("WriteHoldingRegisterDouble64[%1:%2@%3]").arg(host).arg(port).arg(address);
    connectWorkerSignals(worker);
    
    qDebug() << "Queued Double64 register write:" << host << ":" << port << "address" << address << "value" << value << "priority" << static_cast<int>(priority) << "requestId" << requestId;
    return requestId;
}

qint64 ScadaCoreService::writeHoldingRegisterLong32(const QString &host, int port, int address, qint32 value, RequestPriority priority)
{
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, 1);
    if (!worker) {
        emit writeCompleted(0, QString("WriteHoldingRegisterLong32[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to create worker");
        return 0;
    }
    
    // Convert int32 to two 16-bit registers
    QVector<quint16> data;
    data << static_cast<quint16>(value & 0xFFFF);
    data << static_cast<quint16>((value >> 16) & 0xFFFF);
    
    ModbusRequest request;
    request.type = ModbusRequest::WriteHoldingRegisters;
    request.startAddress = address;
    request.count = 2;
    request.writeData = data;
    request.dataType = ModbusDataType::Long32;
    request.unitId = 1;
    
    qint64 requestId = worker->queueWriteRequest(request, priority, true);
    m_pendingWriteRequests[requestId] = QString("WriteHoldingRegisterLong32[%1:%2@%3]").arg(host).arg(port).arg(address);
    connectWorkerSignals(worker);
    
    qDebug() << "Queued Long32 register write:" << host << ":" << port << "address" << address << "value" << value << "priority" << static_cast<int>(priority) << "requestId" << requestId;
    return requestId;
}

qint64 ScadaCoreService::writeHoldingRegisterLong64(const QString &host, int port, int address, qint64 value, RequestPriority priority)
{
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, 1);
    if (!worker) {
        emit writeCompleted(0, QString("WriteHoldingRegisterLong64[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to create worker");
        return 0;
    }
    
    // Convert int64 to four 16-bit registers
    QVector<quint16> data;
    data << static_cast<quint16>(value & 0xFFFF);
    data << static_cast<quint16>((value >> 16) & 0xFFFF);
    data << static_cast<quint16>((value >> 32) & 0xFFFF);
    data << static_cast<quint16>((value >> 48) & 0xFFFF);
    
    ModbusRequest request;
    request.type = ModbusRequest::WriteHoldingRegisters;
    request.startAddress = address;
    request.count = 4;
    request.writeData = data;
    request.dataType = ModbusDataType::Long64;
    request.unitId = 1;
    
    qint64 requestId = worker->queueWriteRequest(request, priority, true);
    m_pendingWriteRequests[requestId] = QString("WriteHoldingRegisterLong64[%1:%2@%3]").arg(host).arg(port).arg(address);
    connectWorkerSignals(worker);
    
    qDebug() << "Queued Long64 register write:" << host << ":" << port << "address" << address << "value" << value << "priority" << static_cast<int>(priority) << "requestId" << requestId;
    return requestId;
}

qint64 ScadaCoreService::writeCoil(const QString &host, int port, int address, bool value, RequestPriority priority)
{
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, 1);
    if (!worker) {
        emit writeCompleted(0, QString("WriteCoil[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to create worker");
        return 0;
    }
    
    // Create Modbus request
    ModbusRequest request;
    request.type = ModbusRequest::WriteCoils;
    request.startAddress = address;
    request.count = 1;
    request.writeBoolData = QVector<bool>() << value;
    request.dataType = ModbusDataType::Coil;
    request.unitId = 1;
    
    qint64 requestId = worker->queueWriteRequest(request, priority, true);
    
    // Track the request
    m_pendingWriteRequests[requestId] = QString("WriteCoil[%1:%2@%3]").arg(host).arg(port).arg(address);
    
    // Connect worker signals if not already connected
    connectWorkerSignals(worker);
    
    qDebug() << "Queued coil write:" << host << ":" << port << "address" << address << "value" << value << "priority" << static_cast<int>(priority) << "requestId" << requestId;
    return requestId;
}

// Batch write operations
QVector<qint64> ScadaCoreService::writeHoldingRegistersBatch(const QString &host, int port, const QVector<QPair<int, quint16>> &addressValuePairs, RequestPriority priority)
{
    QVector<qint64> requestIds;
    for (const auto &pair : addressValuePairs) {
        qint64 requestId = writeHoldingRegister(host, port, pair.first, pair.second, priority);
        requestIds.append(requestId);
    }
    return requestIds;
}

QVector<qint64> ScadaCoreService::writeCoilsBatch(const QString &host, int port, const QVector<QPair<int, bool>> &addressValuePairs, RequestPriority priority)
{
    QVector<qint64> requestIds;
    for (const auto &pair : addressValuePairs) {
        qint64 requestId = writeCoil(host, port, pair.first, pair.second, priority);
        requestIds.append(requestId);
    }
    return requestIds;
}

// Worker management methods
QStringList ScadaCoreService::getActiveDevices() const
{
    if (!m_workerManager) {
        return QStringList();
    }
    
    QStringList devices;
    QMetaObject::invokeMethod(m_workerManager, "getActiveDevices",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QStringList, devices));
    return devices;
}

ModbusWorkerManager::GlobalStatistics ScadaCoreService::getGlobalStatistics() const
{
    if (!m_workerManager) {
        return ModbusWorkerManager::GlobalStatistics();
    }
    
    ModbusWorkerManager::GlobalStatistics stats;
    QMetaObject::invokeMethod(m_workerManager, "getGlobalStatistics",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(ModbusWorkerManager::GlobalStatistics, stats));
    return stats;
}

void ScadaCoreService::setWorkerPollInterval(const QString &deviceKey, int intervalMs)
{
    if (!m_workerManager) {
        return;
    }
    
    QMetaObject::invokeMethod(m_workerManager, "setWorkerPollInterval",
                              Qt::QueuedConnection,
                              Q_ARG(QString, deviceKey),
                              Q_ARG(int, intervalMs));
}

// Helper methods
qint64 ScadaCoreService::generateRequestId()
{
    static QAtomicInteger<qint64> counter(1);
    return counter.fetchAndAddOrdered(1);
}

void ScadaCoreService::connectWorkerSignals(ModbusWorker* worker)
{
    if (!worker || m_connectedWorkers.contains(worker)) {
        return;
    }
    
    connect(worker, &ModbusWorker::readCompleted,
            this, &ScadaCoreService::onWorkerReadCompleted,
            Qt::QueuedConnection);
    
    connect(worker, &ModbusWorker::writeCompleted,
            this, &ScadaCoreService::onWorkerWriteCompleted,
            Qt::QueuedConnection);
    
    connect(worker, &ModbusWorker::errorOccurred,
            this, &ScadaCoreService::onWorkerError,
            Qt::QueuedConnection);
    
    connect(worker, &ModbusWorker::requestInterrupted,
            this, &ScadaCoreService::onWorkerRequestInterrupted,
            Qt::QueuedConnection);
    
    m_connectedWorkers.insert(worker);
}

void ScadaCoreService::onPollTimer()
{
    qDebug() << "ScadaCoreService::onPollTimer() - Poll timer triggered. Service running:" << m_serviceRunning << "Data points count:" << m_dataPoints.size();
    if (!m_serviceRunning || m_dataPoints.isEmpty()) {
        qDebug() << "ScadaCoreService::onPollTimer() - Skipping poll: service running =" << m_serviceRunning << ", data points =" << m_dataPoints.size();
        return;
    }
    qDebug() << "ScadaCoreService::onPollTimer() - Processing next data point...";
    processNextDataPoint();
}

void ScadaCoreService::processNextDataPoint()
{
    if (m_currentPointIndex >= m_dataPoints.size()) {
        m_currentPointIndex = 0;
    }
    
    if (m_dataPoints.isEmpty()) {
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Optimized single-threaded mode: simplified polling without complex priority logic
    if (m_useSingleThreadedMode) {
        // Simple round-robin through all enabled points
        int startIndex = m_currentPointIndex;
        int attempts = 0;
        
        while (attempts < m_dataPoints.size()) {
            const DataAcquisitionPoint &point = m_dataPoints[m_currentPointIndex];
            
            if (point.enabled) {
                qint64 lastPollTime = m_lastPollTimes.value(point.name, 0);
                if (currentTime - lastPollTime >= point.pollInterval) {
                    // Process this point and move to next
                    processDataPoint(point, currentTime);
                    m_currentPointIndex = (m_currentPointIndex + 1) % m_dataPoints.size();
                    return;
                }
            }
            
            m_currentPointIndex = (m_currentPointIndex + 1) % m_dataPoints.size();
            attempts++;
            
            // Prevent infinite loop
            if (m_currentPointIndex == startIndex) {
                break;
            }
        }
        return;
    }
    
    // Multi-threaded mode: complex priority-based polling
    // First pass: Look for ready block points to prioritize by data type
    
    // Collect ready block points and sort by priority
    QVector<QPair<int, int>> readyBlockIndices; // priority, index
    
    for (int i = 0; i < m_dataPoints.size(); i++) {
        const DataAcquisitionPoint &blockPoint = m_dataPoints[i];
        
        // Skip if not enabled or not a block
        if (!blockPoint.enabled || !blockPoint.tags.contains("block_type")) {
            continue;
        }
        
        // Check if block is ready for polling
        qint64 lastPollTime = m_lastPollTimes.value(blockPoint.name, 0);
        if (currentTime - lastPollTime >= blockPoint.pollInterval) {
            // Get priority from block tags (lower number = higher priority)
            int priority = blockPoint.tags.value("data_type_priority", "99").toInt();
            readyBlockIndices.append(qMakePair(priority, i));
        }
    }
    
    // Sort by priority (INT16=1, FLOAT32=2, DOUBLE64=3)
    if (!readyBlockIndices.isEmpty()) {
        std::sort(readyBlockIndices.begin(), readyBlockIndices.end());
        
        // Process ALL ready blocks in priority order to ensure lower priority blocks get processed
        for (const auto &pair : readyBlockIndices) {
            int blockIndex = pair.second;
            const DataAcquisitionPoint &prioritizedBlock = m_dataPoints[blockIndex];
            
            qDebug() << "Processing prioritized block:" << prioritizedBlock.name 
                     << "Data type:" << prioritizedBlock.tags.value("block_data_type", "UNKNOWN")
                     << "Priority:" << prioritizedBlock.tags.value("data_type_priority", "99");
            
            processDataPoint(prioritizedBlock, currentTime);
        }
        return;
    }
    
    // Second pass: Process individual points with priority ordering
    // Collect ready individual points and sort by data type priority
    QVector<QPair<int, int>> readyIndividualIndices; // priority, index
    
    for (int i = 0; i < m_dataPoints.size(); i++) {
        const DataAcquisitionPoint &point = m_dataPoints[i];
        
        // Skip if not enabled or is a block
        if (!point.enabled || point.tags.contains("block_type")) {
            continue;
        }
        
        // Skip individual points if they are covered by an optimized block
        if (isPointCoveredByBlock(point)) {
            continue;
        }
        
        // Check if point is ready for polling
        qint64 lastPollTime = m_lastPollTimes.value(point.name, 0);
        qint64 timeSinceLastPoll = currentTime - lastPollTime;
        bool isReady = timeSinceLastPoll >= point.pollInterval;
        
        if (isReady) {
            // Determine priority based on data type
            int priority = 99; // Default low priority
            switch (point.dataType) {
                case ModbusDataType::HoldingRegister:
                    priority = 1; // INT16 - highest priority
                    break;
                case ModbusDataType::Float32:
                case ModbusDataType::Long32:
                    priority = 2; // FLOAT32/LONG32 - second priority
                    break;
                case ModbusDataType::Double64:
                case ModbusDataType::Long64:
                    priority = 3; // DOUBLE64/LONG64 - third priority
                    break;
                case ModbusDataType::InputRegister:
                case ModbusDataType::Coil:
                case ModbusDataType::DiscreteInput:
                case ModbusDataType::BOOL:
                    priority = 1; // Other types and BOOL - high priority
                    break;
            }
            readyIndividualIndices.append(qMakePair(priority, i));
        }
    }
    
    // Sort by priority and process the highest priority individual point
    if (!readyIndividualIndices.isEmpty()) {
        std::sort(readyIndividualIndices.begin(), readyIndividualIndices.end());
        
        // Find all points with the highest priority
        int highestPriority = readyIndividualIndices.first().first;
        QVector<QPair<int, int>> highestPriorityPoints;
        
        for (const auto &pair : readyIndividualIndices) {
            if (pair.first == highestPriority) {
                highestPriorityPoints.append(pair);
            } else {
                break; // Since sorted, no more highest priority points
            }
        }
        
        // Use round-robin selection for points with same priority
        static int roundRobinIndex = 0;
        int selectedIndex = roundRobinIndex % highestPriorityPoints.size();
        roundRobinIndex = (roundRobinIndex + 1) % highestPriorityPoints.size();
        
        int pointIndex = highestPriorityPoints[selectedIndex].second;
        const DataAcquisitionPoint &prioritizedPoint = m_dataPoints[pointIndex];
        
        qDebug() << "Processing individual point:" << prioritizedPoint.name;
        
        processDataPoint(prioritizedPoint, currentTime);
        return;
    }
    
    // Fallback: process next point in sequence if no prioritized points are ready
    m_currentPointIndex++;
    if (m_currentPointIndex >= m_dataPoints.size()) {
        m_currentPointIndex = 0;
    }
}

void ScadaCoreService::processDataPoint(const DataAcquisitionPoint &point, qint64 currentTime)
{
    QString unitIdStr = point.tags.value("unit_id", "1");
    int unitId = unitIdStr.toInt();
    QString deviceKey = QString("%1:%2:%3").arg(point.host).arg(point.port).arg(unitIdStr);
    
    qint64 requestId = generateRequestId();
    
    // Track operation start time for performance monitoring
    if (m_performanceMonitoringEnabled) {
        QMutexLocker locker(&m_performanceMetricsMutex);
        m_operationStartTimes[requestId] = m_operationTimer.elapsed();
    }
    
    // Check for simulation mode
    if (point.tags.contains("simulation_mode") && point.tags["simulation_mode"] == "true") {
        // Generate simulated data
        m_lastPollTimes[point.name] = currentTime;
        m_responseTimers[point.name] = currentTime;
        
        qDebug() << "🎭 Simulation mode polling:" << point.name << "at address" << point.address;
        
        // Generate mock data based on data type
        QVariant simulatedValue;
        if (point.dataType == ModbusDataType::Float32) {
            // Generate realistic sensor values
            if (point.measurement == "temperature") {
                simulatedValue = 20.0f + (QRandomGenerator::global()->bounded(100)) / 10.0f; // 20-30°C
            } else if (point.measurement == "pressure") {
                simulatedValue = 1000.0f + QRandomGenerator::global()->bounded(200); // 1000-1200 hPa
            } else {
                simulatedValue = QRandomGenerator::global()->bounded(1000) / 10.0f;
            }
        } else {
            // Generate integer values for status/holding registers
            simulatedValue = QRandomGenerator::global()->bounded(100);
        }
        
        // Create simulated result
        AcquiredDataPoint dataPoint;
        dataPoint.pointName = point.name;
        dataPoint.value = simulatedValue;
        dataPoint.isValid = true;
        dataPoint.timestamp = QDateTime::currentMSecsSinceEpoch();
        dataPoint.measurement = point.measurement;
        dataPoint.tags = point.tags;
        
        // Emit the simulated data
        emit dataPointAcquired(dataPoint);
        
        // Update statistics
        updateStatistics(true, 10); // Simulate 10ms response time
        return;
    }
    
    // Handle single-threaded mode
    if (m_useSingleThreadedMode) {
        if (!m_singleThreadModbusManager) {
            emit errorOccurred("Single-threaded ModbusManager not initialized");
            return;
        }
        
        // Start data acquisition in single-threaded mode
        m_lastPollTimes[point.name] = currentTime;
        m_responseTimers[point.name] = currentTime;
        
        qDebug() << "🔧 Single-threaded polling:" << point.name << "at address" << point.address << "Unit ID:" << unitId;
        
        // Create read request for single-threaded mode
        ModbusRequest request;
        request.startAddress = point.address;
        request.unitId = unitId;
        request.requestTime = QDateTime::currentMSecsSinceEpoch();
        request.dataType = point.dataType;
        
        // Check if this is an optimized block read
        if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
            // This is a block read - use block size
            int blockSize = point.tags["block_size"].toInt();
            request.count = blockSize;
            qDebug() << "🔧 Single-threaded block read:" << point.name << "Address:" << point.address 
                     << "Block Size:" << blockSize << "Unit ID:" << unitId;
        } else {
            // Individual point read
            request.count = 1;
            qDebug() << "🔧 Single-threaded individual read:" << point.name << "Address:" << point.address 
                     << "Unit ID:" << unitId;
        }
        
        request.type = ModbusRequest::ReadHoldingRegisters;
        
        // Execute read directly using single ModbusManager
        m_singleThreadModbusManager->readHoldingRegisters(request.startAddress, request.count, request.dataType, request.unitId);
        return;
    }
    
    // Multi-threaded mode: Get or create worker for this device
    ModbusWorker* worker = m_workerManager->getOrCreateWorker(point.host, point.port, unitId);
    if (!worker) {
        emit errorOccurred(QString("Failed to create worker for device: %1").arg(deviceKey));
        return;
    }
    
    // Connect worker signals if not already connected
    connectWorkerSignals(worker);
    
    // Start data acquisition
    m_lastPollTimes[point.name] = currentTime;
    m_responseTimers[point.name] = currentTime;
    
    qDebug() << "Polling data point:" << point.name << "at address" << point.address << "Unit ID:" << unitId << "Device:" << deviceKey;
    
    // Create read request for the worker
    ModbusRequest request;
    request.startAddress = point.address;
    request.unitId = unitId;
    request.requestTime = QDateTime::currentMSecsSinceEpoch();
    
    // Check if this is an optimized block read with combined conditions
    if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
        // This is a block read - use block methods with register_type and data_type validation
        int blockSize = point.tags["block_size"].toInt();
        QString registerType = point.tags.value("register_type", "");
        QString dataType = point.tags.value("data_type", "");
        
        qDebug() << "Performing block read - Address:" << point.address << "Size:" << blockSize
                 << "Register Type:" << registerType << "Data Type:" << dataType << "Unit ID:" << unitId;
        
        request.count = blockSize;
        
        // Use combined condition: register_type + data_type for precise Modbus operation selection
        if (registerType == "HOLDING_REGISTER") {
            // Handle holding register types based on data_type
            if (dataType == "Int16" || point.dataType == ModbusDataType::HoldingRegister) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::HoldingRegister;
            } else if (dataType == "Float32" || point.dataType == ModbusDataType::Float32) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Float32;
            } else if (dataType == "Double64" || point.dataType == ModbusDataType::Double64) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Double64;
            } else if (dataType == "Long32" || point.dataType == ModbusDataType::Long32) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Long32;
            } else if (dataType == "Long64" || point.dataType == ModbusDataType::Long64) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Long64;
            } else {
                // Default to HoldingRegister for unknown data types
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::HoldingRegister;
            }
        } else if (registerType == "INPUT_REGISTER") {
            // Handle input register types
            request.type = ModbusRequest::ReadInputRegisters;
            request.dataType = point.dataType;
        } else if (registerType == "COIL") {
            // Handle coil types
            request.type = ModbusRequest::ReadCoils;
            request.dataType = ModbusDataType::Coil;
        } else if (registerType == "DISCRETE_INPUT") {
            // Handle discrete input types
            request.type = ModbusRequest::ReadDiscreteInputs;
            request.dataType = ModbusDataType::DiscreteInput;
        } else if (registerType == "STATUS") {
            // Handle STATUS register type for block reads - map to DiscreteInput for BOOL data types
            request.type = ModbusRequest::ReadDiscreteInputs;
            request.dataType = ModbusDataType::DiscreteInput;
        } else {
            // Fallback to original dataType-based logic if register_type is not specified
            switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::Float32:
            case ModbusDataType::Double64:
            case ModbusDataType::Long32:
            case ModbusDataType::Long64:
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = point.dataType;
                break;
            case ModbusDataType::InputRegister:
                request.type = ModbusRequest::ReadInputRegisters;
                request.dataType = point.dataType;
                break;
            case ModbusDataType::Coil:
                request.type = ModbusRequest::ReadCoils;
                request.dataType = ModbusDataType::Coil;
                break;
            case ModbusDataType::DiscreteInput:
            case ModbusDataType::BOOL:
                // Default BOOL to DiscreteInput for block reads
                request.type = ModbusRequest::ReadDiscreteInputs;
                request.dataType = ModbusDataType::DiscreteInput;
                break;
            }
        }
    } else {
        // Individual point read with combined conditions
        QString registerType = point.tags.value("register_type", "");
        QString dataType = point.tags.value("data_type", "");
        
        qDebug() << "Performing individual read - Address:" << point.address
                 << "Register Type:" << registerType << "Data Type:" << dataType << "Unit ID:" << unitId;
        
        // Use combined condition: register_type + data_type for precise Modbus operation selection
        if (registerType == "HOLDING_REGISTER") {
            // Handle holding register types based on data_type
            if (dataType == "Int16" || point.dataType == ModbusDataType::HoldingRegister) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::HoldingRegister;
                request.count = 1;
            } else if (dataType == "Float32" || point.dataType == ModbusDataType::Float32) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Float32;
                request.count = 2;
            } else if (dataType == "Double64" || point.dataType == ModbusDataType::Double64) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Double64;
                request.count = 4;
            } else if (dataType == "Long32" || point.dataType == ModbusDataType::Long32) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Long32;
                request.count = 2;
            } else if (dataType == "Long64" || point.dataType == ModbusDataType::Long64) {
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::Long64;
                request.count = 4;
            } else {
                // Default to HoldingRegister for unknown data types
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = ModbusDataType::HoldingRegister;
                request.count = 1;
            }
        } else if (registerType == "INPUT_REGISTER") {
            // Handle input register types
            request.type = ModbusRequest::ReadInputRegisters;
            request.dataType = point.dataType;
            request.count = 1;
        } else if (registerType == "COIL") {
            // Handle coil types
            request.type = ModbusRequest::ReadCoils;
            request.dataType = ModbusDataType::Coil;
            request.count = 1;
        } else if (registerType == "DISCRETE_INPUT") {
            // Handle discrete input types
            request.type = ModbusRequest::ReadDiscreteInputs;
            request.dataType = ModbusDataType::DiscreteInput;
            request.count = 1;
        } else if (registerType == "STATUS") {
            // Handle STATUS register type - map to appropriate Modbus operation based on data type
            if (dataType == "Bool" || point.dataType == ModbusDataType::BOOL || point.dataType == ModbusDataType::Coil) {
                request.type = ModbusRequest::ReadCoils;
                request.dataType = ModbusDataType::Coil;
                request.count = 1;
            } else if (point.dataType == ModbusDataType::DiscreteInput) {
                request.type = ModbusRequest::ReadDiscreteInputs;
                request.dataType = ModbusDataType::DiscreteInput;
                request.count = 1;
            } else {
                // Default STATUS+BOOL to DiscreteInput
                request.type = ModbusRequest::ReadDiscreteInputs;
                request.dataType = ModbusDataType::DiscreteInput;
                request.count = 1;
            }
        } else {
            // Fallback to original dataType-based logic if register_type is not specified
            switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::Float32:
            case ModbusDataType::Double64:
            case ModbusDataType::Long32:
            case ModbusDataType::Long64:
                request.type = ModbusRequest::ReadHoldingRegisters;
                request.dataType = point.dataType;
                request.count = (point.dataType == ModbusDataType::Float32 || point.dataType == ModbusDataType::Long32) ? 2 :
                               (point.dataType == ModbusDataType::Double64 || point.dataType == ModbusDataType::Long64) ? 4 : 1;
                break;
            case ModbusDataType::InputRegister:
                request.type = ModbusRequest::ReadInputRegisters;
                request.dataType = point.dataType;
                request.count = 1;
                break;
            case ModbusDataType::Coil:
                request.type = ModbusRequest::ReadCoils;
                request.dataType = ModbusDataType::Coil;
                request.count = 1;
                break;
            case ModbusDataType::DiscreteInput:
                request.type = ModbusRequest::ReadDiscreteInputs;
                request.dataType = ModbusDataType::DiscreteInput;
                request.count = 1;
                break;
            case ModbusDataType::BOOL:
                // Default BOOL to DiscreteInput when no register_type specified
                request.type = ModbusRequest::ReadDiscreteInputs;
                request.dataType = ModbusDataType::DiscreteInput;
                request.count = 1;
                break;
            }
        }
    }
    
    // Submit the request to the worker using thread-safe queued connection
    requestId = worker->queueReadRequest(request, RequestPriority::Normal);
    
    // Track the request for completion handling with thread safety
    {
        QMutexLocker locker(&m_requestTrackingMutex);
        m_pendingReadRequests[requestId] = point;
    }
    
    // Update statistics with thread safety
    {
        QMutexLocker locker(&m_statisticsMutex);
        m_statistics.totalReadOperations++;
    }
}

// Obsolete ModbusManager methods removed - now using ModbusWorkerManager architecture

bool ScadaCoreService::writeToTelegrafSocket(const QString& socketPath, const QByteArray& message)
{
    if (!QFileInfo::exists(socketPath)) {
        qCritical() << "Unix socket does not exist:" << socketPath;
        return false;
    }
    
    int sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return false;
    }
    
    struct sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.toUtf8().constData(), sizeof(addr.sun_path) - 1);
    
    int result = ::sendto(sockfd, message.constData(), message.size(), 0,
                          reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    ::close(sockfd);
    
    if (result < 0) {
        perror("sendto");
        return false;
    }
    
    qDebug() << "Sent InfluxDB line protocol to Telegraf via UNIX socket:" << message.trimmed();
    return true;
}

bool ScadaCoreService::writeToInflux(const QString& measurement, const QString& device, const QVariant& value, const QString& description)
{
    if (!value.isValid()) {
        qDebug() << "Invalid value for" << measurement << "on device" << device;
        return false;
    }
    
    // Sanitize tag values by removing spaces
    QString sanitizedMeasurement = measurement;
    sanitizedMeasurement.replace(" ", "_");
    
    QString sanitizedDevice = device;
    sanitizedDevice.replace(" ", "_");
    
    QString sanitizedDescription = description.isEmpty() ? 
        QString("SCADA_data_point_for_%1").arg(sanitizedDevice) : description;
    sanitizedDescription.replace(" ", "_");
    
    QString sanitizedStationName = device;
    
    // Create InfluxDB line protocol with enhanced mapping tags (no spaces in tag values)
    QString line = QString("%1,device_name=%2,tag_name=%3,description=%4,station_name=%5 value=%6")
                   .arg(sanitizedMeasurement, sanitizedDevice, sanitizedDevice, sanitizedDescription, sanitizedStationName, value.toString());
    
    line += "\n";
    
    bool success = writeToTelegrafSocket(m_telegrafSocketPath, line.toUtf8());
    qDebug() << "Line to write:" << line;
    
    if (!success) {
        qDebug() << "Failed to write to InfluxDB for" << measurement << "on device" << device;
        m_statistics.socketErrors++;
    } else {
        qDebug() << "Successfully wrote to InfluxDB:" << line;
        m_statistics.totalDataPointsSent++;
    }
    
    return success;
}

// Enhanced InfluxDB write method with full tag support
bool ScadaCoreService::writeToInfluxEnhanced(const AcquiredDataPoint &dataPoint)
{
    if (!dataPoint.isValid || dataPoint.measurement.isEmpty()) {
        qDebug() << "Invalid data point:" << dataPoint.pointName;
        return false;
    }
    
    // Sanitize tag values by removing spaces
    QString sanitizedMeasurement = dataPoint.measurement;
    sanitizedMeasurement.replace(" ", "_");
    
    QString deviceName = dataPoint.tags.value("device_name", "STATION_TEST");
    deviceName.replace(" ", "_");
    
    QString dataType = dataPoint.tags.value("data_type", "Int16");
    dataType.replace(" ", "_");
    
    QString readMode = dataPoint.tags.value("read_mode", "single_register");
    readMode.replace(" ", "_");
    
    QString address = dataPoint.tags.value("address", "0");
    
    QString description = dataPoint.tags.value("description", QString("CURRENT_RTU_%1").arg(address));
    description.replace(" ", "_");
    
    // Create enhanced InfluxDB line protocol with all required tags
    QString line = QString("%1,device_name=%2,data_type=%3,read_mode=%4,address=%5,description=%6 value=%7")
                   .arg(sanitizedMeasurement, deviceName, dataType, readMode, address, description, dataPoint.value.toString());
    
    line += "\n";
    
    bool success = writeToTelegrafSocket(m_telegrafSocketPath, line.toUtf8());
    qDebug() << "Enhanced InfluxDB line:" << line;
    
    if (!success) {
        qDebug() << "Failed to write enhanced data to InfluxDB for" << dataPoint.pointName;
        m_statistics.socketErrors++;
    } else {
        qDebug() << "Successfully wrote enhanced data to InfluxDB:" << line;
        m_statistics.totalDataPointsSent++;
    }
    
    return success;
}

bool ScadaCoreService::sendDataToInflux(const AcquiredDataPoint &dataPoint)
{
    if (!dataPoint.isValid || dataPoint.measurement.isEmpty()) {
        qWarning() << "Invalid data point:" << dataPoint.pointName;
        return false;
    }
    
    // Use the enhanced InfluxDB write method with full tag support
    bool success = writeToInfluxEnhanced(dataPoint);
    
    if (success) {
        emit dataPointSentToInflux(dataPoint.pointName, true);
    } else {
        emit dataPointSentToInflux(dataPoint.pointName, false);
        emit errorOccurred(QString("Failed to send data point to InfluxDB: %1").arg(dataPoint.pointName));
    }
    
    return success;
}



bool ScadaCoreService::connectToModbusHost(const QString &host, int port)
{
    QString deviceKey = QString("%1:%2").arg(host).arg(port);
    
    // Check if we already have a worker for this device
    if (m_connectionStates.contains(deviceKey) && m_connectionStates[deviceKey]) {
        qDebug() << "Already connected to" << deviceKey << "- reusing connection";
        return true;
    }
    
    // Create or get worker for this device
    if (!m_workerManager) {
        qWarning() << "Worker manager not initialized";
        return false;
    }
    
    // The worker manager will handle the connection automatically
    // when we submit requests to this device
    qDebug() << "Device" << deviceKey << "will be connected when needed";
    
    // Mark as connected (the worker will update the actual state)
    m_connectionStates[deviceKey] = true;
    
    return true;
}

void ScadaCoreService::updateStatistics(bool success, qint64 responseTime)
{
    QMutexLocker locker(&m_statisticsMutex);
    
    m_statistics.totalReadOperations++;
    
    // Update success/failure counters
    if (success) {
        m_statistics.successfulReads++;
    } else {
        m_statistics.failedReads++;
    }
    
    if (responseTime > 0) {
        // Update average response time
        qint64 totalOps = m_statistics.successfulReads + m_statistics.failedReads;
        if (totalOps > 0) {
            m_statistics.averageResponseTime = 
                (m_statistics.averageResponseTime * (totalOps - 1) + responseTime) / totalOps;
        } else {
            m_statistics.averageResponseTime = responseTime;
        }
    }
    
    // Emit statistics update periodically (every 100 operations)
    if (m_statistics.totalReadOperations % 100 == 0) {
        ServiceStatistics statsCopy = m_statistics;
        locker.unlock();
        emit statisticsUpdated(statsCopy);
    }
}

QJsonObject ScadaCoreService::dataPointToJson(const AcquiredDataPoint &dataPoint)
{
    QJsonObject json;
    json["pointName"] = dataPoint.pointName;
    json["value"] = QJsonValue::fromVariant(dataPoint.value);
    json["timestamp"] = dataPoint.timestamp;
    json["measurement"] = dataPoint.measurement;
    json["isValid"] = dataPoint.isValid;
    
    if (!dataPoint.errorMessage.isEmpty()) {
        json["error"] = dataPoint.errorMessage;
    }
    
    if (!dataPoint.tags.isEmpty()) {
        QJsonObject tagsObj;
        for (auto it = dataPoint.tags.constBegin(); it != dataPoint.tags.constEnd(); ++it) {
            tagsObj[it.key()] = it.value();
        }
        json["tags"] = tagsObj;
    }
    
    return json;
}

void ScadaCoreService::handleBlockReadResult(const ModbusReadResult &result, const DataAcquisitionPoint &blockPoint)
{
    if (!result.success) {
        qWarning() << "Block read failed for" << blockPoint.name << "Error:" << result.errorString;
        m_statistics.failedReads++;
        return;
    }
    
    // Get block information from tags
    int blockSize = blockPoint.tags.value("block_size", "1").toInt();
    int startAddress = blockPoint.address;
    
    // Get original point metadata from block tags
    QStringList originalAddresses = blockPoint.tags.value("original_addresses", "").split(",", Qt::SkipEmptyParts);
    QStringList originalNames = blockPoint.tags.value("original_names", "").split(",", Qt::SkipEmptyParts);
    QStringList originalDataTypes = blockPoint.tags.value("original_data_types", "").split(",", Qt::SkipEmptyParts);
    QStringList originalDescriptions = blockPoint.tags.value("original_descriptions", "").split(",", Qt::SkipEmptyParts);
    QStringList originalMeasurements = blockPoint.tags.value("original_measurements", "").split(",", Qt::SkipEmptyParts);
    
    qDebug() << "Processing block read result for" << blockPoint.name 
             << "Start address:" << startAddress 
             << "Block size:" << blockSize
             << "Original points:" << originalAddresses.size()
             << "Raw data size:" << result.rawData.size();
    
    // Validate we have enough data
    if (result.rawData.size() < blockSize) {
        qWarning() << "Insufficient data in block read. Expected:" << blockSize 
                   << "Got:" << result.rawData.size();
        return;
    }
    
    // Validate metadata consistency
    if (originalAddresses.size() != originalNames.size() || 
        originalAddresses.size() != originalDataTypes.size() ||
        originalAddresses.size() != originalDescriptions.size() ||
        originalAddresses.size() != originalMeasurements.size()) {
        qWarning() << "Inconsistent original point metadata in block" << blockPoint.name;
        return;
    }
    
    // Process each original point
    for (int pointIndex = 0; pointIndex < originalAddresses.size(); pointIndex++) {
        int originalAddress = originalAddresses[pointIndex].toInt();
        int dataTypeInt = originalDataTypes[pointIndex].toInt();
        ModbusDataType dataType = static_cast<ModbusDataType>(dataTypeInt);
        
        // Calculate offset from start address based on actual register address difference
        int offset = originalAddress - startAddress;
        
        qDebug() << "Processing point" << pointIndex << ":"
                 << "Name:" << originalNames[pointIndex]
                 << "Address:" << originalAddress
                 << "Start Address:" << startAddress
                 << "Calculated Offset:" << offset
                 << "Data Type:" << dataTypeInt
                 << "Raw data at offset:" << (offset < result.rawData.size() ? QString::number(result.rawData[offset]) : "OUT_OF_BOUNDS");
        
        // Register count validation for different data types
        
        // Validate offset bounds
        int registersNeeded = 1;
        switch (dataType) {
            case ModbusDataType::Float32:
            case ModbusDataType::Long32:
                registersNeeded = 2;
                break;
            case ModbusDataType::Double64:
            case ModbusDataType::Long64:
                registersNeeded = 4;
                break;
            default:
                registersNeeded = 1;
                break;
        }
        
        if (offset < 0 || (offset + registersNeeded - 1) >= result.rawData.size()) {
            qWarning() << "Address offset out of range:" << offset << "(needs" << registersNeeded << "registers) for address" << originalAddress << "in block of size" << result.rawData.size();
            continue;
        }
        
        // Create acquired data point with enhanced mapping
        AcquiredDataPoint dataPoint;
        dataPoint.pointName = originalNames[pointIndex];
        dataPoint.timestamp = result.timestamp;
        dataPoint.measurement = originalMeasurements[pointIndex];
        dataPoint.tags = blockPoint.tags; // Start with block tags
        dataPoint.isValid = true;
        
        // Enhanced InfluxDB mapping fields
        dataPoint.tags["device_name"] = blockPoint.tags.value("device_name", "STATION_TEST");
        dataPoint.tags["address"] = QString::number(originalAddress);
        dataPoint.tags["description"] = originalDescriptions[pointIndex];
        
        // Map read_mode based on data type
        QString readMode;
        switch (dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::InputRegister:
                readMode = "single_register";
                break;
            case ModbusDataType::Float32:
            case ModbusDataType::Long32:
                readMode = "dual_register";
                break;
            case ModbusDataType::Double64:
            case ModbusDataType::Long64:
                readMode = "quad_register";
                break;
            case ModbusDataType::Coil:
            case ModbusDataType::DiscreteInput:
                readMode = "single_bit";
                break;
            default:
                readMode = "single_register";
        }
        dataPoint.tags["read_mode"] = readMode;
        
        // Map data_type to string
        QString dataTypeStr;
        switch (dataType) {
            case ModbusDataType::HoldingRegister:
                dataTypeStr = "Int16";
                break;
            case ModbusDataType::InputRegister:
                dataTypeStr = "Int16";
                break;
            case ModbusDataType::Coil:
                dataTypeStr = "Bool";
                break;
            case ModbusDataType::DiscreteInput:
                dataTypeStr = "Bool";
                break;
            case ModbusDataType::Float32:
                dataTypeStr = "Float32";
                break;
            case ModbusDataType::Double64:
                dataTypeStr = "Double64";
                break;
            case ModbusDataType::Long32:
                dataTypeStr = "Long32";
                break;
            case ModbusDataType::Long64:
                dataTypeStr = "Long64";
                break;
            case ModbusDataType::BOOL:
                dataTypeStr = "Bool";
                break;
            default:
                dataTypeStr = "Int16";
        }
        dataPoint.tags["data_type"] = dataTypeStr;
        
        // Extract value based on data type
        switch (dataType) {
        case ModbusDataType::Float32:
            if (offset + 1 < result.rawData.size()) {
                quint16 high = result.rawData[offset];
                quint16 low = result.rawData[offset + 1];
                quint32 combined = (static_cast<quint32>(high) << 16) | low;
                float floatValue;
                memcpy(&floatValue, &combined, sizeof(float));
                dataPoint.value = floatValue;
                
#ifdef FLOAT32_DECODE_DEBUG
                qDebug() << "Float32 decode for" << originalNames[pointIndex] << ":"
                         << "High register [" << offset << "]:" << QString::number(high, 16)
                         << "Low register [" << (offset + 1) << "]:" << QString::number(low, 16)
                         << "Combined hex:" << QString::number(combined, 16)
                         << "Float value:" << floatValue;
#endif
            } else {
                qWarning() << "Float32 decode failed for" << originalNames[pointIndex] 
                          << "- insufficient data. Offset:" << offset 
                          << "Data size:" << result.rawData.size();
            }
            break;
        case ModbusDataType::Double64:
            if (offset + 3 < result.rawData.size()) {
                quint16 reg0 = result.rawData[offset];
                quint16 reg1 = result.rawData[offset + 1];
                quint16 reg2 = result.rawData[offset + 2];
                quint16 reg3 = result.rawData[offset + 3];
                
                quint64 combined = (static_cast<quint64>(reg0) << 48) |
                                 (static_cast<quint64>(reg1) << 32) |
                                 (static_cast<quint64>(reg2) << 16) |
                                 reg3;
                double doubleValue;
                memcpy(&doubleValue, &combined, sizeof(double));
                dataPoint.value = doubleValue;
                
#ifdef MODBUS_DEBUG_ENABLED
                qDebug() << "Double64 decode for" << originalNames[pointIndex] << ":"
                         << "Registers [" << offset << "-" << (offset+3) << "]:" 
                         << "[" << reg0 << ", " << reg1 << ", " << reg2 << ", " << reg3 << "]"
                         << "Hex: [0x" << QString::number(reg0, 16).toUpper()
                         << ", 0x" << QString::number(reg1, 16).toUpper()
                         << ", 0x" << QString::number(reg2, 16).toUpper()
                         << ", 0x" << QString::number(reg3, 16).toUpper() << "]"
                         << "Combined: 0x" << QString::number(combined, 16).toUpper()
                         << "Double value:" << doubleValue;
#endif
            } else {
                qWarning() << "Double64 decode failed for" << originalNames[pointIndex] 
                          << "- insufficient data. Offset:" << offset 
                          << "Data size:" << result.rawData.size();
            }
            break;
        case ModbusDataType::Long32:
            if (offset + 1 < result.rawData.size()) {
                qint32 combined = (static_cast<qint32>(result.rawData[offset]) << 16) | result.rawData[offset + 1];
                dataPoint.value = combined;
            }
            break;
        case ModbusDataType::Long64:
            if (offset + 3 < result.rawData.size()) {
                qint64 combined = (static_cast<qint64>(result.rawData[offset]) << 48) |
                                (static_cast<qint64>(result.rawData[offset + 1]) << 32) |
                                (static_cast<qint64>(result.rawData[offset + 2]) << 16) |
                                result.rawData[offset + 3];
                dataPoint.value = combined;
            }
            break;
        case ModbusDataType::BOOL:
            // Handle BOOL data type with comprehensive error handling
            if (offset < result.rawData.size()) {
                quint16 rawValue = result.rawData[offset];
                
                // Validate raw value range for BOOL conversion
                if (rawValue > 1) {
                    qWarning() << "BOOL conversion warning for" << originalNames[pointIndex] 
                              << "- raw value" << rawValue << "exceeds typical boolean range (0-1)."
                              << "Converting non-zero to true.";
                }
                
                // Convert to boolean: 0 = false, non-zero = true
                bool boolValue = (rawValue != 0);
                dataPoint.value = boolValue;
                
                // Validate QVariant conversion success
                if (!dataPoint.value.canConvert<bool>()) {
                    qWarning() << "BOOL QVariant conversion failed for" << originalNames[pointIndex]
                              << "- unable to convert to boolean type. Using default false.";
                    dataPoint.value = false;
                    dataPoint.isValid = false;
                    dataPoint.errorMessage = QString("BOOL conversion failed: QVariant cannot convert to bool");
                }
                
#ifdef MODBUS_DEBUG_ENABLED
                qDebug() << "BOOL decode for" << originalNames[pointIndex] << ":"
                         << "Raw value:" << rawValue
                         << "Boolean value:" << boolValue
                         << "Conversion valid:" << dataPoint.value.canConvert<bool>();
#endif
            } else {
                qWarning() << "BOOL decode failed for" << originalNames[pointIndex] 
                          << "- insufficient data. Offset:" << offset 
                          << "Data size:" << result.rawData.size();
                dataPoint.value = false; // Default to false on error
                dataPoint.isValid = false;
                dataPoint.errorMessage = QString("BOOL decode failed: insufficient data at offset %1").arg(offset);
            }
            break;
        default:
            // For simple types (HoldingRegister, InputRegister)
            dataPoint.value = result.rawData[offset];
            break;
        }
        
        qDebug() << "📊 Block Data Point Extracted:";
        qDebug() << "   Point Name:" << dataPoint.pointName;
        qDebug() << "   Address:" << originalAddress << "(offset" << offset << "from block start" << startAddress << ")";
        qDebug() << "   Data Type:" << dataTypeInt;
        qDebug() << "   Raw Value:" << result.rawData[offset] << "(register" << (startAddress + offset) << ")";
        qDebug() << "   Processed Value:" << dataPoint.value.toString();
        qDebug() << "   Measurement:" << dataPoint.measurement;
        qDebug() << "   Description:" << originalDescriptions[pointIndex];
        qDebug() << "   Timestamp:" << QDateTime::fromMSecsSinceEpoch(dataPoint.timestamp).toString("yyyy-MM-dd hh:mm:ss.zzz");
        qDebug() << "   ----------------------------------------";
        
        // Create a temporary source point for validation
        DataAcquisitionPoint tempSourcePoint;
        tempSourcePoint.address = originalAddress;
        tempSourcePoint.host = blockPoint.host;
        tempSourcePoint.dataType = static_cast<ModbusDataType>(dataTypeInt);
        tempSourcePoint.name = originalNames[pointIndex];
        tempSourcePoint.tags = blockPoint.tags;
        
        // Validate and ensure all required InfluxDB tags are present
        validateAndSetInfluxTags(dataPoint, tempSourcePoint);
        
        // Emit signal and send to InfluxDB
        emit dataPointAcquired(dataPoint);
        
        if (dataPoint.isValid) {
            bool sent = sendDataToInflux(dataPoint);
            
            if (sent) {
                m_statistics.totalDataPointsSent++;
            }
        }
    }
    
    m_statistics.successfulReads++;
    updateStatistics(true, 0);
    emit statisticsUpdated(m_statistics);
}

bool ScadaCoreService::isPointCoveredByBlock(const DataAcquisitionPoint &point)
{
    // Check if there's a block that covers this individual point
    for (const DataAcquisitionPoint &blockPoint : m_dataPoints) {
        // Skip if this is not a block point
        if (!blockPoint.tags.contains("block_type")) {
            continue;
        }
        
        // Check if the individual point matches the block's device and register type
        if (blockPoint.host != point.host || blockPoint.port != point.port) {
            continue;
        }
        
        // Get block information
        bool ok;
        int blockStartAddress = blockPoint.tags.value("block_start_address").toInt(&ok);
        if (!ok) continue;
        
        int blockSize = blockPoint.tags.value("block_size").toInt(&ok);
        if (!ok) continue;
        
        QString blockRegisterType = blockPoint.tags.value("register_type");
        QString pointRegisterType;
        
        // Map point data type to register type string
        switch (point.dataType) {
        case ModbusDataType::HoldingRegister:
        case ModbusDataType::Float32:
        case ModbusDataType::Double64:
        case ModbusDataType::Long32:
        case ModbusDataType::Long64:
            pointRegisterType = "HOLDING_REGISTER";
            break;
        case ModbusDataType::InputRegister:
            pointRegisterType = "INPUT_REGISTER";
            break;
        case ModbusDataType::Coil:
            pointRegisterType = "COIL";
            break;
        case ModbusDataType::DiscreteInput:
        case ModbusDataType::BOOL:
            pointRegisterType = "DISCRETE_INPUT";
            break;
        }
        
        // Check if register types match and point address is within block range
        if (blockRegisterType == pointRegisterType &&
            point.address >= blockStartAddress &&
            point.address < blockStartAddress + blockSize) {
            return true;
        }
    }
    
    return false;
}

void ScadaCoreService::validateAndSetInfluxTags(AcquiredDataPoint &dataPoint, const DataAcquisitionPoint &sourcePoint)
{
    // Ensure address tag is always set (critical for InfluxDB mapping)
    if (!dataPoint.tags.contains("address") || dataPoint.tags["address"].isEmpty()) {
        dataPoint.tags["address"] = QString::number(sourcePoint.address);
        qDebug() << "WARNING: Missing address tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Ensure device_name tag is set
    if (!dataPoint.tags.contains("device_name") || dataPoint.tags["device_name"].isEmpty()) {
        dataPoint.tags["device_name"] = sourcePoint.host;
        qDebug() << "WARNING: Missing device_name tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Ensure data_type tag is set
    if (!dataPoint.tags.contains("data_type") || dataPoint.tags["data_type"].isEmpty()) {
        QString dataTypeStr;
        switch (sourcePoint.dataType) {
            case ModbusDataType::HoldingRegister:
                dataTypeStr = "INT16";
                break;
            case ModbusDataType::InputRegister:
                dataTypeStr = "INPUT_REGISTER";
                break;
            case ModbusDataType::Coil:
                dataTypeStr = "COIL";
                break;
            case ModbusDataType::DiscreteInput:
                dataTypeStr = "DISCRETE_INPUT";
                break;
            case ModbusDataType::BOOL:
                dataTypeStr = "BOOL";
                break;
            case ModbusDataType::Float32:
                dataTypeStr = "FLOAT32";
                break;
            case ModbusDataType::Double64:
                dataTypeStr = "DOUBLE64";
                break;
            case ModbusDataType::Long32:
                dataTypeStr = "INT32";
                break;
            case ModbusDataType::Long64:
                dataTypeStr = "INT64";
                break;
            default:
                dataTypeStr = "UNKNOWN";
                break;
        }
        dataPoint.tags["data_type"] = dataTypeStr;
        qDebug() << "WARNING: Missing data_type tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Ensure description tag is set (use point name as fallback)
    if (!dataPoint.tags.contains("description") || dataPoint.tags["description"].isEmpty()) {
        dataPoint.tags["description"] = sourcePoint.name.isEmpty() ? dataPoint.pointName : sourcePoint.name;
        qDebug() << "WARNING: Missing description tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Copy any additional tags from source point
    for (auto it = sourcePoint.tags.constBegin(); it != sourcePoint.tags.constEnd(); ++it) {
        if (!dataPoint.tags.contains(it.key()) || dataPoint.tags[it.key()].isEmpty()) {
            dataPoint.tags[it.key()] = it.value();
        }
    }
    
    // Validate that critical tags are not empty after setting
    QStringList criticalTags = {"address", "device_name", "data_type", "description"};
    for (const QString &tag : criticalTags) {
        if (!dataPoint.tags.contains(tag) || dataPoint.tags[tag].isEmpty()) {
            qWarning() << "CRITICAL: Tag validation failed for" << tag << "in point:" << dataPoint.pointName;
        }
    }
}

// Worker-specific slot methods
void ScadaCoreService::onWorkerReadCompleted(qint64 requestId, const ModbusReadResult &result)
{
    // Handle read completion from worker threads
    qDebug() << "Worker read completed - Request ID:" << requestId << "success:" << result.success;
    
    // Track performance metrics for multi-threaded operations
    if (m_performanceMonitoringEnabled) {
        QMutexLocker perfLocker(&m_performanceMetricsMutex);
        if (m_operationStartTimes.contains(requestId)) {
            qint64 operationTime = m_operationTimer.elapsed() - m_operationStartTimes[requestId];
            m_operationStartTimes.remove(requestId);
            
            // Update multi-threaded metrics
            m_performanceMetrics.multiThreadedOperations++;
            m_performanceMetrics.multiThreadedTotalTime += operationTime;
            
            if (!result.success) {
                double totalOps = static_cast<double>(m_performanceMetrics.multiThreadedOperations);
                double errorCount = m_performanceMetrics.multiThreadedErrorRate * (totalOps - 1) + 1;
                m_performanceMetrics.multiThreadedErrorRate = errorCount / totalOps;
            }
        }
    }
    
    // Remove from pending requests and get the corresponding data point with thread safety
    DataAcquisitionPoint point;
    bool found = false;
    {
        QMutexLocker locker(&m_requestTrackingMutex);
        if (m_pendingReadRequests.contains(requestId)) {
            point = m_pendingReadRequests.take(requestId);
            found = true;
        }
    }
    
    if (!found) {
        qWarning() << "Received read completion for unknown request ID:" << requestId;
        return;
    }
    
    if (result.success && result.hasValidData) {
        // Update statistics
        updateStatistics(true, 0); // Response time not available in this context
        
        // Create acquired data point
        AcquiredDataPoint acquiredPoint;
        acquiredPoint.pointName = point.name;
        acquiredPoint.timestamp = result.timestamp;
        acquiredPoint.measurement = point.measurement;
        acquiredPoint.tags = point.tags;
        acquiredPoint.isValid = true;
        
        // Extract value from processed data
        if (!result.processedData.isEmpty()) {
            acquiredPoint.value = result.processedData.first();
        } else if (!result.rawData.isEmpty()) {
            acquiredPoint.value = result.rawData.first();
        }
        
        validateAndSetInfluxTags(acquiredPoint, point);
        
        // Send to InfluxDB
        sendDataToInflux(acquiredPoint);
        
        // Emit signal
        emit dataPointAcquired(acquiredPoint);
    } else {
        updateStatistics(false, 0);
        qWarning() << "Worker read failed - Request ID:" << requestId << "error:" << result.errorString;
        
        // Emit error signal
        emit errorOccurred(QString("Read failed for %1: %2").arg(point.name).arg(result.errorString));
    }
}

void ScadaCoreService::onWorkerWriteCompleted(qint64 requestId, const ModbusWriteResult &result)
{
    QString operation;
    {
        QMutexLocker locker(&m_requestTrackingMutex);
        operation = m_pendingWriteRequests.value(requestId, "Unknown");
        m_pendingWriteRequests.remove(requestId);
    }
    
    qDebug() << "Worker write completed:" << operation << "requestId" << requestId << "success" << result.success;
    
    // Update statistics
    updateStatistics(result.success, 0); // Response time not available for writes
    
    // Emit completion signal
    emit writeCompleted(requestId, operation, result.success, result.errorString);
}

void ScadaCoreService::onWorkerError(const QString &deviceKey, const QString &error)
{
    qWarning() << "Worker error for device" << deviceKey << ":" << error;
    
    // Update statistics
    updateStatistics(false, 0);
    
    // Emit error signal
    emit errorOccurred(QString("Device %1: %2").arg(deviceKey).arg(error));
}

void ScadaCoreService::onWorkerRequestInterrupted(qint64 requestId, const QString &reason)
{
    qDebug() << "Worker request interrupted:" << requestId << "reason:" << reason;
    
    // Remove from pending requests if it was a write
    if (m_pendingWriteRequests.contains(requestId)) {
        QString operation = m_pendingWriteRequests.take(requestId);
        emit writeCompleted(requestId, operation, false, QString("Interrupted: %1").arg(reason));
    }
    
    // Emit interruption signal
    emit requestInterrupted(requestId, reason);
}

void ScadaCoreService::onWorkerCreated(const QString &deviceKey)
{
    qDebug() << "🔧 Worker created for device:" << deviceKey;
    
    // Start and connect the newly created worker
    if (m_workerManager) {
        // Get the worker and start it
        QStringList parts = deviceKey.split(":");
        if (parts.size() >= 3) {
            QString host = parts[0];
            int port = parts[1].toInt();
            int unitId = parts[2].toInt();
            
            ModbusWorker* worker = m_workerManager->getOrCreateWorker(host, port, unitId);
            if (worker) {
                worker->startWorker();
                worker->connectToDevice();
                qDebug() << "   Started and connected worker for device:" << deviceKey;
            }
        }
        
        auto stats = m_workerManager->getGlobalStatistics();
        qDebug() << "   Total active workers:" << stats.activeWorkers;
        qDebug() << "   Total successful requests:" << stats.totalSuccessfulRequests;
        qDebug() << "   Total failed requests:" << stats.totalFailedRequests;
    }
    
    emit workerCreated(deviceKey);
}

void ScadaCoreService::onWorkerRemoved(const QString &deviceKey)
{
    qDebug() << "Worker removed for device:" << deviceKey;
    emit workerRemoved(deviceKey);
}

void ScadaCoreService::onWorkerConnectionStateChanged(const QString &deviceId, bool connected)
{
    qDebug() << "Worker connection state changed for device" << deviceId << ":" << (connected ? "connected" : "disconnected");
    
    // Update connection state tracking
    m_connectionStates[deviceId] = connected;
    
    // Emit connection state change signal if needed
    // emit connectionStateChanged(deviceId, connected);
}

void ScadaCoreService::onWorkerStatisticsUpdated(const QString &deviceId, const ModbusWorker::WorkerStatistics &stats)
{
    qDebug() << "Worker statistics updated for device" << deviceId 
             << "- Total requests:" << stats.totalRequests
             << "- Successful:" << stats.successfulRequests
             << "- Failed:" << stats.failedRequests
             << "- Interrupted:" << stats.interruptedRequests
             << "- Avg response time:" << stats.averageResponseTime << "ms";
    
    // Update service-level statistics based on worker statistics
    // This could aggregate statistics from individual workers
}

void ScadaCoreService::onGlobalStatisticsUpdated(const ModbusWorkerManager::GlobalStatistics &stats)
{
    // Update service-level statistics based on global worker statistics
    // This could be used to aggregate statistics from all workers
    emit globalStatisticsUpdated(stats);
}

void ScadaCoreService::onSingleThreadReadCompleted(const ModbusReadResult &result)
{
    // Handle read completion in single-threaded mode
    qint64 responseTime = QDateTime::currentMSecsSinceEpoch() - result.timestamp;
    
    // Track performance metrics for single-threaded operations
    if (m_performanceMonitoringEnabled) {
        QMutexLocker perfLocker(&m_performanceMetricsMutex);
        
        // For single-threaded mode, we track based on the operation completion
        m_performanceMetrics.singleThreadedOperations++;
        m_performanceMetrics.singleThreadedTotalTime += responseTime;
        
        if (!result.success) {
            double totalOps = static_cast<double>(m_performanceMetrics.singleThreadedOperations);
            double errorCount = m_performanceMetrics.singleThreadedErrorRate * (totalOps - 1) + 1;
            m_performanceMetrics.singleThreadedErrorRate = errorCount / totalOps;
        }
    }
    
    if (result.success) {
        // Find the corresponding data point
        QMutexLocker locker(&m_dataPointsMutex);
        for (const auto &point : m_dataPoints) {
            if (point.address == result.startAddress) {
                // Check if this is an optimized block read
                if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
                    qDebug() << "📊 Processing block read result in single-threaded mode:" << point.name
                             << "Start Address:" << result.startAddress << "Data Size:" << result.rawData.size();
                    
                    // Use the existing block processing logic
                    handleBlockReadResult(result, point);
                } else {
                    // Handle individual point read
                    AcquiredDataPoint dataPoint;
                    dataPoint.pointName = point.name;
                    dataPoint.timestamp = QDateTime::currentMSecsSinceEpoch();
                    dataPoint.measurement = point.measurement;
                    dataPoint.tags = point.tags;
                    dataPoint.isValid = true;
                    
                    // Extract value based on data type
                    if (!result.rawData.isEmpty()) {
                        switch (point.dataType) {
                            case ModbusDataType::HoldingRegister:
                                dataPoint.value = result.rawData.first();
                                break;
                            case ModbusDataType::InputRegister:
                                dataPoint.value = result.rawData.first();
                                break;
                            case ModbusDataType::Float32:
                                if (result.rawData.size() >= 2) {
                                    // Combine two 16-bit registers into a 32-bit float
                                    quint32 combined = (static_cast<quint32>(result.rawData[0]) << 16) | result.rawData[1];
                                    float floatValue;
                                    std::memcpy(&floatValue, &combined, sizeof(float));
                                    dataPoint.value = floatValue;
                                }
                                break;
                            default:
                                dataPoint.value = result.rawData.first();
                                break;
                        }
                    }
                    
                    // Send data to InfluxDB
                    sendDataToInflux(dataPoint);
                    emit dataPointAcquired(dataPoint);
                }
                
                updateStatistics(true, responseTime);
                break;
            }
        }
    } else {
        qWarning() << "Single-threaded read failed:" << result.errorString;
        updateStatistics(false, responseTime);
    }
}
#include "../include/scada_core_service.h"
#include "../include/influxdb_line_protocol.h"
#include <QDebug>
#include <QJsonArray>
#include <QThread>
#include <QMetaType>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QSettings>
#include <QFileInfo>
#include <QMutexLocker>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <fcntl.h>

// Register types for cross-thread communication
Q_DECLARE_METATYPE(AcquiredDataPoint)
Q_DECLARE_METATYPE(BufferedDataPoint)

ScadaCoreService::ScadaCoreService(QObject *parent)
    : QObject(parent)
    , m_pollTimer(nullptr)
    , m_modbusManager(nullptr)
    , m_retryTimer(nullptr)
    , m_influxWorkerThread(nullptr)
    , m_influxWorker(nullptr)
    , m_telegrafSocketPath("/tmp/telegraf.sock")
    , m_serviceRunning(false)
    , m_currentPointIndex(0)
    , m_maxBufferSize(1000)
    , m_maxRetryCount(3)
    , m_baseRetryDelayMs(1000)
    , m_maxRetryDelayMs(30000)
    , m_retryBackoffMultiplier(2.0)
    , m_jitterEnabled(true)
    , m_validationEnabled(true)
    , m_maxStringLength(255)
    , m_maxNumericPrecision(6)
    , m_sanitizationEnabled(true)
    , m_strictValidation(false)
    , m_socketWriteTimeout(5000)
    , m_socketReconnectDelay(2000)
    , m_socketMaxReconnectAttempts(5)
    , m_bufferFlushInterval(30000)
    , m_bufferPersistenceEnabled(true)
    , m_influxAddressFormat("clean")
    , m_includeDataTypeTags(false)
    , m_alertingEnabled(true)
    , m_minSuccessRateThreshold(95.0)
    , m_maxConsecutiveFailuresThreshold(5)
    , m_maxResponseTimeThreshold(10000)
    , m_maxBufferSizeThreshold(800)
    , m_alertCheckInterval(30000)
    , m_lastHealthCheck(0)
    , m_systemHealthy(true)
{
    // Load configuration from config.ini
    loadDataResilienceConfiguration();
    
    // Initialize components
    m_pollTimer = new QTimer(this);
    m_modbusManager = new ModbusManager(this);
    
    // Initialize retry timer for InfluxDB buffer processing
    m_retryTimer = new QTimer(this);
    m_retryTimer->setSingleShot(false);
    m_retryTimer->setInterval(m_bufferFlushInterval); // Use configured interval
    connect(m_retryTimer, &QTimer::timeout, this, &ScadaCoreService::processInfluxBuffer);
    
    // Load configuration for ModbusManager from existing config.ini
    if (!m_modbusManager->loadConfigurationFromFile("/home/Pttaov1/TSO_SCADA/qtworkplace/modbusdriver/config/config.ini")) {
        qWarning() << "Failed to load ModbusManager configuration, using defaults";
    }
    
    // Connect signals
    connect(m_pollTimer, &QTimer::timeout, this, &ScadaCoreService::onPollTimer);
    connect(m_modbusManager, &ModbusManager::readCompleted, 
            this, &ScadaCoreService::onModbusReadCompleted);
    connect(m_modbusManager, &ModbusManager::writeCompleted, 
            this, &ScadaCoreService::onModbusWriteCompleted);
    connect(m_modbusManager, &ModbusManager::connectionStateChanged,
            this, &ScadaCoreService::onModbusConnectionStateChanged);
    connect(m_modbusManager, &ModbusManager::errorOccurred,
            this, &ScadaCoreService::onModbusError);
    
    // Initialize statistics
    resetStatistics();
    
    // Register meta types for cross-thread communication
    qRegisterMetaType<AcquiredDataPoint>("AcquiredDataPoint");
    qRegisterMetaType<BufferedDataPoint>("BufferedDataPoint");
    
    // Initialize asynchronous InfluxDB worker
    initializeInfluxWorker();
}

ScadaCoreService::~ScadaCoreService()
{
    stopService();
    
    // Clean up InfluxDB worker thread
    if (m_influxWorkerThread && m_influxWorkerThread->isRunning()) {
        if (m_influxWorker) {
            QMetaObject::invokeMethod(m_influxWorker, "stop", Qt::QueuedConnection);
        }
        m_influxWorkerThread->quit();
        m_influxWorkerThread->wait(5000);
        delete m_influxWorkerThread;
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
    
    // Note: Using direct Unix socket calls for each data transmission
    
    // Initialize service state
    m_serviceRunning = true;
    m_currentPointIndex = 0;
    m_statistics.serviceStartTime = QDateTime::currentMSecsSinceEpoch();
    
    // Start polling timer (3000ms for stable Modbus connections)
    m_pollTimer->start(3000);
    
    // Start retry timer for buffer processing
    m_retryTimer->start();
    
    emit serviceStarted();
    qDebug() << "SCADA Core Service started successfully";
    return true;
}

// InfluxDBWorker implementation
InfluxDBWorker::InfluxDBWorker(QObject *parent)
    : QObject(parent)
    , m_maxRetryCount(3)
    , m_baseRetryDelayMs(1000)
    , m_maxRetryDelayMs(30000)
    , m_retryBackoffMultiplier(2.0)
    , m_jitterEnabled(true)
    , m_stopRequested(false)
{
}

InfluxDBWorker::~InfluxDBWorker()
{
}

void InfluxDBWorker::setConfiguration(const QString &socketPath, int maxRetryCount, int baseRetryDelayMs,
                                    int maxRetryDelayMs, double retryBackoffMultiplier, bool jitterEnabled)
{
    QMutexLocker locker(&m_configMutex);
    m_socketPath = socketPath;
    m_maxRetryCount = maxRetryCount;
    m_baseRetryDelayMs = baseRetryDelayMs;
    m_maxRetryDelayMs = maxRetryDelayMs;
    m_retryBackoffMultiplier = retryBackoffMultiplier;
    m_jitterEnabled = jitterEnabled;
}

void InfluxDBWorker::writeDataPoint(const AcquiredDataPoint &dataPoint)
{
    if (m_stopRequested) {
        return;
    }
    
    // Direct format and send without any validation
    QByteArray message = formatInfluxLineProtocol(dataPoint);
    
    // Direct write without checks
    bool success = writeToTelegrafSocketAsync(m_socketPath, message, 0);
    
    // Minimal signal emission for performance
    emit writeCompleted(dataPoint.pointName, success, success ? QString() : "Socket write failed");
}

void InfluxDBWorker::processBufferedDataPoint(const BufferedDataPoint &bufferedPoint)
{
    if (m_stopRequested) {
        return;
    }
    
    qDebug() << "[InfluxDB Worker] Processing buffered data point:" << bufferedPoint.dataPoint.pointName
             << "retry count:" << bufferedPoint.retryCount;
    
    writeDataPoint(bufferedPoint.dataPoint);
}

void InfluxDBWorker::stop()
{
    m_stopRequested = true;
    qDebug() << "[InfluxDB Worker] Stop requested";
}

bool InfluxDBWorker::writeToTelegrafSocketAsync(const QString &socketPath, const QByteArray &message, int maxRetries)
{
    // Optimized socket writing with minimal overhead
    static thread_local int cached_sockfd = -1;
    static thread_local struct sockaddr_un cached_addr {};
    static thread_local QString cached_path;
    
    // Cache socket address structure to avoid repeated setup
    if (cached_path != socketPath || cached_sockfd < 0) {
        if (cached_sockfd >= 0) {
            ::close(cached_sockfd);
        }
        
        cached_sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
        if (cached_sockfd < 0) {
            return false;
        }
        
        // Set socket to non-blocking for better performance
        int flags = fcntl(cached_sockfd, F_GETFL, 0);
        fcntl(cached_sockfd, F_SETFL, flags | O_NONBLOCK);
        
        cached_addr.sun_family = AF_UNIX;
        strncpy(cached_addr.sun_path, socketPath.toUtf8().constData(), sizeof(cached_addr.sun_path) - 1);
        cached_path = socketPath;
    }
    
    // Fast path: single attempt with cached socket
    int result = ::sendto(cached_sockfd, message.constData(), message.size(), 0,
                          reinterpret_cast<struct sockaddr*>(&cached_addr), sizeof(cached_addr));
    
    if (result >= 0) {
        return true;
    }
    
    // Handle EAGAIN/EWOULDBLOCK for non-blocking socket
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Socket buffer full, try once more with blocking
        int flags = fcntl(cached_sockfd, F_GETFL, 0);
        fcntl(cached_sockfd, F_SETFL, flags & ~O_NONBLOCK);
        
        result = ::sendto(cached_sockfd, message.constData(), message.size(), 0,
                          reinterpret_cast<struct sockaddr*>(&cached_addr), sizeof(cached_addr));
        
        fcntl(cached_sockfd, F_SETFL, flags | O_NONBLOCK); // Restore non-blocking
        
        if (result >= 0) {
            return true;
        }
    }
    
    // Error occurred, invalidate cache and retry with new socket if retries allowed
    ::close(cached_sockfd);
    cached_sockfd = -1;
    cached_path.clear();
    
    if (maxRetries > 0) {
        // Single retry with fresh socket
        int sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            return false;
        }
        
        struct sockaddr_un addr {};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socketPath.toUtf8().constData(), sizeof(addr.sun_path) - 1);
        
        result = ::sendto(sockfd, message.constData(), message.size(), 0,
                          reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
        ::close(sockfd);
        
        return result >= 0;
    }
    
    return false;
}

qint64 InfluxDBWorker::calculateRetryDelay(int retryCount) const
{
    // Exponential backoff with jitter
    qint64 baseDelay = m_baseRetryDelayMs * qPow(m_retryBackoffMultiplier, retryCount);
    qint64 delay = qMin(baseDelay, static_cast<qint64>(m_maxRetryDelayMs));
    
    if (m_jitterEnabled) {
        // Add random jitter (±25%)
        int jitter = QRandomGenerator::global()->bounded(-delay / 4, delay / 4 + 1);
        delay += jitter;
        delay = qMax(delay, static_cast<qint64>(m_baseRetryDelayMs / 2)); // Minimum delay
    }
    
    return delay;
}

QByteArray InfluxDBWorker::formatInfluxLineProtocol(const AcquiredDataPoint &dataPoint)
{
    // Convert AcquiredDataPoint to InfluxDBLineProtocol::DataPoint
    InfluxDBLineProtocol::DataPoint influxPoint;
    influxPoint.measurement = dataPoint.measurement;
    influxPoint.tags = dataPoint.tags;
    influxPoint.timestamp = dataPoint.timestamp;
    influxPoint.precision = InfluxDBLineProtocol::TimestampPrecision::Nanoseconds;
    
    // Add the main value as a field named "value"
    influxPoint.fields["value"] = dataPoint.value;
    
    // Use the new compliant line protocol formatter
    return InfluxDBLineProtocol::formatLineProtocol(influxPoint);
}

QString InfluxDBWorker::escapeInfluxTagKey(const QString &key)
{
    // Delegate to the new compliant line protocol handler
    return InfluxDBLineProtocol::escapeTagKey(key);
}

QString InfluxDBWorker::escapeInfluxTagValue(const QString &value)
{
    // Delegate to the new compliant line protocol handler
    return InfluxDBLineProtocol::escapeTagValue(value);
}

void ScadaCoreService::initializeInfluxWorker()
{
    // Create worker thread for asynchronous InfluxDB operations
    m_influxWorkerThread = new QThread(this);
    m_influxWorker = new InfluxDBWorker();
    m_influxWorker->moveToThread(m_influxWorkerThread);
    
    // Configure worker with current settings
    m_influxWorker->setConfiguration(m_telegrafSocketPath, m_maxRetryCount, m_baseRetryDelayMs,
                                   m_maxRetryDelayMs, m_retryBackoffMultiplier, m_jitterEnabled);
    
    // Connect signals
    connect(m_influxWorker, &InfluxDBWorker::writeCompleted,
            this, &ScadaCoreService::onInfluxWriteCompleted);
    connect(m_influxWorker, &InfluxDBWorker::statisticsUpdated,
            this, [this](qint64 socketErrors) {
                m_statistics.socketErrors += socketErrors;
            });
    
    // Start worker thread
    m_influxWorkerThread->start();
    
    qDebug() << "[InfluxDB Worker] Asynchronous InfluxDB worker thread initialized";
}

void ScadaCoreService::onInfluxWriteCompleted(const QString &pointName, bool success, const QString &error)
{
    if (success) {
        qDebug() << "[InfluxDB Async Success] Data point written successfully:" << pointName;
        emit dataPointSentToInflux(pointName, true);
    } else {
        qWarning() << "[InfluxDB Async Failed] Failed to write data point:" << pointName << "Error:" << error;
        emit dataPointSentToInflux(pointName, false);
        emit errorOccurred(QString("[InfluxDB Async Write Failure] %1: %2").arg(pointName, error));
    }
}

void ScadaCoreService::stopService()
{
    if (!m_serviceRunning) {
        return;
    }
    
    qDebug() << "Stopping SCADA Core Service...";
    
    m_serviceRunning = false;
    m_pollTimer->stop();
    
    // Stop retry timer
    if (m_retryTimer) {
        m_retryTimer->stop();
    }
    
    // Disconnect from Modbus
    if (m_modbusManager) {
        m_modbusManager->disconnectFromServer();
    }
    
    // Clear the buffer when stopping service
    m_influxBuffer.clear();
    
    // Note: Using direct Unix socket calls, no persistent connection needed
    
    emit serviceStopped();
    qDebug() << "SCADA Core Service stopped";
}

bool ScadaCoreService::isRunning() const
{
    return m_serviceRunning;
}

void ScadaCoreService::addDataPoint(const DataAcquisitionPoint &point)
{
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
    return m_dataPoints;
}

void ScadaCoreService::clearDataPoints()
{
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

// Modbus write operations
void ScadaCoreService::writeHoldingRegister(const QString &host, int port, int address, quint16 value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegister[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    // Convert from one-based (database) to zero-based (Modbus) addressing
    int modbusAddress = address - 1;
    qDebug() << "Writing holding register:" << host << ":" << port << "address" << address << "(Modbus:" << modbusAddress << ") value" << value;
    m_modbusManager->writeHoldingRegister(modbusAddress, value);
}

void ScadaCoreService::writeHoldingRegisterFloat32(const QString &host, int port, int address, float value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterFloat32[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    // Convert from one-based (database) to zero-based (Modbus) addressing
    int modbusAddress = address - 1;
    qDebug() << "Writing holding register Float32:" << host << ":" << port << "address" << address << "(Modbus:" << modbusAddress << ") value" << value;
    m_modbusManager->writeHoldingRegisterFloat32(modbusAddress, value);
}

void ScadaCoreService::writeHoldingRegisterDouble64(const QString &host, int port, int address, double value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterDouble64[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    // Convert from one-based (database) to zero-based (Modbus) addressing
    int modbusAddress = address - 1;
    qDebug() << "Writing holding register Double64:" << host << ":" << port << "address" << address << "(Modbus:" << modbusAddress << ") value" << value;
    m_modbusManager->writeHoldingRegisterDouble64(modbusAddress, value);
}

void ScadaCoreService::writeHoldingRegisterLong32(const QString &host, int port, int address, qint32 value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterLong32[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    // Convert from one-based (database) to zero-based (Modbus) addressing
    int modbusAddress = address - 1;
    qDebug() << "Writing holding register Long32:" << host << ":" << port << "address" << address << "(Modbus:" << modbusAddress << ") value" << value;
    m_modbusManager->writeHoldingRegisterLong32(modbusAddress, value);
}

void ScadaCoreService::writeHoldingRegisterLong64(const QString &host, int port, int address, qint64 value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterLong64[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    // Convert from one-based (database) to zero-based (Modbus) addressing
    int modbusAddress = address - 1;
    qDebug() << "Writing holding register Long64:" << host << ":" << port << "address" << address << "(Modbus:" << modbusAddress << ") value" << value;
    m_modbusManager->writeHoldingRegisterLong64(modbusAddress, value);
}

void ScadaCoreService::writeCoil(const QString &host, int port, int address, bool value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteCoil[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    // Convert from one-based (database) to zero-based (Modbus) addressing
    int modbusAddress = address - 1;
    qDebug() << "Writing coil:" << host << ":" << port << "address" << address << "(Modbus:" << modbusAddress << ") value" << value;
    m_modbusManager->writeCoil(modbusAddress, value);
}

void ScadaCoreService::onPollTimer()
{
    if (!m_serviceRunning || m_dataPoints.isEmpty()) {
        return;
    }
    
    // Perform system health monitoring
    checkSystemHealth();
    
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
    
    // First pass: Look for ready block points to prioritize by data type
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
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
    // Update polling timestamps for backward compatibility
    m_lastPollTimes[point.name] = currentTime;
    m_responseTimers[point.name] = currentTime;
    
    // Determine read mode based on existing logic for backward compatibility
    ReadMode mode = ReadMode::SINGLE_READ;
    if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
        mode = ReadMode::BLOCK_READ;
    }
    
    // Use unified acquisition function
    QVector<AcquiredDataPoint> results = acquireDataUnified(point, mode);
    
    // Process results using existing data handling pipeline for backward compatibility
    for (const auto &acquiredPoint : results) {
        // Update statistics
        updateStatistics(acquiredPoint.isValid, QDateTime::currentMSecsSinceEpoch() - currentTime);
        
        // Emit signals for backward compatibility
        emit dataPointAcquired(acquiredPoint);
        
        // Add to buffer if buffering is enabled
        if (m_bufferingEnabled && !acquiredPoint.isValid) {
            bufferFailedDataPoint(acquiredPoint);
        }
        
        // Send to InfluxDB if configured
        if (m_influxEnabled && acquiredPoint.isValid) {
            sendDataToInflux(acquiredPoint);
        }
        
        // Debug output for backward compatibility
        if (acquiredPoint.isValid) {
            qDebug() << "📊 Data acquired:" << acquiredPoint.pointName 
                     << "=" << acquiredPoint.value.toString()
                     << "(" << acquiredPoint.measurement << ")"
                     << "Unit ID:" << acquiredPoint.tags.value("unit_id", "N/A");
        } else {
            qDebug() << "❌ Data acquisition failed:" << acquiredPoint.pointName 
                     << "Error:" << acquiredPoint.errorMessage;
        }
    }
    
    // Legacy logging for backward compatibility
    int unitId = point.tags.value("unit_id", "1").toInt();
    qDebug() << "Processed data point:" << point.name << "at address" << point.address << "Unit ID:" << unitId << "Results:" << results.size();
    
    // Check if this is an optimized block read with combined conditions
    if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
        // This is a block read - use block methods with register_type and data_type validation
        int blockSize = point.tags["block_size"].toInt();
        QString registerType = point.tags.value("register_type", "");
        QString dataType = point.tags.value("data_type", "");
        
        // Convert from one-based (database) to zero-based (Modbus) addressing
        int modbusAddress = point.address - 1;
        
        qDebug() << "Performing block read - Address:" << point.address << "(Modbus:" << modbusAddress << ") Size:" << blockSize
                 << "Register Type:" << registerType << "Data Type:" << dataType << "Unit ID:" << unitId;
        
        // Use combined condition: register_type + data_type for precise Modbus operation selection
        // Standardized to use consistent uppercase format and identical conditions as individual points
        if (registerType == "HOLDING_REGISTER") {
            // Handle holding register types based on data_type - use consistent uppercase format
            if (dataType == "INT16" || dataType == "FLOAT32" || dataType == "DOUBLE" || dataType == "LONG" || 
                point.dataType == ModbusDataType::HoldingRegister || point.dataType == ModbusDataType::Float32 || 
                point.dataType == ModbusDataType::Double64 || point.dataType == ModbusDataType::Long32 || 
                point.dataType == ModbusDataType::Long64) {
                m_modbusManager->readHoldingRegisters(modbusAddress, blockSize, point.dataType, unitId);
            } else {
                // Default UNKNOWN data types to BOOL with Read Discrete Inputs
                qDebug() << "UNKNOWN data type detected for block read - defaulting to BOOL/DiscreteInput for address:" << point.address << "(Modbus:" << modbusAddress << ")";
                m_modbusManager->readDiscreteInputs(modbusAddress, blockSize, unitId);
            }
        } else if (registerType == "INPUT_REGISTER") {
            // Handle input register types
            m_modbusManager->readInputRegisters(modbusAddress, blockSize, point.dataType, unitId);
        } else if (registerType == "COIL") {
            // Handle coil types
            m_modbusManager->readCoils(modbusAddress, blockSize, unitId);
        } else if (registerType == "DISCRETE_INPUT") {
            // Handle discrete input types
            m_modbusManager->readDiscreteInputs(modbusAddress, blockSize, unitId);
        } else if (registerType == "STATUS") {
            // Handle STATUS register type for block reads - map to DiscreteInput for BOOL data types
            m_modbusManager->readDiscreteInputs(modbusAddress, blockSize, unitId);
        } else {
            // Fallback to original dataType-based logic if register_type is not specified
            switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::Float32:
            case ModbusDataType::Double64:
            case ModbusDataType::Long32:
            case ModbusDataType::Long64:
                m_modbusManager->readHoldingRegisters(modbusAddress, blockSize, point.dataType, unitId);
                break;
            case ModbusDataType::InputRegister:
                m_modbusManager->readInputRegisters(modbusAddress, blockSize, point.dataType, unitId);
                break;
            case ModbusDataType::Coil:
                m_modbusManager->readCoils(modbusAddress, blockSize, unitId);
                break;
            case ModbusDataType::DiscreteInput:
            case ModbusDataType::BOOL:
                // Default BOOL to DiscreteInput for block reads
                m_modbusManager->readDiscreteInputs(modbusAddress, blockSize, unitId);
                break;
            }
        }
    } else {
        // Individual point read with combined conditions
        QString registerType = point.tags.value("register_type", "");
        QString dataType = point.tags.value("data_type", "");
        
        qDebug() << "Performing individual read - Address:" << point.address
                 << "Register Type:" << registerType << "Data Type:" << dataType << "Unit ID:" << unitId;
        
        // Convert from one-based (database) to zero-based (Modbus) addressing
        int modbusAddress = point.address - 1;
        
        // Use combined condition: register_type + data_type for precise Modbus operation selection
        // Standardized to use consistent uppercase format matching block processing
        if (registerType == "HOLDING_REGISTER") {
            // Handle holding register types based on data_type - use consistent uppercase format
            if (dataType == "INT16" || dataType == "FLOAT32" || dataType == "DOUBLE" || dataType == "LONG" || 
                point.dataType == ModbusDataType::HoldingRegister || point.dataType == ModbusDataType::Float32 || 
                point.dataType == ModbusDataType::Double64 || point.dataType == ModbusDataType::Long32 || 
                point.dataType == ModbusDataType::Long64) {
                m_modbusManager->readHoldingRegister(modbusAddress, point.dataType, unitId);
            } else {
                // Default UNKNOWN data types to BOOL with Read Discrete Inputs
                qDebug() << "UNKNOWN data type detected for individual read - defaulting to BOOL/DiscreteInput for address:" << point.address << "(Modbus:" << modbusAddress << ")";
                m_modbusManager->readDiscreteInput(modbusAddress, unitId);
            }
        } else if (registerType == "INPUT_REGISTER") {
            // Handle input register types
            m_modbusManager->readInputRegister(modbusAddress, point.dataType, unitId);
        } else if (registerType == "COIL") {
            // Handle coil types
            m_modbusManager->readCoil(modbusAddress, unitId);
        } else if (registerType == "DISCRETE_INPUT") {
            // Handle discrete input types
            m_modbusManager->readDiscreteInput(modbusAddress, unitId);
        } else if (registerType == "STATUS") {
            // Handle STATUS register type - map to DiscreteInput for BOOL data types
            m_modbusManager->readDiscreteInput(modbusAddress, unitId);
        } else {
            // Fallback to original dataType-based logic if register_type is not specified
            switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::Float32:
            case ModbusDataType::Double64:
            case ModbusDataType::Long32:
            case ModbusDataType::Long64:
                m_modbusManager->readHoldingRegister(modbusAddress, point.dataType, unitId);
                break;
            case ModbusDataType::InputRegister:
                m_modbusManager->readInputRegister(modbusAddress, point.dataType, unitId);
                break;
            case ModbusDataType::Coil:
                m_modbusManager->readCoil(modbusAddress, unitId);
                break;
            case ModbusDataType::DiscreteInput:
                m_modbusManager->readDiscreteInput(modbusAddress, unitId);
                break;
            case ModbusDataType::BOOL:
                // Default BOOL to DiscreteInput when no register_type specified
                m_modbusManager->readDiscreteInput(modbusAddress, unitId);
                break;
            }
        }
    }
    
    m_statistics.totalReadOperations++;
}

void ScadaCoreService::onModbusReadCompleted(const ModbusReadResult &result)
{
    qDebug() << "📥 Modbus Read Result Received:";
    qDebug() << "   Start Address:" << result.startAddress;
    qDebug() << "   Success:" << result.success;
    qDebug() << "   Raw Data Size:" << result.rawData.size();
    qDebug() << "   Error String:" << result.errorString;
    
    // Find the corresponding data point
    // Convert 0-based Modbus address back to 1-based database address for comparison
    DataAcquisitionPoint *targetPoint = nullptr;
    for (auto &point : m_dataPoints) {
        if (point.address == (result.startAddress + 1)) {
            targetPoint = &point;
            break;
        }
    }
    
    if (!targetPoint) {
        qWarning() << "Received read result for unknown address:" << result.startAddress;
        qDebug() << "Available data point addresses:";
        for (const auto &point : m_dataPoints) {
            qDebug() << "   Point:" << point.name << "Address:" << point.address;
        }
        return;
    }
    
    // Check if this is a block read result
    if (targetPoint->tags.contains("block_type") && targetPoint->tags["block_type"] == "optimized_read") {
        // Handle block read - extract individual point values
        handleBlockReadResult(result, *targetPoint);
        return;
    }
    
    // Calculate response time
    qint64 responseTime = 0;
    if (m_responseTimers.contains(targetPoint->name)) {
        responseTime = QDateTime::currentMSecsSinceEpoch() - m_responseTimers[targetPoint->name];
        m_responseTimers.remove(targetPoint->name);
    }
    
    // Create acquired data point
    AcquiredDataPoint dataPoint;
    dataPoint.pointName = targetPoint->name;
    dataPoint.timestamp = result.timestamp;
    dataPoint.measurement = targetPoint->measurement;
    dataPoint.tags = targetPoint->tags;
    dataPoint.isValid = result.success;
    
    // Add address tag for InfluxDB mapping (individual points)
    // Format address according to configuration:
    // 'clean' = zero-based addressing (e.g., "368")
    // 'suffix' = one-based Modbus addressing (e.g., "369")
    if (m_influxAddressFormat == "suffix") {
        // Use one-based Modbus addressing
        dataPoint.tags["address"] = QString::number(targetPoint->address);
    } else {
        // Use zero-based addressing (default)
        dataPoint.tags["address"] = QString::number(targetPoint->address - 1);
    }
    
    // Validate and ensure all required InfluxDB tags are present
    validateAndSetInfluxTags(dataPoint, *targetPoint);
    
    // Debug specific problematic tags
    if (targetPoint->name == "A1020-OMA-001" || targetPoint->name == "SA1020-FRS-001A" || 
        targetPoint->name.contains("A1020-OMA-001") || targetPoint->name.contains("SA1020-FRS-001A")) {
        debugDataPointProcessing(*targetPoint, dataPoint);
    }
    
    if (result.success) {
        // Extract value based on data type
        if (!result.processedData.isEmpty()) {
            QString key = result.processedData.keys().first();
            dataPoint.value = result.processedData[key];
        } else if (!result.rawData.isEmpty()) {
            dataPoint.value = result.rawData.first();
        }
        
        // Additional BOOL-specific validation for individual points
        if (targetPoint->dataType == ModbusDataType::BOOL) {
            // Validate BOOL conversion for individual points
            if (!dataPoint.value.canConvert<bool>()) {
                qWarning() << "BOOL conversion failed for individual point" << targetPoint->name
                          << "- QVariant cannot convert to bool. Using default false.";
                dataPoint.value = false;
                dataPoint.isValid = false;
                dataPoint.errorMessage = "BOOL conversion failed: QVariant cannot convert to bool";
            } else {
                // Log successful BOOL conversion
                qDebug() << "BOOL conversion successful for" << targetPoint->name
                         << "- value:" << dataPoint.value.toBool();
            }
        }
        
        m_statistics.successfulReads++;
        updateStatistics(true, responseTime);
        
        qDebug() << "Successfully read data point:" << targetPoint->name 
                 << "Value:" << dataPoint.value.toString();
    } else {
        dataPoint.errorMessage = result.errorString;
        m_statistics.failedReads++;
        updateStatistics(false, responseTime);
        
        qWarning() << "Failed to read data point:" << targetPoint->name 
                   << "Error:" << result.errorString;
    }
    
    // Emit signal
    emit dataPointAcquired(dataPoint);
    
    // Send to InfluxDB via Telegraf
    if (dataPoint.isValid) {
        bool sent = sendDataToInflux(dataPoint);
        
        if (sent) {
            m_statistics.totalDataPointsSent++;
        }
    }
    
    // Update statistics
    emit statisticsUpdated(m_statistics);
}

void ScadaCoreService::onModbusConnectionStateChanged(bool connected)
{
    if (connected) {
        qDebug() << "Modbus connection established to:" << m_currentHost;
    } else {
        qDebug() << "Modbus connection lost to:" << m_currentHost;
        m_currentHost.clear();
    }
}

void ScadaCoreService::onModbusError(const QString &error)
{
    qWarning() << "Modbus error:" << error;
    emit errorOccurred(QString("Modbus error: %1").arg(error));
}

void ScadaCoreService::onModbusWriteCompleted(const ModbusWriteResult &result)
{
    QString operation = QString("Write@%1[%2]").arg(result.startAddress).arg(result.registerCount);
    
    if (result.success) {
        qDebug() << "✅ Modbus write completed successfully:" << operation;
        emit writeCompleted(operation, true);
    } else {
        qWarning() << "❌ Modbus write failed:" << operation << "Error:" << result.errorString;
        emit writeCompleted(operation, false, result.errorString);
    }
}

bool ScadaCoreService::writeToTelegrafSocket(const QString& socketPath, const QByteArray& message)
{
    // Enhanced error logging with detailed diagnostics and exponential backoff retry
    qint64 operationStartTime = QDateTime::currentMSecsSinceEpoch();
    
    // Implement exponential backoff retry mechanism
    for (int attempt = 0; attempt <= m_maxRetryCount; attempt++) {
        qint64 attemptStartTime = QDateTime::currentMSecsSinceEpoch();
        
        // Log retry attempt (except for first attempt)
        if (attempt > 0) {
            qDebug() << "[InfluxDB Socket Retry]" << attempt << "of" << m_maxRetryCount
                     << "for socket write - message size:" << message.size() << "bytes";
        }
        
        // Check socket existence
        if (!QFileInfo::exists(socketPath)) {
            QString errorMsg = QString("[InfluxDB Write Error] Unix socket does not exist: %1 - Check if Telegraf is running and socket path is correct").arg(socketPath);
            if (attempt == m_maxRetryCount) {
                qCritical() << errorMsg << "- Final attempt failed";
                m_statistics.socketErrors++;
                return false;
            } else {
                qWarning() << errorMsg << "- Attempt" << (attempt + 1) << "failed, retrying...";
            }
        } else {
            // Check socket permissions
            QFileInfo socketInfo(socketPath);
            if (!socketInfo.isWritable()) {
                QString errorMsg = QString("[InfluxDB Write Error] Unix socket is not writable: %1 - Check socket permissions").arg(socketPath);
                if (attempt == m_maxRetryCount) {
                    qCritical() << errorMsg << "- Final attempt failed";
                    m_statistics.socketErrors++;
                    return false;
                } else {
                    qWarning() << errorMsg << "- Attempt" << (attempt + 1) << "failed, retrying...";
                }
            } else {
                // Attempt socket write
                int sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
                if (sockfd < 0) {
                    int errorCode = errno;
                    QString errorMsg = QString("[InfluxDB Write Error] Failed to create socket - errno: %1 error: %2").arg(errorCode).arg(strerror(errorCode));
                    if (attempt == m_maxRetryCount) {
                        qCritical() << errorMsg << "message size:" << message.size() << "bytes - Final attempt failed";
                        m_statistics.socketErrors++;
                        return false;
                    } else {
                        qWarning() << errorMsg << "- Attempt" << (attempt + 1) << "failed, retrying...";
                    }
                } else {
                    struct sockaddr_un addr {};
                    addr.sun_family = AF_UNIX;
                    strncpy(addr.sun_path, socketPath.toUtf8().constData(), sizeof(addr.sun_path) - 1);
                    
                    int result = ::sendto(sockfd, message.constData(), message.size(), 0,
                                          reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
                    int sendError = errno; // Capture errno before close()
                    ::close(sockfd);
                    
                    qint64 attemptDuration = QDateTime::currentMSecsSinceEpoch() - attemptStartTime;
                    qint64 totalDuration = QDateTime::currentMSecsSinceEpoch() - operationStartTime;
                    
                    if (result < 0) {
                        QString errorMsg = QString("[InfluxDB Write Error] Failed to send data to socket - errno: %1 error: %2").arg(sendError).arg(strerror(sendError));
                        if (attempt == m_maxRetryCount) {
                            qCritical() << errorMsg << "socket:" << socketPath
                                       << "message size:" << message.size() << "bytes"
                                       << "attempt_duration:" << attemptDuration << "ms"
                                       << "total_duration:" << totalDuration << "ms - Final attempt failed";
                            m_statistics.socketErrors++;
                            return false;
                        } else {
                            qWarning() << errorMsg << "- Attempt" << (attempt + 1) << "failed"
                                      << "attempt_duration:" << attemptDuration << "ms, retrying...";
                        }
                    } else if (result != message.size()) {
                        qWarning() << "[InfluxDB Write Warning] Partial write to socket - sent:" << result
                                  << "expected:" << message.size() << "bytes"
                                  << "socket:" << socketPath
                                  << "attempt_duration:" << attemptDuration << "ms"
                                  << "total_duration:" << totalDuration << "ms"
                                  << "- Attempt" << (attempt + 1) << "succeeded with partial write";
                        // Consider partial write as success
                        qDebug() << "[InfluxDB Write Success] Sent" << result << "bytes to Telegraf socket after"
                                 << (attempt + 1) << "attempts, total_duration:" << totalDuration << "ms"
                                 << "data:" << message.trimmed();
                        return true;
                    } else {
                        // Complete success
                        qDebug() << "[InfluxDB Write Success] Sent" << result << "bytes to Telegraf socket"
                                 << "attempt:" << (attempt + 1) << "attempt_duration:" << attemptDuration << "ms"
                                 << "total_duration:" << totalDuration << "ms"
                                 << "data:" << message.trimmed();
                        return true;
                    }
                }
            }
        }
        
        // Apply exponential backoff delay before next retry (except after last attempt)
        if (attempt < m_maxRetryCount) {
            qint64 retryDelay = calculateRetryDelay(attempt);
            qDebug() << "[InfluxDB Socket Retry] Waiting" << retryDelay << "ms before next attempt";
            QThread::msleep(retryDelay);
        }
    }
    
    // All retry attempts failed
    qCritical() << "[InfluxDB Write Error] All" << (m_maxRetryCount + 1) << "attempts failed for socket write"
               << "message size:" << message.size() << "bytes"
               << "total_duration:" << (QDateTime::currentMSecsSinceEpoch() - operationStartTime) << "ms";
    m_statistics.socketErrors++;
    return false;
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

// Comprehensive debug function to identify data inconsistencies
void ScadaCoreService::debugDataPointProcessing(const DataAcquisitionPoint &sourcePoint, const AcquiredDataPoint &dataPoint)
{
    qDebug() << "=== DEBUG DATA POINT PROCESSING ===";
    qDebug() << "Source Point Details:";
    qDebug() << "  - Point Name:" << sourcePoint.name;
    qDebug() << "  - Host:" << sourcePoint.host;
    qDebug() << "  - Port:" << sourcePoint.port;
    qDebug() << "  - Address:" << sourcePoint.address;
    qDebug() << "  - Data Type:" << static_cast<int>(sourcePoint.dataType);
    qDebug() << "  - Poll Interval:" << sourcePoint.pollInterval;
    qDebug() << "  - Measurement:" << sourcePoint.measurement;
    qDebug() << "  - Enabled:" << sourcePoint.enabled;
    qDebug() << "  - Tags count:" << sourcePoint.tags.size();
    
    // Debug tags content
    for (auto it = sourcePoint.tags.begin(); it != sourcePoint.tags.end(); ++it) {
        qDebug() << "    Tag:" << it.key() << "=" << it.value();
    }
    
    qDebug() << "Acquired Data Point Details:";
    qDebug() << "  - Point Name:" << dataPoint.pointName;
    qDebug() << "  - Is Valid:" << dataPoint.isValid;
    qDebug() << "  - Measurement:" << dataPoint.measurement;
    qDebug() << "  - Value:" << dataPoint.value;
    qDebug() << "  - Value Type:" << dataPoint.value.typeName();
    qDebug() << "  - Timestamp:" << dataPoint.timestamp;
    qDebug() << "  - Error Message:" << dataPoint.errorMessage;
    qDebug() << "  - Tags count:" << dataPoint.tags.size();
    
    // Debug acquired tags content
    for (auto it = dataPoint.tags.begin(); it != dataPoint.tags.end(); ++it) {
        qDebug() << "    Acquired Tag:" << it.key() << "=" << it.value();
    }
    
    // Check for null/empty values in critical fields
    if (dataPoint.tags.contains("register_type")) {
        QString regType = dataPoint.tags["register_type"];
        qDebug() << "  - Register Type from tags:" << regType << "(empty:" << regType.isEmpty() << ")";
    } else {
        qDebug() << "  - WARNING: register_type tag missing!";
    }
    
    if (dataPoint.tags.contains("data_type")) {
        QString dataType = dataPoint.tags["data_type"];
        qDebug() << "  - Data Type from tags:" << dataType << "(empty:" << dataType.isEmpty() << ")";
    } else {
        qDebug() << "  - WARNING: data_type tag missing!";
    }
    
    // Generate and debug InfluxDB line protocol
    QString sanitizedMeasurement = sanitizeInfluxIdentifier(dataPoint.measurement);
    QString sanitizedDevice = sanitizeInfluxTagValue(sourcePoint.host);
    QString sanitizedDescription = sanitizeInfluxTagValue(sourcePoint.name);
    sanitizedDescription.replace(" ", "_");
    QString sanitizedStationName = sourcePoint.host;
    
    QString line = QString("%1,device_name=%2,tag_name=%3,description=%4,station_name=%5 value=%6")
                   .arg(sanitizedMeasurement, sanitizedDevice, sanitizedDevice, sanitizedDescription, sanitizedStationName, dataPoint.value.toString());
    
    qDebug() << "Generated InfluxDB Line Protocol:" << line;
    qDebug() << "=== END DEBUG DATA POINT PROCESSING ===\n";
}

// Enhanced InfluxDB write method with full tag support
bool ScadaCoreService::validateAndSanitizeDataPoint(AcquiredDataPoint &dataPoint)
{
    // Fast-path validation for real-time performance
    // Only essential checks to prevent system failures
    
    // 1. Critical validation only - no logging for performance
    if (!dataPoint.isValid || dataPoint.measurement.isEmpty() || !dataPoint.value.isValid()) {
        return false;
    }
    
    // 2. Minimal numeric validation - only check for NaN/Inf
    if (dataPoint.value.typeId() == QMetaType::Double || dataPoint.value.typeId() == QMetaType::Float) {
        double numericValue = dataPoint.value.toDouble();
        if (std::isnan(numericValue) || std::isinf(numericValue)) {
            return false;
        }
        // Skip precision rounding for performance - let InfluxDB handle it
    }
    
    // 3. Skip string sanitization for performance - only truncate extremely long strings
    if (dataPoint.value.typeId() == QMetaType::QString) {
        QString stringValue = dataPoint.value.toString();
        if (stringValue.length() > 1000) {
            dataPoint.value = stringValue.left(1000);
        }
    }
    
    // 4. Skip measurement sanitization - assume pre-validated
    
    // 5. Skip tag sanitization - assume pre-validated tags
    
    // 6. Ensure only critical tags with minimal processing
    if (!dataPoint.tags.contains("device_name")) {
        dataPoint.tags["device_name"] = "UNKNOWN_DEVICE";
    }
    
    if (!dataPoint.tags.contains("address")) {
        dataPoint.tags["address"] = "0";
    }
    
    return true;
}

QString ScadaCoreService::sanitizeInfluxIdentifier(const QString &identifier)
{
    QString sanitized = identifier;
    
    // Replace spaces and special characters with underscores
    sanitized.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    
    // Remove consecutive underscores
    sanitized.replace(QRegularExpression("_{2,}"), "_");
    
    // Remove leading/trailing underscores
    sanitized = sanitized.trimmed();
    while (sanitized.startsWith("_")) sanitized.remove(0, 1);
    while (sanitized.endsWith("_")) sanitized.chop(1);
    
    // Ensure it's not empty and doesn't start with a number
    if (sanitized.isEmpty() || sanitized[0].isDigit()) {
        sanitized = "data_" + sanitized;
    }
    
    return sanitized;
}

QString ScadaCoreService::sanitizeInfluxTagValue(const QString &tagValue)
{
    QString sanitized = tagValue;
    
    // Replace spaces with underscores
    sanitized.replace(" ", "_");
    
    // Remove or replace problematic characters
    sanitized.replace(",", "_");  // Commas are field separators in InfluxDB
    sanitized.replace("=", "_");  // Equals are key-value separators
    sanitized.replace("\n", "_"); // Remove newlines
    sanitized.replace("\r", "_"); // Remove carriage returns
    sanitized.replace("\t", "_"); // Remove tabs
    sanitized.replace("\\", "_"); // Remove backslashes
    sanitized.replace("\"", "_"); // Remove quotes
    
    // Limit length
    const int MAX_TAG_LENGTH = 200;
    if (sanitized.length() > MAX_TAG_LENGTH) {
        sanitized = sanitized.left(MAX_TAG_LENGTH);
    }
    
    return sanitized.isEmpty() ? "unknown" : sanitized;
}

bool ScadaCoreService::writeToInfluxEnhanced(const AcquiredDataPoint &dataPoint)
{
    // Direct transmission without validation or sanitization
    
    // Extract values using consistent tag mapping logic
    QString measurement = dataPoint.measurement;
    QString deviceName = dataPoint.tags.value("device_name", "STATION_TEST");
    QString readMode = dataPoint.tags.value("read_mode", "single_register");
    QString address = dataPoint.tags.value("address", "0");
    QString description = dataPoint.tags.value("description", QString("CURRENT_RTU_%1").arg(address));
    
    // Build InfluxDB line protocol with consistent tag handling
    QString line = QString("%1,device_name=%2,read_mode=%3,address=%4,description=%5")
                   .arg(measurement, deviceName, readMode, address, description);
    
    // Add data_type tag only if configured and present
    if (m_includeDataTypeTags && dataPoint.tags.contains("data_type")) {
        QString dataType = dataPoint.tags.value("data_type", "INT16");
        line += QString(",data_type=%1").arg(dataType);
    }
    
    // Add field value
    line += QString(" value=%1\n").arg(dataPoint.value.toString());
    
    // Send directly to Unix socket
    return writeToTelegrafSocket(m_telegrafSocketPath, line.toUtf8());
}

bool ScadaCoreService::sendDataToInflux(const AcquiredDataPoint &dataPoint)
{
    // Direct transmission without any validation or preprocessing
    
    // Use asynchronous write instead of blocking write
    if (m_influxWorker) {
        QMetaObject::invokeMethod(m_influxWorker, "writeDataPoint", Qt::QueuedConnection,
                                Q_ARG(AcquiredDataPoint, dataPoint));
        return true; // Return true immediately for async operation
    } else {
        // Fallback to synchronous write if worker is not available
        return writeToInfluxEnhanced(dataPoint);
    }
}



bool ScadaCoreService::connectToModbusHost(const QString &host, int port)
{
    QString hostKey = QString("%1:%2").arg(host).arg(port);
    
    // Check if we're already connected to the same host
    if (m_modbusManager->isConnected() && m_currentHost == hostKey) {
        qDebug() << "Already connected to" << hostKey << "- reusing connection";
        return true;
    }
    
    // Only disconnect if we're connected to a different host
    if (m_modbusManager->isConnected() && m_currentHost != hostKey) {
        qDebug() << "Switching from" << m_currentHost << "to" << hostKey;
        m_modbusManager->disconnectFromServer();
        // Give some time for disconnection
        QThread::msleep(100);
        m_currentHost.clear();
    }
    
    // Connect to the new host
    qDebug() << "Connecting to Modbus host:" << hostKey;
    bool connectionStarted = m_modbusManager->connectToServer(host, port);
    
    if (!connectionStarted) {
        qWarning() << "Failed to start connection to" << hostKey;
        m_currentHost.clear();
        return false;
    }
    
    // Wait for connection to be established (with timeout)
    int waitTime = 0;
    const int maxWaitTime = 15000; // Use ConnectionResilience timeout (15 seconds default)
    const int checkInterval = 100; // 100ms
    
    while (waitTime < maxWaitTime && !m_modbusManager->isConnected()) {
        QThread::msleep(checkInterval);
        waitTime += checkInterval;
        QCoreApplication::processEvents(); // Process Qt events
    }
    
    bool connected = m_modbusManager->isConnected();
    if (connected) {
        m_currentHost = hostKey;
        qDebug() << "Successfully connected to" << hostKey;
    } else {
        qWarning() << "Failed to connect to" << hostKey << "(timeout after" << waitTime << "ms)";
        m_currentHost.clear();
        // Ensure we disconnect if connection attempt failed
        m_modbusManager->disconnectFromServer();
    }
    
    return connected;
}

void ScadaCoreService::updateStatistics(bool success, qint64 responseTime)
{
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
        qWarning() << "  Addresses size:" << originalAddresses.size();
        qWarning() << "  Names size:" << originalNames.size();
        qWarning() << "  DataTypes size:" << originalDataTypes.size();
        qWarning() << "  Descriptions size:" << originalDescriptions.size();
        qWarning() << "  Measurements size:" << originalMeasurements.size();
        qWarning() << "  Raw metadata - Addresses:" << blockPoint.tags.value("original_addresses", "MISSING");
        qWarning() << "  Raw metadata - Names:" << blockPoint.tags.value("original_names", "MISSING");
        qWarning() << "  Raw metadata - DataTypes:" << blockPoint.tags.value("original_data_types", "MISSING");
        qWarning() << "  Raw metadata - Descriptions:" << blockPoint.tags.value("original_descriptions", "MISSING");
        qWarning() << "  Raw metadata - Measurements:" << blockPoint.tags.value("original_measurements", "MISSING");
        return;
    }
    
    // Process each original point
    int currentRegisterOffset = 0; // Track actual register position in raw data
    for (int pointIndex = 0; pointIndex < originalAddresses.size(); pointIndex++) {
        int originalAddress = originalAddresses[pointIndex].toInt();
        int dataTypeInt = originalDataTypes[pointIndex].toInt();
        ModbusDataType dataType = static_cast<ModbusDataType>(dataTypeInt);
        
        // Calculate offset based on cumulative register usage, not address difference
        int offset = currentRegisterOffset;
        
        qDebug() << "Processing point" << pointIndex << ":"
                 << "Name:" << originalNames[pointIndex]
                 << "Address:" << originalAddress
                 << "Start Address:" << startAddress
                 << "Register Offset:" << offset
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
            qWarning() << "Register offset out of range:" << offset << "(needs" << registersNeeded << "registers) for address" << originalAddress << "in block of size" << result.rawData.size();
            continue;
        }
        
        // Update register offset for next point
        currentRegisterOffset += registersNeeded;
        
        // Create acquired data point with minimal essential tags
        AcquiredDataPoint dataPoint;
        dataPoint.pointName = originalNames[pointIndex];
        dataPoint.timestamp = result.timestamp;
        dataPoint.measurement = originalMeasurements[pointIndex];
        dataPoint.isValid = true;
        
        // Only copy essential tags to reduce InfluxDB filter overhead
        dataPoint.tags["device_name"] = blockPoint.tags.value("device_name", "STATION_TEST");
        // Format address according to configuration:
        // 'clean' = zero-based addressing, 'suffix' = one-based Modbus addressing
        if (m_influxAddressFormat == "suffix") {
            dataPoint.tags["address"] = QString::number(originalAddress);
        } else {
            dataPoint.tags["address"] = QString::number(originalAddress - 1);
        }
        dataPoint.tags["description"] = originalDescriptions[pointIndex];
        
        // Data type information is handled by validateAndSetInfluxTags if needed
        
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
        
        // Debug specific problematic tags from block reads
        if (dataPoint.pointName.contains("A1020-OMA-001") || dataPoint.pointName.contains("SA1020-FRS-001A")) {
            qDebug() << "🔍 [BLOCK READ DEBUG] Processing problematic tag:" << dataPoint.pointName;
            debugDataPointProcessing(tempSourcePoint, dataPoint);
        }
        
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

QString ScadaCoreService::modbusDataTypeToString(ModbusDataType dataType)
{
    switch (dataType) {
        case ModbusDataType::HoldingRegister:
        case ModbusDataType::InputRegister:
            return "INT16";
        case ModbusDataType::Float32:
            return "FLOAT32";
        case ModbusDataType::Double64:
            return "DOUBLE";
        case ModbusDataType::Long32:
        case ModbusDataType::Long64:
            return "LONG";
        case ModbusDataType::BOOL:
        case ModbusDataType::Coil:
        case ModbusDataType::DiscreteInput:
            return "BOOL";
        default:
            return "INT16"; // Default fallback
    }
}

QMap<QString, QString> ScadaCoreService::prepareStandardInfluxTags(const DataAcquisitionPoint &sourcePoint, const QMap<QString, QString> &existingTags)
{
    QMap<QString, QString> tags = existingTags;
    
    // Ensure address tag is always set (critical for InfluxDB mapping)
    if (!tags.contains("address") || tags["address"].isEmpty()) {
        // Format address according to configuration:
        // 'clean' = zero-based addressing, 'suffix' = one-based Modbus addressing
        if (m_influxAddressFormat == "suffix") {
            tags["address"] = QString::number(sourcePoint.address);
        } else {
            tags["address"] = QString::number(sourcePoint.address - 1);
        }
    }
    
    // Ensure device_name tag is set
    if (!tags.contains("device_name") || tags["device_name"].isEmpty()) {
        tags["device_name"] = sourcePoint.host;
    }
    
    // Conditionally include data_type tag based on configuration
    if (m_includeDataTypeTags && !tags.contains("data_type")) {
        // First try to get from sourcePoint tags (for individual reads)
        if (sourcePoint.tags.contains("data_type") && !sourcePoint.tags["data_type"].isEmpty()) {
            tags["data_type"] = sourcePoint.tags["data_type"];
        } else {
            // For block reads, convert from ModbusDataType enum to consistent string format
            tags["data_type"] = modbusDataTypeToString(sourcePoint.dataType);
        }
    }
    
    // Set default read_mode if not present
    if (!tags.contains("read_mode")) {
        tags["read_mode"] = "single_register";
    }
    
    // Set default description if not present
    if (!tags.contains("description")) {
        QString address = tags.value("address", "0");
        tags["description"] = QString("CURRENT_RTU_%1").arg(address);
    }
    
    // Validate essential tags
    QStringList essentialTags = {"address", "device_name", "description"};
    if (m_includeDataTypeTags && tags.contains("data_type")) {
        essentialTags.append("data_type");
    }
    for (const QString &tag : essentialTags) {
        if (!tags.contains(tag) || tags[tag].isEmpty()) {
            qWarning() << "CRITICAL: Essential tag validation failed for" << tag << "in point:" << sourcePoint.name;
        }
    }
    
    return tags;
}

void ScadaCoreService::validateAndSetInfluxTags(AcquiredDataPoint &dataPoint, const DataAcquisitionPoint &sourcePoint)
{
    // Use standardized tag preparation algorithm
    dataPoint.tags = prepareStandardInfluxTags(sourcePoint, dataPoint.tags);
    
    QString tagMode = m_includeDataTypeTags ? "analysis" : "performance";
    qDebug() << "Tag count for point:" << dataPoint.pointName << "Total tags:" << dataPoint.tags.size() << "Mode:" << tagMode;
}

void ScadaCoreService::bufferFailedDataPoint(const AcquiredDataPoint &dataPoint)
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Check buffer size limit with enhanced logging
    if (m_influxBuffer.size() >= m_maxBufferSize) {
        BufferedDataPoint droppedPoint = m_influxBuffer.dequeue();
        qCritical() << "[InfluxDB Buffer Full] Buffer at maximum capacity (" << m_maxBufferSize
                   << ") - DROPPING oldest entry to make room"
                   << "- Dropped point:" << droppedPoint.dataPoint.pointName
                   << "measurement:" << droppedPoint.dataPoint.measurement
                   << "value:" << droppedPoint.dataPoint.value.toString()
                   << "retry_count:" << droppedPoint.retryCount
                   << "- DATA LOSS OCCURRED";
        
        emit errorOccurred(QString("[InfluxDB Buffer Overflow] Buffer full - dropped data point: %1 (measurement: %2, value: %3) - DATA LOST")
                          .arg(droppedPoint.dataPoint.pointName, droppedPoint.dataPoint.measurement, droppedPoint.dataPoint.value.toString()));
    }
    
    // Create buffered data point with current timestamp for retry scheduling
    BufferedDataPoint bufferedPoint(dataPoint);
    qint64 initialDelay = calculateRetryDelay(0);
    bufferedPoint.nextRetryTime = currentTime + initialDelay;
    
    m_influxBuffer.enqueue(bufferedPoint);
    
    qWarning() << "[InfluxDB Buffer] Buffering failed data point:" << dataPoint.pointName
              << "measurement:" << dataPoint.measurement
              << "value:" << dataPoint.value.toString()
              << "buffer_size:" << m_influxBuffer.size() << "/" << m_maxBufferSize
              << "first_retry_in:" << initialDelay << "ms"
              << "timestamp:" << QDateTime::fromMSecsSinceEpoch(currentTime).toString("yyyy-MM-dd hh:mm:ss.zzz");
}

void ScadaCoreService::processInfluxBuffer()
{
    if (m_influxBuffer.isEmpty()) {
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    QQueue<BufferedDataPoint> retryQueue;
    int processedCount = 0;
    int successCount = 0;
    int failedCount = 0;
    int droppedCount = 0;
    
    qDebug() << "[InfluxDB Buffer] Processing" << m_influxBuffer.size() << "buffered data points";
    
    // Process all buffered data points
    while (!m_influxBuffer.isEmpty()) {
        BufferedDataPoint bufferedPoint = m_influxBuffer.dequeue();
        processedCount++;
        
        // Check if it's time to retry
        if (currentTime >= bufferedPoint.nextRetryTime) {
            qint64 waitTime = currentTime - bufferedPoint.nextRetryTime;
            qDebug() << "[InfluxDB Retry] Attempting retry for:" << bufferedPoint.dataPoint.pointName
                     << "attempt:" << (bufferedPoint.retryCount + 1) << "/" << m_maxRetryCount
                     << "measurement:" << bufferedPoint.dataPoint.measurement
                     << "value:" << bufferedPoint.dataPoint.value.toString()
                     << "wait_time:" << waitTime << "ms";
            
            qint64 retryStartTime = QDateTime::currentMSecsSinceEpoch();
            
            // Attempt to write to InfluxDB
            bool success = writeToInfluxEnhanced(bufferedPoint.dataPoint);
            
            qint64 retryDuration = QDateTime::currentMSecsSinceEpoch() - retryStartTime;
            
            if (success) {
                successCount++;
                qDebug() << "[InfluxDB Retry Success] Buffered data point sent successfully:"
                         << bufferedPoint.dataPoint.pointName
                         << "after" << (bufferedPoint.retryCount + 1) << "attempts"
                         << "retry_duration:" << retryDuration << "ms";
                emit dataPointSentToInflux(bufferedPoint.dataPoint.pointName, true);
            } else {
                failedCount++;
                // Increment retry count
                bufferedPoint.retryCount++;
                
                if (bufferedPoint.retryCount < m_maxRetryCount) {
                    qint64 nextDelay = calculateRetryDelay(bufferedPoint.retryCount);
                    // Schedule for next retry with exponential backoff
                    bufferedPoint.nextRetryTime = currentTime + nextDelay;
                    retryQueue.enqueue(bufferedPoint);
                    qWarning() << "[InfluxDB Retry Failed] Retry" << bufferedPoint.retryCount
                              << "failed for:" << bufferedPoint.dataPoint.pointName
                              << "measurement:" << bufferedPoint.dataPoint.measurement
                              << "value:" << bufferedPoint.dataPoint.value.toString()
                              << "retry_duration:" << retryDuration << "ms"
                              << "- scheduling next retry in" << nextDelay << "ms";
                } else {
                    droppedCount++;
                    qCritical() << "[InfluxDB Retry Exhausted] Max retries (" << m_maxRetryCount
                               << ") exceeded for data point:" << bufferedPoint.dataPoint.pointName
                               << "measurement:" << bufferedPoint.dataPoint.measurement
                               << "value:" << bufferedPoint.dataPoint.value.toString()
                               << "total_attempts:" << bufferedPoint.retryCount
                               << "- DROPPING from buffer (DATA LOSS)";
                    emit errorOccurred(QString("[InfluxDB Data Loss] Max retries exceeded for data point: %1 (measurement: %2, value: %3) - DATA PERMANENTLY LOST")
                                      .arg(bufferedPoint.dataPoint.pointName, bufferedPoint.dataPoint.measurement, bufferedPoint.dataPoint.value.toString()));
                }
            }
        } else {
            // Not time to retry yet, put back in queue
            qint64 timeUntilRetry = bufferedPoint.nextRetryTime - currentTime;
            qDebug() << "[InfluxDB Buffer] Data point" << bufferedPoint.dataPoint.pointName
                     << "scheduled for retry in" << timeUntilRetry << "ms"
                     << "(attempt" << (bufferedPoint.retryCount + 1) << "/" << m_maxRetryCount << ")";
            retryQueue.enqueue(bufferedPoint);
        }
    }
    
    // Put back items that need more retries
    m_influxBuffer = retryQueue;
    
    // Enhanced buffer status logging
    if (!m_influxBuffer.isEmpty()) {
        qDebug() << "[InfluxDB Buffer Status] Buffer contains" << m_influxBuffer.size() << "items pending retry"
                 << "- Processed:" << processedCount
                 << "Success:" << successCount
                 << "Failed:" << failedCount
                 << "Dropped:" << droppedCount;
    } else if (processedCount > 0) {
        qDebug() << "[InfluxDB Buffer Status] Buffer is now empty"
                 << "- Processed:" << processedCount
                 << "Success:" << successCount
                 << "Failed:" << failedCount
                 << "Dropped:" << droppedCount;
    }
}

qint64 ScadaCoreService::calculateRetryDelay(int retryCount)
{
    // Exponential backoff: base_delay * multiplier^retryCount
    // With jitter to avoid thundering herd
    qint64 baseDelay = static_cast<qint64>(m_baseRetryDelayMs * qPow(m_retryBackoffMultiplier, retryCount));
    
    // Cap at maximum retry delay
    if (baseDelay > m_maxRetryDelayMs) {
        baseDelay = m_maxRetryDelayMs;
    }
    
    // Add jitter if enabled
    if (m_jitterEnabled) {
        qint64 jitter = QRandomGenerator::global()->bounded(static_cast<int>(baseDelay * 0.1)); // 10% jitter
        baseDelay += jitter;
    }
    
    return baseDelay;
}

bool ScadaCoreService::loadDataResilienceConfiguration(const QString &configPath)
{
    // Check if config file exists
    if (!QFileInfo::exists(configPath)) {
        qWarning() << "[Config] Data resilience config file not found:" << configPath << "- using default values";
        return false;
    }
    
    QSettings settings(configPath, QSettings::IniFormat);
    
    // Load DataResilience section parameters
    settings.beginGroup("DataResilience");
    
    // InfluxDB Buffer Settings
    m_maxBufferSize = settings.value("max_buffer_size", m_maxBufferSize).toInt();
    m_bufferFlushInterval = settings.value("buffer_flush_interval_ms", m_bufferFlushInterval).toInt();
    m_bufferPersistenceEnabled = settings.value("buffer_persistence_enabled", m_bufferPersistenceEnabled).toBool();
    
    // Retry Mechanism Settings
    m_maxRetryCount = settings.value("max_retry_count", m_maxRetryCount).toInt();
    m_baseRetryDelayMs = settings.value("base_retry_delay_ms", m_baseRetryDelayMs).toInt();
    m_maxRetryDelayMs = settings.value("max_retry_delay_ms", m_maxRetryDelayMs).toInt();
    m_retryBackoffMultiplier = settings.value("retry_backoff_multiplier", m_retryBackoffMultiplier).toDouble();
    m_jitterEnabled = settings.value("jitter_enabled", m_jitterEnabled).toBool();
    
    // Data Validation Settings
    m_validationEnabled = settings.value("validation_enabled", m_validationEnabled).toBool();
    m_maxStringLength = settings.value("max_string_length", m_maxStringLength).toInt();
    m_maxNumericPrecision = settings.value("max_numeric_precision", m_maxNumericPrecision).toInt();
    m_sanitizationEnabled = settings.value("sanitization_enabled", m_sanitizationEnabled).toBool();
    m_strictValidation = settings.value("strict_validation", m_strictValidation).toBool();
    
    // Telegraf Socket Settings
    m_socketWriteTimeout = settings.value("socket_write_timeout_ms", m_socketWriteTimeout).toInt();
    m_socketReconnectDelay = settings.value("socket_reconnect_delay_ms", m_socketReconnectDelay).toInt();
    m_socketMaxReconnectAttempts = settings.value("socket_max_reconnect_attempts", m_socketMaxReconnectAttempts).toInt();
    
    settings.endGroup();
    
    // Load InfluxDB Address Format Settings
    settings.beginGroup("InfluxDB Address Format Settings");
    m_influxAddressFormat = settings.value("address_format", "clean").toString();
    m_includeDataTypeTags = settings.value("include_data_type_tags", false).toBool();
    settings.endGroup();
    
    // Update retry timer interval with new configuration
    if (m_retryTimer) {
        m_retryTimer->setInterval(m_bufferFlushInterval);
    }
    
    qDebug() << "[Config] Data resilience configuration loaded:";
    qDebug() << "  - Buffer: max_size=" << m_maxBufferSize << "flush_interval=" << m_bufferFlushInterval << "ms";
    qDebug() << "  - Retry: max_count=" << m_maxRetryCount << "base_delay=" << m_baseRetryDelayMs << "ms max_delay=" << m_maxRetryDelayMs << "ms";
    qDebug() << "  - Validation: enabled=" << m_validationEnabled << "max_string_len=" << m_maxStringLength;
    qDebug() << "  - Socket: timeout=" << m_socketWriteTimeout << "ms reconnect_delay=" << m_socketReconnectDelay << "ms";
    qDebug() << "  - InfluxDB Address Format:" << m_influxAddressFormat;
    qDebug() << "  - Include Data Type Tags:" << m_includeDataTypeTags;
    
    return true;
}

// Monitoring and alerting implementation
void ScadaCoreService::setAlertThresholds(double minSuccessRate, qint64 maxConsecutiveFailures, qint64 maxResponseTime, qint64 maxBufferSize)
{
    m_minSuccessRateThreshold = minSuccessRate;
    m_maxConsecutiveFailuresThreshold = maxConsecutiveFailures;
    m_maxResponseTimeThreshold = maxResponseTime;
    m_maxBufferSizeThreshold = maxBufferSize;
    
    qDebug() << "Alert thresholds updated:" 
             << "Success rate:" << minSuccessRate << "%"
             << "Max consecutive failures:" << maxConsecutiveFailures
             << "Max response time:" << maxResponseTime << "ms"
             << "Max buffer size:" << maxBufferSize;
}

void ScadaCoreService::enableAlerting(bool enabled)
{
    m_alertingEnabled = enabled;
    qDebug() << "Alerting" << (enabled ? "enabled" : "disabled");
    
    if (!enabled) {
        m_activeAlerts.clear();
        emit healthStatusChanged(true);
    }
}

QVector<ScadaCoreService::SystemAlert> ScadaCoreService::getActiveAlerts() const
{
    return m_activeAlerts;
}

QVector<ScadaCoreService::SystemAlert> ScadaCoreService::getAlertHistory(qint64 fromTimestamp) const
{
    if (fromTimestamp == 0) {
        return m_alertHistory;
    }
    
    QVector<SystemAlert> filteredHistory;
    for (const auto &alert : m_alertHistory) {
        if (alert.timestamp >= fromTimestamp) {
            filteredHistory.append(alert);
        }
    }
    return filteredHistory;
}

void ScadaCoreService::clearAlerts()
{
    m_activeAlerts.clear();
    m_alertHistory.clear();
    m_systemHealthy = true;
    emit healthStatusChanged(true);
    qDebug() << "All alerts cleared";
}

void ScadaCoreService::acknowledgeAlert(qint64 alertTimestamp)
{
    for (int i = 0; i < m_activeAlerts.size(); ++i) {
        if (m_activeAlerts[i].timestamp == alertTimestamp) {
            SystemAlert alert = m_activeAlerts.takeAt(i);
            m_alertHistory.append(alert);
            
            qDebug() << "Alert acknowledged:" << alertTypeToString(alert.type) 
                     << "from" << alert.source;
            
            // Check if system is now healthy
            if (m_activeAlerts.isEmpty()) {
                m_systemHealthy = true;
                emit healthStatusChanged(true);
            }
            break;
        }
    }
}

void ScadaCoreService::checkSystemHealth()
{
    if (!m_alertingEnabled) {
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Only check health periodically
    if (currentTime - m_lastHealthCheck < m_alertCheckInterval) {
        return;
    }
    
    m_lastHealthCheck = currentTime;
    
    // Update monitoring metrics
    updateMonitoringMetrics();
    
    // Perform health checks
    checkDataLoss();
    checkConnectionHealth();
    checkBufferHealth();
    checkResponseTimes();
    
    // Update system health status
    bool wasHealthy = m_systemHealthy;
    m_systemHealthy = m_activeAlerts.isEmpty();
    
    if (wasHealthy != m_systemHealthy) {
        emit healthStatusChanged(m_systemHealthy);
    }
}

void ScadaCoreService::generateAlert(AlertType type, AlertSeverity severity, const QString &message, 
                                   const QString &source, const QMap<QString, QString> &metadata)
{
    if (!m_alertingEnabled) {
        return;
    }
    
    // Check if this alert is already active
    if (isAlertActive(type, source)) {
        return;
    }
    
    SystemAlert alert(type, severity, message, source);
    alert.metadata = metadata;
    
    m_activeAlerts.append(alert);
    m_statistics.alertsGenerated++;
    
    qWarning() << "ALERT [" << alertSeverityToString(severity) << "]" 
               << alertTypeToString(type) << ":" << message
               << (source.isEmpty() ? "" : "(Source: " + source + ")");
    
    emit alertGenerated(alert);
    
    // Update system health
    if (m_systemHealthy) {
        m_systemHealthy = false;
        emit healthStatusChanged(false);
    }
}

void ScadaCoreService::clearAlert(AlertType type, const QString &source)
{
    for (int i = 0; i < m_activeAlerts.size(); ++i) {
        if (m_activeAlerts[i].type == type && 
            (source.isEmpty() || m_activeAlerts[i].source == source)) {
            
            SystemAlert alert = m_activeAlerts.takeAt(i);
            m_alertHistory.append(alert);
            
            qDebug() << "Alert cleared:" << alertTypeToString(type)
                     << (source.isEmpty() ? "" : "from " + source);
            
            emit alertCleared(type, source);
            break;
        }
    }
    
    // Update system health if no more alerts
    if (m_activeAlerts.isEmpty() && !m_systemHealthy) {
        m_systemHealthy = true;
        emit healthStatusChanged(true);
    }
}

bool ScadaCoreService::isAlertActive(AlertType type, const QString &source) const
{
    for (const auto &alert : m_activeAlerts) {
        if (alert.type == type && (source.isEmpty() || alert.source == source)) {
            return true;
        }
    }
    return false;
}

void ScadaCoreService::updateMonitoringMetrics()
{
    // Update success rate
    if (m_statistics.totalReadOperations > 0) {
        m_statistics.successRate = (double)m_statistics.successfulReads / m_statistics.totalReadOperations * 100.0;
    }
    
    // Update buffer size
    m_statistics.bufferSize = m_influxBuffer.size();
    
    // Update last successful read time
    if (m_statistics.successfulReads > 0) {
        m_statistics.lastSuccessfulRead = QDateTime::currentMSecsSinceEpoch();
    }
}

void ScadaCoreService::checkDataLoss()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Check for missed readings on each data point
    for (const auto &point : m_dataPoints) {
        if (!point.enabled) continue;
        
        qint64 lastReadTime = m_lastDataPointTime.value(point.name, 0);
        qint64 timeSinceLastRead = currentTime - lastReadTime;
        
        // Consider data lost if no reading for 3x the poll interval
        qint64 dataLossThreshold = point.pollInterval * 3;
        
        if (lastReadTime > 0 && timeSinceLastRead > dataLossThreshold) {
            qint64 missedReadings = timeSinceLastRead / point.pollInterval;
            m_missedReadingsCount[point.name] = missedReadings;
            
            if (!isAlertActive(AlertType::DataLoss, point.name)) {
                QMap<QString, QString> metadata;
                metadata["missed_readings"] = QString::number(missedReadings);
                metadata["last_read_time"] = QString::number(lastReadTime);
                metadata["poll_interval"] = QString::number(point.pollInterval);
                
                generateAlert(AlertType::DataLoss, AlertSeverity::Warning,
                            QString("Data loss detected for point '%1': %2 missed readings")
                            .arg(point.name).arg(missedReadings),
                            point.name, metadata);
                
                emit dataLossDetected(point.name, missedReadings);
            }
        } else if (isAlertActive(AlertType::DataLoss, point.name)) {
            // Clear data loss alert if data is now being received
            clearAlert(AlertType::DataLoss, point.name);
            m_missedReadingsCount.remove(point.name);
        }
    }
}

void ScadaCoreService::checkConnectionHealth()
{
    // Check for high error rate
    if (m_statistics.successRate < m_minSuccessRateThreshold && m_statistics.totalReadOperations > 10) {
        if (!isAlertActive(AlertType::HighErrorRate)) {
            QMap<QString, QString> metadata;
            metadata["success_rate"] = QString::number(m_statistics.successRate, 'f', 1);
            metadata["threshold"] = QString::number(m_minSuccessRateThreshold, 'f', 1);
            
            generateAlert(AlertType::HighErrorRate, AlertSeverity::Critical,
                        QString("High error rate detected: %1% (threshold: %2%)")
                        .arg(m_statistics.successRate, 0, 'f', 1)
                        .arg(m_minSuccessRateThreshold, 0, 'f', 1),
                        QString(), metadata);
        }
    } else if (isAlertActive(AlertType::HighErrorRate)) {
        clearAlert(AlertType::HighErrorRate, QString());
    }
    
    // Check for consecutive failures
    if (m_statistics.consecutiveFailures >= m_maxConsecutiveFailuresThreshold) {
        if (!isAlertActive(AlertType::ConnectionFailure)) {
            QMap<QString, QString> metadata;
            metadata["consecutive_failures"] = QString::number(m_statistics.consecutiveFailures);
            metadata["threshold"] = QString::number(m_maxConsecutiveFailuresThreshold);
            
            generateAlert(AlertType::ConnectionFailure, AlertSeverity::Critical,
                        QString("Connection failure: %1 consecutive failures (threshold: %2)")
                        .arg(m_statistics.consecutiveFailures)
                        .arg(m_maxConsecutiveFailuresThreshold),
                        QString(), metadata);
        }
    } else if (isAlertActive(AlertType::ConnectionFailure)) {
        clearAlert(AlertType::ConnectionFailure, QString());
    }
}

void ScadaCoreService::checkBufferHealth()
{
    if (m_statistics.bufferSize >= m_maxBufferSizeThreshold) {
        if (!isAlertActive(AlertType::BufferOverflow)) {
            QMap<QString, QString> metadata;
            metadata["buffer_size"] = QString::number(m_statistics.bufferSize);
            metadata["threshold"] = QString::number(m_maxBufferSizeThreshold);
            metadata["max_buffer_size"] = QString::number(m_maxBufferSize);
            
            AlertSeverity severity = (m_statistics.bufferSize >= m_maxBufferSize * 0.9) ? 
                                   AlertSeverity::Emergency : AlertSeverity::Warning;
            
            generateAlert(AlertType::BufferOverflow, severity,
                        QString("Buffer overflow warning: %1 items (threshold: %2, max: %3)")
                        .arg(m_statistics.bufferSize)
                        .arg(m_maxBufferSizeThreshold)
                        .arg(m_maxBufferSize),
                        QString(), metadata);
        }
    } else if (isAlertActive(AlertType::BufferOverflow)) {
        clearAlert(AlertType::BufferOverflow, QString());
    }
}

void ScadaCoreService::checkResponseTimes()
{
    if (m_statistics.maxResponseTime > m_maxResponseTimeThreshold) {
        if (!isAlertActive(AlertType::ResponseTimeout)) {
            QMap<QString, QString> metadata;
            metadata["max_response_time"] = QString::number(m_statistics.maxResponseTime);
            metadata["avg_response_time"] = QString::number(m_statistics.averageResponseTime, 'f', 2);
            metadata["threshold"] = QString::number(m_maxResponseTimeThreshold);
            
            generateAlert(AlertType::ResponseTimeout, AlertSeverity::Warning,
                        QString("High response time detected: %1ms (avg: %2ms, threshold: %3ms)")
                        .arg(m_statistics.maxResponseTime)
                        .arg(m_statistics.averageResponseTime, 0, 'f', 2)
                        .arg(m_maxResponseTimeThreshold),
                        QString(), metadata);
        }
    } else if (isAlertActive(AlertType::ResponseTimeout)) {
        clearAlert(AlertType::ResponseTimeout, QString());
    }
}

QString ScadaCoreService::alertTypeToString(AlertType type) const
{
    switch (type) {
        case AlertType::DataLoss: return "Data Loss";
        case AlertType::ConnectionFailure: return "Connection Failure";
        case AlertType::HighErrorRate: return "High Error Rate";
        case AlertType::BufferOverflow: return "Buffer Overflow";
        case AlertType::ResponseTimeout: return "Response Timeout";
        case AlertType::SystemHealth: return "System Health";
        default: return "Unknown";
    }
}

QString ScadaCoreService::alertSeverityToString(AlertSeverity severity) const
{
    switch (severity) {
        case AlertSeverity::Info: return "INFO";
        case AlertSeverity::Warning: return "WARNING";
        case AlertSeverity::Critical: return "CRITICAL";
        case AlertSeverity::Emergency: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

// Unified Data Acquisition Function
QVector<AcquiredDataPoint> ScadaCoreService::acquireDataUnified(const DataAcquisitionPoint &point, ReadMode mode, const QVector<DataAcquisitionPoint> &blockPoints)
{
    QVector<AcquiredDataPoint> results;
    
    // Input validation for invalid modes
    if (mode != ReadMode::SINGLE_READ && mode != ReadMode::BLOCK_READ) {
        qCritical() << "Invalid read mode specified for point:" << point.name;
        return results;
    }
    
    // Validate block points for block read mode
    if (mode == ReadMode::BLOCK_READ && blockPoints.isEmpty()) {
        qWarning() << "Block read mode requires block points to be specified";
        // Fallback to single read mode for backward compatibility
        mode = ReadMode::SINGLE_READ;
        qInfo() << "Falling back to single read mode for point:" << point.name;
    }
    
    // Performance optimization: early return for empty point name
    if (point.name.isEmpty()) {
        qWarning() << "Cannot process point with empty name";
        return results;
    }
    
    // Standardized preparation process
    if (!prepareAcquisitionStandard(point, mode)) {
        qWarning() << "Failed to prepare acquisition for point:" << point.name;
        return results;
    }
    
    try {
        // Execute Modbus read based on mode
        ModbusReadResult readResult = executeModbusRead(point, mode, blockPoints);
        
        // Validate read result before processing
        if (readResult.pointName.isEmpty()) {
            readResult.pointName = point.name; // Ensure point name is set
        }
        
        // Process acquisition result
        results = processAcquisitionResult(readResult, point, mode, blockPoints);
        
        // Apply consistent formatting and validation
        for (auto &dataPoint : results) {
            formatDataPointStandard(dataPoint, point);
        }
        
        // Performance logging for large datasets
        if (results.size() > 10) {
            qDebug() << "Processed" << results.size() << "data points for" << point.name;
        }
        
    } catch (const std::exception &e) {
        qCritical() << "Exception in unified acquisition for point" << point.name << ":" << e.what();
        // Generate error data point for tracking
        AcquiredDataPoint errorPoint;
        errorPoint.name = point.name + "_error";
        errorPoint.timestamp = QDateTime::currentMSecsSinceEpoch();
        errorPoint.value = QVariant();
        errorPoint.tags["error"] = QString(e.what());
        errorPoint.tags["acquisition_mode"] = "unified_error";
        results.append(errorPoint);
    } catch (...) {
        qCritical() << "Unknown exception in unified acquisition for point" << point.name;
    }
    
    return results;
}

bool ScadaCoreService::prepareAcquisitionStandard(const DataAcquisitionPoint &point, ReadMode mode)
{
    // Validate input parameters
    if (point.name.isEmpty() || point.host.isEmpty()) {
        qWarning() << "Invalid point configuration: missing name or host";
        return false;
    }
    
    if (point.port <= 0 || point.port > 65535) {
        qWarning() << "Invalid port number:" << point.port;
        return false;
    }
    
    if (point.address < 0) {
        qWarning() << "Invalid address:" << point.address;
        return false;
    }
    
    // Validate mode-specific requirements
    if (mode == ReadMode::BLOCK_READ) {
        QString blockType = point.tags.value("block_type");
        if (blockType.isEmpty()) {
            qWarning() << "Block read mode requires block_type tag";
            return false;
        }
    }
    
    // Ensure Modbus connection
    if (!connectToModbusHost(point.host, point.port)) {
        qWarning() << "Failed to connect to Modbus host:" << point.host << ":" << point.port;
        return false;
    }
    
    return true;
}

ModbusReadResult ScadaCoreService::executeModbusRead(const DataAcquisitionPoint &point, ReadMode mode, const QVector<DataAcquisitionPoint> &blockPoints)
{
    ModbusReadResult result;
    result.success = false;
    result.pointName = point.name;
    result.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    if (!m_modbusManager) {
        result.errorString = "Modbus manager not initialized";
        return result;
    }
    
    // Determine register type and data type
    QString registerType = point.tags.value("register_type", "holding").toLower();
    QString dataType = point.tags.value("data_type", "uint16").toLower();
    
    // Convert 1-based address to 0-based
    int modbusAddress = point.address - 1;
    
    if (mode == ReadMode::SINGLE_READ) {
        // Single point read
        int registerCount = 1;
        
        // Determine register count based on data type
        if (dataType == "float32" || dataType == "long32") {
            registerCount = 2;
        } else if (dataType == "double64" || dataType == "long64") {
            registerCount = 4;
        }
        
        // Execute appropriate Modbus read function
        if (registerType == "holding") {
            result = m_modbusManager->readHoldingRegisters(modbusAddress, registerCount);
        } else if (registerType == "input") {
            result = m_modbusManager->readInputRegisters(modbusAddress, registerCount);
        } else if (registerType == "coil") {
            result = m_modbusManager->readCoils(modbusAddress, 1);
        } else if (registerType == "discrete") {
            result = m_modbusManager->readDiscreteInputs(modbusAddress, 1);
        } else {
            // Fallback to holding registers
            result = m_modbusManager->readHoldingRegisters(modbusAddress, registerCount);
        }
        
    } else if (mode == ReadMode::BLOCK_READ) {
        // Block read
        QString blockType = point.tags.value("block_type");
        int blockSize = point.tags.value("block_size", "1").toInt();
        
        if (blockSize <= 0) {
            result.errorString = "Invalid block size";
            return result;
        }
        
        // Execute block read based on register type
        if (registerType == "holding" || blockType == "holding") {
            result = m_modbusManager->readHoldingRegisters(modbusAddress, blockSize);
        } else if (registerType == "input" || blockType == "input") {
            result = m_modbusManager->readInputRegisters(modbusAddress, blockSize);
        } else if (registerType == "coil" || blockType == "coil") {
            result = m_modbusManager->readCoils(modbusAddress, blockSize);
        } else if (registerType == "discrete" || blockType == "discrete") {
            result = m_modbusManager->readDiscreteInputs(modbusAddress, blockSize);
        } else {
            // Fallback to holding registers
            result = m_modbusManager->readHoldingRegisters(modbusAddress, blockSize);
        }
    }
    
    return result;
}

QVector<AcquiredDataPoint> ScadaCoreService::processAcquisitionResult(const ModbusReadResult &result, const DataAcquisitionPoint &point, ReadMode mode, const QVector<DataAcquisitionPoint> &blockPoints)
{
    QVector<AcquiredDataPoint> results;
    
    if (!result.success) {
        qWarning() << "Modbus read failed for point" << point.name << ":" << result.errorString;
        return results;
    }
    
    if (mode == ReadMode::SINGLE_READ) {
        // Process single point result
        AcquiredDataPoint dataPoint;
        dataPoint.name = point.name;
        dataPoint.timestamp = result.timestamp;
        dataPoint.tags = prepareStandardInfluxTags(point, point.tags);
        
        // Extract value based on data type
        QString dataType = point.tags.value("data_type", "uint16").toLower();
        
        if (result.data.size() < 2) {
            qWarning() << "Insufficient data for point" << point.name;
            return results;
        }
        
        if (dataType == "uint16") {
            dataPoint.value = static_cast<quint16>(result.data[0]);
        } else if (dataType == "int16") {
            dataPoint.value = static_cast<qint16>(result.data[0]);
        } else if (dataType == "float32" && result.data.size() >= 4) {
            // Combine two 16-bit registers into a 32-bit float
            quint32 combined = (static_cast<quint32>(result.data[1]) << 16) | result.data[0];
            float floatValue;
            std::memcpy(&floatValue, &combined, sizeof(float));
            dataPoint.value = floatValue;
        } else if (dataType == "long32" && result.data.size() >= 4) {
            // Combine two 16-bit registers into a 32-bit integer
            qint32 combined = (static_cast<qint32>(result.data[1]) << 16) | result.data[0];
            dataPoint.value = combined;
        } else if (dataType == "double64" && result.data.size() >= 8) {
            // Combine four 16-bit registers into a 64-bit double
            quint64 combined = (static_cast<quint64>(result.data[3]) << 48) |
                              (static_cast<quint64>(result.data[2]) << 32) |
                              (static_cast<quint64>(result.data[1]) << 16) |
                              result.data[0];
            double doubleValue;
            std::memcpy(&doubleValue, &combined, sizeof(double));
            dataPoint.value = doubleValue;
        } else if (dataType == "long64" && result.data.size() >= 8) {
            // Combine four 16-bit registers into a 64-bit integer
            qint64 combined = (static_cast<qint64>(result.data[3]) << 48) |
                             (static_cast<qint64>(result.data[2]) << 32) |
                             (static_cast<qint64>(result.data[1]) << 16) |
                             result.data[0];
            dataPoint.value = combined;
        } else {
            // Default to uint16
            dataPoint.value = static_cast<quint16>(result.data[0]);
        }
        
        results.append(dataPoint);
        
    } else if (mode == ReadMode::BLOCK_READ) {
        // Process block read result - extract data points from block
        if (result.data.size() < 2) {
            qWarning() << "Insufficient block data for point" << point.name;
            return results;
        }
        
        // Extract original point metadata from block point tags
        QStringList originalAddresses = point.tags.value("original_addresses").split(",", Qt::SkipEmptyParts);
        QStringList originalNames = point.tags.value("original_names").split(",", Qt::SkipEmptyParts);
        QStringList originalDataTypes = point.tags.value("original_data_types").split(",", Qt::SkipEmptyParts);
        QStringList originalDescriptions = point.tags.value("original_descriptions").split(",", Qt::SkipEmptyParts);
        QStringList originalMeasurements = point.tags.value("original_measurements").split(",", Qt::SkipEmptyParts);
        
        // Validate metadata consistency
        if (originalAddresses.size() != originalNames.size() || 
            originalNames.size() != originalDataTypes.size()) {
            qWarning() << "Inconsistent block metadata for point" << point.name;
            return results;
        }
        
        // Process each original point in the block
        for (int i = 0; i < originalNames.size(); ++i) {
            AcquiredDataPoint dataPoint;
            dataPoint.name = originalNames[i];
            dataPoint.timestamp = result.timestamp;
            
            // Set basic tags
            dataPoint.tags["device"] = point.tags.value("device", point.host);
            dataPoint.tags["address"] = originalAddresses[i];
            if (i < originalDescriptions.size()) {
                dataPoint.tags["description"] = originalDescriptions[i];
            }
            if (i < originalMeasurements.size()) {
                dataPoint.measurement = originalMeasurements[i];
            }
            
            // Calculate offset in the raw data for this point
            int originalAddress = originalAddresses[i].toInt();
            int blockStartAddress = point.address;
            int registerOffset = originalAddress - blockStartAddress;
            
            if (registerOffset < 0 || registerOffset >= result.data.size() / 2) {
                qWarning() << "Register offset out of range for point" << originalNames[i];
                continue;
            }
            
            // Extract value based on data type
            QString dataType = (i < originalDataTypes.size()) ? originalDataTypes[i].toLower() : "uint16";
            
            if (dataType == "uint16") {
                if (registerOffset * 2 + 1 < result.data.size()) {
                    dataPoint.value = static_cast<quint16>(result.data[registerOffset * 2]);
                }
            } else if (dataType == "int16") {
                if (registerOffset * 2 + 1 < result.data.size()) {
                    dataPoint.value = static_cast<qint16>(result.data[registerOffset * 2]);
                }
            } else if (dataType == "float32") {
                if (registerOffset * 2 + 3 < result.data.size()) {
                    quint32 combined = (static_cast<quint32>(result.data[registerOffset * 2 + 1]) << 16) | 
                                      result.data[registerOffset * 2];
                    float floatValue;
                    std::memcpy(&floatValue, &combined, sizeof(float));
                    dataPoint.value = floatValue;
                }
            } else if (dataType == "long32") {
                if (registerOffset * 2 + 3 < result.data.size()) {
                    qint32 combined = (static_cast<qint32>(result.data[registerOffset * 2 + 1]) << 16) | 
                                     result.data[registerOffset * 2];
                    dataPoint.value = combined;
                }
            } else if (dataType == "double64") {
                if (registerOffset * 2 + 7 < result.data.size()) {
                    quint64 combined = (static_cast<quint64>(result.data[registerOffset * 2 + 3]) << 48) |
                                      (static_cast<quint64>(result.data[registerOffset * 2 + 2]) << 32) |
                                      (static_cast<quint64>(result.data[registerOffset * 2 + 1]) << 16) |
                                      result.data[registerOffset * 2];
                    double doubleValue;
                    std::memcpy(&doubleValue, &combined, sizeof(double));
                    dataPoint.value = doubleValue;
                }
            } else if (dataType == "long64") {
                if (registerOffset * 2 + 7 < result.rawData.size()) {
                    qint64 combined = (static_cast<qint64>(result.rawData[registerOffset * 2 + 3]) << 48) |
                                     (static_cast<qint64>(result.rawData[registerOffset * 2 + 2]) << 32) |
                                     (static_cast<qint64>(result.rawData[registerOffset * 2 + 1]) << 16) |
                                     result.rawData[registerOffset * 2];
                    dataPoint.value = combined;
                }
            } else {
                // Default to uint16
                if (registerOffset * 2 + 1 < result.rawData.size()) {
                    dataPoint.value = static_cast<quint16>(result.rawData[registerOffset * 2]);
                }
            }
            
            results.append(dataPoint);
        }
    }
    
    return results;
}

void ScadaCoreService::formatDataPointStandard(AcquiredDataPoint &dataPoint, const DataAcquisitionPoint &sourcePoint)
{
    // Apply consistent formatting and validation
    validateAndSanitizeDataPoint(dataPoint);
    
    // Ensure standard tags are present
    validateAndSetInfluxTags(dataPoint, sourcePoint);
    
    // Add acquisition metadata
    dataPoint.tags["acquisition_mode"] = "unified";
    dataPoint.tags["acquisition_timestamp"] = QString::number(QDateTime::currentMSecsSinceEpoch());
    
    // Ensure measurement name is set
    if (dataPoint.measurement.isEmpty()) {
        dataPoint.measurement = sourcePoint.measurement.isEmpty() ? "modbus_data" : sourcePoint.measurement;
    }
}

// Performance Optimization Functions
QVector<AcquiredDataPoint> ScadaCoreService::acquireDataBatch(const QVector<DataAcquisitionPoint> &points, ReadMode defaultMode)
{
    QVector<AcquiredDataPoint> allResults;
    
    if (points.isEmpty()) {
        return allResults;
    }
    
    // Group points by host and port for connection optimization
    QMap<QPair<QString, int>, QVector<DataAcquisitionPoint>> groupedPoints;
    for (const auto &point : points) {
        QPair<QString, int> hostPort(point.host, point.port);
        groupedPoints[hostPort].append(point);
    }
    
    // Process each group with optimized connection handling
    for (auto it = groupedPoints.begin(); it != groupedPoints.end(); ++it) {
        const QPair<QString, int> &hostPort = it.key();
        const QVector<DataAcquisitionPoint> &hostPoints = it.value();
        
        // Establish connection once per host
        if (!connectToModbusHost(hostPort.first, hostPort.second)) {
            qWarning() << "Failed to connect to host" << hostPort.first << ":" << hostPort.second;
            continue;
        }
        
        // Process points for this host
        for (const auto &point : hostPoints) {
            ReadMode mode = defaultMode;
            
            // Auto-detect optimal read mode based on point configuration
            if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
                mode = ReadMode::BLOCK_READ;
            }
            
            QVector<AcquiredDataPoint> results = acquireDataUnified(point, mode);
            allResults.append(results);
        }
    }
    
    return allResults;
}

bool ScadaCoreService::isOptimalForBlockRead(const QVector<DataAcquisitionPoint> &points)
{
    if (points.size() < 3) {
        return false; // Not worth block reading for small datasets
    }
    
    // Check if points are contiguous and on same host
    if (points.isEmpty()) {
        return false;
    }
    
    QString firstHost = points.first().host;
    int firstPort = points.first().port;
    int lastAddress = points.first().address;
    
    for (int i = 1; i < points.size(); ++i) {
        const auto &point = points[i];
        
        // Must be same host and port
        if (point.host != firstHost || point.port != firstPort) {
            return false;
        }
        
        // Check for address continuity (allowing small gaps)
        if (point.address - lastAddress > 5) {
            return false; // Gap too large
        }
        
        lastAddress = point.address;
    }
    
    return true;
}

QVector<AcquiredDataPoint> ScadaCoreService::acquireDataOptimized(const QVector<DataAcquisitionPoint> &points)
{
    QVector<AcquiredDataPoint> results;
    
    if (points.isEmpty()) {
        return results;
    }
    
    // Performance optimization: decide between block and individual reads
    if (isOptimalForBlockRead(points)) {
        // Create a synthetic block point for optimized reading
        DataAcquisitionPoint blockPoint = points.first();
        blockPoint.name = "optimized_block_" + QString::number(QDateTime::currentMSecsSinceEpoch());
        blockPoint.tags["block_type"] = "optimized_read";
        blockPoint.tags["block_size"] = QString::number(points.last().address - points.first().address + 1);
        
        // Encode original point information
        QStringList addresses, names, dataTypes, descriptions, measurements;
        for (const auto &point : points) {
            addresses << QString::number(point.address);
            names << point.name;
            dataTypes << point.tags.value("data_type", "uint16");
            descriptions << point.tags.value("description", "N/A");
            measurements << point.measurement;
        }
        
        blockPoint.tags["original_addresses"] = addresses.join(",");
        blockPoint.tags["original_names"] = names.join(",");
        blockPoint.tags["original_data_types"] = dataTypes.join(",");
        blockPoint.tags["original_descriptions"] = descriptions.join(",");
        blockPoint.tags["original_measurements"] = measurements.join(",");
        
        results = acquireDataUnified(blockPoint, ReadMode::BLOCK_READ, points);
        
        qDebug() << "Optimized block read for" << points.size() << "points, got" << results.size() << "results";
        
    } else {
        // Use batch processing for individual reads
        results = acquireDataBatch(points, ReadMode::SINGLE_READ);
        
        qDebug() << "Individual batch read for" << points.size() << "points, got" << results.size() << "results";
    }
    
    return results;
}
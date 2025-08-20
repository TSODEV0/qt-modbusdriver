#include "modbus_worker.h"
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QUuid>
#include <QMetaObject>
#include <QCoreApplication>
#include <QDateTime>
#include <QMutexLocker>

// Initialize static semaphore to allow max 2 simultaneous connections
QSemaphore ModbusWorker::s_connectionSemaphore(2);

// ModbusWorker Implementation
ModbusWorker::ModbusWorker(const QString& host, int port, int unitId, QObject* parent)
    : QObject(parent)
    , m_host(host)
    , m_port(port)
    , m_unitId(unitId)
    , m_deviceKey(QString("%1:%2:%3").arg(host).arg(port).arg(unitId))
    , m_modbusManager(nullptr)
    , m_nextRequestId(1)
    , m_requestInProgress(false)
    , m_pollTimer(nullptr)
    , m_pollInterval(2000)  // Increased from 1000ms to reduce connection drops
    , m_pollingEnabled(false)
    , m_workerRunning(false)
    , m_stopRequested(false)
    , m_lastConnectionAttempt(0)
    , m_reconnectionDelay(1000)
    , m_connectionAttempts(0)
    , m_basePollInterval(2000)
    , m_adaptivePollInterval(2000)
    , m_consecutiveSuccesses(0)
    , m_consecutiveFailures(0)
    , m_lastSuccessTime(0)
    , m_lastFailureTime(0)
    , m_batchingEnabled(true)
    , m_maxBatchSize(5)
    , m_batchTimer(nullptr)
    , m_healthMonitoringEnabled(true)
    , m_connectionHealthScore(1.0)
    , m_lastHealthCheck(0)
    , m_healthCheckInterval(DEFAULT_HEALTH_CHECK_INTERVAL)
    , m_healthCheckTimer(nullptr)
    , m_maxReconnectionAttempts(MAX_RECONNECTION_ATTEMPTS)
    , m_healthCheckWindow(HEALTH_CHECK_WINDOW)
    , m_heartbeatEnabled(true)
    , m_heartbeatInterval(DEFAULT_HEARTBEAT_INTERVAL)
    , m_heartbeatTimer(nullptr)
    , m_lastHeartbeatTime(0)
{
    // Initialize statistics
    m_statistics.totalRequests = 0;
    m_statistics.successfulRequests = 0;
    m_statistics.failedRequests = 0;
    m_statistics.isConnected = false;
    
    // Timers will be created in initializeTimers() after moveToThread()
    m_pollTimer = nullptr;
    m_requestTimeoutTimer = nullptr;
}

void ModbusWorker::initializeTimers()
{
    // This method should be called after moveToThread() to create timers in the correct thread
    if (!m_pollTimer) {
        m_pollTimer = new QTimer(this);
        connect(m_pollTimer, &QTimer::timeout, this, &ModbusWorker::onPollTimer);
        qDebug() << "ModbusWorker::initializeTimers - Poll timer created for device:" << m_deviceKey;
    }
    
    if (!m_requestTimeoutTimer) {
        m_requestTimeoutTimer = new QTimer(this);
        m_requestTimeoutTimer->setSingleShot(true);
        connect(m_requestTimeoutTimer, &QTimer::timeout, this, [this]() {
            if (m_requestInProgress) {
                qDebug() << "ModbusWorker::requestTimeout - Request timeout for device:" << m_deviceKey;
                completeCurrentRequest(false, "Request timeout");
                
                // Try to reconnect if connection seems lost
                if (m_statistics.isConnected) {
                    qDebug() << "ModbusWorker::requestTimeout - Attempting reconnection due to timeout";
                    disconnectFromDevice();
                    QTimer::singleShot(1000, this, [this]() {
                        connectToDevice();
                    });
                }
            }
        });
        qDebug() << "ModbusWorker::initializeTimers - Request timeout timer created for device:" << m_deviceKey;
    }
    
    if (!m_batchTimer) {
        m_batchTimer = new QTimer(this);
        m_batchTimer->setSingleShot(true);
        connect(m_batchTimer, &QTimer::timeout, this, &ModbusWorker::onBatchTimeout);
        qDebug() << "ModbusWorker::initializeTimers - Batch timer created for device:" << m_deviceKey;
    }
    
    if (!m_healthCheckTimer) {
        m_healthCheckTimer = new QTimer(this);
        m_healthCheckTimer->setSingleShot(false);
        connect(m_healthCheckTimer, &QTimer::timeout, this, &ModbusWorker::onHealthCheckTimer);
        if (m_healthMonitoringEnabled) {
            m_healthCheckTimer->start(m_healthCheckInterval);
        }
        qDebug() << "ModbusWorker::initializeTimers - Health check timer created for device:" << m_deviceKey;
    }
    
    if (!m_heartbeatTimer) {
        m_heartbeatTimer = new QTimer(this);
        m_heartbeatTimer->setSingleShot(false);
        connect(m_heartbeatTimer, &QTimer::timeout, this, &ModbusWorker::onHeartbeatTimer);
        if (m_heartbeatEnabled) {
            m_heartbeatTimer->start(m_heartbeatInterval);
        }
        qDebug() << "ModbusWorker::initializeTimers - Heartbeat timer created for device:" << m_deviceKey;
    }
}

ModbusWorker::~ModbusWorker()
{
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->stop();
    }
    if (m_requestTimeoutTimer && m_requestTimeoutTimer->isActive()) {
        m_requestTimeoutTimer->stop();
    }
    disconnectFromDevice();
}

void ModbusWorker::connectToDevice()
{
    qDebug() << "ModbusWorker::connectToDevice() - Entry for device:" << m_deviceKey;
    
    if (m_statistics.isConnected) {
        qDebug() << "ModbusWorker::connectToDevice() - Already connected, returning";
        return;
    }
    
    if (!m_modbusManager) {
        qDebug() << "ModbusWorker::connectToDevice() - ERROR: m_modbusManager is null!";
        emitClassifiedError("ModbusManager not available");
        return;
    }
    
    // Verify client is initialized before connection attempt
    if (!m_modbusManager->isClientInitialized()) {
        qCritical() << "ModbusWorker::connectToDevice() - ModbusManager client not initialized for device:" << m_deviceKey;
        emitClassifiedError("Modbus client not initialized");
        return;
    }
    
    // Connection coordination: Acquire semaphore to limit simultaneous connections
    if (!s_connectionSemaphore.tryAcquire(1, 5000)) { // Wait up to 5 seconds
        qWarning() << "ModbusWorker::connectToDevice() - Connection semaphore timeout for device:" << m_deviceKey;
        emitClassifiedError("Connection coordination timeout - too many simultaneous connections");
        return;
    }
    
    qDebug() << "ModbusWorker::connectToDevice() - Acquired connection semaphore for device:" << m_deviceKey;
    qDebug() << "ModbusWorker::connectToDevice() - m_modbusManager pointer:" << (void*)m_modbusManager;
    qDebug() << "ModbusWorker::connectToDevice() - About to call connectToServer with host:" << m_host << "port:" << m_port;
    
    // Attempt connection with proper error handling
    bool connectionResult = m_modbusManager->connectToServer(m_host, m_port);
    if (!connectionResult) {
        qDebug() << "ModbusWorker::connectToDevice() - Initial connection attempt failed for device:" << m_deviceKey;
        handleConnectionFailure("Failed to initiate connection");
        return;
    }
    
    // Set connection timeout (20 seconds for industrial environments)
    int connectionTimeout = 20000; // Default 20 seconds
    if (m_modbusManager && m_modbusManager->getConnectionTimeout() > 0) {
        connectionTimeout = m_modbusManager->getConnectionTimeout();
    }
    
    QTimer::singleShot(connectionTimeout, this, [this]() {
        if (!m_statistics.isConnected && m_workerRunning) {
            qDebug() << "ModbusWorker::connectToDevice() - Connection timeout for device:" << m_deviceKey;
            handleConnectionFailure("Connection timeout");
        }
    });
    
    qDebug() << "ModbusWorker::connectToDevice() - Connection initiated for device:" << m_deviceKey;
    // Connection state will be updated via onModbusConnectionStateChanged callback
}

void ModbusWorker::disconnectFromDevice()
{
    if (!m_statistics.isConnected) {
        return;
    }
    
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->stop();
    }
    
    m_modbusManager->disconnectFromServer();
    m_statistics.isConnected = false;
    
    // Clear pending requests
    m_requestQueue.clear();
    
    emit connectionStateChanged(m_deviceKey, false);
}

void ModbusWorker::setPollInterval(int intervalMs)
{
    m_pollInterval = intervalMs;
    m_basePollInterval = intervalMs;
    m_adaptivePollInterval = intervalMs;
    
    // Update the timer if it's running
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->setInterval(m_adaptivePollInterval);
    }
    
    qDebug() << "ModbusWorker::setPollInterval() - Set base and adaptive poll interval to" << intervalMs << "ms for device:" << m_deviceKey;
}

int ModbusWorker::getPollInterval() const
{
    return m_adaptivePollInterval;
}

void ModbusWorker::setBatchingEnabled(bool enabled)
{
    QMutexLocker locker(&m_queueMutex);
    m_batchingEnabled = enabled;
    qDebug() << "ModbusWorker::setBatchingEnabled() - Batching" << (enabled ? "enabled" : "disabled") << "for device:" << m_deviceKey;
    
    // If disabling batching and batch timer is active, stop it and process any queued batches
    if (!enabled && m_batchTimer && m_batchTimer->isActive()) {
        m_batchTimer->stop();
        if (!m_batchQueue.isEmpty()) {
            processBatchQueue();
        }
    }
}

bool ModbusWorker::isBatchingEnabled() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_batchingEnabled;
}

void ModbusWorker::setMaxBatchSize(int maxSize)
{
    QMutexLocker locker(&m_queueMutex);
    if (maxSize > 0) {
        m_maxBatchSize = maxSize;
        qDebug() << "ModbusWorker::setMaxBatchSize() - Max batch size set to" << maxSize << "for device:" << m_deviceKey;
        
        // If current batch exceeds new max size, process it immediately
        if (m_batchQueue.size() >= m_maxBatchSize) {
            if (m_batchTimer && m_batchTimer->isActive()) {
                m_batchTimer->stop();
            }
            processBatchQueue();
        }
    }
}

int ModbusWorker::getMaxBatchSize() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_maxBatchSize;
}

void ModbusWorker::setHealthMonitoringEnabled(bool enabled)
{
    QMutexLocker locker(&m_queueMutex);
    if (m_healthMonitoringEnabled != enabled) {
        m_healthMonitoringEnabled = enabled;
        qDebug() << "ModbusWorker::setHealthMonitoringEnabled() - Health monitoring" 
                 << (enabled ? "enabled" : "disabled") << "for device:" << m_deviceKey;
        
        if (m_healthCheckTimer) {
            if (enabled) {
                m_healthCheckTimer->start(m_healthCheckInterval);
            } else {
                m_healthCheckTimer->stop();
            }
        }
    }
}

bool ModbusWorker::isHealthMonitoringEnabled() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_healthMonitoringEnabled;
}

void ModbusWorker::setHealthCheckInterval(int intervalMs)
{
    QMutexLocker locker(&m_queueMutex);
    if (intervalMs > 0 && m_healthCheckInterval != intervalMs) {
        m_healthCheckInterval = intervalMs;
        qDebug() << "ModbusWorker::setHealthCheckInterval() - Health check interval set to" 
                 << intervalMs << "ms for device:" << m_deviceKey;
        
        if (m_healthCheckTimer && m_healthMonitoringEnabled) {
            m_healthCheckTimer->setInterval(intervalMs);
        }
    }
}

int ModbusWorker::getHealthCheckInterval() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_healthCheckInterval;
}

double ModbusWorker::getConnectionHealthScore() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_connectionHealthScore;
}

void ModbusWorker::setHeartbeatEnabled(bool enabled)
{
    QMutexLocker locker(&m_queueMutex);
    m_heartbeatEnabled = enabled;
    
    if (m_heartbeatTimer) {
        if (enabled && m_workerRunning && !m_stopRequested) {
            if (!m_heartbeatTimer->isActive()) {
                m_heartbeatTimer->start(m_heartbeatInterval);
                qDebug() << "ModbusWorker::setHeartbeatEnabled - Heartbeat timer started for device:" << m_deviceKey;
            }
        } else {
            if (m_heartbeatTimer->isActive()) {
                m_heartbeatTimer->stop();
                qDebug() << "ModbusWorker::setHeartbeatEnabled - Heartbeat timer stopped for device:" << m_deviceKey;
            }
        }
    }
}

bool ModbusWorker::isHeartbeatEnabled() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_heartbeatEnabled;
}

void ModbusWorker::setHeartbeatInterval(int intervalMs)
{
    QMutexLocker locker(&m_queueMutex);
    if (intervalMs > 0) {
        m_heartbeatInterval = intervalMs;
        if (m_heartbeatTimer && m_heartbeatEnabled) {
            m_heartbeatTimer->setInterval(intervalMs);
        }
    }
}

int ModbusWorker::getHeartbeatInterval() const
{
    QMutexLocker locker(&m_queueMutex);
    return m_heartbeatInterval;
}

void ModbusWorker::setPollingEnabled(bool enabled)
{
    qDebug() << "ModbusWorker::setPollingEnabled() - Setting polling to" << enabled << "for device:" << m_deviceKey
             << "(workerRunning:" << m_workerRunning << ", stopRequested:" << m_stopRequested << ")";
    
    m_pollingEnabled = enabled;
    
    if (m_pollTimer) {
        if (enabled && m_workerRunning && !m_stopRequested) {
            // Start polling if worker is running and not stopping - connection state will be checked in onPollTimer()
            if (!m_pollTimer->isActive()) {
                qDebug() << "ModbusWorker::setPollingEnabled() - Starting poll timer for device:" << m_deviceKey;
                m_pollTimer->start(m_pollInterval);
            }
        } else {
            // Stop polling
            if (m_pollTimer->isActive()) {
                qDebug() << "ModbusWorker::setPollingEnabled() - Stopping poll timer for device:" << m_deviceKey;
                m_pollTimer->stop();
            }
        }
    } else {
        qDebug() << "ModbusWorker::setPollingEnabled() - Poll timer not initialized for device:" << m_deviceKey;
    }
}

// Data point management methods removed - not declared in header

// submitRequest and submitRead methods removed - not declared in header

// submitWrite and submitWriteCoils methods removed - not declared in header

ModbusWorker::WorkerStatistics ModbusWorker::getStatistics() const
{
    QMutexLocker locker(&m_statsMutex);
    return m_statistics;
}

void ModbusWorker::insertRequestByPriority(const PriorityModbusRequest& request)
{
    // Find insertion point based on priority
    int insertIndex = 0;
    
    for (int i = 0; i < m_requestQueue.size(); ++i) {
        if (static_cast<int>(request.priority) > static_cast<int>(m_requestQueue[i].priority)) {
            insertIndex = i;
            break;
        }
        insertIndex = i + 1;
    }
    
    m_requestQueue.insert(insertIndex, request);
}

// processNextRequest method removed - functionality moved to processRequestQueue

// executeModbusRequest and processPollCycle methods removed - functionality moved to executeRequest

void ModbusWorker::onModbusReadCompleted(const ModbusReadResult& result)
{
    if (!m_requestInProgress) {
        return; // Not our request
    }
    
    // Check if this is a heartbeat request (low priority, single register read at address 0)
    bool isHeartbeat = (m_currentRequest.priority == RequestPriority::Low &&
                       m_currentRequest.request.type == ModbusRequest::ReadHoldingRegisters &&
                       m_currentRequest.request.startAddress == 0 &&
                       m_currentRequest.request.count == 1);
    
    m_statistics.totalRequests++;
    if (result.success) {
        m_statistics.successfulRequests++;
        
        if (isHeartbeat) {
            // Handle heartbeat response
            handleHeartbeatResponse(true);
        } else {
            // Normal read request
            emit readCompleted(m_currentRequest.requestId, result);
        }
        
        completeCurrentRequest(true);
    } else {
        m_statistics.failedRequests++;
        
        if (isHeartbeat) {
            // Handle heartbeat failure
            handleHeartbeatResponse(false);
        } else {
            // Normal read request failure
            emit errorOccurred(m_deviceKey, result.errorString);
        }
        
        completeCurrentRequest(false, result.errorString);
    }
}

void ModbusWorker::onModbusWriteCompleted(const ModbusWriteResult& result)
{
    if (!m_requestInProgress) {
        return; // Not our request
    }
    
    m_statistics.totalRequests++;
    if (result.success) {
        m_statistics.successfulRequests++;
        emit writeCompleted(m_currentRequest.requestId, result);
        completeCurrentRequest(true);
    } else {
        m_statistics.failedRequests++;
        emit errorOccurred(m_deviceKey, result.errorString);
        completeCurrentRequest(false, result.errorString);
    }
}

void ModbusWorker::onModbusConnectionStateChanged(bool connected)
{
    qDebug() << "ModbusWorker::onModbusConnectionStateChanged() - Device:" << m_deviceKey << "Connected:" << connected;
    
    m_statistics.isConnected = connected;
    
    if (!connected) {
        // Clear queue on disconnection but maintain polling for reconnection attempts
        QMutexLocker locker(&m_queueMutex);
        m_requestQueue.clear();
        qDebug() << "ModbusWorker - Device disconnected, clearing request queue but maintaining polling for:" << m_deviceKey;
        // Polling timer continues running to enable automatic reconnection attempts
        
        // Release connection semaphore on disconnection
        s_connectionSemaphore.release();
        qDebug() << "ModbusWorker - Released connection semaphore for disconnected device:" << m_deviceKey;
    } else {
        // Success-based reset: Reset backoff strategy on successful connection
        m_connectionAttempts = 0;
        m_reconnectionDelay = 1000; // Reset to initial delay
        
        qDebug() << "ModbusWorker - Device connected, polling will resume normal operation for:" << m_deviceKey;
        qDebug() << "ModbusWorker - Connection successful, reset backoff strategy for:" << m_deviceKey;
        // No need to manually manage polling timer here - setPollingEnabled() handles it properly
        // Connection established, normal polling operations will resume automatically
        
        // Release connection semaphore on successful connection
        s_connectionSemaphore.release();
        qDebug() << "ModbusWorker - Released connection semaphore for connected device:" << m_deviceKey;
    }
    
    emit connectionStateChanged(m_deviceKey, connected);
}

void ModbusWorker::onModbusError(const QString& error)
{
    emitClassifiedError(error);
}

void ModbusWorker::completeCurrentRequest(bool success, const QString &error)
{
    Q_UNUSED(error); // Suppress unused parameter warning
    
    // Stop request timeout timer
    if (m_requestTimeoutTimer && m_requestTimeoutTimer->isActive()) {
        m_requestTimeoutTimer->stop();
    }
    
    m_currentRequest = PriorityModbusRequest();
    m_requestInProgress = false;
    
    // Update statistics
    updateStatistics(success);
    
    // Update connection health monitoring
    updateConnectionHealth(success);
    
    // Adjust adaptive polling interval based on request success/failure
    adjustAdaptivePollInterval(success);
    
    // Process next request if available
    QTimer::singleShot(0, this, &ModbusWorker::processRequestQueue);
}

void ModbusWorker::updateStatistics(bool success, qint64 responseTime)
{
    QMutexLocker locker(&m_statsMutex);
    
    if (success) {
        m_statistics.successfulRequests++;
    } else {
        m_statistics.failedRequests++;
    }
    
    if (responseTime > 0) {
        // Update average response time
        qint64 totalRequests = m_statistics.successfulRequests + m_statistics.failedRequests;
        if (totalRequests > 0) {
            m_statistics.averageResponseTime = 
                (m_statistics.averageResponseTime * (totalRequests - 1) + responseTime) / totalRequests;
        }
    }
    
    m_statistics.lastActivityTime = QDateTime::currentMSecsSinceEpoch();
    
    // Emit statistics update periodically
    emitStatisticsUpdate();
}

void ModbusWorker::emitStatisticsUpdate()
{
    emit statisticsUpdated(m_deviceKey, m_statistics);
}

void ModbusWorker::processRequestQueue()
{
    QMutexLocker locker(&m_queueMutex);
    
    if (m_requestInProgress || m_requestQueue.isEmpty()) {
        return;
    }
    
    // Check if batching is enabled and we have multiple requests
    if (m_batchingEnabled && m_requestQueue.size() > 1) {
        // Get next request by priority
        PriorityModbusRequest nextRequest = getNextRequest();
        if (nextRequest.requestId == 0) {
            return; // No valid request
        }
        
        // Add to batch queue and start batch timer if not already running
        m_batchQueue.enqueue(nextRequest);
        
        if (m_batchTimer && !m_batchTimer->isActive()) {
            m_batchTimer->start(BATCH_TIMEOUT_MS);
        }
        
        // If batch is full, process immediately
        if (m_batchQueue.size() >= m_maxBatchSize) {
            if (m_batchTimer && m_batchTimer->isActive()) {
                m_batchTimer->stop();
            }
            processBatchQueue();
        }
        
        return;
    }
    
    // Standard single request processing
    PriorityModbusRequest nextRequest = getNextRequest();
    if (nextRequest.requestId == 0) {
        return; // No valid request
    }
    
    m_currentRequest = nextRequest;
    m_requestInProgress = true;
    
    // Execute the request
    executeRequest(nextRequest);
}

void ModbusWorker::onPollTimer()
{
    // Enhanced polling implementation with complete state validation
    if (!m_workerRunning || m_stopRequested || !m_pollingEnabled) {
        qDebug() << "ModbusWorker::onPollTimer() - Skipping poll for device:" << m_deviceKey
                 << "(workerRunning:" << m_workerRunning << ", stopRequested:" << m_stopRequested
                 << ", pollingEnabled:" << m_pollingEnabled << ")";
        return;
    }
    
    // Verify ModbusManager and client initialization before any operations
    if (!m_modbusManager) {
        qWarning() << "ModbusWorker::onPollTimer() - ModbusManager is null for device:" << m_deviceKey;
        return;
    }
    
    // Verify client is initialized before attempting any operations
    if (!m_modbusManager->isClientInitialized()) {
        qCritical() << "ModbusWorker::onPollTimer() - ModbusManager client not initialized for device:" << m_deviceKey;
        // Try to reinitialize the client asynchronously
        m_modbusManager->initializeClient();
        
        // Use non-blocking approach - schedule retry after brief delay
        QTimer::singleShot(50, this, [this]() {
            if (!m_modbusManager || !m_modbusManager->isClientInitialized()) {
                qCritical() << "ModbusWorker::onPollTimer() - Failed to reinitialize client for device:" << m_deviceKey;
                emit errorOccurred(m_deviceKey, "Modbus client initialization failed");
            }
        });
        return; // Skip this poll cycle, let timer retry
    }
    
    // Check connection state and attempt reconnection if needed with enhanced resilience
    if (!isConnected()) {
        qDebug() << "ModbusWorker::onPollTimer() - Device not connected, attempting reconnection for:" << m_deviceKey;
        
        // Implement exponential backoff for reconnection attempts
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - m_lastConnectionAttempt < m_reconnectionDelay) {
            return; // Wait before next attempt
        }
        
        m_lastConnectionAttempt = currentTime;
        connectToDevice();
        
        // Exponential backoff: increase delay up to maximum
        m_reconnectionDelay = qMin(m_reconnectionDelay * 2, 30000); // Max 30 seconds
        qDebug() << "ModbusWorker::onPollTimer() - Next reconnection attempt in" << m_reconnectionDelay << "ms";
        return;
    }
    
    // Reset reconnection delay when connected
    if (isConnected()) {
        m_reconnectionDelay = 1000;
    }
    
    // Process any queued requests only when properly connected
    // This ensures continuous operation and allows for connection recovery
    processRequestQueue();
}

PriorityModbusRequest ModbusWorker::getNextRequest()
{
    if (m_requestQueue.isEmpty()) {
        return PriorityModbusRequest();
    }
    
    return m_requestQueue.dequeue();
}

void ModbusWorker::executeRequest(const PriorityModbusRequest &request)
{
    if (!m_modbusManager) {
        completeCurrentRequest(false, "ModbusManager not available");
        return;
    }
    
    // Start request timeout timer with configurable timeout
    if (m_requestTimeoutTimer) {
        int requestTimeout = 15000; // Default 15s for industrial environments
        if (m_modbusManager) {
            requestTimeout = m_modbusManager->getRequestTimeout();
        }
        m_requestTimeoutTimer->start(requestTimeout);
    }
    
    const ModbusRequest &req = request.request;
    
    switch (req.type) {
        case ModbusRequest::ReadHoldingRegisters:
            if (req.count == 1) {
                m_modbusManager->readHoldingRegister(req.startAddress, req.dataType, req.unitId);
            } else {
                m_modbusManager->readHoldingRegisters(req.startAddress, req.count, req.dataType, req.unitId);
            }
            break;
            
        case ModbusRequest::ReadInputRegisters:
            if (req.count == 1) {
                m_modbusManager->readInputRegister(req.startAddress, req.dataType, req.unitId);
            } else {
                m_modbusManager->readInputRegisters(req.startAddress, req.count, req.dataType, req.unitId);
            }
            break;
            
        case ModbusRequest::ReadCoils:
            if (req.count == 1) {
                m_modbusManager->readCoil(req.startAddress, req.unitId);
            } else {
                m_modbusManager->readCoils(req.startAddress, req.count, req.unitId);
            }
            break;
            
        case ModbusRequest::ReadDiscreteInputs:
            if (req.count == 1) {
                m_modbusManager->readDiscreteInput(req.startAddress, req.unitId);
            } else {
                m_modbusManager->readDiscreteInputs(req.startAddress, req.count, req.unitId);
            }
            break;
            
        case ModbusRequest::WriteHoldingRegisters:
            if (req.count == 1 && !req.writeData.isEmpty()) {
                m_modbusManager->writeHoldingRegister(req.startAddress, req.writeData.first(), req.unitId);
            } else {
                m_modbusManager->writeHoldingRegisters(req.startAddress, req.writeData, req.unitId);
            }
            break;
            
        case ModbusRequest::WriteCoils:
            if (req.count == 1 && !req.writeBoolData.isEmpty()) {
                m_modbusManager->writeCoil(req.startAddress, req.writeBoolData.first(), req.unitId);
            } else {
                m_modbusManager->writeCoils(req.startAddress, req.writeBoolData, req.unitId);
            }
            break;
            
        default:
            completeCurrentRequest(false, "Unknown request type");
            return;
    }
}

QString ModbusWorker::getDeviceKey() const
{
    return m_deviceKey;
}

bool ModbusWorker::isConnected() const
{
    return m_statistics.isConnected;
}

void ModbusWorker::resetStatistics()
{
    QMutexLocker locker(&m_statsMutex);
    m_statistics = WorkerStatistics();
}

qint64 ModbusWorker::queueReadRequest(const ModbusRequest &request, RequestPriority priority)
{
    QMutexLocker locker(&m_queueMutex);
    
    qint64 requestId = m_nextRequestId.fetchAndAddOrdered(1);
    
    PriorityModbusRequest priorityRequest;
    priorityRequest.request = request;
    priorityRequest.priority = priority;
    priorityRequest.requestId = requestId;
    priorityRequest.queueTime = QDateTime::currentMSecsSinceEpoch();
    priorityRequest.interruptible = false; // Read requests are not interruptible
    
    insertRequestByPriority(priorityRequest);
    
    // Process queue if not currently processing
    if (!m_requestInProgress) {
        QMetaObject::invokeMethod(this, "processRequestQueue", Qt::QueuedConnection);
    }
    
    return requestId;
}

qint64 ModbusWorker::queueWriteRequest(const ModbusRequest &request, RequestPriority priority, bool interruptible)
{
    QMutexLocker locker(&m_queueMutex);
    
    qint64 requestId = m_nextRequestId.fetchAndAddOrdered(1);
    
    PriorityModbusRequest priorityRequest;
    priorityRequest.request = request;
    priorityRequest.priority = priority;
    priorityRequest.requestId = requestId;
    priorityRequest.queueTime = QDateTime::currentMSecsSinceEpoch();
    priorityRequest.interruptible = interruptible;
    
    insertRequestByPriority(priorityRequest);
    
    // Process queue if not currently processing
    if (!m_requestInProgress) {
        QMetaObject::invokeMethod(this, "processRequestQueue", Qt::QueuedConnection);
    }
    
    return requestId;
}

void ModbusWorker::startWorker()
{
    qDebug() << "ModbusWorker::startWorker() - Starting worker for device:" << m_deviceKey;
    
    if (m_workerRunning) {
        qWarning() << "ModbusWorker::startWorker() - Worker already running for device:" << m_deviceKey;
        return;
    }
    
    m_workerRunning = true;
    m_stopRequested = false;
    
    // Create ModbusManager in the worker thread
    if (!m_modbusManager) {
        m_modbusManager = new ModbusManager(this);
        
        // Initialize the ModbusManager's Qt objects in the worker thread synchronously
        // This ensures the client is ready before any connection attempts
        m_modbusManager->initializeClient();
        
        // Add a small delay to ensure initialization is complete
        QThread::msleep(100);
        
        // Verify initialization was successful
        if (!m_modbusManager->isClientInitialized()) {
            qCritical() << "ModbusWorker::startWorker() - Failed to initialize ModbusManager client for device:" << m_deviceKey;
            emitClassifiedError("Failed to initialize Modbus client");
            return;
        }
        
        qDebug() << "ModbusWorker::startWorker() - ModbusManager initialized successfully for device:" << m_deviceKey;
        
        // Connect ModbusManager signals
        connect(m_modbusManager, &ModbusManager::readCompleted,
                this, &ModbusWorker::onModbusReadCompleted);
        connect(m_modbusManager, &ModbusManager::writeCompleted,
                this, &ModbusWorker::onModbusWriteCompleted);
        connect(m_modbusManager, &ModbusManager::connectionStateChanged,
                this, &ModbusWorker::onModbusConnectionStateChanged);
        connect(m_modbusManager, &ModbusManager::errorOccurred,
                this, &ModbusWorker::onModbusError);
    }
    
    // Connect to device
    connectToDevice();
    
    // Enable polling immediately during startup to ensure reliability
    m_pollingEnabled = true;
    
    // Start polling timer unconditionally - connection state will be checked during poll execution
    if (m_pollTimer) {
        qDebug() << "ModbusWorker::startWorker() - Starting poll timer with adaptive interval" << m_adaptivePollInterval << "ms for device:" << m_deviceKey;
        m_pollTimer->start(m_adaptivePollInterval);
    }
    
    emit workerStarted(m_deviceKey);
    qDebug() << "ModbusWorker::startWorker() - Worker started successfully for device:" << m_deviceKey;
}

void ModbusWorker::stopWorker()
{
    if (!m_workerRunning) {
        return;
    }
    
    qDebug() << "ModbusWorker::stopWorker() - Stopping worker for device:" << m_deviceKey;
    
    m_stopRequested = true;
    m_workerRunning = false;
    
    // Disable polling to ensure consistent state
    m_pollingEnabled = false;
    
    // Stop polling timer
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->stop();
        qDebug() << "ModbusWorker::stopWorker() - Stopped polling timer for device:" << m_deviceKey;
    }
    
    // Stop request timeout timer
    if (m_requestTimeoutTimer && m_requestTimeoutTimer->isActive()) {
        m_requestTimeoutTimer->stop();
        qDebug() << "ModbusWorker::stopWorker() - Stopped request timeout timer for device:" << m_deviceKey;
    }
    
    // Clear request queue and interrupt current request if any
    if (m_requestInProgress) {
        interruptCurrentRequest("Worker stopping");
    }
    clearRequestQueue();
    
    // Disconnect from device
    disconnectFromDevice();
    
    qDebug() << "ModbusWorker::stopWorker() - Worker stopped for device:" << m_deviceKey;
    emit workerStopped(m_deviceKey);
}

void ModbusWorker::handleConnectionFailure(const QString& errorMessage)
{
    // Emit classified error for immediate notification
    emitClassifiedError(errorMessage);
    
    // Update connection attempts and use health monitoring for reconnection logic
    m_connectionAttempts++;
    
    qDebug() << "ModbusWorker::handleConnectionFailure() - Connection attempt" << m_connectionAttempts
             << "failed for device:" << m_deviceKey << "- Error:" << errorMessage;
    
    // Release connection semaphore immediately on any connection failure
    s_connectionSemaphore.release();
    qDebug() << "ModbusWorker - Released connection semaphore after connection failure for device:" << m_deviceKey;
    
    // Use health monitoring to determine reconnection strategy
    if (m_healthMonitoringEnabled && shouldAttemptReconnection()) {
        qDebug() << "ModbusWorker::handleConnectionFailure() - Health monitoring allows reconnection"
                 << "- Health score:" << m_connectionHealthScore
                 << "- Scheduling reconnection attempt in" << m_reconnectionDelay << "ms for device:" << m_deviceKey;
        
        // Schedule reconnection attempt if worker is still running
        if (m_workerRunning && !m_stopRequested) {
            QTimer::singleShot(m_reconnectionDelay, this, [this]() {
                if (m_workerRunning && !m_stopRequested && m_modbusManager && !m_modbusManager->isConnected()) {
                    qDebug() << "ModbusWorker::handleConnectionFailure() - Executing scheduled reconnection attempt" << m_connectionAttempts + 1 << "for device:" << m_deviceKey;
                    connectToDevice();
                }
            });
        }
    } else {
        // Progressive backoff strategy: 5s → 60s with exponential growth
        const int maxAttempts = 10;
        const int minDelay = 5000;  // Start at 5 seconds
        const int maxDelay = 60000; // Cap at 60 seconds
        
        if (m_connectionAttempts >= maxAttempts) {
            qCritical() << "ModbusWorker::handleConnectionFailure() - Maximum connection attempts reached for device:" << m_deviceKey;
            emitClassifiedError(QString("Maximum connection attempts (%1) reached").arg(maxAttempts));
            return;
        }
        
        // Progressive backoff: 5s, 10s, 20s, 40s, 60s (capped)
        // Formula: min(minDelay * 2^(attempt-1), maxDelay)
        int progressiveDelay = minDelay * (1 << qMin(m_connectionAttempts - 1, 4)); // Cap exponential at 2^4
        m_reconnectionDelay = qMin(progressiveDelay, maxDelay);
        
        qDebug() << "ModbusWorker::handleConnectionFailure() - Using fallback exponential backoff"
                 << "- Scheduling reconnection attempt in" << m_reconnectionDelay << "ms for device:" << m_deviceKey;
        
        if (m_workerRunning && !m_stopRequested) {
            QTimer::singleShot(m_reconnectionDelay, this, [this]() {
                if (m_workerRunning && !m_stopRequested && m_modbusManager && !m_modbusManager->isConnected()) {
                    qDebug() << "ModbusWorker::handleConnectionFailure() - Executing scheduled reconnection attempt" << m_connectionAttempts + 1 << "for device:" << m_deviceKey;
                    connectToDevice();
                }
            });
        }
    }
}

void ModbusWorker::interruptCurrentRequest(const QString &reason)
{
    if (!m_requestInProgress) {
        return;
    }
    
    // Only interrupt if the current request is interruptible
    if (m_currentRequest.interruptible) {
        // Cancel the current request
        completeCurrentRequest(false, reason.isEmpty() ? "Request interrupted" : reason);
        
        // Emit interruption signal
        emit requestInterrupted(m_currentRequest.requestId, reason);
        
        // Process next request in queue
        QMetaObject::invokeMethod(this, "processRequestQueue", Qt::QueuedConnection);
    }
}

void ModbusWorker::clearRequestQueue()
{
    QMutexLocker locker(&m_queueMutex);
    
    // Emit interruption signals for all queued requests
    while (!m_requestQueue.isEmpty()) {
        PriorityModbusRequest request = m_requestQueue.dequeue();
        emit requestInterrupted(request.requestId, "Queue cleared");
    }
}

bool ModbusWorker::hasHigherPriorityRequest(RequestPriority currentPriority) const
{
    QMutexLocker locker(&m_queueMutex);
    
    for (const auto &request : m_requestQueue) {
        if (request.priority > currentPriority) {
            return true;
        }
    }
    
    return false;
}

void ModbusWorker::generateAutomaticPollingRequests()
{
    // This method can be extended to automatically generate polling requests
    // based on configured data points for this specific device
    // For now, it serves as a placeholder for future enhancement
    
    // Example implementation would:
    // 1. Check if there are configured data points for this device
    // 2. Generate read requests based on poll intervals
    // 3. Queue the requests with appropriate priority
    
    // This functionality is currently handled by ScadaCoreService
    // but could be moved here for better encapsulation
}

void ModbusWorker::adjustAdaptivePollInterval(bool success)
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    if (success) {
        m_consecutiveSuccesses++;
        m_consecutiveFailures = 0;
        m_lastSuccessTime = currentTime;
        
        // Gradually decrease polling interval on consecutive successes (more frequent polling)
        if (m_consecutiveSuccesses >= 5 && m_adaptivePollInterval > MIN_POLL_INTERVAL) {
            m_adaptivePollInterval = qMax(MIN_POLL_INTERVAL, 
                                        static_cast<int>(m_adaptivePollInterval * 0.9));
            qDebug() << "ModbusWorker::adjustAdaptivePollInterval - Decreased interval to" 
                     << m_adaptivePollInterval << "ms for device:" << m_deviceKey;
        }
    } else {
        m_consecutiveFailures++;
        m_consecutiveSuccesses = 0;
        m_lastFailureTime = currentTime;
        
        // Increase polling interval on failures (less frequent polling)
        if (m_consecutiveFailures >= 3 && m_adaptivePollInterval < MAX_POLL_INTERVAL) {
            m_adaptivePollInterval = qMin(MAX_POLL_INTERVAL, 
                                        static_cast<int>(m_adaptivePollInterval * 1.5));
            qDebug() << "ModbusWorker::adjustAdaptivePollInterval - Increased interval to" 
                     << m_adaptivePollInterval << "ms for device:" << m_deviceKey;
        }
    }
    
    // Update the poll timer with new interval if it has changed
    if (m_pollTimer && m_pollTimer->interval() != m_adaptivePollInterval) {
        m_pollTimer->setInterval(m_adaptivePollInterval);
    }
}

void ModbusWorker::onBatchTimeout()
{
    // Process any pending batched requests when timeout occurs
    if (!m_batchQueue.isEmpty()) {
        processBatchQueue();
    }
}

void ModbusWorker::processBatchQueue()
{
    if (m_batchQueue.isEmpty()) {
        return;
    }
    
    QList<PriorityModbusRequest> batch;
    
    // Collect requests for batching (up to maxBatchSize)
    while (!m_batchQueue.isEmpty() && batch.size() < m_maxBatchSize) {
        PriorityModbusRequest request = m_batchQueue.dequeue();
        
        // Check if this request can be batched with existing ones
        bool canBatch = batch.isEmpty();
        if (!canBatch && !batch.isEmpty()) {
            canBatch = canBatchRequests(batch.first(), request);
        }
        
        if (canBatch) {
            batch.append(request);
        } else {
            // Can't batch this request, put it back and process current batch
            m_batchQueue.prepend(request);
            break;
        }
    }
    
    if (!batch.isEmpty()) {
        executeBatchedRequests(batch);
    }
    
    // If there are still requests in batch queue, schedule another processing
    if (!m_batchQueue.isEmpty()) {
        QTimer::singleShot(10, this, &ModbusWorker::processBatchQueue);
    }
}

void ModbusWorker::executeBatchedRequests(const QList<PriorityModbusRequest> &batch)
{
    if (batch.isEmpty()) {
        return;
    }
    
    qDebug() << "ModbusWorker::executeBatchedRequests - Processing batch of" << batch.size() << "requests for device:" << m_deviceKey;
    
    // For now, execute requests sequentially
    // Future optimization: combine consecutive register reads into single requests
    for (const auto &request : batch) {
        executeRequest(request);
    }
}

bool ModbusWorker::canBatchRequests(const PriorityModbusRequest &req1, const PriorityModbusRequest &req2) const
{
    // Only batch requests of the same type and similar priority
    if (req1.request.type != req2.request.type) {
        return false;
    }
    
    // Don't batch high priority requests with lower priority ones
    if (req1.priority == RequestPriority::High || req2.priority == RequestPriority::High) {
        return req1.priority == req2.priority;
    }
    
    // For read requests, check if they are for consecutive registers
    if (req1.request.type == ModbusRequest::ReadHoldingRegisters ||
        req1.request.type == ModbusRequest::ReadInputRegisters) {
        
        int addr1End = req1.request.startAddress + req1.request.count;
        int addr2Start = req2.request.startAddress;
        
        // Allow batching if registers are consecutive or overlapping
        return (addr2Start <= addr1End + 5);  // Allow small gaps
    }
    
    return true;  // Allow batching for other request types
}

void ModbusWorker::onHealthCheckTimer()
{
    if (m_healthMonitoringEnabled) {
        performHealthCheck();
    }
}

void ModbusWorker::updateConnectionHealth(bool success)
{
    if (!m_healthMonitoringEnabled) {
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Calculate health score based on recent success/failure ratio
    // Use exponential moving average for smooth transitions
    double alpha = 0.1; // Smoothing factor
    double newScore = success ? 1.0 : 0.0;
    m_connectionHealthScore = alpha * newScore + (1.0 - alpha) * m_connectionHealthScore;
    
    // Clamp to valid range
    m_connectionHealthScore = qMax(0.0, qMin(1.0, m_connectionHealthScore));
    
    qDebug() << "ModbusWorker::updateConnectionHealth - Health score updated to" << m_connectionHealthScore 
             << "for device:" << m_deviceKey << "(success:" << success << ")";
}

void ModbusWorker::performHealthCheck()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    m_lastHealthCheck = currentTime;
    
    // Check if connection health is poor and we should back off reconnection attempts
    if (m_connectionHealthScore < 0.3 && m_connectionAttempts >= m_maxReconnectionAttempts) {
        qDebug() << "ModbusWorker::performHealthCheck - Poor connection health (" << m_connectionHealthScore 
                 << "), backing off reconnection attempts for device:" << m_deviceKey;
        
        // Increase reconnection delay exponentially
        m_reconnectionDelay = qMin(m_reconnectionDelay * 2, 60000); // Max 1 minute
        
        // Reset connection attempts after backoff period
        if (currentTime - m_lastConnectionAttempt > m_reconnectionDelay) {
            m_connectionAttempts = 0;
            qDebug() << "ModbusWorker::performHealthCheck - Resetting connection attempts after backoff for device:" << m_deviceKey;
        }
    }
    
    // If health is good, reduce reconnection delay
    if (m_connectionHealthScore > 0.7) {
        m_reconnectionDelay = qMax(m_reconnectionDelay / 2, 1000); // Min 1 second
    }
    
    qDebug() << "ModbusWorker::performHealthCheck - Health check completed for device:" << m_deviceKey 
             << "(score:" << m_connectionHealthScore << ", delay:" << m_reconnectionDelay << "ms)";
}

bool ModbusWorker::shouldAttemptReconnection() const
{
    if (!m_healthMonitoringEnabled) {
        return true; // Always attempt if monitoring is disabled
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Don't attempt if we've exceeded max attempts and health is poor
    if (m_connectionAttempts >= m_maxReconnectionAttempts && m_connectionHealthScore < 0.5) {
        // Check if enough time has passed for backoff
        if (currentTime - m_lastConnectionAttempt < m_reconnectionDelay) {
            return false;
        }
    }
    
    return true;
}

void ModbusWorker::resetConnectionHealth()
{
    m_connectionHealthScore = 1.0;
    m_lastHealthCheck = 0;
    m_connectionAttempts = 0;
    m_reconnectionDelay = 1000;
    
    qDebug() << "ModbusWorker::resetConnectionHealth - Health monitoring reset for device:" << m_deviceKey;
}

void ModbusWorker::onRequestTimeout()
{
    qDebug() << "ModbusWorker::onRequestTimeout() - Request timeout for device:" << m_deviceKey;
    
    if (m_requestInProgress) {
        QString timeoutReason = QString("Request timeout after %1ms").arg(m_requestTimeoutTimer ? m_requestTimeoutTimer->interval() : 5000);
        completeCurrentRequest(false, timeoutReason);
        
        // Update health monitoring for timeout
        if (m_healthMonitoringEnabled) {
            updateConnectionHealth(false);
        }
        
        // Emit classified error for request timeout
        emitClassifiedError(timeoutReason);
    }
}

ModbusErrorType ModbusWorker::classifyError(const QString &errorMessage) const
{
    QString lowerError = errorMessage.toLower();
    
    // Connection-related errors
    if (lowerError.contains("connection timeout") || lowerError.contains("timeout")) {
        return ModbusErrorType::ConnectionTimeout;
    }
    if (lowerError.contains("connection refused") || lowerError.contains("refused")) {
        return ModbusErrorType::ConnectionRefused;
    }
    if (lowerError.contains("network") || lowerError.contains("host not found") || 
        lowerError.contains("unreachable")) {
        return ModbusErrorType::NetworkError;
    }
    
    // Device overload indicators
    if (lowerError.contains("device busy") || lowerError.contains("busy") ||
        lowerError.contains("resource temporarily unavailable")) {
        return ModbusErrorType::DeviceBusy;
    }
    if (lowerError.contains("overload") || lowerError.contains("too many") ||
        lowerError.contains("queue full") || lowerError.contains("backlog") ||
        lowerError.contains("simultaneous connections")) {
        return ModbusErrorType::DeviceOverload;
    }
    if (lowerError.contains("resource exhausted") || lowerError.contains("out of memory") ||
        lowerError.contains("no resources")) {
        return ModbusErrorType::ResourceExhaustion;
    }
    
    // Protocol and configuration errors
    if (lowerError.contains("protocol") || lowerError.contains("invalid response") ||
        lowerError.contains("malformed")) {
        return ModbusErrorType::ProtocolError;
    }
    if (lowerError.contains("configuration") || lowerError.contains("not initialized") ||
        lowerError.contains("invalid parameter")) {
        return ModbusErrorType::ConfigurationError;
    }
    if (lowerError.contains("request timeout")) {
        return ModbusErrorType::RequestTimeout;
    }
    
    return ModbusErrorType::Unknown;
}

void ModbusWorker::onHeartbeatTimer()
{
    if (m_heartbeatEnabled && isConnected()) {
        sendHeartbeat();
    }
}

void ModbusWorker::sendHeartbeat()
{
    if (!m_modbusManager || !isConnected()) {
        qDebug() << "ModbusWorker::sendHeartbeat - Cannot send heartbeat, not connected for device:" << m_deviceKey;
        return;
    }
    
    // Create a simple read request as heartbeat (read 1 holding register at address 0)
    ModbusRequest heartbeatRequest;
    heartbeatRequest.type = ModbusRequest::ReadHoldingRegisters;
    heartbeatRequest.startAddress = 0;
    heartbeatRequest.count = 1;
    heartbeatRequest.unitId = m_unitId;
    heartbeatRequest.dataType = ModbusDataType::HoldingRegister;
    
    // Queue heartbeat with low priority to not interfere with normal operations
    qint64 heartbeatId = queueReadRequest(heartbeatRequest, RequestPriority::Low);
    m_lastHeartbeatTime = QDateTime::currentMSecsSinceEpoch();
    
    qDebug() << "ModbusWorker::sendHeartbeat - Heartbeat sent for device:" << m_deviceKey 
             << "(request ID:" << heartbeatId << ")";
}

void ModbusWorker::handleHeartbeatResponse(bool success)
{
    if (success) {
        qDebug() << "ModbusWorker::handleHeartbeatResponse - Heartbeat successful for device:" << m_deviceKey;
        // Update connection health positively for successful heartbeat
        updateConnectionHealth(true);
    } else {
        qDebug() << "ModbusWorker::handleHeartbeatResponse - Heartbeat failed for device:" << m_deviceKey;
        // Update connection health negatively for failed heartbeat
        updateConnectionHealth(false);
        
        // Consider reconnection if heartbeat consistently fails
        if (m_connectionHealthScore < 0.3) {
            qDebug() << "ModbusWorker::handleHeartbeatResponse - Poor health score, considering reconnection for device:" << m_deviceKey;
        }
    }
}

void ModbusWorker::emitClassifiedError(const QString &errorMessage)
{
    ModbusErrorType errorType = classifyError(errorMessage);
    
    // Emit both signals for backward compatibility and enhanced handling
    emit errorOccurred(m_deviceKey, errorMessage);
    emit errorOccurredClassified(m_deviceKey, errorMessage, errorType);
    
    // Apply adaptive behavior for device overload scenarios
    if (errorType == ModbusErrorType::DeviceOverload || errorType == ModbusErrorType::DeviceBusy) {
        // Increase polling interval to reduce device load
        int currentInterval = getPollInterval();
        int newInterval = qMin(currentInterval * 2, MAX_POLL_INTERVAL);
        if (newInterval != currentInterval) {
            setPollInterval(newInterval);
            qDebug() << "ModbusWorker::emitClassifiedError() - Device overload detected, increased polling interval from" 
                     << currentInterval << "ms to" << newInterval << "ms for device:" << m_deviceKey;
        }
        
        // Reduce batch size to minimize device load
        if (m_batchingEnabled && m_maxBatchSize > 1) {
            int newBatchSize = qMax(1, m_maxBatchSize / 2);
            setMaxBatchSize(newBatchSize);
            qDebug() << "ModbusWorker::emitClassifiedError() - Device overload detected, reduced batch size to" 
                     << newBatchSize << "for device:" << m_deviceKey;
        }
    }
    
    // Log classified error for debugging
    QString errorTypeName;
    switch (errorType) {
        case ModbusErrorType::ConnectionTimeout: errorTypeName = "ConnectionTimeout"; break;
        case ModbusErrorType::ConnectionRefused: errorTypeName = "ConnectionRefused"; break;
        case ModbusErrorType::DeviceOverload: errorTypeName = "DeviceOverload"; break;
        case ModbusErrorType::DeviceBusy: errorTypeName = "DeviceBusy"; break;
        case ModbusErrorType::NetworkError: errorTypeName = "NetworkError"; break;
        case ModbusErrorType::ProtocolError: errorTypeName = "ProtocolError"; break;
        case ModbusErrorType::ConfigurationError: errorTypeName = "ConfigurationError"; break;
        case ModbusErrorType::RequestTimeout: errorTypeName = "RequestTimeout"; break;
        case ModbusErrorType::ResourceExhaustion: errorTypeName = "ResourceExhaustion"; break;
        default: errorTypeName = "Unknown"; break;
    }
    
    qDebug() << "ModbusWorker::emitClassifiedError() - Device:" << m_deviceKey 
             << "Error Type:" << errorTypeName << "Message:" << errorMessage;
}
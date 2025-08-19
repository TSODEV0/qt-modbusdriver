#include "modbus_worker.h"
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QUuid>
#include <QMetaObject>
#include <QCoreApplication>
#include <QDateTime>
#include <QMutexLocker>

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
    , m_pollInterval(1000)
    , m_pollingEnabled(false)
    , m_workerRunning(false)
    , m_stopRequested(false)
{
    // Initialize statistics
    m_statistics.totalRequests = 0;
    m_statistics.successfulRequests = 0;
    m_statistics.failedRequests = 0;
    m_statistics.isConnected = false;
    
    // Setup polling timer
    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, this, &ModbusWorker::onPollTimer);
}

ModbusWorker::~ModbusWorker()
{
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->stop();
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
        return;
    }
    
    qDebug() << "ModbusWorker::connectToDevice() - m_modbusManager pointer:" << (void*)m_modbusManager;
    qDebug() << "ModbusWorker::connectToDevice() - About to call connectToServer with host:" << m_host << "port:" << m_port;
    
    bool success = m_modbusManager->connectToServer(m_host, m_port);
    
    qDebug() << "ModbusWorker::connectToDevice() - connectToServer returned:" << success;
    
    if (success) {
        m_statistics.isConnected = true;
        emit connectionStateChanged(m_deviceKey, true);
    }
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
    if (m_pollTimer) {
        m_pollTimer->setInterval(m_pollInterval);
    }
}

int ModbusWorker::getPollInterval() const
{
    return m_pollInterval;
}

void ModbusWorker::setPollingEnabled(bool enabled)
{
    m_pollingEnabled = enabled;
    
    if (m_pollTimer) {
        if (enabled && m_workerRunning && isConnected()) {
            // Start polling if worker is running and connected
            m_pollTimer->start(m_pollInterval);
        } else {
            // Stop polling
            m_pollTimer->stop();
        }
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
    
    m_statistics.totalRequests++;
    if (result.success) {
        m_statistics.successfulRequests++;
        emit readCompleted(m_currentRequest.requestId, result);
        completeCurrentRequest(true);
    } else {
        m_statistics.failedRequests++;
        emit errorOccurred(m_deviceKey, result.errorString);
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
        // Clear queue on disconnection but keep polling enabled for reconnection attempts
        m_requestQueue.clear();
        qDebug() << "ModbusWorker - Device disconnected, clearing request queue but maintaining polling for:" << m_deviceKey;
        // Note: We no longer stop polling timer or disable polling to maintain continuous operation
    } else {
        // Force enable polling when connected and ensure timer is running
        m_pollingEnabled = true;
        if (m_pollTimer && m_workerRunning) {
            if (!m_pollTimer->isActive()) {
                qDebug() << "ModbusWorker - Starting poll timer after connection established for:" << m_deviceKey;
                m_pollTimer->start(m_pollInterval);
            } else {
                qDebug() << "ModbusWorker - Poll timer already active for connected device:" << m_deviceKey;
            }
        }
    }
    
    emit connectionStateChanged(m_deviceKey, connected);
}

void ModbusWorker::onModbusError(const QString& error)
{
    emit errorOccurred(m_deviceKey, error);
}

void ModbusWorker::completeCurrentRequest(bool success, const QString &error)
{
    Q_UNUSED(error); // Suppress unused parameter warning
    
    m_currentRequest = PriorityModbusRequest();
    m_requestInProgress = false;
    
    // Update statistics
    updateStatistics(success);
    
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
    
    // Get next request by priority
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
    // Enhanced polling implementation with connection state management
    if (!m_workerRunning || m_stopRequested) {
        return;
    }
    
    // Check connection state and attempt reconnection if needed
    if (!isConnected() && m_modbusManager) {
        qDebug() << "ModbusWorker::onPollTimer() - Device not connected, attempting reconnection for:" << m_deviceKey;
        connectToDevice();
    }
    
    // Process any queued requests regardless of connection state
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
    if (m_workerRunning) {
        return;
    }
    
    m_workerRunning = true;
    m_stopRequested = false;
    
    // Create ModbusManager in the worker thread
    if (!m_modbusManager) {
        m_modbusManager = new ModbusManager(this);
        
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
        qDebug() << "ModbusWorker::startWorker() - Starting poll timer immediately for device:" << m_deviceKey;
        m_pollTimer->start(m_pollInterval);
    }
    
    emit workerStarted(m_deviceKey);
}

void ModbusWorker::stopWorker()
{
    if (!m_workerRunning) {
        return;
    }
    
    m_stopRequested = true;
    m_workerRunning = false;
    
    // Stop polling
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->stop();
    }
    
    // Clear request queue
    clearRequestQueue();
    
    // Disconnect from device
    disconnectFromDevice();
    
    emit workerStopped(m_deviceKey);
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
#include "../include/connection_resilience_manager.h"
#include <QDebug>
#include <QDateTime>
#include <QNetworkInterface>
#include <QHostInfo>

ConnectionResilienceManager::ConnectionResilienceManager(QObject *parent)
    : QObject(parent)
    , m_connectionTimeout(10000)  // 10 seconds for poor connections
    , m_requestTimeout(8000)      // 8 seconds per request
    , m_maxRetries(5)             // More retries for poor connections
    , m_retryDelay(2000)          // 2 seconds between retries
    , m_heartbeatInterval(30000)  // 30 seconds heartbeat
    , m_connectionQuality(ConnectionQuality::Unknown)
    , m_consecutiveFailures(0)
    , m_isMonitoring(false)
{
    // Initialize timers
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(false);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &ConnectionResilienceManager::performHeartbeat);
    
    m_retryTimer = new QTimer(this);
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &ConnectionResilienceManager::retryConnection);
    
    m_qualityTimer = new QTimer(this);
    m_qualityTimer->setSingleShot(false);
    m_qualityTimer->setInterval(5000); // Check quality every 5 seconds
    connect(m_qualityTimer, &QTimer::timeout, this, &ConnectionResilienceManager::assessConnectionQuality);
}

ConnectionResilienceManager::~ConnectionResilienceManager()
{
    stopMonitoring();
}

void ConnectionResilienceManager::configureForPoorConnection()
{
    qDebug() << "Configuring system for poor network conditions...";
    
    // Aggressive timeout settings for poor connections
    m_connectionTimeout = 15000;   // 15 seconds
    m_requestTimeout = 12000;      // 12 seconds per request
    m_maxRetries = 8;              // More retries
    m_retryDelay = 3000;           // 3 seconds between retries
    m_heartbeatInterval = 20000;   // More frequent heartbeat (20s)
    
    qDebug() << "Poor connection configuration applied:";
    qDebug() << "  - Connection timeout:" << m_connectionTimeout << "ms";
    qDebug() << "  - Request timeout:" << m_requestTimeout << "ms";
    qDebug() << "  - Max retries:" << m_maxRetries;
    qDebug() << "  - Retry delay:" << m_retryDelay << "ms";
    qDebug() << "  - Heartbeat interval:" << m_heartbeatInterval << "ms";
    
    emit configurationChanged("Poor connection settings applied");
}

void ConnectionResilienceManager::configureForGoodConnection()
{
    qDebug() << "Configuring system for good network conditions...";
    
    // Standard timeout settings
    m_connectionTimeout = 5000;    // 5 seconds
    m_requestTimeout = 3000;       // 3 seconds per request
    m_maxRetries = 3;              // Standard retries
    m_retryDelay = 1000;           // 1 second between retries
    m_heartbeatInterval = 60000;   // Less frequent heartbeat (60s)
    
    qDebug() << "Good connection configuration applied:";
    qDebug() << "  - Connection timeout:" << m_connectionTimeout << "ms";
    qDebug() << "  - Request timeout:" << m_requestTimeout << "ms";
    qDebug() << "  - Max retries:" << m_maxRetries;
    qDebug() << "  - Retry delay:" << m_retryDelay << "ms";
    qDebug() << "  - Heartbeat interval:" << m_heartbeatInterval << "ms";
    
    emit configurationChanged("Good connection settings applied");
}

void ConnectionResilienceManager::setModbusManager(ModbusManager *manager)
{
    if (m_modbusManager) {
        disconnect(m_modbusManager, nullptr, this, nullptr);
    }
    
    m_modbusManager = manager;
    
    if (m_modbusManager) {
        // Connect to ModbusManager signals
        connect(m_modbusManager, &ModbusManager::connectionStateChanged,
                this, &ConnectionResilienceManager::onConnectionStateChanged);
        connect(m_modbusManager, &ModbusManager::errorOccurred,
                this, &ConnectionResilienceManager::onModbusError);
        connect(m_modbusManager, &ModbusManager::readCompleted,
                this, &ConnectionResilienceManager::onOperationCompleted);
        connect(m_modbusManager, &ModbusManager::writeCompleted,
                this, &ConnectionResilienceManager::onWriteCompleted);
    }
}

void ConnectionResilienceManager::startMonitoring(const QString &host, int port)
{
    m_targetHost = host;
    m_targetPort = port;
    m_isMonitoring = true;
    
    qDebug() << "Starting connection monitoring for" << host << ":" << port;
    
    // Start quality assessment
    m_qualityTimer->start();
    
    // Start heartbeat monitoring
    m_heartbeatTimer->setInterval(m_heartbeatInterval);
    m_heartbeatTimer->start();
    
    // Initial connection quality assessment
    assessConnectionQuality();
    
    emit monitoringStarted(host, port);
}

void ConnectionResilienceManager::stopMonitoring()
{
    if (!m_isMonitoring) {
        return;
    }
    
    qDebug() << "Stopping connection monitoring";
    
    m_isMonitoring = false;
    m_heartbeatTimer->stop();
    m_qualityTimer->stop();
    m_retryTimer->stop();
    
    emit monitoringStopped();
}

void ConnectionResilienceManager::performHeartbeat()
{
    if (!m_modbusManager || !m_isMonitoring) {
        return;
    }
    
    qDebug() << "Performing connection heartbeat...";
    
    if (!m_modbusManager->isConnected()) {
        qDebug() << "Connection lost - attempting reconnection";
        attemptReconnection();
        return;
    }
    
    // Perform a simple read operation as heartbeat
    // This will trigger connection quality assessment
    m_heartbeatStartTime = QDateTime::currentMSecsSinceEpoch();
    m_modbusManager->readHoldingRegister(1, ModbusDataType::HoldingRegister);
}

void ConnectionResilienceManager::attemptReconnection()
{
    if (!m_modbusManager || m_targetHost.isEmpty()) {
        return;
    }
    
    m_consecutiveFailures++;
    
    qDebug() << "Attempting reconnection (attempt" << m_consecutiveFailures << "of" << m_maxRetries << ")";
    
    if (m_consecutiveFailures >= m_maxRetries) {
        qDebug() << "Maximum reconnection attempts reached. Switching to poor connection mode.";
        configureForPoorConnection();
        m_consecutiveFailures = 0; // Reset for next cycle
    }
    
    // Disconnect first
    m_modbusManager->disconnectFromServer();
    
    // Wait before reconnecting
    m_retryTimer->setInterval(m_retryDelay);
    m_retryTimer->start();
    
    emit reconnectionAttempt(m_consecutiveFailures, m_maxRetries);
}

void ConnectionResilienceManager::triggerHeartbeat()
{
    performHeartbeat();
}

void ConnectionResilienceManager::retryConnection()
{
    if (!m_modbusManager || m_targetHost.isEmpty()) {
        return;
    }
    
    qDebug() << "Retrying connection to" << m_targetHost << ":" << m_targetPort;
    
    bool success = m_modbusManager->connectToServer(m_targetHost, m_targetPort);
    
    if (success) {
        qDebug() << "Reconnection successful";
        m_consecutiveFailures = 0;
        emit reconnectionSuccessful();
    } else {
        qDebug() << "Reconnection failed";
        // Will be handled by connection state change
    }
}

void ConnectionResilienceManager::assessConnectionQuality()
{
    if (!m_modbusManager || !m_isMonitoring) {
        return;
    }
    
    ConnectionQuality oldQuality = m_connectionQuality;
    
    // Assess based on consecutive failures and response times
    if (m_consecutiveFailures == 0 && m_averageResponseTime < 1000) {
        m_connectionQuality = ConnectionQuality::Excellent;
    } else if (m_consecutiveFailures <= 1 && m_averageResponseTime < 3000) {
        m_connectionQuality = ConnectionQuality::Good;
    } else if (m_consecutiveFailures <= 3 && m_averageResponseTime < 8000) {
        m_connectionQuality = ConnectionQuality::Fair;
    } else {
        m_connectionQuality = ConnectionQuality::Poor;
    }
    
    if (oldQuality != m_connectionQuality) {
        qDebug() << "Connection quality changed from" << qualityToString(oldQuality) 
                 << "to" << qualityToString(m_connectionQuality);
        
        // Auto-configure based on quality
        if (m_connectionQuality == ConnectionQuality::Poor || m_connectionQuality == ConnectionQuality::Fair) {
            configureForPoorConnection();
        } else if (m_connectionQuality == ConnectionQuality::Excellent || m_connectionQuality == ConnectionQuality::Good) {
            configureForGoodConnection();
        }
        
        emit connectionQualityChanged(m_connectionQuality);
    }
}

QString ConnectionResilienceManager::qualityToString(ConnectionQuality quality) const
{
    switch (quality) {
        case ConnectionQuality::Excellent: return "Excellent";
        case ConnectionQuality::Good: return "Good";
        case ConnectionQuality::Fair: return "Fair";
        case ConnectionQuality::Poor: return "Poor";
        case ConnectionQuality::Unknown: return "Unknown";
        default: return "Unknown";
    }
}

void ConnectionResilienceManager::onConnectionStateChanged(bool connected)
{
    if (connected) {
        qDebug() << "Connection established successfully";
        m_consecutiveFailures = 0;
        emit connectionRestored();
    } else {
        qDebug() << "Connection lost";
        if (m_isMonitoring) {
            attemptReconnection();
        }
        emit connectionLost();
    }
}

void ConnectionResilienceManager::onModbusError(const QString &error)
{
    qDebug() << "Modbus error occurred:" << error;
    m_consecutiveFailures++;
    
    // Assess if this is a connection-related error
    if (error.contains("timeout", Qt::CaseInsensitive) ||
        error.contains("connection", Qt::CaseInsensitive) ||
        error.contains("network", Qt::CaseInsensitive)) {
        
        qDebug() << "Connection-related error detected. Consecutive failures:" << m_consecutiveFailures;
        
        if (m_consecutiveFailures >= 3) {
            qDebug() << "Multiple connection failures detected. Switching to poor connection mode.";
            configureForPoorConnection();
        }
    }
    
    emit errorDetected(error, m_consecutiveFailures);
}

void ConnectionResilienceManager::onOperationCompleted(const ModbusReadResult &result)
{
    if (result.success) {
        // Calculate response time if this was a heartbeat
        if (m_heartbeatStartTime > 0) {
            qint64 responseTime = QDateTime::currentMSecsSinceEpoch() - m_heartbeatStartTime;
            updateResponseTime(responseTime);
            m_heartbeatStartTime = 0;
        }
        
        // Reset failure count on successful operation
        if (m_consecutiveFailures > 0) {
            qDebug() << "Operation successful after" << m_consecutiveFailures << "failures. Resetting failure count.";
            m_consecutiveFailures = 0;
        }
    } else {
        m_consecutiveFailures++;
        qDebug() << "Operation failed. Consecutive failures:" << m_consecutiveFailures;
    }
}

void ConnectionResilienceManager::onWriteCompleted(const ModbusWriteResult &result)
{
    if (result.success) {
        // Reset failure count on successful operation
        if (m_consecutiveFailures > 0) {
            qDebug() << "Write operation successful after" << m_consecutiveFailures << "failures. Resetting failure count.";
            m_consecutiveFailures = 0;
        }
    } else {
        m_consecutiveFailures++;
        qDebug() << "Write operation failed. Consecutive failures:" << m_consecutiveFailures;
    }
}

void ConnectionResilienceManager::updateResponseTime(qint64 responseTime)
{
    m_responseTimes.append(responseTime);
    
    // Keep only last 10 response times for average calculation
    if (m_responseTimes.size() > 10) {
        m_responseTimes.removeFirst();
    }
    
    // Calculate average response time
    qint64 total = 0;
    for (qint64 time : m_responseTimes) {
        total += time;
    }
    m_averageResponseTime = total / m_responseTimes.size();
    
    qDebug() << "Response time:" << responseTime << "ms, Average:" << m_averageResponseTime << "ms";
}

// Getters for current configuration
int ConnectionResilienceManager::getConnectionTimeout() const { return m_connectionTimeout; }
int ConnectionResilienceManager::getRequestTimeout() const { return m_requestTimeout; }
int ConnectionResilienceManager::getMaxRetries() const { return m_maxRetries; }
int ConnectionResilienceManager::getRetryDelay() const { return m_retryDelay; }
int ConnectionResilienceManager::getHeartbeatInterval() const { return m_heartbeatInterval; }
ConnectionResilienceManager::ConnectionQuality ConnectionResilienceManager::getConnectionQuality() const { return m_connectionQuality; }
int ConnectionResilienceManager::getConsecutiveFailures() const { return m_consecutiveFailures; }
qint64 ConnectionResilienceManager::getAverageResponseTime() const { return m_averageResponseTime; }

// Manual configuration setters
void ConnectionResilienceManager::setConnectionTimeout(int timeout) { m_connectionTimeout = timeout; }
void ConnectionResilienceManager::setRequestTimeout(int timeout) { m_requestTimeout = timeout; }
void ConnectionResilienceManager::setMaxRetries(int retries) { m_maxRetries = retries; }
void ConnectionResilienceManager::setRetryDelay(int delay) { m_retryDelay = delay; }
void ConnectionResilienceManager::setHeartbeatInterval(int interval) { 
    m_heartbeatInterval = interval;
    if (m_heartbeatTimer->isActive()) {
        m_heartbeatTimer->setInterval(interval);
    }
}
#ifndef CONNECTION_RESILIENCE_MANAGER_H
#define CONNECTION_RESILIENCE_MANAGER_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QDateTime>
#include "modbusmanager.h"

/**
 * @brief The ConnectionResilienceManager class provides robust connection handling
 * for poor network conditions in SCADA systems.
 * 
 * This class automatically detects connection quality and adjusts timeout settings,
 * retry mechanisms, and monitoring intervals to maintain reliable communication
 * even under challenging network conditions.
 */
class ConnectionResilienceManager : public QObject
{
    Q_OBJECT

public:
    enum class ConnectionQuality {
        Unknown,
        Excellent,  // < 1s response, no failures
        Good,       // < 3s response, rare failures
        Fair,       // < 8s response, occasional failures
        Poor        // > 8s response, frequent failures
    };
    Q_ENUM(ConnectionQuality)

    explicit ConnectionResilienceManager(QObject *parent = nullptr);
    ~ConnectionResilienceManager();

    // Configuration methods for different network conditions
    void configureForPoorConnection();
    void configureForGoodConnection();
    
    // ModbusManager integration
    void setModbusManager(ModbusManager *manager);
    
    // Connection monitoring
    void startMonitoring(const QString &host, int port = 502);
    void stopMonitoring();
    bool isMonitoring() const { return m_isMonitoring; }
    
    // Manual connection management
    void attemptReconnection();
    void triggerHeartbeat();  // Public method to trigger heartbeat
    
    // Configuration getters
    int getConnectionTimeout() const;
    int getRequestTimeout() const;
    int getMaxRetries() const;
    int getRetryDelay() const;
    int getHeartbeatInterval() const;
    ConnectionQuality getConnectionQuality() const;
    int getConsecutiveFailures() const;
    qint64 getAverageResponseTime() const;
    
    // Manual configuration setters
    void setConnectionTimeout(int timeout);
    void setRequestTimeout(int timeout);
    void setMaxRetries(int retries);
    void setRetryDelay(int delay);
    void setHeartbeatInterval(int interval);
    
    // Utility methods
    QString qualityToString(ConnectionQuality quality) const;

signals:
    // Configuration signals
    void configurationChanged(const QString &description);
    
    // Monitoring signals
    void monitoringStarted(const QString &host, int port);
    void monitoringStopped();
    
    // Connection state signals
    void connectionQualityChanged(ConnectionQuality quality);
    void connectionLost();
    void connectionRestored();
    
    // Reconnection signals
    void reconnectionAttempt(int attempt, int maxAttempts);
    void reconnectionSuccessful();
    void reconnectionFailed();
    
    // Error signals
    void errorDetected(const QString &error, int consecutiveFailures);
    
    // Statistics signals
    void responseTimeUpdated(qint64 responseTime, qint64 averageTime);

private slots:
    // Timer slots
    void performHeartbeat();
    void retryConnection();
    void assessConnectionQuality();
    
    // ModbusManager signal handlers
    void onConnectionStateChanged(bool connected);
    void onModbusError(const QString &error);
    void onOperationCompleted(const ModbusReadResult &result);
    void onWriteCompleted(const ModbusWriteResult &result);

private:
    // Configuration parameters (initialized first in constructor)
    int m_connectionTimeout;    // Connection establishment timeout
    int m_requestTimeout;       // Individual request timeout
    int m_maxRetries;          // Maximum retry attempts
    int m_retryDelay;          // Delay between retry attempts
    int m_heartbeatInterval;   // Heartbeat check interval
    
    // Connection quality assessment (initialized next)
    ConnectionQuality m_connectionQuality;
    int m_consecutiveFailures;
    bool m_isMonitoring;
    
    // Core components
    ModbusManager *m_modbusManager = nullptr;
    
    // Timers (initialized last in constructor)
    QTimer *m_heartbeatTimer;
    QTimer *m_retryTimer;
    QTimer *m_qualityTimer;
    
    // Connection monitoring
    QString m_targetHost;
    int m_targetPort;
    
    // Response time tracking
    QQueue<qint64> m_responseTimes;
    qint64 m_averageResponseTime = 0;
    qint64 m_heartbeatStartTime = 0;
    
    // Helper methods
    void updateResponseTime(qint64 responseTime);
};

#endif // CONNECTION_RESILIENCE_MANAGER_H
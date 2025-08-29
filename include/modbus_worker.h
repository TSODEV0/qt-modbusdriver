#ifndef MODBUS_WORKER_H
#define MODBUS_WORKER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QMap>
#include <QAtomicInteger>
#include <QSemaphore>
#include "modbusmanager.h"

// Forward declaration
struct DataAcquisitionPoint;

// Error classification for enhanced error handling
enum class ModbusErrorType {
    Unknown,
    ConnectionTimeout,
    ConnectionRefused,
    DeviceOverload,
    DeviceBusy,
    NetworkError,
    ProtocolError,
    ConfigurationError,
    RequestTimeout,
    ResourceExhaustion
};
Q_DECLARE_METATYPE(ModbusErrorType)

// PriorityModbusRequest structure for queue management
struct PriorityModbusRequest {
    ModbusRequest request;
    RequestPriority priority;
    qint64 requestId;
    qint64 queueTime;
    bool interruptible;
    
    PriorityModbusRequest() : priority(RequestPriority::Normal), requestId(0), 
                             queueTime(0), interruptible(false) {}
    
    // Comparison operator for priority queue (higher priority first)
    bool operator<(const PriorityModbusRequest &other) const {
        if (priority != other.priority) {
            return static_cast<int>(priority) < static_cast<int>(other.priority);
        }
        // Same priority: FIFO order (earlier queue time first)
        return queueTime > other.queueTime;
    }
};

// Forward declarations
class ModbusWorkerManager;

/**
 * ModbusWorker - Per-device worker thread for Modbus operations
 * 
 * Each worker runs in its own QThread and owns a ModbusManager instance.
 * Provides priority-based request queuing with interruptible writes.
 * Handles polling and write operations independently per device.
 */
class ModbusWorker : public QObject
{
    Q_OBJECT

public:
    struct WorkerStatistics {
        qint64 totalRequests;
        qint64 successfulRequests;
        qint64 failedRequests;
        qint64 interruptedRequests;
        qint64 highPriorityRequests;
        double averageResponseTime;
        qint64 lastActivityTime;
        bool isConnected;
        
        WorkerStatistics() : totalRequests(0), successfulRequests(0), failedRequests(0),
                           interruptedRequests(0), highPriorityRequests(0), 
                           averageResponseTime(0.0), lastActivityTime(0), isConnected(false) {}
    };

    explicit ModbusWorker(const QString &host, int port, int unitId, QObject *parent = nullptr);
    ~ModbusWorker();
    
    // Device identification
    QString getDeviceKey() const;
    QString getHost() const { return m_host; }
    int getPort() const { return m_port; }
    int getUnitId() const { return m_unitId; }
    
    // Connection management
    bool isConnected() const;
    
    // Statistics
    WorkerStatistics getStatistics() const;
    void resetStatistics();
    
    // Request queuing (thread-safe)
    qint64 queueReadRequest(const ModbusRequest &request, RequestPriority priority = RequestPriority::Normal);
    qint64 queueWriteRequest(const ModbusRequest &request, RequestPriority priority = RequestPriority::Normal, bool interruptible = true);
    
    // Polling control
    int getPollInterval() const;
    
    // Request batching configuration
    void setBatchingEnabled(bool enabled);
    bool isBatchingEnabled() const;
    void setMaxBatchSize(int maxSize);
    int getMaxBatchSize() const;
    
    // Connection health monitoring configuration
    void setHealthMonitoringEnabled(bool enabled);
    bool isHealthMonitoringEnabled() const;
    void setHealthCheckInterval(int intervalMs);
    int getHealthCheckInterval() const;
    double getConnectionHealthScore() const;
    
    // Keep-alive heartbeat configuration
    void setHeartbeatEnabled(bool enabled);
    bool isHeartbeatEnabled() const;
    void setHeartbeatInterval(int intervalMs);
    int getHeartbeatInterval() const;
    
    // Data point management methods (getters)
    QVector<DataAcquisitionPoint> getDataPoints() const;
    bool isAutomaticPollingEnabled() const;
    
public slots:
    // Worker lifecycle (called from worker thread)
    void startWorker();
    void stopWorker();
    
    // Timer initialization (called after moveToThread)
    void initializeTimers();
    
    // Connection management (called from worker thread)
    void connectToDevice();
    void disconnectFromDevice();
    
    // Request interruption (thread-safe)
    void interruptCurrentRequest(const QString &reason = QString());
    void clearRequestQueue();
    
    // Polling control (thread-safe)
    void setPollInterval(int intervalMs);
    void setPollingEnabled(bool enabled);
    
    // Data point management methods (setters - must be slots for cross-thread calls)
    void setDataPointCount(int count);
    void addDataPointByName(const QString &pointName, const QString &host, int port, int unitId, int address, int dataType, int pollInterval, const QString &measurement, bool enabled, const QVariantMap &tags = QVariantMap());
    void removeDataPoint(const QString &pointName);
    void clearDataPoints();
    
    // Data point polling control
    void enableAutomaticPolling(bool enabled);
    
signals:
    // Request completion signals (emitted from worker thread)
    void readCompleted(qint64 requestId, const ModbusReadResult &result);
    void writeCompleted(qint64 requestId, const ModbusWriteResult &result);
    
    // Status signals
    void connectionStateChanged(const QString &deviceKey, bool connected);
    void errorOccurred(const QString &deviceKey, const QString &error);
    void errorOccurredClassified(const QString &deviceKey, const QString &error, ModbusErrorType errorType);
    void requestInterrupted(qint64 requestId, const QString &reason);
    void statisticsUpdated(const QString &deviceKey, const WorkerStatistics &stats);
    
    // Internal worker signals
    void workerStarted(const QString &deviceKey);
    void workerStopped(const QString &deviceKey);
    
private slots:
    // Internal event handlers (called from worker thread)
    void onRequestTimeout();
    void processRequestQueue();
    void onPollTimer();
    void onModbusReadCompleted(const ModbusReadResult &result);
    void onModbusWriteCompleted(const ModbusWriteResult &result);
    void onModbusConnectionStateChanged(bool connected);
    void onModbusError(const QString &error);
    void onBatchTimeout();  // Handle batch timeout
    void onHealthCheckTimer();  // Handle health check timer
    void onHeartbeatTimer();  // Handle keep-alive heartbeat timer
private:
    // Device configuration
    QString m_host;
    int m_port;
    int m_unitId;
    QString m_deviceKey;
    
    // Modbus communication
    ModbusManager *m_modbusManager;
    
    // Request queue management
    mutable QMutex m_queueMutex;
    QQueue<PriorityModbusRequest> m_requestQueue;
    QAtomicInteger<qint64> m_nextRequestId;
    
    // Current request tracking
    PriorityModbusRequest m_currentRequest;
    bool m_requestInProgress;
    QTimer *m_requestTimeoutTimer;
    
    // Polling
    QTimer *m_pollTimer;
    int m_pollInterval;
    bool m_pollingEnabled;
    
    // Statistics
    mutable QMutex m_statsMutex;
    WorkerStatistics m_statistics;
    QMap<qint64, qint64> m_requestStartTimes;
    
    // Worker state
    bool m_workerRunning;
    bool m_stopRequested;
    
    // Connection resilience
    qint64 m_lastConnectionAttempt;
    qint64 m_reconnectionDelay;
    int m_connectionAttempts;
    
    // Adaptive polling and connection health
    int m_basePollInterval;          // Base polling interval (2000ms)
    int m_adaptivePollInterval;      // Current adaptive interval
    int m_consecutiveSuccesses;      // Track successful requests
    int m_consecutiveFailures;       // Track failed requests
    qint64 m_lastSuccessTime;        // Last successful request time
    qint64 m_lastFailureTime;        // Last failure time
    static const int MAX_POLL_INTERVAL = 10000;  // Maximum 10 seconds
    static const int MIN_POLL_INTERVAL = 1000;   // Minimum 1 second
    
    // Request batching
    bool m_batchingEnabled;          // Enable/disable request batching
    int m_maxBatchSize;              // Maximum requests per batch
    QTimer *m_batchTimer;            // Timer for batch processing
    QQueue<PriorityModbusRequest> m_batchQueue;  // Temporary batch queue
    static const int BATCH_TIMEOUT_MS = 100;     // Batch collection timeout
    
    // Connection health monitoring
    bool m_healthMonitoringEnabled;  // Enable/disable health monitoring
    double m_connectionHealthScore;  // Current health score (0.0-1.0)
    qint64 m_lastHealthCheck;        // Last health check timestamp
    int m_healthCheckInterval;       // Health check interval (ms)
    QTimer *m_healthCheckTimer;      // Timer for periodic health checks
    int m_maxReconnectionAttempts;   // Maximum reconnection attempts before backing off
    qint64 m_healthCheckWindow;      // Time window for health calculations (ms)
    static const int DEFAULT_HEALTH_CHECK_INTERVAL = 30000;  // 30 seconds
    static const int MAX_RECONNECTION_ATTEMPTS = 3;          // Max attempts before backoff
    static const qint64 HEALTH_CHECK_WINDOW = 300000;        // 5 minutes window
    
    // Keep-alive heartbeat settings
    bool m_heartbeatEnabled;         // Enable/disable heartbeat functionality
    int m_heartbeatInterval;         // Heartbeat interval (ms)
    QTimer *m_heartbeatTimer;        // Timer for periodic heartbeats
    qint64 m_lastHeartbeatTime;      // Last heartbeat timestamp
    static const int DEFAULT_HEARTBEAT_INTERVAL = 30000;  // 30 seconds
    
    // Data point management for automatic polling
    mutable QMutex m_dataPointsMutex;
    QVector<DataAcquisitionPoint> m_dataPoints;
    QMap<QString, qint64> m_lastPollTimes;  // Track last poll time for each data point
    bool m_automaticPollingEnabled;         // Enable/disable automatic data point polling
    
    // Connection coordination
    static QSemaphore s_connectionSemaphore;                 // Limit simultaneous connections
    
    // Private methods
    void executeRequest(const PriorityModbusRequest &request);
    void completeCurrentRequest(bool success, const QString &error = QString());
    void updateStatistics(bool success, qint64 responseTime = 0);
    void insertRequestByPriority(const PriorityModbusRequest &request);
    PriorityModbusRequest getNextRequest();
    bool hasHigherPriorityRequest(RequestPriority currentPriority) const;
    void emitStatisticsUpdate();
    void generateAutomaticPollingRequests();
    void handleConnectionFailure(const QString &errorMessage);
    void adjustAdaptivePollInterval(bool success);  // Adjust polling based on connection health
    void processBatchQueue();                       // Process batched requests
    void executeBatchedRequests(const QList<PriorityModbusRequest> &batch);  // Execute a batch of requests
    bool canBatchRequests(const PriorityModbusRequest &req1, const PriorityModbusRequest &req2) const;  // Check if requests can be batched
    void updateConnectionHealth(bool success);      // Update connection health score
    void performHealthCheck();                      // Perform periodic health check
    bool shouldAttemptReconnection() const;         // Check if reconnection should be attempted
    void resetConnectionHealth();                   // Reset health monitoring state
    ModbusErrorType classifyError(const QString &errorMessage) const;  // Classify error types for enhanced handling
    void emitClassifiedError(const QString &errorMessage);             // Emit both regular and classified error signals
    void sendHeartbeat();                                              // Send keep-alive heartbeat request
    void handleHeartbeatResponse(bool success);                        // Handle heartbeat response
};



#endif // MODBUS_WORKER_H
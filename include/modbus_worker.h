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
#include "modbusmanager.h"

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
    
public slots:
    // Worker lifecycle (called from worker thread)
    void startWorker();
    void stopWorker();
    
    // Connection control (thread-safe)
    void connectToDevice();
    void disconnectFromDevice();
    
    // Request interruption (thread-safe)
    void interruptCurrentRequest(const QString &reason = QString());
    void clearRequestQueue();
    
    // Polling control (thread-safe)
    void setPollInterval(int intervalMs);
    void setPollingEnabled(bool enabled);
    
signals:
    // Request completion signals (emitted from worker thread)
    void readCompleted(qint64 requestId, const ModbusReadResult &result);
    void writeCompleted(qint64 requestId, const ModbusWriteResult &result);
    
    // Status signals
    void connectionStateChanged(const QString &deviceKey, bool connected);
    void errorOccurred(const QString &deviceKey, const QString &error);
    void requestInterrupted(qint64 requestId, const QString &reason);
    void statisticsUpdated(const QString &deviceKey, const WorkerStatistics &stats);
    
    // Internal worker signals
    void workerStarted(const QString &deviceKey);
    void workerStopped(const QString &deviceKey);
    
private slots:
    // Internal event handlers (called from worker thread)
    void processRequestQueue();
    void onPollTimer();
    void onModbusReadCompleted(const ModbusReadResult &result);
    void onModbusWriteCompleted(const ModbusWriteResult &result);
    void onModbusConnectionStateChanged(bool connected);
    void onModbusError(const QString &error);
    
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
    
    // Helper methods
    void executeRequest(const PriorityModbusRequest &request);
    void completeCurrentRequest(bool success, const QString &error = QString());
    void updateStatistics(bool success, qint64 responseTime = 0);
    void insertRequestByPriority(const PriorityModbusRequest &request);
    PriorityModbusRequest getNextRequest();
    bool hasHigherPriorityRequest(RequestPriority currentPriority) const;
    void emitStatisticsUpdate();
};

/**
 * ModbusWorkerManager - Manages multiple ModbusWorker instances
 * 
 * Creates and manages worker threads for different Modbus devices.
 * Routes requests to appropriate workers based on device key.
 * Provides global statistics and worker lifecycle management.
 */
class ModbusWorkerManager : public QObject
{
    Q_OBJECT
    
public:
    struct GlobalStatistics {
        int activeWorkers;
        int connectedDevices;
        qint64 totalRequests;
        qint64 totalSuccessfulRequests;
        qint64 totalFailedRequests;
        qint64 totalInterruptedRequests;
        double globalAverageResponseTime;
        qint64 lastUpdateTime;
        
        GlobalStatistics() : activeWorkers(0), connectedDevices(0), totalRequests(0),
                           totalSuccessfulRequests(0), totalFailedRequests(0),
                           totalInterruptedRequests(0), globalAverageResponseTime(0.0),
                           lastUpdateTime(0) {}
    };
    
    explicit ModbusWorkerManager(QObject *parent = nullptr);
    ~ModbusWorkerManager();
    
    // Worker management
    ModbusWorker* getOrCreateWorker(const QString &host, int port, int unitId = 1);
    ModbusWorker* getWorker(const QString &deviceKey) const;
    void removeWorker(const QString &deviceKey);
    void removeAllWorkers();
    
    // Device management
    QStringList getActiveDevices() const;
    QStringList getConnectedDevices() const;
    int getWorkerCount() const;
    
    // Statistics
    GlobalStatistics getGlobalStatistics() const;
    void resetGlobalStatistics();
    
    // Configuration
    void setDefaultPollInterval(int intervalMs);
    void setWorkerPollInterval(const QString &deviceKey, int intervalMs);
    
    // Load balancing and coordination
    QString getLeastLoadedWorker() const;
    void distributeLoad();
    void setLoadBalancingEnabled(bool enabled);
    bool isLoadBalancingEnabled() const;
    double getWorkerLoad(const QString &deviceKey) const;
    void optimizeWorkerDistribution();
    
public slots:
    void startAllWorkers();
    void stopAllWorkers();
    void connectAllDevices();
    void disconnectAllDevices();
    
signals:
    void workerCreated(const QString &deviceKey);
    void workerRemoved(const QString &deviceKey);
    void globalStatisticsUpdated(const GlobalStatistics &stats);
    
private slots:
    void onWorkerStarted(const QString &deviceKey);
    void onWorkerStopped(const QString &deviceKey);
    void onWorkerConnectionStateChanged(const QString &deviceKey, bool connected);
    void onWorkerStatisticsUpdated(const QString &deviceKey, const ModbusWorker::WorkerStatistics &stats);
    void updateGlobalStatistics();
    
private:
    struct WorkerInfo {
        ModbusWorker *worker;
        QThread *thread;
        bool isConnected;
        ModbusWorker::WorkerStatistics lastStats;
        
        WorkerInfo() : worker(nullptr), thread(nullptr), isConnected(false) {}
    };
    
    mutable QMutex m_workersMutex;
    QMap<QString, WorkerInfo> m_workers;
    
    GlobalStatistics m_globalStats;
    mutable QMutex m_globalStatsMutex;
    
    int m_defaultPollInterval;
    QTimer *m_statsUpdateTimer;
    
    // Load balancing
    bool m_loadBalancingEnabled;
    QTimer *m_loadBalancingTimer;
    mutable QMutex m_loadBalancingMutex;
    
    // Helper methods
    QString createDeviceKey(const QString &host, int port, int unitId) const;
    void connectWorkerSignals(ModbusWorker *worker);
    void cleanupWorker(const QString &deviceKey);
    void calculateWorkerLoads() const;
    void rebalanceWorkerLoads();
};

#endif // MODBUS_WORKER_H
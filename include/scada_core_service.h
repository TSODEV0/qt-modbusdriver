#ifndef SCADA_CORE_SERVICE_H
#define SCADA_CORE_SERVICE_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QMap>
#include <QVector>
#include <QSet>
#include <QFileInfo>
#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QThreadPool>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "modbusmanager.h"
#include "modbus_worker.h"

// Configuration structure for data acquisition points
struct DataAcquisitionPoint {
    QString id;                      // Unique point identifier
    QString name;                    // Point name/identifier
    QString host;                    // Modbus server host
    int port;                        // Modbus server port
    int address;                     // Register address
    int unitId;                      // Modbus unit ID
    ModbusDataType dataType;         // Data type
    int pollInterval;                // Polling interval in milliseconds
    QString measurement;             // InfluxDB measurement name
    QMap<QString, QString> tags;     // InfluxDB tags
    bool enabled;                    // Enable/disable this point
    
    DataAcquisitionPoint() : port(502), address(0), unitId(1), dataType(ModbusDataType::HoldingRegister), 
                           pollInterval(1000), enabled(true) {}
};

// Data point with timestamp and value
struct AcquiredDataPoint {
    QString pointName;
    QVariant value;
    qint64 timestamp;
    QString measurement;
    QMap<QString, QString> tags;
    bool isValid;
    QString errorMessage;
    
    AcquiredDataPoint() : timestamp(0), isValid(false) {}
};

class ScadaCoreService : public QObject
{
    Q_OBJECT

public:
    explicit ScadaCoreService(QObject *parent = nullptr);
    ~ScadaCoreService();
    
    // Service control
    bool startService();
    void stopService();
    bool isRunning() const;
    
    // Threading strategy
    enum class ThreadingMode {
        Auto,           // Automatically choose based on device count
        SingleThreaded, // Force single-threaded mode
        MultiThreaded   // Force multi-threaded mode
    };
    
    // Configuration options for deployment requirements
    struct DeploymentConfig {
        ThreadingMode threadingMode;
        int maxWorkerThreads;           // Maximum number of worker threads
        int deviceCountThreshold;       // Device count threshold for auto mode
        int pollIntervalMs;             // Default poll interval
        bool enableLoadBalancing;       // Enable/disable load balancing
        bool enablePerformanceMonitoring; // Enable/disable performance monitoring
        int connectionTimeoutMs;        // Connection timeout
        int maxRetryAttempts;          // Maximum retry attempts
        QString configFilePath;         // Path to configuration file
        
        DeploymentConfig() : threadingMode(ThreadingMode::Auto), maxWorkerThreads(10),
                           deviceCountThreshold(1), pollIntervalMs(1000),
                           enableLoadBalancing(true), enablePerformanceMonitoring(false),
                           connectionTimeoutMs(5000), maxRetryAttempts(3),
                           configFilePath("scada_config.json") {}
    };
    
    void setThreadingMode(ThreadingMode mode);
    ThreadingMode getThreadingMode() const;
    bool isSingleThreadedMode() const;
    
    // Configuration management
    void setDeploymentConfig(const DeploymentConfig &config);
    DeploymentConfig getDeploymentConfig() const;
    bool loadConfigFromFile(const QString &filePath);
    bool saveConfigToFile(const QString &filePath) const;
    void resetConfigToDefaults();
    
    // Performance optimization
    void optimizePollInterval();
    
    // Load balancing and coordination
    void enableLoadBalancing(bool enabled);
    bool isLoadBalancingEnabled() const;
    QString getLeastLoadedDevice() const;
    void optimizeWorkerDistribution();
    double getDeviceLoad(const QString &deviceKey) const;
    
    // Configuration management
    void addDataPoint(const DataAcquisitionPoint &point);
    void removeDataPoint(const QString &pointName);
    void updateDataPoint(const QString &pointName, const DataAcquisitionPoint &point);
    QVector<DataAcquisitionPoint> getDataPoints() const;
    void clearDataPoints();
    
    // Telegraf socket configuration
    void setTelegrafSocketPath(const QString &socketPath);
    QString getTelegrafSocketPath() const;
    
    // Statistics
    struct ServiceStatistics {
        qint64 totalReadOperations;
        qint64 successfulReads;
        qint64 failedReads;
        qint64 totalDataPointsSent;
        qint64 socketErrors;
        qint64 serviceStartTime;
        double averageResponseTime;
        
        ServiceStatistics() : totalReadOperations(0), successfulReads(0), failedReads(0),
                            totalDataPointsSent(0), socketErrors(0), serviceStartTime(0),
                            averageResponseTime(0.0) {}
    };
    
    ServiceStatistics getStatistics() const;
    void resetStatistics();
    
    // Performance monitoring
    struct PerformanceMetrics {
        // Threading mode metrics
        qint64 singleThreadedOperations;
        qint64 multiThreadedOperations;
        double singleThreadedAvgResponseTime;
        double multiThreadedAvgResponseTime;
        qint64 singleThreadedTotalTime;
        qint64 multiThreadedTotalTime;
        
        // Resource usage
        int maxConcurrentWorkers;
        int currentActiveWorkers;
        double cpuUsagePercent;
        qint64 memoryUsageBytes;
        
        // Efficiency metrics
        double throughputSingleThreaded;  // operations per second
        double throughputMultiThreaded;   // operations per second
        double efficiencyRatio;           // multi-threaded vs single-threaded efficiency
        
        // Error rates
        double singleThreadedErrorRate;
        double multiThreadedErrorRate;
        
        PerformanceMetrics() : singleThreadedOperations(0), multiThreadedOperations(0),
                              singleThreadedAvgResponseTime(0.0), multiThreadedAvgResponseTime(0.0),
                              singleThreadedTotalTime(0), multiThreadedTotalTime(0),
                              maxConcurrentWorkers(0), currentActiveWorkers(0),
                              cpuUsagePercent(0.0), memoryUsageBytes(0),
                              throughputSingleThreaded(0.0), throughputMultiThreaded(0.0),
                              efficiencyRatio(0.0), singleThreadedErrorRate(0.0),
                              multiThreadedErrorRate(0.0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    QString getPerformanceReport() const;
    void enablePerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoringEnabled() const;
    
    // Modbus write operations with priority support
    qint64 writeHoldingRegister(const QString &host, int port, int address, quint16 value, RequestPriority priority = RequestPriority::Normal);
    qint64 writeHoldingRegisterFloat32(const QString &host, int port, int address, float value, RequestPriority priority = RequestPriority::Normal);
    qint64 writeHoldingRegisterDouble64(const QString &host, int port, int address, double value, RequestPriority priority = RequestPriority::Normal);
    qint64 writeHoldingRegisterLong32(const QString &host, int port, int address, qint32 value, RequestPriority priority = RequestPriority::Normal);
    qint64 writeHoldingRegisterLong64(const QString &host, int port, int address, qint64 value, RequestPriority priority = RequestPriority::Normal);
    qint64 writeCoil(const QString &host, int port, int address, bool value, RequestPriority priority = RequestPriority::Normal);
    
    // Batch write operations
    QVector<qint64> writeHoldingRegistersBatch(const QString &host, int port, const QVector<QPair<int, quint16>> &addressValuePairs, RequestPriority priority = RequestPriority::Normal);
    QVector<qint64> writeCoilsBatch(const QString &host, int port, const QVector<QPair<int, bool>> &addressValuePairs, RequestPriority priority = RequestPriority::Normal);
    
    // Worker management
    QStringList getActiveDevices() const;
    ModbusWorkerManager::GlobalStatistics getGlobalStatistics() const;
    void setWorkerPollInterval(const QString &deviceKey, int intervalMs);
    
signals:
    void serviceStarted();
    void serviceStopped();
    void dataPointAcquired(const AcquiredDataPoint &dataPoint);
    void dataPointSentToInflux(const QString &pointName, bool success);
    
    // Worker-related signals
    void workerCreated(const QString &deviceId);
    void workerRemoved(const QString &deviceId);
    void requestCompleted(qint64 requestId, bool success);
    void requestInterrupted(qint64 requestId, const QString &reason);
    void writeCompleted(qint64 requestId, const QString &operation, bool success, const QString &error = QString());
    void errorOccurred(const QString &error);
    void statisticsUpdated(const ServiceStatistics &stats);
    void globalStatisticsUpdated(const ModbusWorkerManager::GlobalStatistics &stats);
    
private slots:
    void onPollTimer();
    void onWorkerReadCompleted(qint64 requestId, const ModbusReadResult &result);
    void onWorkerWriteCompleted(qint64 requestId, const ModbusWriteResult &result);
    void onWorkerConnectionStateChanged(const QString &deviceId, bool connected);
    void onWorkerError(const QString &deviceId, const QString &error);
    void onWorkerRequestInterrupted(qint64 requestId, const QString &reason);
    void onWorkerStatisticsUpdated(const QString &deviceId, const ModbusWorker::WorkerStatistics &stats);
    void onWorkerCreated(const QString &deviceKey);
    void onWorkerRemoved(const QString &deviceKey);
    void onGlobalStatisticsUpdated(const ModbusWorkerManager::GlobalStatistics &stats);
    
    // Single-threaded mode handlers
    void onSingleThreadReadCompleted(const ModbusReadResult &result);
    // Socket connection methods removed (using direct Unix socket calls)
    
private:
    // Core components
    QTimer *m_pollTimer;
    ModbusWorkerManager *m_workerManager;
    // Unix socket implementation (no QLocalSocket needed)
    
    // Configuration
    QVector<DataAcquisitionPoint> m_dataPoints;
    QString m_telegrafSocketPath;
    bool m_serviceRunning;
    
    // State management
    QMap<QString, qint64> m_lastPollTimes;  // Track last poll time for each point
    QMap<QString, bool> m_connectionStates; // Track connection state for each device
    int m_currentPointIndex;                // Current point being processed
    
    // Request tracking
    QMap<qint64, DataAcquisitionPoint> m_pendingReadRequests; // Track pending read requests
    QMap<qint64, QString> m_pendingWriteRequests; // Track pending write requests
    
    // Statistics
    ServiceStatistics m_statistics;
    QMap<QString, qint64> m_responseTimers; // Track response times
    QSet<ModbusWorker*> m_connectedWorkers; // Track connected workers
    
    // Performance monitoring
    PerformanceMetrics m_performanceMetrics;
    mutable QMutex m_performanceMetricsMutex;
    bool m_performanceMonitoringEnabled;
    QElapsedTimer m_operationTimer;
    QHash<qint64, qint64> m_operationStartTimes;  // requestId -> start time
    
    // Thread safety
    mutable QMutex m_dataPointsMutex;        // Protects m_dataPoints and related data
    mutable QMutex m_statisticsMutex;        // Protects m_statistics and m_responseTimers
    mutable QMutex m_requestTrackingMutex;   // Protects request tracking maps
    
    // Threading configuration
    ThreadingMode m_threadingMode;
    bool m_useSingleThreadedMode;
    ModbusManager *m_singleThreadModbusManager; // For single-threaded mode
    DeploymentConfig m_deploymentConfig;        // Deployment configuration
    
    // Helper methods
    bool writeToInflux(const QString& measurement, const QString& device, const QVariant& value, const QString& description = QString());
    bool writeToInfluxEnhanced(const AcquiredDataPoint &dataPoint);
    bool writeToTelegrafSocket(const QString& socketPath, const QByteArray& message);
    bool sendDataToInflux(const AcquiredDataPoint &dataPoint);
    void processNextDataPoint();
    void processDataPoint(const DataAcquisitionPoint &point, qint64 currentTime);
    bool connectToModbusHost(const QString &host, int port);
    void updateStatistics(bool success, qint64 responseTime = 0);
    QJsonObject dataPointToJson(const AcquiredDataPoint &dataPoint);
    void handleBlockReadResult(const ModbusReadResult &result, const DataAcquisitionPoint &blockPoint);
    bool isPointCoveredByBlock(const DataAcquisitionPoint &point);
    void validateAndSetInfluxTags(AcquiredDataPoint &dataPoint, const DataAcquisitionPoint &sourcePoint);
    qint64 generateRequestId();
    void connectWorkerSignals(ModbusWorker* worker);
};

#endif // SCADA_CORE_SERVICE_H
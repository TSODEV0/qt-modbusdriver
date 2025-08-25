#ifndef SCADA_CORE_SERVICE_H
#define SCADA_CORE_SERVICE_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QMap>
#include <QVector>
#include <QFileInfo>
#include <QQueue>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "modbusmanager.h"

// Configuration structure for data acquisition points
struct DataAcquisitionPoint {
    QString name;                    // Point name/identifier
    QString host;                    // Modbus server host
    int port;                        // Modbus server port
    int address;                     // Register address
    ModbusDataType dataType;         // Data type
    int pollInterval;                // Polling interval in milliseconds
    QString measurement;             // InfluxDB measurement name
    QMap<QString, QString> tags;     // InfluxDB tags
    bool enabled;                    // Enable/disable this point
    
    DataAcquisitionPoint() : port(502), address(0), dataType(ModbusDataType::HoldingRegister), 
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

// Buffered data point for retry mechanism
struct BufferedDataPoint {
    AcquiredDataPoint dataPoint;
    int retryCount;
    qint64 nextRetryTime;
    
    BufferedDataPoint() : retryCount(0), nextRetryTime(0) {}
    BufferedDataPoint(const AcquiredDataPoint &dp) : dataPoint(dp), retryCount(0), nextRetryTime(0) {}
};

// Forward declaration for InfluxDB worker thread
class InfluxDBWorker;

class ScadaCoreService : public QObject
{
    Q_OBJECT

public:
    // Reading mode enumeration for unified acquisition
    enum class ReadMode {
        SINGLE_READ,    // Individual register/coil read
        BLOCK_READ      // Optimized block read for multiple consecutive registers
    };
    
    explicit ScadaCoreService(QObject *parent = nullptr);
    ~ScadaCoreService();
    
    // Service control
    bool startService();
    void stopService();
    bool isRunning() const;
    
    // Configuration management
    void addDataPoint(const DataAcquisitionPoint &point);
    void removeDataPoint(const QString &pointName);
    void updateDataPoint(const QString &pointName, const DataAcquisitionPoint &point);
    QVector<DataAcquisitionPoint> getDataPoints() const;
    void clearDataPoints();
    
    // Telegraf socket configuration
    void setTelegrafSocketPath(const QString &socketPath);
    QString getTelegrafSocketPath() const;
    
    // Alert severity levels
    enum class AlertSeverity {
        Info,
        Warning,
        Critical,
        Emergency
    };
    
    // Alert types for monitoring
    enum class AlertType {
        DataLoss,
        ConnectionFailure,
        HighErrorRate,
        BufferOverflow,
        ResponseTimeout,
        SystemHealth
    };
    
    // Alert structure
    struct SystemAlert {
        AlertType type;
        AlertSeverity severity;
        QString message;
        QString source;
        qint64 timestamp;
        QMap<QString, QString> metadata;
        
        SystemAlert() : type(AlertType::SystemHealth), severity(AlertSeverity::Info), timestamp(0) {}
        SystemAlert(AlertType t, AlertSeverity s, const QString &msg, const QString &src = QString())
            : type(t), severity(s), message(msg), source(src), timestamp(QDateTime::currentMSecsSinceEpoch()) {}
    };
    
    // Enhanced statistics with monitoring thresholds
    struct ServiceStatistics {
        qint64 totalReadOperations;
        qint64 successfulReads;
        qint64 failedReads;
        qint64 totalDataPointsSent;
        qint64 socketErrors;
        qint64 serviceStartTime;
        double averageResponseTime;
        
        // Monitoring metrics
        qint64 consecutiveFailures;
        qint64 lastSuccessfulRead;
        qint64 bufferSize;
        qint64 alertsGenerated;
        double successRate;
        qint64 maxResponseTime;
        qint64 minResponseTime;
        
        ServiceStatistics() : totalReadOperations(0), successfulReads(0), failedReads(0),
                            totalDataPointsSent(0), socketErrors(0), serviceStartTime(0),
                            averageResponseTime(0.0), consecutiveFailures(0), lastSuccessfulRead(0),
                            bufferSize(0), alertsGenerated(0), successRate(100.0),
                            maxResponseTime(0), minResponseTime(0) {}
    };
    
    ServiceStatistics getStatistics() const;
    void resetStatistics();
    
    // Monitoring and alerting
    void setAlertThresholds(double minSuccessRate, qint64 maxConsecutiveFailures, qint64 maxResponseTime, qint64 maxBufferSize);
    void enableAlerting(bool enabled);
    QVector<SystemAlert> getActiveAlerts() const;
    QVector<SystemAlert> getAlertHistory(qint64 fromTimestamp = 0) const;
    void clearAlerts();
    void acknowledgeAlert(qint64 alertTimestamp);
    
    // Unified data acquisition function
    QVector<AcquiredDataPoint> acquireDataUnified(const DataAcquisitionPoint &point, ReadMode mode = ReadMode::SINGLE_READ, 
                           const QVector<DataAcquisitionPoint> &blockPoints = QVector<DataAcquisitionPoint>());
    
    // Modbus write operations
    void writeHoldingRegister(const QString &host, int port, int address, quint16 value);
    void writeHoldingRegisterFloat32(const QString &host, int port, int address, float value);
    void writeHoldingRegisterDouble64(const QString &host, int port, int address, double value);
    void writeHoldingRegisterLong32(const QString &host, int port, int address, qint32 value);
    void writeHoldingRegisterLong64(const QString &host, int port, int address, qint64 value);
    void writeCoil(const QString &host, int port, int address, bool value);
    
signals:
    void serviceStarted();
    void serviceStopped();
    void dataPointAcquired(const AcquiredDataPoint &dataPoint);
    void dataPointSentToInflux(const QString &pointName, bool success);
    void writeCompleted(const QString &operation, bool success, const QString &error = QString());
    void errorOccurred(const QString &error);
    void statisticsUpdated(const ServiceStatistics &stats);
    
    // Monitoring and alerting signals
    void alertGenerated(const SystemAlert &alert);
    void alertCleared(AlertType alertType, const QString &source);
    void healthStatusChanged(bool healthy);
    void dataLossDetected(const QString &pointName, qint64 missedReadings);
    
private slots:
    void onPollTimer();
    void onModbusReadCompleted(const ModbusReadResult &result);
    void onModbusWriteCompleted(const ModbusWriteResult &result);
    void onModbusConnectionStateChanged(bool connected);
    void onModbusError(const QString &error);
    void onInfluxWriteCompleted(const QString &pointName, bool success, const QString &error);
    // Socket connection methods removed (using direct Unix socket calls)
    
private:
    // Core components
    QTimer *m_pollTimer;
    ModbusManager *m_modbusManager;
    QTimer *m_retryTimer;
    // Unix socket implementation (no QLocalSocket needed)
    
    // Asynchronous InfluxDB writing
    QThread *m_influxWorkerThread;
    InfluxDBWorker *m_influxWorker;
    
    // Configuration
    QVector<DataAcquisitionPoint> m_dataPoints;
    QString m_telegrafSocketPath;
    bool m_serviceRunning;
    
    // State management
    QMap<QString, qint64> m_lastPollTimes;  // Track last poll time for each point
    QMap<QString, bool> m_connectionStates; // Track connection state for each host
    QString m_currentHost;                  // Currently connected host
    int m_currentPointIndex;                // Current point being processed
    
    // Statistics
    ServiceStatistics m_statistics;
    QMap<QString, qint64> m_responseTimers; // Track response times
    
    // Data buffering for failed// InfluxDB buffer and retry mechanism
    QQueue<BufferedDataPoint> m_influxBuffer;
    
    // Configuration parameters (loaded from config.ini)
    int m_maxBufferSize;
    int m_maxRetryCount;
    int m_baseRetryDelayMs;
    int m_maxRetryDelayMs;
    double m_retryBackoffMultiplier;
    bool m_jitterEnabled;
    bool m_validationEnabled;
    int m_maxStringLength;
    int m_maxNumericPrecision;
    bool m_sanitizationEnabled;
    bool m_strictValidation;
    int m_socketWriteTimeout;
    int m_socketReconnectDelay;
    int m_socketMaxReconnectAttempts;
    int m_bufferFlushInterval;
    bool m_bufferPersistenceEnabled;
    
    // InfluxDB Address Format Settings
    QString m_influxAddressFormat;
    bool m_includeDataTypeTags;
    
    // Monitoring and alerting configuration
    bool m_alertingEnabled;
    double m_minSuccessRateThreshold;
    qint64 m_maxConsecutiveFailuresThreshold;
    qint64 m_maxResponseTimeThreshold;
    qint64 m_maxBufferSizeThreshold;
    qint64 m_alertCheckInterval;
    
    // Alert storage
    QVector<SystemAlert> m_activeAlerts;
    QVector<SystemAlert> m_alertHistory;
    QMap<QString, qint64> m_lastDataPointTime;
    QMap<QString, qint64> m_missedReadingsCount;
    qint64 m_lastHealthCheck;
    bool m_systemHealthy;
    
    // Private methods
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
    QMap<QString, QString> prepareStandardInfluxTags(const DataAcquisitionPoint &sourcePoint, const QMap<QString, QString> &existingTags);
    QString modbusDataTypeToString(ModbusDataType dataType);
    bool validateAndSanitizeDataPoint(AcquiredDataPoint &dataPoint);
    QString sanitizeInfluxIdentifier(const QString &identifier);
    QString sanitizeInfluxTagValue(const QString &tagValue);
    void debugDataPointProcessing(const DataAcquisitionPoint &sourcePoint, const AcquiredDataPoint &dataPoint);
    void bufferFailedDataPoint(const AcquiredDataPoint &dataPoint);
    void processInfluxBuffer();
    qint64 calculateRetryDelay(int retryCount);
    bool loadDataResilienceConfiguration(const QString &configPath = "config/config.ini");
    void initializeInfluxWorker();
    
    // Unified acquisition helper functions
    bool prepareAcquisitionStandard(const DataAcquisitionPoint &point, ReadMode mode);
    ModbusReadResult executeModbusRead(const DataAcquisitionPoint &point, ReadMode mode, const QVector<DataAcquisitionPoint> &blockPoints);
    QVector<AcquiredDataPoint> processAcquisitionResult(const ModbusReadResult &result, const DataAcquisitionPoint &point, ReadMode mode, 
                                 const QVector<DataAcquisitionPoint> &blockPoints = QVector<DataAcquisitionPoint>());
    void formatDataPointStandard(AcquiredDataPoint &dataPoint, const DataAcquisitionPoint &sourcePoint);
    
    // Performance optimization functions
    QVector<AcquiredDataPoint> acquireDataBatch(const QVector<DataAcquisitionPoint> &points, ReadMode defaultMode = ReadMode::SINGLE_READ);
    bool isOptimalForBlockRead(const QVector<DataAcquisitionPoint> &points);
    QVector<AcquiredDataPoint> acquireDataOptimized(const QVector<DataAcquisitionPoint> &points);
    
    // Monitoring and alerting
    void checkSystemHealth();
    void generateAlert(AlertType type, AlertSeverity severity, const QString &message, const QString &source = QString(), const QMap<QString, QString> &metadata = QMap<QString, QString>());
    void clearAlert(AlertType type, const QString &source);
    bool isAlertActive(AlertType type, const QString &source = QString()) const;
    void updateMonitoringMetrics();
    void checkDataLoss();
    void checkConnectionHealth();
    void checkBufferHealth();
    void checkResponseTimes();
    QString alertTypeToString(AlertType type) const;
    QString alertSeverityToString(AlertSeverity severity) const;
};

// Worker thread class for asynchronous InfluxDB operations
class InfluxDBWorker : public QObject
{
    Q_OBJECT

public:
    explicit InfluxDBWorker(QObject *parent = nullptr);
    ~InfluxDBWorker();
    
    void setConfiguration(const QString &socketPath, int maxRetryCount, int baseRetryDelayMs,
                         int maxRetryDelayMs, double retryBackoffMultiplier, bool jitterEnabled);

public slots:
    void writeDataPoint(const AcquiredDataPoint &dataPoint);
    void processBufferedDataPoint(const BufferedDataPoint &bufferedPoint);
    void stop();

signals:
    void writeCompleted(const QString &pointName, bool success, const QString &error);
    void statisticsUpdated(qint64 socketErrors);

private:
    bool writeToTelegrafSocketAsync(const QString &socketPath, const QByteArray &message, int maxRetries);
    qint64 calculateRetryDelay(int retryCount) const;
    QByteArray formatInfluxLineProtocol(const AcquiredDataPoint &dataPoint);
    QString escapeInfluxTagKey(const QString &key);
    QString escapeInfluxTagValue(const QString &value);
    
    QString m_socketPath;
    int m_maxRetryCount;
    int m_baseRetryDelayMs;
    int m_maxRetryDelayMs;
    double m_retryBackoffMultiplier;
    bool m_jitterEnabled;
    bool m_stopRequested;
    QMutex m_configMutex;
};

#endif // SCADA_CORE_SERVICE_H
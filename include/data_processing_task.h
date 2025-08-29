#ifndef DATA_PROCESSING_TASK_H
#define DATA_PROCESSING_TASK_H

#include <QRunnable>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QThreadPool>
#include "modbusmanager.h"

// Forward declarations
class ScadaCoreService;

// Data structures (moved from scada_core_service.h to avoid circular dependency)
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

/**
 * @brief Parallel data processing task for decoding Modbus responses and mapping to InfluxDB
 * 
 * This class implements QRunnable to enable parallel processing of Modbus response data
 * from multiple devices simultaneously. Each task handles:
 * 1. Raw data decoding based on ModbusDataType
 * 2. Value conversion and validation
 * 3. InfluxDB tag mapping and formatting
 * 4. Thread-safe result delivery
 */
class DataProcessingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Data Processing Task
     * @param requestId Unique request identifier
     * @param result Modbus read result containing raw data
     * @param point Data acquisition point configuration
     * @param service Pointer to ScadaCoreService for result delivery
     */
    explicit DataProcessingTask(qint64 requestId, 
                               const ModbusReadResult &result,
                               const DataAcquisitionPoint &point,
                               ScadaCoreService *service);
    
    /**
     * @brief Main processing function executed in thread pool
     * 
     * Performs parallel data decoding and InfluxDB mapping:
     * - Decodes raw Modbus data based on data type (Float32, Double64, etc.)
     * - Validates data integrity and range
     * - Creates AcquiredDataPoint with proper InfluxDB tags
     * - Emits results through thread-safe signal mechanism
     */
    void run() override;
    
    /**
     * @brief Get the device identifier for this processing task
     * @return QString Device key (host:port)
     */
    QString getDeviceKey() const;
    
    /**
     * @brief Get the request ID for this processing task
     * @return qint64 Request identifier
     */
    qint64 getRequestId() const;

signals:
    /**
     * @brief Emitted when data processing is completed successfully
     * @param requestId Request identifier
     * @param dataPoint Processed and validated data point
     * @param deviceKey Device identifier
     */
    void dataProcessingCompleted(qint64 requestId, const AcquiredDataPoint &dataPoint, const QString &deviceKey);
    
    /**
     * @brief Emitted when data processing fails
     * @param requestId Request identifier
     * @param errorMessage Error description
     * @param deviceKey Device identifier
     */
    void dataProcessingFailed(qint64 requestId, const QString &errorMessage, const QString &deviceKey);

private:
    /**
     * @brief Decode raw Modbus data based on data type
     * @param rawData Raw register values from Modbus response
     * @param dataType Target data type for conversion
     * @param offset Starting offset in raw data array
     * @return QVariant Decoded value or invalid QVariant on error
     */
    QVariant decodeModbusData(const QVector<quint16> &rawData, ModbusDataType dataType, int offset = 0);
    
    /**
     * @brief Validate decoded data value and range
     * @param value Decoded data value
     * @param dataType Expected data type
     * @return bool True if validation passes
     */
    bool validateDataValue(const QVariant &value, ModbusDataType dataType);
    
    /**
     * @brief Create InfluxDB tags for the data point
     * @param point Source data acquisition point
     * @return QMap<QString, QString> InfluxDB tag map
     */
    QMap<QString, QString> createInfluxTags(const DataAcquisitionPoint &point);
    
    /**
     * @brief Get read mode string based on data type
     * @param dataType Modbus data type
     * @return QString Read mode description
     */
    QString getReadModeString(ModbusDataType dataType);

private:
    qint64 m_requestId;                    ///< Request identifier
    ModbusReadResult m_result;             ///< Modbus read result
    DataAcquisitionPoint m_point;          ///< Data acquisition configuration
    ScadaCoreService *m_service;           ///< Service pointer for result delivery
    QString m_deviceKey;                   ///< Device identifier (host:port)
};

/**
 * @brief Parallel data processing coordinator
 * 
 * Manages parallel data processing tasks using QThreadPool to ensure
 * simultaneous processing of responses from multiple devices.
 */
class ParallelDataProcessor : public QObject
{
    Q_OBJECT

public:
    explicit ParallelDataProcessor(QObject *parent = nullptr);
    ~ParallelDataProcessor();
    
    /**
     * @brief Submit data processing task for parallel execution
     * @param requestId Request identifier
     * @param result Modbus read result
     * @param point Data acquisition point
     * @param service ScadaCoreService pointer
     */
    void submitProcessingTask(qint64 requestId,
                             const ModbusReadResult &result,
                             const DataAcquisitionPoint &point,
                             ScadaCoreService *service);
    
    /**
     * @brief Get current active task count
     * @return int Number of active processing tasks
     */
    int getActiveTaskCount() const;
    
    /**
     * @brief Set maximum number of parallel processing threads
     * @param maxThreads Maximum thread count (default: 4)
     */
    void setMaxThreads(int maxThreads);
    
    /**
     * @brief Wait for all active tasks to complete
     * @param timeoutMs Timeout in milliseconds (default: 5000)
     * @return bool True if all tasks completed within timeout
     */
    bool waitForCompletion(int timeoutMs = 5000);

signals:
    /**
     * @brief Emitted when a processing task completes
     * @param requestId Request identifier
     * @param dataPoint Processed data point
     * @param deviceKey Device identifier
     */
    void taskCompleted(qint64 requestId, const AcquiredDataPoint &dataPoint, const QString &deviceKey);
    
    /**
     * @brief Emitted when a processing task fails
     * @param requestId Request identifier
     * @param errorMessage Error description
     * @param deviceKey Device identifier
     */
    void taskFailed(qint64 requestId, const QString &errorMessage, const QString &deviceKey);

private slots:
    /**
     * @brief Handle completion of individual processing tasks
     */
    void onTaskCompleted(qint64 requestId, const AcquiredDataPoint &dataPoint, const QString &deviceKey);
    
    /**
     * @brief Handle failure of individual processing tasks
     */
    void onTaskFailed(qint64 requestId, const QString &errorMessage, const QString &deviceKey);

private:
    QThreadPool *m_threadPool;             ///< Thread pool for parallel processing
    mutable QMutex m_taskCountMutex;       ///< Mutex for task counting
    QWaitCondition m_completionCondition;  ///< Condition for waiting completion
    int m_activeTaskCount;                 ///< Current active task count
};

#endif // DATA_PROCESSING_TASK_H
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
    
private slots:
    void onPollTimer();
    void onModbusReadCompleted(const ModbusReadResult &result);
    void onModbusWriteCompleted(const ModbusWriteResult &result);
    void onModbusConnectionStateChanged(bool connected);
    void onModbusError(const QString &error);
    // Socket connection methods removed (using direct Unix socket calls)
    
private:
    // Core components
    QTimer *m_pollTimer;
    ModbusManager *m_modbusManager;
    // Unix socket implementation (no QLocalSocket needed)
    
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
};

#endif // SCADA_CORE_SERVICE_H
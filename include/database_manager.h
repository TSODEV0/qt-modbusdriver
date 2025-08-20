#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QSettings>
#include "scada_core_service.h"

struct ModbusDeviceConfig {
    int deviceId;
    QString deviceName;
    QString ipAddress;
    int port;
    int unitId;
    QString protocol;
    int pollInterval;
    bool enabled;
    
    ModbusDeviceConfig() : deviceId(0), port(502), unitId(1), protocol("TCP"), pollInterval(3000), enabled(true) {}
};

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    // Configuration loading
    bool loadConfigurationFromFile(const QString &configPath = "/home/Pttaov1/TSO_SCADA/qtworkplace/modbusdriver/config/config.ini");
    
    // Database connection
    bool connectToDatabase();
    bool connectToDatabase(const QString &host, const QString &database, 
                          const QString &username, const QString &password, 
                          int port = 5432);
    void disconnectFromDatabase();
    bool isConnected() const;
    
    // Execution mode setting
    void setExecutionMode(const QString &mode);
    QString getExecutionMode() const;
    
    // Configuration loading
    QVector<ModbusDeviceConfig> loadModbusDevices();
    QVector<DataAcquisitionPoint> loadDataPoints();
    QVector<DataAcquisitionPoint> optimizeModbusReadBlocks(const QVector<DataAcquisitionPoint> &dataPoints);
    bool updateDeviceStatus(int deviceId, bool online);
    
    // Error handling
    QString lastError() const;
    
signals:
    void databaseConnected();
    void databaseDisconnected();
    void configurationLoaded();
    void errorOccurred(const QString &error);
    
private:
    QSqlDatabase m_database;
    QString m_lastError;
    QString m_connectionName;
    QSettings *m_settings;
    
    // Database connection parameters from config
    QString m_dbHost;
    QString m_dbName;
    QString m_dbUsername;
    QString m_dbPassword;
    int m_dbPort;
    QString m_executionMode;
    
    void setLastError(const QString &error);
    
    // Helper methods for block optimization
    bool isDataTypeCompatibleForBlock(ModbusDataType type1, ModbusDataType type2) const;
    int getDataTypeRegisterSize(ModbusDataType dataType) const;
};

#endif // DATABASE_MANAGER_H
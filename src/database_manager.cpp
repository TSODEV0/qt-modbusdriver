#include "../include/database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QUuid>
#include <QSettings>
#include <QFileInfo>
#include <algorithm>

// Constructor initializes settings pointer
DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_connectionName(QUuid::createUuid().toString())
    , m_settings(nullptr)  // QSettings pointer initialized
    , m_dbPort(5432)
    , m_executionMode("multiple")  // Default to multiple device mode
{
    m_database = QSqlDatabase::addDatabase("QPSQL", m_connectionName);
}

DatabaseManager::~DatabaseManager()
{
    disconnectFromDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
    if (m_settings) {
        delete m_settings;
    }
}

bool DatabaseManager::loadConfigurationFromFile(const QString &configPath)
{
    if (m_settings) {
        delete m_settings;
    }
    
    QFileInfo configFile(configPath);
    if (!configFile.exists()) {
        setLastError("Configuration file not found: " + configPath);
        return false;
    }
    
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    
    // Load database configuration from [Database] section
    m_settings->beginGroup("Database");
    m_dbHost = m_settings->value("host", "localhost").toString();
    m_dbName = m_settings->value("database", "postgres").toString();
    m_dbUsername = m_settings->value("username", "postgres").toString();
    m_dbPassword = m_settings->value("password", "postgres").toString();
    m_dbPort = m_settings->value("port", 5432).toInt();
    m_settings->endGroup();
    
    // Configuration loaded successfully
    
    emit configurationLoaded();
    return true;
}

bool DatabaseManager::connectToDatabase()
{
    if (!m_settings) {
        setLastError("Configuration not loaded. Call loadConfigurationFromFile() first.");
        return false;
    }
    
    return connectToDatabase(m_dbHost, m_dbName, m_dbUsername, m_dbPassword, m_dbPort);
}

bool DatabaseManager::connectToDatabase(const QString &host, const QString &database,
                                      const QString &username, const QString &password,
                                      int port)
{
    m_database.setHostName(host);
    m_database.setDatabaseName(database);
    m_database.setUserName(username);
    m_database.setPassword(password);
    m_database.setPort(port);
    
    if (m_database.open()) {
        emit databaseConnected();
        return true;
    } else {
        setLastError("Failed to connect to database: " + m_database.lastError().text());
        return false;
    }
}

void DatabaseManager::disconnectFromDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
        emit databaseDisconnected();
    }
}

bool DatabaseManager::isConnected() const
{
    return m_database.isOpen();
}

QVector<ModbusDeviceConfig> DatabaseManager::loadModbusDevices()
{
    QVector<ModbusDeviceConfig> devices;
    
    if (!isConnected()) {
        setLastError("Database not connected");
        return devices;
    }
    
    QSqlQuery query(m_database);
    QString deviceFilter = (m_executionMode == "single") ? "device_id IN (2)" : "device_id IN (2,3)";
    QString sql = QString("SELECT device_id, device_name, ip_address, port, unit_id, protocol_type, pollinterval "
                         "FROM devices "
                         "WHERE protocol_type = 'TCP' AND %1 "
                         "ORDER BY device_id").arg(deviceFilter);
    
    if (query.exec(sql)) {
        while (query.next()) {
            ModbusDeviceConfig config;
            config.deviceId = query.value("device_id").toInt();
            config.deviceName = query.value("device_name").toString();
            config.ipAddress = query.value("ip_address").toString();
            config.port = query.value("port").toInt();
            config.unitId = query.value("unit_id").toInt();
            config.protocol = query.value("protocol_type").toString();
            config.pollInterval = query.value("pollinterval").toInt();
            config.enabled = true;
            
            devices.append(config);
            
            // Device loaded successfully
        }
        
        // Devices loaded successfully
    } else {
        setLastError("Failed to load devices: " + query.lastError().text());
    }
    
    return devices;
}

QVector<DataAcquisitionPoint> DatabaseManager::loadDataPoints()
{
    QVector<DataAcquisitionPoint> dataPoints;
    
    if (!isConnected()) {
        setLastError("Database not connected");
        return dataPoints;
    }
    
    // Load devices first to get IP addresses and ports
    QVector<ModbusDeviceConfig> devices = loadModbusDevices();
    QMap<int, ModbusDeviceConfig> deviceMap;
    for (const auto &device : devices) {
        deviceMap[device.deviceId] = device;
    }
    
    QSqlQuery query(m_database);
    QString deviceFilter = (m_executionMode == "single") ? "t.device_id IN (2)" : "t.device_id IN (2,3)";
    QString sql = QString("SELECT t.tag_id, t.device_id, t.tag_name, t.register_type, t.register_address, t.data_type, t.description, t.influx_measurement, "
                         "       d.device_name, d.ip_address, d.port, d.unit_id, d.protocol_type, d.pollinterval "
                         "FROM public.tags t "
                         "JOIN public.devices d ON t.device_id = d.device_id "
                         "WHERE %1 "
                         "ORDER BY t.device_id, t.tag_name").arg(deviceFilter);
    
    if (query.exec(sql)) {
        while (query.next()) {
            DataAcquisitionPoint point;
            
            // Device information from JOIN
            QString deviceName = query.value("device_name").toString();
            QString tagName = query.value("tag_name").toString();
            QString dataTypeStr = query.value("data_type").toString();
            QString description = query.value("description").toString();
            QString registerType = query.value("register_type").toString();
            
            point.name = QString("%1_%2").arg(deviceName).arg(tagName);
            point.host = query.value("ip_address").toString();
            point.port = query.value("port").toInt();
            point.address = query.value("register_address").toInt() - 1;  // Convert to zero-based addressing
            point.pollInterval = query.value("pollinterval").toInt();
            point.measurement = query.value("influx_measurement").toString();
            point.enabled = true;
            
            // Set data type conversion (handle database format)
            if (dataTypeStr == "FLOAT32" || dataTypeStr == "Float32") {
                point.dataType = ModbusDataType::Float32;
            } else if (dataTypeStr == "DOUBLE" || dataTypeStr == "Double64") {
                point.dataType = ModbusDataType::Double64;
            } else if (dataTypeStr == "INT16" || dataTypeStr == "Int16") {
                point.dataType = ModbusDataType::HoldingRegister;
            } else if (dataTypeStr == "INT32" || dataTypeStr == "Int32") {
                point.dataType = ModbusDataType::Long32;
            } else if (dataTypeStr == "INT64" || dataTypeStr == "Int64") {
                point.dataType = ModbusDataType::Long64;
            } else if (dataTypeStr == "COIL" || dataTypeStr == "Coil") {
                point.dataType = ModbusDataType::Coil;
            } else if (dataTypeStr == "DISCRETE_INPUT" || dataTypeStr == "DiscreteInput") {
                point.dataType = ModbusDataType::DiscreteInput;
            } else if (dataTypeStr == "BOOL" || dataTypeStr == "Bool" || dataTypeStr == "Boolean") {
                point.dataType = ModbusDataType::BOOL;
            } else {
                point.dataType = ModbusDataType::HoldingRegister; // Default
            }
            
            // Set tags
            point.tags["device_name"] = deviceName;
            point.tags["tag_name"] = tagName;
            point.tags["description"] = description;
            point.tags["register_type"] = registerType;
            point.tags["data_type"] = dataTypeStr;  // Add data_type tag for combined conditions
            point.tags["unit_id"] = query.value("unit_id").toString();
            point.tags["protocol_type"] = query.value("protocol_type").toString();
            point.tags["station_name"] = "field_site";
            point.tags["address"] = QString::number(point.address);  // Add address tag to prevent default "0" in InfluxDB
            
            dataPoints.append(point);
        }
        
        // Data points loaded successfully
    } else {
        setLastError("Failed to load data points: " + query.lastError().text());
    }
    
    return dataPoints;
}

QVector<DataAcquisitionPoint> DatabaseManager::optimizeModbusReadBlocks(const QVector<DataAcquisitionPoint> &dataPoints)
{
    QVector<DataAcquisitionPoint> optimizedPoints;
    
    // Group data points by device (host:port), register type, and data type for prioritization
    QMap<QString, QVector<DataAcquisitionPoint>> deviceRegisterDataTypeGroups;
    
    for (const DataAcquisitionPoint &point : dataPoints) {
        QString registerTypeStr;
        QString dataTypeStr;
        int priority = 0;
        
        switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
                registerTypeStr = "HOLDING";
                dataTypeStr = "INT16";
                priority = 1; // Highest priority
                break;
            case ModbusDataType::Float32:
                registerTypeStr = "HOLDING";
                dataTypeStr = "FLOAT32";
                priority = 2; // Second priority
                break;
            case ModbusDataType::Double64:
                registerTypeStr = "HOLDING";
                dataTypeStr = "DOUBLE64";
                priority = 3; // Third priority
                break;
            case ModbusDataType::Long32:
                registerTypeStr = "HOLDING";
                dataTypeStr = "LONG32";
                priority = 2; // Same as FLOAT32
                break;
            case ModbusDataType::Long64:
                registerTypeStr = "HOLDING";
                dataTypeStr = "LONG64";
                priority = 3; // Same as DOUBLE64
                break;
            case ModbusDataType::InputRegister:
                registerTypeStr = "INPUT";
                dataTypeStr = "INPUT";
                priority = 1;
                break;
            case ModbusDataType::Coil:
                registerTypeStr = "COIL";
                dataTypeStr = "COIL";
                priority = 1;
                break;
            case ModbusDataType::DiscreteInput:
                registerTypeStr = "DISCRETE";
                dataTypeStr = "DISCRETE";
                priority = 1;
                break;
        }
        
        // Create key with priority prefix for sorting: device_register_datatype_priority
        QString deviceRegisterDataTypeKey = QString("%1:%2_%3_%4_%5")
            .arg(point.host).arg(point.port).arg(registerTypeStr).arg(dataTypeStr).arg(priority, 2, 10, QChar('0'));
        deviceRegisterDataTypeGroups[deviceRegisterDataTypeKey].append(point);
    }
    
    // Process each device-register-datatype group in priority order
    QStringList sortedKeys = deviceRegisterDataTypeGroups.keys();
    std::sort(sortedKeys.begin(), sortedKeys.end()); // This will sort by priority due to key format
    
    for (const QString &key : sortedKeys) {
        QVector<DataAcquisitionPoint> devicePoints = deviceRegisterDataTypeGroups[key];
        
        // Processing data type group
        
        // Sort points by register address
        std::sort(devicePoints.begin(), devicePoints.end(), 
                  [](const DataAcquisitionPoint &a, const DataAcquisitionPoint &b) {
                      return a.address < b.address;
                  });
        
        // Create optimized read blocks with data type compatibility
        int i = 0;
        while (i < devicePoints.size()) {
            DataAcquisitionPoint blockPoint = devicePoints[i];
            int startAddress = blockPoint.address;
            int currentRegisterSize = getDataTypeRegisterSize(blockPoint.dataType);
            int endAddress = startAddress + currentRegisterSize - 1;
            ModbusDataType blockDataType = blockPoint.dataType;
            
            // Find consecutive addresses with compatible data types
            int j = i + 1;
            while (j < devicePoints.size()) {
                int nextAddress = devicePoints[j].address;
                int nextRegisterSize = getDataTypeRegisterSize(devicePoints[j].dataType);
                ModbusDataType nextDataType = devicePoints[j].dataType;
                
                // Check data type compatibility for block optimization
                bool dataTypeCompatible = isDataTypeCompatibleForBlock(blockDataType, nextDataType);
                
                // Calculate gap considering register sizes
                int gap = nextAddress - (endAddress + 1);
                int nextEndAddress = nextAddress + nextRegisterSize - 1;
                int totalBlockSize = nextEndAddress - startAddress + 1;
                
                // Allow small gaps (up to 3 registers for mixed types, 5 for same types) to create larger blocks
                // But limit total block size to 125 registers (Modbus limit)
                int maxGap = dataTypeCompatible ? 5 : 3;
                if (gap <= maxGap && totalBlockSize <= 125 && dataTypeCompatible) {
                    endAddress = nextEndAddress;
                    j++;
                } else {
                    break;
                }
            }
            
            // Create optimized block point
            if (j > i + 1) {
                // Multiple points in block - create a block read point
                DataAcquisitionPoint optimizedBlock = blockPoint;
                int blockSize = endAddress - startAddress + 1;
                optimizedBlock.name = QString("%1_BLOCK_%2_%3")
                                     .arg(blockPoint.tags["device_name"])
                                     .arg(startAddress)
                                     .arg(endAddress);
                optimizedBlock.address = startAddress;
                optimizedBlock.tags["block_size"] = QString::number(blockSize);
                optimizedBlock.tags["block_start_address"] = QString::number(startAddress);
                optimizedBlock.tags["block_end_address"] = QString::number(endAddress);
                optimizedBlock.tags["block_type"] = "optimized_read";
                optimizedBlock.tags["original_points"] = QString::number(j - i);
                
                // Add priority information for polling order
                QString keyParts = key.split('_').last();
                optimizedBlock.tags["data_type_priority"] = keyParts;
                
                // Extract data type from key for block identification
                QStringList keyComponents = key.split('_');
                if (keyComponents.size() >= 3) {
                    optimizedBlock.tags["block_data_type"] = keyComponents[keyComponents.size()-2];
                }
                
                // Store original point addresses and metadata for data extraction
                QStringList originalAddresses;
                QStringList originalNames;
                QStringList originalDataTypes;
                QStringList originalDescriptions;
                QStringList originalMeasurements;
                
                for (int k = i; k < j; k++) {
                    originalAddresses << QString::number(devicePoints[k].address);
                    originalNames << devicePoints[k].name;
                    // Use original string data type from database instead of enum integer
                    originalDataTypes << devicePoints[k].tags.value("data_type", "UNKNOWN");
                    originalDescriptions << devicePoints[k].tags.value("description", QString("CURRENT_RTU_%1").arg(devicePoints[k].address));
                    originalMeasurements << devicePoints[k].measurement;
                }
                
                optimizedBlock.tags["original_addresses"] = originalAddresses.join(",");
                optimizedBlock.tags["original_names"] = originalNames.join(",");
                optimizedBlock.tags["original_data_types"] = originalDataTypes.join(",");
                optimizedBlock.tags["original_descriptions"] = originalDescriptions.join(",");
                optimizedBlock.tags["original_measurements"] = originalMeasurements.join(",");
                
                optimizedPoints.append(optimizedBlock);
                
                // Optimized block created
            } else {
                // Single point - add as is
                optimizedPoints.append(blockPoint);
            }
            
            i = j;
        }
    }
    
    // Modbus read optimization completed
    
    return optimizedPoints;
}

bool DatabaseManager::updateDeviceStatus(int deviceId, bool online)
{
    if (!isConnected()) {
        setLastError("Database not connected");
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("UPDATE devices SET last_seen = NOW(), online_status = ? WHERE device_id = ?");
    query.addBindValue(online);
    query.addBindValue(deviceId);
    
    if (query.exec()) {
        return true;
    } else {
        setLastError("Failed to update device status: " + query.lastError().text());
        return false;
    }
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

void DatabaseManager::setLastError(const QString &error)
{
    m_lastError = error;
    emit errorOccurred(error);
}

void DatabaseManager::setExecutionMode(const QString &mode)
{
    m_executionMode = mode;
}

QString DatabaseManager::getExecutionMode() const
{
    return m_executionMode;
}

// Helper methods for block optimization
bool DatabaseManager::isDataTypeCompatibleForBlock(ModbusDataType type1, ModbusDataType type2) const
{
    // Group compatible data types for block reading
    // All holding register types can be read together
    QSet<ModbusDataType> holdingRegisterTypes = {
        ModbusDataType::HoldingRegister,
        ModbusDataType::Float32,
        ModbusDataType::Double64,
        ModbusDataType::Long32,
        ModbusDataType::Long64
    };
    
    // Input registers are separate
    QSet<ModbusDataType> inputRegisterTypes = {
        ModbusDataType::InputRegister
    };
    
    // Coils are separate
    QSet<ModbusDataType> coilTypes = {
        ModbusDataType::Coil
    };
    
    // Discrete inputs are separate
    QSet<ModbusDataType> discreteInputTypes = {
        ModbusDataType::DiscreteInput,
        ModbusDataType::BOOL
    };
    
    // Check if both types belong to the same register category
    if (holdingRegisterTypes.contains(type1) && holdingRegisterTypes.contains(type2)) {
        return true;
    }
    if (inputRegisterTypes.contains(type1) && inputRegisterTypes.contains(type2)) {
        return true;
    }
    if (coilTypes.contains(type1) && coilTypes.contains(type2)) {
        return true;
    }
    if (discreteInputTypes.contains(type1) && discreteInputTypes.contains(type2)) {
        return true;
    }
    
    return false;
}

int DatabaseManager::getDataTypeRegisterSize(ModbusDataType dataType) const
{
    switch (dataType) {
        case ModbusDataType::HoldingRegister:
        case ModbusDataType::InputRegister:
        case ModbusDataType::Coil:
        case ModbusDataType::DiscreteInput:
        case ModbusDataType::BOOL:
            return 1; // 1 register (16-bit)
        case ModbusDataType::Float32:
        case ModbusDataType::Long32:
            return 2; // 2 registers (32-bit)
        case ModbusDataType::Double64:
        case ModbusDataType::Long64:
            return 4; // 4 registers (64-bit)
        default:
            return 1; // Default to 1 register
    }
}
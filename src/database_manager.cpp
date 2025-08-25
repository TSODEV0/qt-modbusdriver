#include "../include/database_manager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QUuid>
#include <algorithm>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_dbPort(5432)
{
    m_connectionName = QUuid::createUuid().toString();
}

DatabaseManager::~DatabaseManager()
{
    disconnectFromDatabase();
    if (m_settings) {
        delete m_settings;
    }
}

bool DatabaseManager::loadConfigurationFromFile(const QString &configPath)
{
    if (m_settings) {
        delete m_settings;
    }
    
    m_settings = new QSettings(configPath, QSettings::IniFormat);
    
    if (m_settings->status() != QSettings::NoError) {
        setLastError(QString("Failed to load configuration file: %1").arg(configPath));
        return false;
    }
    
    // Load database configuration
    m_settings->beginGroup("Database");
    m_dbHost = m_settings->value("host", "localhost").toString();
    m_dbName = m_settings->value("database", "postgres").toString();
    m_dbUsername = m_settings->value("username", "postgres").toString();
    m_dbPassword = m_settings->value("password", "").toString();
    m_dbPort = m_settings->value("port", 5432).toInt();
    m_settings->endGroup();
    
    emit configurationLoaded();
    return true;
}

bool DatabaseManager::connectToDatabase()
{
    return connectToDatabase(m_dbHost, m_dbName, m_dbUsername, m_dbPassword, m_dbPort);
}

bool DatabaseManager::connectToDatabase(const QString &host, const QString &database,
                                      const QString &username, const QString &password, int port)
{
    if (m_database.isOpen()) {
        disconnectFromDatabase();
    }
    
    m_database = QSqlDatabase::addDatabase("QPSQL", m_connectionName);
    m_database.setHostName(host);
    m_database.setDatabaseName(database);
    m_database.setUserName(username);
    m_database.setPassword(password);
    m_database.setPort(port);
    
    if (!m_database.open()) {
        qWarning() << "Database connection failed, running in offline mode:" << m_database.lastError().text();
        return false;
    }
    
    emit databaseConnected();
    return true;
}

void DatabaseManager::disconnectFromDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
        emit databaseDisconnected();
    }
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool DatabaseManager::isConnected() const
{
    return m_database.isOpen();
}

QVector<ModbusDeviceConfig> DatabaseManager::loadModbusDevices()
{
    QVector<ModbusDeviceConfig> devices;
    
    if (!isConnected()) {
        qDebug() << "Database not connected, returning empty device list";
        return devices;
    }
    
    QSqlQuery query(m_database);
    QString sql = "SELECT device_id, device_name, ip_address, port, unit_id, protocol_type, pollinterval "
                  "FROM devices "
                  "WHERE protocol_type = 'TCP' "
                  "ORDER BY device_id";
    
    if (!query.exec(sql)) {
        qWarning() << "Failed to load Modbus devices (table may not exist):" << query.lastError().text();
        return devices;
    }
    
    while (query.next()) {
        ModbusDeviceConfig device;
        device.deviceId = query.value(0).toInt();
        device.deviceName = query.value(1).toString();
        device.ipAddress = query.value(2).toString();
        device.port = query.value(3).toInt();
        device.unitId = query.value(4).toInt();
        device.protocol = query.value(5).toString();
        device.pollInterval = query.value(6).toInt();
        device.enabled = true; // All selected devices are enabled by filter
        devices.append(device);
    }
    
    return devices;
}

QVector<DataAcquisitionPoint> DatabaseManager::loadDataPoints(const QString &executionMode)
{
    QVector<DataAcquisitionPoint> dataPoints;
    
    if (!isConnected()) {
        qDebug() << "Database not connected, returning empty data points list";
        return dataPoints;
    }
    
    QSqlQuery query(m_database);
    // Dynamic device filtering based on execution mode
    QString deviceFilter;
    if (executionMode == "single") {
        // For single mode, use only device_id = 2
        deviceFilter = "d.protocol_type = 'TCP' AND d.device_id = 2";
    } else {
        // For multiple mode, use device_id = 2 and 3
        deviceFilter = "d.protocol_type = 'TCP' AND d.device_id IN (2, 3)";
    }
    
    QString sql;
    if (executionMode == "single") {
        // For single mode, get tags from device_id = 2 only
        // Enhanced query with MAX function and optimized sorting for grouping
        sql = QString("SELECT t.tag_id, t.device_id, t.tag_name, t.register_type, t.register_address, t.data_type, t.description, t.influx_measurement, "
                     "       d.device_name, d.ip_address, d.port, d.unit_id, d.protocol_type, d.pollinterval, "
                     "       MAX(t.register_address) OVER (PARTITION BY d.device_id, t.register_type, t.data_type) as max_address_in_group "
                     "FROM public.tags t "
                     "JOIN public.devices d ON t.device_id = d.device_id "
                     "WHERE %1 "
                     "ORDER BY d.device_id, t.register_type, t.data_type, t.register_address").arg(deviceFilter);
    } else {
        // For multiple mode, get tags from device_id = 2 and 3 with enhanced grouping
        // Reorganized by data types and register types for optimal block creation
        sql = QString("SELECT t.tag_id, t.device_id, t.tag_name, t.register_type, t.register_address, t.data_type, t.description, t.influx_measurement, "
                     "       d.device_name, d.ip_address, d.port, d.unit_id, d.protocol_type, d.pollinterval, "
                     "       MAX(t.register_address) OVER (PARTITION BY d.device_id, t.register_type, t.data_type) as max_address_in_group "
                     "FROM public.tags t "
                     "JOIN public.devices d ON t.device_id = d.device_id "
                     "WHERE %1 "
                     "ORDER BY d.device_id, "
                     "         CASE t.register_type "
                     "           WHEN 'HOLDING_REGISTER' THEN 1 "
                     "           WHEN 'STATUS' THEN 2 "
                     "           ELSE 3 END, "
                     "         CASE t.data_type "
                     "           WHEN 'BOOL' THEN 1 "
                     "           WHEN 'INT16' THEN 2 "
                     "           WHEN 'FLOAT32' THEN 3 "
                     "           WHEN 'DOUBLE' THEN 4 "
                     "           ELSE 5 END, "
                     "         t.register_address").arg(deviceFilter);
    }
    
    qDebug() << "Loading data points with execution mode:" << executionMode;
    qDebug() << "Device filter:" << deviceFilter;
    if (executionMode == "single") {
        qDebug() << "Using single device mode - device_id = 2 only";
    } else {
        qDebug() << "Using multiple device mode - device_id = 2 and 3";
    }
    
    if (!query.exec(sql)) {
        qWarning() << "Failed to load data points (table may not exist):" << query.lastError().text();
        return dataPoints;
    }
    
    while (query.next()) {
        DataAcquisitionPoint point;
        point.name = query.value(2).toString(); // t.tag_name
        point.host = query.value(9).toString(); // d.ip_address
        point.port = query.value(10).toInt(); // d.port
        point.address = query.value(4).toInt(); // t.register_address
        
        // Get register_type and data_type from database
        QString registerTypeStr = query.value(3).toString(); // t.register_type
        QString dataTypeStr = query.value(5).toString(); // t.data_type
        
        // Map string data type to enum based on data_type field
        if (dataTypeStr == "INT16") {
            point.dataType = ModbusDataType::HoldingRegister;
        } else if (dataTypeStr == "UINT16") {
            point.dataType = ModbusDataType::HoldingRegister;
        } else if (dataTypeStr == "FLOAT32") {
            point.dataType = ModbusDataType::Float32;
        } else if (dataTypeStr == "DOUBLE") {
            point.dataType = ModbusDataType::Double64;
        } else if (dataTypeStr == "LONG") {
            point.dataType = ModbusDataType::Long32;
        } else if (dataTypeStr == "BOOL") {
            point.dataType = ModbusDataType::BOOL;
        } else {
            point.dataType = ModbusDataType::HoldingRegister; // Default
        }
        
        point.pollInterval = query.value(13).toInt(); // d.pollinterval
        point.measurement = query.value(7).toString(); // t.influx_measurement
        point.enabled = true; // All selected points are enabled by filter
        
        // Set basic tags including register_type and data_type from database
        point.tags["device_name"] = query.value(8).toString(); // d.device_name
        point.tags["address"] = QString::number(point.address);
        point.tags["description"] = query.value(6).toString(); // t.description
        point.tags["unit_id"] = QString::number(query.value(11).toInt()); // d.unit_id
        point.tags["register_type"] = registerTypeStr; // Store register_type from database
        point.tags["data_type"] = dataTypeStr; // Store data_type from database
        
        dataPoints.append(point);
    }
    
    return dataPoints;
}

QVector<DataAcquisitionPoint> DatabaseManager::optimizeModbusReadBlocks(const QVector<DataAcquisitionPoint> &dataPoints)
{
    if (dataPoints.isEmpty()) {
        return dataPoints;
    }
    
    QVector<DataAcquisitionPoint> optimizedPoints;
    
    // Enhanced grouping by device, register type, and data type with priority-based sorting
    QMap<QString, QVector<DataAcquisitionPoint>> dataTypeGroups;
    
    for (const DataAcquisitionPoint &point : dataPoints) {
        QString registerTypeStr = point.tags.value("register_type", "HOLDING_REGISTER");
        QString dataTypeStr = point.tags.value("data_type", "INT16");
        
        // Create hierarchical grouping key with priority ordering
        int registerPriority = (registerTypeStr == "HOLDING_REGISTER") ? 1 : 
                              (registerTypeStr == "STATUS") ? 2 : 3;
        int dataTypePriority = (dataTypeStr == "BOOL") ? 1 :
                              (dataTypeStr == "INT16") ? 2 :
                              (dataTypeStr == "FLOAT32") ? 3 :
                              (dataTypeStr == "DOUBLE") ? 4 : 5;
        
        QString groupKey = QString("%1:%2_%3_%4_%5_%6")
                          .arg(point.host)
                          .arg(point.port)
                          .arg(registerPriority, 2, 10, QChar('0'))  // Zero-padded for proper sorting
                          .arg(dataTypePriority, 2, 10, QChar('0'))
                          .arg(registerTypeStr)
                          .arg(dataTypeStr);
        dataTypeGroups[groupKey].append(point);
    }
    
    qDebug() << "📊 Loaded" << dataPoints.size() << "data acquisition points";
    
    // Process each data type group separately
    for (auto it = dataTypeGroups.begin(); it != dataTypeGroups.end(); ++it) {
        QString groupKey = it.key();
        QVector<DataAcquisitionPoint> devicePoints = it.value();
        
        qDebug() << "Processing data type group:" << groupKey << "with" << devicePoints.size() << "points";
        
        // Sort points by address for block optimization
        std::sort(devicePoints.begin(), devicePoints.end(), 
                 [](const DataAcquisitionPoint &a, const DataAcquisitionPoint &b) {
                     return a.address < b.address;
                 });
        
        // Group consecutive addresses into blocks
        QVector<QVector<DataAcquisitionPoint>> blocks;
        QVector<DataAcquisitionPoint> currentBlock;
        
        for (int i = 0; i < devicePoints.size(); ++i) {
            const DataAcquisitionPoint &point = devicePoints[i];
            
            if (currentBlock.isEmpty()) {
                // Start new block
                currentBlock.append(point);
            } else {
                const DataAcquisitionPoint &lastPoint = currentBlock.last();
                int expectedNextAddress = lastPoint.address + getDataTypeRegisterSize(lastPoint.dataType);
                
                // Enhanced block creation with adaptive gap tolerance and memory-efficient sizing
                int addressGap = point.address - expectedNextAddress;
                
                // Adaptive gap tolerance based on data type for optimal memory usage
                int maxGap = (lastPoint.dataType == ModbusDataType::BOOL) ? 20 :  // Smaller gaps for BOOL
                            (lastPoint.dataType == ModbusDataType::Float32 || lastPoint.dataType == ModbusDataType::Double64) ? 30 : // Medium gaps for floating point
                            50; // Larger gaps for integer types
                
                // Dynamic block size limits based on data type for better memory management
                int maxBlockSize = (lastPoint.dataType == ModbusDataType::Double64) ? 200 :  // Smaller blocks for 64-bit data
                                  (lastPoint.dataType == ModbusDataType::Float32) ? 300 :     // Medium blocks for 32-bit data
                                  500; // Larger blocks for 16-bit data
                
                bool canAddToBlock = (addressGap >= 0 && addressGap <= maxGap) &&
                                   isDataTypeCompatibleForBlock(lastPoint.dataType, point.dataType) &&
                                   (currentBlock.size() < maxBlockSize);
                
                if (canAddToBlock) {
                    currentBlock.append(point);
                } else {
                    // Finalize current block and start new one
                    blocks.append(currentBlock);
                    currentBlock.clear();
                    currentBlock.append(point);
                }
            }
        }
        
        // Add the last block
        if (!currentBlock.isEmpty()) {
            blocks.append(currentBlock);
        }
        
        // Enhanced block processing with intelligent size optimization
        for (const QVector<DataAcquisitionPoint> &block : blocks) {
            // Dynamic threshold based on data type for optimal performance
            int individualThreshold = (block.first().dataType == ModbusDataType::Double64) ? 2 :  // Lower threshold for 64-bit data
                                     (block.first().dataType == ModbusDataType::Float32) ? 3 :    // Medium threshold for 32-bit data
                                     4; // Higher threshold for 16-bit data
            
            if (block.size() <= individualThreshold) {
                // Small blocks - add as individual points for better optimization
                for (const DataAcquisitionPoint &point : block) {
                    optimizedPoints.append(point);
                }
            } else {
                // Create block read point
                DataAcquisitionPoint blockPoint = block.first();
                int startAddr = block.first().address;
                int endAddr = block.last().address + getDataTypeRegisterSize(block.last().dataType) - 1;
                int totalSize = endAddr - startAddr + 1;
                
                // Generate block name based on device and address range
                QString devicePrefix = blockPoint.tags.value("device_name", "DEVICE").replace(".", "_");
                blockPoint.name = QString("%1_BLOCK_%2_%3")
                                 .arg(devicePrefix)
                                 .arg(startAddr)
                                 .arg(endAddr);
                
                // Calculate block statistics
                QStringList originalNames, originalAddresses, originalDataTypes;
                QStringList originalDescriptions, originalMeasurements;
                
                for (const DataAcquisitionPoint &point : block) {
                    originalNames.append(point.name);
                    originalAddresses.append(QString::number(point.address));
                    originalDataTypes.append(QString::number(static_cast<int>(point.dataType)));
                    originalDescriptions.append(point.tags.value("description", ""));
                    originalMeasurements.append(point.measurement);
                }
                
                qDebug() << "Created optimized block:" << blockPoint.name 
                         << "Address range:" << startAddr << "-" << endAddr
                         << "Block size:" << totalSize 
                         << "Original points:" << block.size();
                
                // Determine the dominant data type and priority for the block
                ModbusDataType dominantDataType = block.first().dataType;
                QString blockDataTypeStr = "INT16"; // Default
                int blockPriority = 99; // Default
                
                // Set block data type and priority based on dominant type
                // Use database naming conventions to ensure consistency with individual points
                switch (dominantDataType) {
                    case ModbusDataType::HoldingRegister:
                        blockDataTypeStr = "INT16";
                        blockPriority = 1;
                        break;
                    case ModbusDataType::Float32:
                        blockDataTypeStr = "FLOAT32";
                        blockPriority = 2;
                        break;
                    case ModbusDataType::Double64:
                        blockDataTypeStr = "DOUBLE";  // Fixed: Match database value instead of "DOUBLE64"
                        blockPriority = 3;
                        break;
                    case ModbusDataType::Long32:
                        blockDataTypeStr = "LONG";    // Fixed: Match database value instead of "LONG32"
                        blockPriority = 2;
                        break;
                    case ModbusDataType::Long64:
                        blockDataTypeStr = "LONG";    // Fixed: Match database value instead of "LONG64"
                        blockPriority = 3;
                        break;
                    case ModbusDataType::BOOL:
                    case ModbusDataType::Coil:
                    case ModbusDataType::DiscreteInput:
                        blockDataTypeStr = "BOOL";
                        blockPriority = 1;
                        break;
                    case ModbusDataType::InputRegister:
                        blockDataTypeStr = "INT16";
                        blockPriority = 1;
                        break;
                }
                
                // Set block metadata in tags
                blockPoint.tags["block_type"] = "optimized_read";
                blockPoint.tags["block_size"] = QString::number(totalSize);
                blockPoint.tags["original_names"] = originalNames.join(",");
                blockPoint.tags["original_addresses"] = originalAddresses.join(",");
                blockPoint.tags["original_data_types"] = originalDataTypes.join(",");
                blockPoint.tags["original_descriptions"] = originalDescriptions.join(",");
                blockPoint.tags["original_measurements"] = originalMeasurements.join(",");
                blockPoint.tags["register_type"] = "HOLDING_REGISTER"; // Assume holding registers for now
                blockPoint.tags["data_type"] = blockDataTypeStr;
                blockPoint.tags["block_data_type"] = blockDataTypeStr;
                blockPoint.tags["data_type_priority"] = QString::number(blockPriority);
                
                optimizedPoints.append(blockPoint);
            }
        }
    }
    
    // Calculate and display optimization statistics
    int originalCount = dataPoints.size();
    int optimizedCount = optimizedPoints.size();
    double reductionPercent = ((double)(originalCount - optimizedCount) / originalCount) * 100.0;
    
    qDebug() << "Modbus read optimization completed:";
    qDebug() << "Original points:" << originalCount;
    qDebug() << "Optimized points:" << optimizedCount;
    qDebug() << "Reduction:" << QString("\"%1%\"").arg(reductionPercent, 0, 'f', 1);
    qDebug() << "Optimized to" << optimizedCount << "read blocks";
    
    return optimizedPoints;
}

bool DatabaseManager::updateDeviceStatus(int deviceId, bool online)
{
    if (!isConnected()) {
        qDebug() << "Database not connected, skipping device status update";
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("UPDATE devices SET online = :online, last_seen = NOW() WHERE device_id = :device_id");
    query.bindValue(":online", online);
    query.bindValue(":device_id", deviceId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update device status:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

void DatabaseManager::setLastError(const QString &error)
{
    m_lastError = error;
    qWarning() << "DatabaseManager Error:" << error;
    emit errorOccurred(error);
}

bool DatabaseManager::isDataTypeCompatibleForBlock(ModbusDataType type1, ModbusDataType type2) const
{
    // Simplified compatibility check - same register types can be grouped
    return (type1 == type2) || 
           ((type1 == ModbusDataType::HoldingRegister || type1 == ModbusDataType::Float32 || 
             type1 == ModbusDataType::Double64 || type1 == ModbusDataType::Long32 || type1 == ModbusDataType::Long64) &&
            (type2 == ModbusDataType::HoldingRegister || type2 == ModbusDataType::Float32 || 
             type2 == ModbusDataType::Double64 || type2 == ModbusDataType::Long32 || type2 == ModbusDataType::Long64));
}

int DatabaseManager::getDataTypeRegisterSize(ModbusDataType dataType) const
{
    switch (dataType) {
        case ModbusDataType::HoldingRegister:
        case ModbusDataType::InputRegister:
            return 1;
        case ModbusDataType::Float32:
        case ModbusDataType::Long32:
            return 2;
        case ModbusDataType::Double64:
        case ModbusDataType::Long64:
            return 4;
        case ModbusDataType::Coil:
        case ModbusDataType::DiscreteInput:
        case ModbusDataType::BOOL:
            return 1;
        default:
            return 1;
    }
}
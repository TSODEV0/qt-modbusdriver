#include "../include/scada_core_service.h"
#include <QDebug>
#include <QJsonArray>
#include <QThread>
#include <QMetaType>
#include <QCoreApplication>
#include <cstring>
#include <algorithm>

ScadaCoreService::ScadaCoreService(QObject *parent)
    : QObject(parent)
    , m_pollTimer(nullptr)
    , m_modbusManager(nullptr)
    , m_telegrafSocketPath("/tmp/telegraf.sock")
    , m_serviceRunning(false)
    , m_currentPointIndex(0)
{
    // Initialize components
    m_pollTimer = new QTimer(this);
    m_modbusManager = new ModbusManager(this);
    
    // Load configuration for ModbusManager
    if (!m_modbusManager->loadConfigurationFromFile("config/modbus_config.ini")) {
        qWarning() << "Failed to load ModbusManager configuration, using defaults";
    }
    
    // Connect signals
    connect(m_pollTimer, &QTimer::timeout, this, &ScadaCoreService::onPollTimer);
    connect(m_modbusManager, &ModbusManager::readCompleted, 
            this, &ScadaCoreService::onModbusReadCompleted);
    connect(m_modbusManager, &ModbusManager::writeCompleted, 
            this, &ScadaCoreService::onModbusWriteCompleted);
    connect(m_modbusManager, &ModbusManager::connectionStateChanged,
            this, &ScadaCoreService::onModbusConnectionStateChanged);
    connect(m_modbusManager, &ModbusManager::errorOccurred,
            this, &ScadaCoreService::onModbusError);
    
    // Initialize statistics
    resetStatistics();
}

ScadaCoreService::~ScadaCoreService()
{
    stopService();
}

bool ScadaCoreService::startService()
{
    if (m_serviceRunning) {
        qDebug() << "SCADA Core Service is already running";
        return true;
    }
    
    if (m_dataPoints.isEmpty()) {
        emit errorOccurred("No data points configured for acquisition");
        return false;
    }
    
    qDebug() << "Starting SCADA Core Service...";
    qDebug() << "Configured data points:" << m_dataPoints.size();
    qDebug() << "Telegraf socket path:" << m_telegrafSocketPath;
    
    // Note: Using direct Unix socket calls for each data transmission
    
    // Initialize service state
    m_serviceRunning = true;
    m_currentPointIndex = 0;
    m_statistics.serviceStartTime = QDateTime::currentMSecsSinceEpoch();
    
    // Start polling timer (3000ms for stable Modbus connections)
    m_pollTimer->start(3000);
    
    emit serviceStarted();
    qDebug() << "SCADA Core Service started successfully";
    return true;
}

void ScadaCoreService::stopService()
{
    if (!m_serviceRunning) {
        return;
    }
    
    qDebug() << "Stopping SCADA Core Service...";
    
    m_serviceRunning = false;
    m_pollTimer->stop();
    
    // Disconnect from Modbus
    if (m_modbusManager) {
        m_modbusManager->disconnectFromServer();
    }
    
    // Note: Using direct Unix socket calls, no persistent connection needed
    
    emit serviceStopped();
    qDebug() << "SCADA Core Service stopped";
}

bool ScadaCoreService::isRunning() const
{
    return m_serviceRunning;
}

void ScadaCoreService::addDataPoint(const DataAcquisitionPoint &point)
{
    // Check if point already exists
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == point.name) {
            m_dataPoints[i] = point;
            qDebug() << "Updated existing data point:" << point.name;
            return;
        }
    }
    
    m_dataPoints.append(point);
    m_lastPollTimes[point.name] = 0; // Initialize last poll time
    qDebug() << "Added new data point:" << point.name << "at" << point.host << ":" << point.port;
}

void ScadaCoreService::removeDataPoint(const QString &pointName)
{
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == pointName) {
            m_dataPoints.removeAt(i);
            m_lastPollTimes.remove(pointName);
            qDebug() << "Removed data point:" << pointName;
            return;
        }
    }
}

void ScadaCoreService::updateDataPoint(const QString &pointName, const DataAcquisitionPoint &point)
{
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == pointName) {
            m_dataPoints[i] = point;
            qDebug() << "Updated data point:" << pointName;
            return;
        }
    }
}

QVector<DataAcquisitionPoint> ScadaCoreService::getDataPoints() const
{
    return m_dataPoints;
}

void ScadaCoreService::clearDataPoints()
{
    m_dataPoints.clear();
    m_lastPollTimes.clear();
    qDebug() << "Cleared all data points";
}

void ScadaCoreService::setTelegrafSocketPath(const QString &socketPath)
{
    m_telegrafSocketPath = socketPath;
    qDebug() << "Telegraf socket path set to:" << socketPath;
}

QString ScadaCoreService::getTelegrafSocketPath() const
{
    return m_telegrafSocketPath;
}

ScadaCoreService::ServiceStatistics ScadaCoreService::getStatistics() const
{
    return m_statistics;
}

void ScadaCoreService::resetStatistics()
{
    m_statistics = ServiceStatistics();
    m_responseTimers.clear();
}

// Modbus write operations
void ScadaCoreService::writeHoldingRegister(const QString &host, int port, int address, quint16 value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegister[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    qDebug() << "Writing holding register:" << host << ":" << port << "address" << address << "value" << value;
    m_modbusManager->writeHoldingRegister(address, value);
}

void ScadaCoreService::writeHoldingRegisterFloat32(const QString &host, int port, int address, float value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterFloat32[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    qDebug() << "Writing holding register Float32:" << host << ":" << port << "address" << address << "value" << value;
    m_modbusManager->writeHoldingRegisterFloat32(address, value);
}

void ScadaCoreService::writeHoldingRegisterDouble64(const QString &host, int port, int address, double value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterDouble64[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    qDebug() << "Writing holding register Double64:" << host << ":" << port << "address" << address << "value" << value;
    m_modbusManager->writeHoldingRegisterDouble64(address, value);
}

void ScadaCoreService::writeHoldingRegisterLong32(const QString &host, int port, int address, qint32 value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterLong32[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    qDebug() << "Writing holding register Long32:" << host << ":" << port << "address" << address << "value" << value;
    m_modbusManager->writeHoldingRegisterLong32(address, value);
}

void ScadaCoreService::writeHoldingRegisterLong64(const QString &host, int port, int address, qint64 value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteHoldingRegisterLong64[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    qDebug() << "Writing holding register Long64:" << host << ":" << port << "address" << address << "value" << value;
    m_modbusManager->writeHoldingRegisterLong64(address, value);
}

void ScadaCoreService::writeCoil(const QString &host, int port, int address, bool value)
{
    if (!connectToModbusHost(host, port)) {
        emit writeCompleted(QString("WriteCoil[%1:%2@%3]").arg(host).arg(port).arg(address), false, "Failed to connect to Modbus host");
        return;
    }
    
    qDebug() << "Writing coil:" << host << ":" << port << "address" << address << "value" << value;
    m_modbusManager->writeCoil(address, value);
}

void ScadaCoreService::onPollTimer()
{
    if (!m_serviceRunning || m_dataPoints.isEmpty()) {
        return;
    }
    
    processNextDataPoint();
}

void ScadaCoreService::processNextDataPoint()
{
    if (m_currentPointIndex >= m_dataPoints.size()) {
        m_currentPointIndex = 0;
    }
    
    if (m_dataPoints.isEmpty()) {
        return;
    }
    
    // First pass: Look for ready block points to prioritize by data type
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Collect ready block points and sort by priority
    QVector<QPair<int, int>> readyBlockIndices; // priority, index
    
    for (int i = 0; i < m_dataPoints.size(); i++) {
        const DataAcquisitionPoint &blockPoint = m_dataPoints[i];
        
        // Skip if not enabled or not a block
        if (!blockPoint.enabled || !blockPoint.tags.contains("block_type")) {
            continue;
        }
        
        // Check if block is ready for polling
        qint64 lastPollTime = m_lastPollTimes.value(blockPoint.name, 0);
        if (currentTime - lastPollTime >= blockPoint.pollInterval) {
            // Get priority from block tags (lower number = higher priority)
            int priority = blockPoint.tags.value("data_type_priority", "99").toInt();
            readyBlockIndices.append(qMakePair(priority, i));
        }
    }
    
    // Sort by priority (INT16=1, FLOAT32=2, DOUBLE64=3)
    if (!readyBlockIndices.isEmpty()) {
        std::sort(readyBlockIndices.begin(), readyBlockIndices.end());
        
        // Process ALL ready blocks in priority order to ensure lower priority blocks get processed
        for (const auto &pair : readyBlockIndices) {
            int blockIndex = pair.second;
            const DataAcquisitionPoint &prioritizedBlock = m_dataPoints[blockIndex];
            
            qDebug() << "Processing prioritized block:" << prioritizedBlock.name 
                     << "Data type:" << prioritizedBlock.tags.value("block_data_type", "UNKNOWN")
                     << "Priority:" << prioritizedBlock.tags.value("data_type_priority", "99");
            
            processDataPoint(prioritizedBlock, currentTime);
        }
        return;
    }
    
    // Second pass: Process individual points with priority ordering
    // Collect ready individual points and sort by data type priority
    QVector<QPair<int, int>> readyIndividualIndices; // priority, index
    
    for (int i = 0; i < m_dataPoints.size(); i++) {
        const DataAcquisitionPoint &point = m_dataPoints[i];
        
        // Skip if not enabled or is a block
        if (!point.enabled || point.tags.contains("block_type")) {
            continue;
        }
        
        // Skip individual points if they are covered by an optimized block
        if (isPointCoveredByBlock(point)) {
            continue;
        }
        
        // Check if point is ready for polling
        qint64 lastPollTime = m_lastPollTimes.value(point.name, 0);
        qint64 timeSinceLastPoll = currentTime - lastPollTime;
        bool isReady = timeSinceLastPoll >= point.pollInterval;
        
        if (isReady) {
            // Determine priority based on data type
            int priority = 99; // Default low priority
            switch (point.dataType) {
                case ModbusDataType::HoldingRegister:
                    priority = 1; // INT16 - highest priority
                    break;
                case ModbusDataType::Float32:
                case ModbusDataType::Long32:
                    priority = 2; // FLOAT32/LONG32 - second priority
                    break;
                case ModbusDataType::Double64:
                case ModbusDataType::Long64:
                    priority = 3; // DOUBLE64/LONG64 - third priority
                    break;
                case ModbusDataType::InputRegister:
                case ModbusDataType::Coil:
                case ModbusDataType::DiscreteInput:
                case ModbusDataType::BOOL:
                    priority = 1; // Other types and BOOL - high priority
                    break;
            }
            readyIndividualIndices.append(qMakePair(priority, i));
        }
    }
    
    // Sort by priority and process the highest priority individual point
    if (!readyIndividualIndices.isEmpty()) {
        std::sort(readyIndividualIndices.begin(), readyIndividualIndices.end());
        
        // Find all points with the highest priority
        int highestPriority = readyIndividualIndices.first().first;
        QVector<QPair<int, int>> highestPriorityPoints;
        
        for (const auto &pair : readyIndividualIndices) {
            if (pair.first == highestPriority) {
                highestPriorityPoints.append(pair);
            } else {
                break; // Since sorted, no more highest priority points
            }
        }
        
        // Use round-robin selection for points with same priority
        static int roundRobinIndex = 0;
        int selectedIndex = roundRobinIndex % highestPriorityPoints.size();
        roundRobinIndex = (roundRobinIndex + 1) % highestPriorityPoints.size();
        
        int pointIndex = highestPriorityPoints[selectedIndex].second;
        const DataAcquisitionPoint &prioritizedPoint = m_dataPoints[pointIndex];
        
        qDebug() << "Processing individual point:" << prioritizedPoint.name;
        
        processDataPoint(prioritizedPoint, currentTime);
        return;
    }
    
    // Fallback: process next point in sequence if no prioritized points are ready
    m_currentPointIndex++;
    if (m_currentPointIndex >= m_dataPoints.size()) {
        m_currentPointIndex = 0;
    }
}

void ScadaCoreService::processDataPoint(const DataAcquisitionPoint &point, qint64 currentTime)
{
    // Connect to Modbus host (will reuse existing connection if same host)
    if (!connectToModbusHost(point.host, point.port)) {
        emit errorOccurred(QString("Failed to connect to Modbus host: %1:%2")
                         .arg(point.host).arg(point.port));
        return;
    }
    
    // Wait for connection to be fully established before polling
    if (!m_modbusManager->isConnected()) {
        qDebug() << "Waiting for Modbus connection to be established for" << point.name;
        return; // Skip this polling cycle, will retry on next cycle
    }
    
    // Start data acquisition
    m_lastPollTimes[point.name] = currentTime;
    m_responseTimers[point.name] = currentTime;
    
    // Extract unit ID from tags (default to 1 if not specified)
    int unitId = point.tags.value("unit_id", "1").toInt();
    
    qDebug() << "Polling data point:" << point.name << "at address" << point.address << "Unit ID:" << unitId;
    
    // Check if this is an optimized block read with combined conditions
    if (point.tags.contains("block_type") && point.tags["block_type"] == "optimized_read") {
        // This is a block read - use block methods with register_type and data_type validation
        int blockSize = point.tags["block_size"].toInt();
        QString registerType = point.tags.value("register_type", "");
        QString dataType = point.tags.value("data_type", "");
        
        qDebug() << "Performing block read - Address:" << point.address << "Size:" << blockSize
                 << "Register Type:" << registerType << "Data Type:" << dataType << "Unit ID:" << unitId;
        
        // Use combined condition: register_type + data_type for precise Modbus operation selection
        if (registerType == "HOLDING_REGISTER") {
            // Handle holding register types based on data_type
            if (dataType == "Int16" || point.dataType == ModbusDataType::HoldingRegister) {
                m_modbusManager->readHoldingRegisters(point.address, blockSize, ModbusDataType::HoldingRegister, unitId);
            } else if (dataType == "Float32" || point.dataType == ModbusDataType::Float32) {
                m_modbusManager->readHoldingRegisters(point.address, blockSize, ModbusDataType::Float32, unitId);
            } else if (dataType == "Double64" || point.dataType == ModbusDataType::Double64) {
                m_modbusManager->readHoldingRegisters(point.address, blockSize, ModbusDataType::Double64, unitId);
            } else if (dataType == "Long32" || point.dataType == ModbusDataType::Long32) {
                m_modbusManager->readHoldingRegisters(point.address, blockSize, ModbusDataType::Long32, unitId);
            } else if (dataType == "Long64" || point.dataType == ModbusDataType::Long64) {
                m_modbusManager->readHoldingRegisters(point.address, blockSize, ModbusDataType::Long64, unitId);
            } else {
                // Default to HoldingRegister for unknown data types
                m_modbusManager->readHoldingRegisters(point.address, blockSize, ModbusDataType::HoldingRegister, unitId);
            }
        } else if (registerType == "INPUT_REGISTER") {
            // Handle input register types
            m_modbusManager->readInputRegisters(point.address, blockSize, point.dataType, unitId);
        } else if (registerType == "COIL") {
            // Handle coil types
            m_modbusManager->readCoils(point.address, blockSize, unitId);
        } else if (registerType == "DISCRETE_INPUT") {
            // Handle discrete input types
            m_modbusManager->readDiscreteInputs(point.address, blockSize, unitId);
        } else if (registerType == "STATUS") {
            // Handle STATUS register type for block reads - map to DiscreteInput for BOOL data types
            m_modbusManager->readDiscreteInputs(point.address, blockSize, unitId);
        } else {
            // Fallback to original dataType-based logic if register_type is not specified
            switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::Float32:
            case ModbusDataType::Double64:
            case ModbusDataType::Long32:
            case ModbusDataType::Long64:
                m_modbusManager->readHoldingRegisters(point.address, blockSize, point.dataType, unitId);
                break;
            case ModbusDataType::InputRegister:
                m_modbusManager->readInputRegisters(point.address, blockSize, point.dataType, unitId);
                break;
            case ModbusDataType::Coil:
                m_modbusManager->readCoils(point.address, blockSize, unitId);
                break;
            case ModbusDataType::DiscreteInput:
            case ModbusDataType::BOOL:
                // Default BOOL to DiscreteInput for block reads
                m_modbusManager->readDiscreteInputs(point.address, blockSize, unitId);
                break;
            }
        }
    } else {
        // Individual point read with combined conditions
        QString registerType = point.tags.value("register_type", "");
        QString dataType = point.tags.value("data_type", "");
        
        qDebug() << "Performing individual read - Address:" << point.address
                 << "Register Type:" << registerType << "Data Type:" << dataType << "Unit ID:" << unitId;
        
        // Use combined condition: register_type + data_type for precise Modbus operation selection
        if (registerType == "HOLDING_REGISTER") {
            // Handle holding register types based on data_type
            if (dataType == "Int16" || point.dataType == ModbusDataType::HoldingRegister) {
                m_modbusManager->readHoldingRegister(point.address, ModbusDataType::HoldingRegister, unitId);
            } else if (dataType == "Float32" || point.dataType == ModbusDataType::Float32) {
                m_modbusManager->readHoldingRegister(point.address, ModbusDataType::Float32, unitId);
            } else if (dataType == "Double64" || point.dataType == ModbusDataType::Double64) {
                m_modbusManager->readHoldingRegister(point.address, ModbusDataType::Double64, unitId);
            } else if (dataType == "Long32" || point.dataType == ModbusDataType::Long32) {
                m_modbusManager->readHoldingRegister(point.address, ModbusDataType::Long32, unitId);
            } else if (dataType == "Long64" || point.dataType == ModbusDataType::Long64) {
                m_modbusManager->readHoldingRegister(point.address, ModbusDataType::Long64, unitId);
            } else {
                // Default to HoldingRegister for unknown data types
                m_modbusManager->readHoldingRegister(point.address, ModbusDataType::HoldingRegister, unitId);
            }
        } else if (registerType == "INPUT_REGISTER") {
            // Handle input register types
            m_modbusManager->readInputRegister(point.address, point.dataType, unitId);
        } else if (registerType == "COIL") {
            // Handle coil types
            m_modbusManager->readCoil(point.address, unitId);
        } else if (registerType == "DISCRETE_INPUT") {
            // Handle discrete input types
            m_modbusManager->readDiscreteInput(point.address, unitId);
        } else if (registerType == "STATUS") {
            // Handle STATUS register type - map to appropriate Modbus operation based on data type
            if (dataType == "Bool" || point.dataType == ModbusDataType::BOOL || point.dataType == ModbusDataType::Coil) {
                m_modbusManager->readCoil(point.address, unitId);
            } else if (point.dataType == ModbusDataType::DiscreteInput) {
                m_modbusManager->readDiscreteInput(point.address, unitId);
            } else {
                // Default STATUS+BOOL to DiscreteInput
                m_modbusManager->readDiscreteInput(point.address, unitId);
            }
        } else {
            // Fallback to original dataType-based logic if register_type is not specified
            switch (point.dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::Float32:
            case ModbusDataType::Double64:
            case ModbusDataType::Long32:
            case ModbusDataType::Long64:
                m_modbusManager->readHoldingRegister(point.address, point.dataType, unitId);
                break;
            case ModbusDataType::InputRegister:
                m_modbusManager->readInputRegister(point.address, point.dataType, unitId);
                break;
            case ModbusDataType::Coil:
                m_modbusManager->readCoil(point.address, unitId);
                break;
            case ModbusDataType::DiscreteInput:
                m_modbusManager->readDiscreteInput(point.address, unitId);
                break;
            case ModbusDataType::BOOL:
                // Default BOOL to DiscreteInput when no register_type specified
                m_modbusManager->readDiscreteInput(point.address, unitId);
                break;
            }
        }
    }
    
    m_statistics.totalReadOperations++;
}

void ScadaCoreService::onModbusReadCompleted(const ModbusReadResult &result)
{
    qDebug() << "📥 Modbus Read Result Received:";
    qDebug() << "   Start Address:" << result.startAddress;
    qDebug() << "   Success:" << result.success;
    qDebug() << "   Raw Data Size:" << result.rawData.size();
    qDebug() << "   Error String:" << result.errorString;
    
    // Find the corresponding data point
    DataAcquisitionPoint *targetPoint = nullptr;
    for (auto &point : m_dataPoints) {
        if (point.address == result.startAddress) {
            targetPoint = &point;
            break;
        }
    }
    
    if (!targetPoint) {
        qWarning() << "Received read result for unknown address:" << result.startAddress;
        qDebug() << "Available data point addresses:";
        for (const auto &point : m_dataPoints) {
            qDebug() << "   Point:" << point.name << "Address:" << point.address;
        }
        return;
    }
    
    // Check if this is a block read result
    if (targetPoint->tags.contains("block_type") && targetPoint->tags["block_type"] == "optimized_read") {
        // Handle block read - extract individual point values
        handleBlockReadResult(result, *targetPoint);
        return;
    }
    
    // Calculate response time
    qint64 responseTime = 0;
    if (m_responseTimers.contains(targetPoint->name)) {
        responseTime = QDateTime::currentMSecsSinceEpoch() - m_responseTimers[targetPoint->name];
        m_responseTimers.remove(targetPoint->name);
    }
    
    // Create acquired data point
    AcquiredDataPoint dataPoint;
    dataPoint.pointName = targetPoint->name;
    dataPoint.timestamp = result.timestamp;
    dataPoint.measurement = targetPoint->measurement;
    dataPoint.tags = targetPoint->tags;
    dataPoint.isValid = result.success;
    
    // Add address tag for InfluxDB mapping (individual points)
    dataPoint.tags["address"] = QString::number(targetPoint->address);
    
    // Validate and ensure all required InfluxDB tags are present
    validateAndSetInfluxTags(dataPoint, *targetPoint);
    
    if (result.success) {
        // Extract value based on data type
        if (!result.processedData.isEmpty()) {
            QString key = result.processedData.keys().first();
            dataPoint.value = result.processedData[key];
        } else if (!result.rawData.isEmpty()) {
            dataPoint.value = result.rawData.first();
        }
        
        // Additional BOOL-specific validation for individual points
        if (targetPoint->dataType == ModbusDataType::BOOL) {
            // Validate BOOL conversion for individual points
            if (!dataPoint.value.canConvert<bool>()) {
                qWarning() << "BOOL conversion failed for individual point" << targetPoint->name
                          << "- QVariant cannot convert to bool. Using default false.";
                dataPoint.value = false;
                dataPoint.isValid = false;
                dataPoint.errorMessage = "BOOL conversion failed: QVariant cannot convert to bool";
            } else {
                // Log successful BOOL conversion
                qDebug() << "BOOL conversion successful for" << targetPoint->name
                         << "- value:" << dataPoint.value.toBool();
            }
        }
        
        m_statistics.successfulReads++;
        updateStatistics(true, responseTime);
        
        qDebug() << "Successfully read data point:" << targetPoint->name 
                 << "Value:" << dataPoint.value.toString();
    } else {
        dataPoint.errorMessage = result.errorString;
        m_statistics.failedReads++;
        updateStatistics(false, responseTime);
        
        qWarning() << "Failed to read data point:" << targetPoint->name 
                   << "Error:" << result.errorString;
    }
    
    // Emit signal
    emit dataPointAcquired(dataPoint);
    
    // Send to InfluxDB via Telegraf
    if (dataPoint.isValid) {
        bool sent = sendDataToInflux(dataPoint);
        
        if (sent) {
            m_statistics.totalDataPointsSent++;
        }
    }
    
    // Update statistics
    emit statisticsUpdated(m_statistics);
}

void ScadaCoreService::onModbusConnectionStateChanged(bool connected)
{
    if (connected) {
        qDebug() << "Modbus connection established to:" << m_currentHost;
    } else {
        qDebug() << "Modbus connection lost to:" << m_currentHost;
        m_currentHost.clear();
    }
}

void ScadaCoreService::onModbusError(const QString &error)
{
    qWarning() << "Modbus error:" << error;
    emit errorOccurred(QString("Modbus error: %1").arg(error));
}

void ScadaCoreService::onModbusWriteCompleted(const ModbusWriteResult &result)
{
    QString operation = QString("Write@%1[%2]").arg(result.startAddress).arg(result.registerCount);
    
    if (result.success) {
        qDebug() << "✅ Modbus write completed successfully:" << operation;
        emit writeCompleted(operation, true);
    } else {
        qWarning() << "❌ Modbus write failed:" << operation << "Error:" << result.errorString;
        emit writeCompleted(operation, false, result.errorString);
    }
}

bool ScadaCoreService::writeToTelegrafSocket(const QString& socketPath, const QByteArray& message)
{
    if (!QFileInfo::exists(socketPath)) {
        qCritical() << "Unix socket does not exist:" << socketPath;
        return false;
    }
    
    int sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return false;
    }
    
    struct sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.toUtf8().constData(), sizeof(addr.sun_path) - 1);
    
    int result = ::sendto(sockfd, message.constData(), message.size(), 0,
                          reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    ::close(sockfd);
    
    if (result < 0) {
        perror("sendto");
        return false;
    }
    
    qDebug() << "Sent InfluxDB line protocol to Telegraf via UNIX socket:" << message.trimmed();
    return true;
}

bool ScadaCoreService::writeToInflux(const QString& measurement, const QString& device, const QVariant& value, const QString& description)
{
    if (!value.isValid()) {
        qDebug() << "Invalid value for" << measurement << "on device" << device;
        return false;
    }
    
    // Sanitize tag values by removing spaces
    QString sanitizedMeasurement = measurement;
    sanitizedMeasurement.replace(" ", "_");
    
    QString sanitizedDevice = device;
    sanitizedDevice.replace(" ", "_");
    
    QString sanitizedDescription = description.isEmpty() ? 
        QString("SCADA_data_point_for_%1").arg(sanitizedDevice) : description;
    sanitizedDescription.replace(" ", "_");
    
    QString sanitizedStationName = device;
    
    // Create InfluxDB line protocol with enhanced mapping tags (no spaces in tag values)
    QString line = QString("%1,device_name=%2,tag_name=%3,description=%4,station_name=%5 value=%6")
                   .arg(sanitizedMeasurement, sanitizedDevice, sanitizedDevice, sanitizedDescription, sanitizedStationName, value.toString());
    
    line += "\n";
    
    bool success = writeToTelegrafSocket(m_telegrafSocketPath, line.toUtf8());
    qDebug() << "Line to write:" << line;
    
    if (!success) {
        qDebug() << "Failed to write to InfluxDB for" << measurement << "on device" << device;
        m_statistics.socketErrors++;
    } else {
        qDebug() << "Successfully wrote to InfluxDB:" << line;
        m_statistics.totalDataPointsSent++;
    }
    
    return success;
}

// Enhanced InfluxDB write method with full tag support
bool ScadaCoreService::writeToInfluxEnhanced(const AcquiredDataPoint &dataPoint)
{
    if (!dataPoint.isValid || dataPoint.measurement.isEmpty()) {
        qDebug() << "Invalid data point:" << dataPoint.pointName;
        return false;
    }
    
    // Sanitize tag values by removing spaces
    QString sanitizedMeasurement = dataPoint.measurement;
    sanitizedMeasurement.replace(" ", "_");
    
    QString deviceName = dataPoint.tags.value("device_name", "STATION_TEST");
    deviceName.replace(" ", "_");
    
    QString dataType = dataPoint.tags.value("data_type", "Int16");
    dataType.replace(" ", "_");
    
    QString readMode = dataPoint.tags.value("read_mode", "single_register");
    readMode.replace(" ", "_");
    
    QString address = dataPoint.tags.value("address", "0");
    
    QString description = dataPoint.tags.value("description", QString("CURRENT_RTU_%1").arg(address));
    description.replace(" ", "_");
    
    // Create enhanced InfluxDB line protocol with all required tags
    QString line = QString("%1,device_name=%2,data_type=%3,read_mode=%4,address=%5,description=%6 value=%7")
                   .arg(sanitizedMeasurement, deviceName, dataType, readMode, address, description, dataPoint.value.toString());
    
    line += "\n";
    
    bool success = writeToTelegrafSocket(m_telegrafSocketPath, line.toUtf8());
    qDebug() << "Enhanced InfluxDB line:" << line;
    
    if (!success) {
        qDebug() << "Failed to write enhanced data to InfluxDB for" << dataPoint.pointName;
        m_statistics.socketErrors++;
    } else {
        qDebug() << "Successfully wrote enhanced data to InfluxDB:" << line;
        m_statistics.totalDataPointsSent++;
    }
    
    return success;
}

bool ScadaCoreService::sendDataToInflux(const AcquiredDataPoint &dataPoint)
{
    if (!dataPoint.isValid || dataPoint.measurement.isEmpty()) {
        qWarning() << "Invalid data point:" << dataPoint.pointName;
        return false;
    }
    
    // Use the enhanced InfluxDB write method with full tag support
    bool success = writeToInfluxEnhanced(dataPoint);
    
    if (success) {
        emit dataPointSentToInflux(dataPoint.pointName, true);
    } else {
        emit dataPointSentToInflux(dataPoint.pointName, false);
        emit errorOccurred(QString("Failed to send data point to InfluxDB: %1").arg(dataPoint.pointName));
    }
    
    return success;
}



bool ScadaCoreService::connectToModbusHost(const QString &host, int port)
{
    QString hostKey = QString("%1:%2").arg(host).arg(port);
    
    // Check if we're already connected to the same host
    if (m_modbusManager->isConnected() && m_currentHost == hostKey) {
        qDebug() << "Already connected to" << hostKey << "- reusing connection";
        return true;
    }
    
    // Only disconnect if we're connected to a different host
    if (m_modbusManager->isConnected() && m_currentHost != hostKey) {
        qDebug() << "Switching from" << m_currentHost << "to" << hostKey;
        m_modbusManager->disconnectFromServer();
        // Give some time for disconnection
        QThread::msleep(100);
        m_currentHost.clear();
    }
    
    // Connect to the new host
    qDebug() << "Connecting to Modbus host:" << hostKey;
    bool connectionStarted = m_modbusManager->connectToServer(host, port);
    
    if (!connectionStarted) {
        qWarning() << "Failed to start connection to" << hostKey;
        m_currentHost.clear();
        return false;
    }
    
    // Wait for connection to be established (with timeout)
    int waitTime = 0;
    const int maxWaitTime = 15000; // Use ConnectionResilience timeout (15 seconds default)
    const int checkInterval = 100; // 100ms
    
    while (waitTime < maxWaitTime && !m_modbusManager->isConnected()) {
        QThread::msleep(checkInterval);
        waitTime += checkInterval;
        QCoreApplication::processEvents(); // Process Qt events
    }
    
    bool connected = m_modbusManager->isConnected();
    if (connected) {
        m_currentHost = hostKey;
        qDebug() << "Successfully connected to" << hostKey;
    } else {
        qWarning() << "Failed to connect to" << hostKey << "(timeout after" << waitTime << "ms)";
        m_currentHost.clear();
        // Ensure we disconnect if connection attempt failed
        m_modbusManager->disconnectFromServer();
    }
    
    return connected;
}

void ScadaCoreService::updateStatistics(bool success, qint64 responseTime)
{
    // Update success/failure counters
    if (success) {
        m_statistics.successfulReads++;
    } else {
        m_statistics.failedReads++;
    }
    
    if (responseTime > 0) {
        // Update average response time
        qint64 totalOps = m_statistics.successfulReads + m_statistics.failedReads;
        if (totalOps > 0) {
            m_statistics.averageResponseTime = 
                (m_statistics.averageResponseTime * (totalOps - 1) + responseTime) / totalOps;
        } else {
            m_statistics.averageResponseTime = responseTime;
        }
    }
}

QJsonObject ScadaCoreService::dataPointToJson(const AcquiredDataPoint &dataPoint)
{
    QJsonObject json;
    json["pointName"] = dataPoint.pointName;
    json["value"] = QJsonValue::fromVariant(dataPoint.value);
    json["timestamp"] = dataPoint.timestamp;
    json["measurement"] = dataPoint.measurement;
    json["isValid"] = dataPoint.isValid;
    
    if (!dataPoint.errorMessage.isEmpty()) {
        json["error"] = dataPoint.errorMessage;
    }
    
    if (!dataPoint.tags.isEmpty()) {
        QJsonObject tagsObj;
        for (auto it = dataPoint.tags.constBegin(); it != dataPoint.tags.constEnd(); ++it) {
            tagsObj[it.key()] = it.value();
        }
        json["tags"] = tagsObj;
    }
    
    return json;
}

void ScadaCoreService::handleBlockReadResult(const ModbusReadResult &result, const DataAcquisitionPoint &blockPoint)
{
    if (!result.success) {
        qWarning() << "Block read failed for" << blockPoint.name << "Error:" << result.errorString;
        m_statistics.failedReads++;
        return;
    }
    
    // Get block information from tags
    int blockSize = blockPoint.tags.value("block_size", "1").toInt();
    int startAddress = blockPoint.address;
    
    // Get original point metadata from block tags
    QStringList originalAddresses = blockPoint.tags.value("original_addresses", "").split(",", Qt::SkipEmptyParts);
    QStringList originalNames = blockPoint.tags.value("original_names", "").split(",", Qt::SkipEmptyParts);
    QStringList originalDataTypes = blockPoint.tags.value("original_data_types", "").split(",", Qt::SkipEmptyParts);
    QStringList originalDescriptions = blockPoint.tags.value("original_descriptions", "").split(",", Qt::SkipEmptyParts);
    QStringList originalMeasurements = blockPoint.tags.value("original_measurements", "").split(",", Qt::SkipEmptyParts);
    
    qDebug() << "Processing block read result for" << blockPoint.name 
             << "Start address:" << startAddress 
             << "Block size:" << blockSize
             << "Original points:" << originalAddresses.size()
             << "Raw data size:" << result.rawData.size();
    
    // Validate we have enough data
    if (result.rawData.size() < blockSize) {
        qWarning() << "Insufficient data in block read. Expected:" << blockSize 
                   << "Got:" << result.rawData.size();
        return;
    }
    
    // Validate metadata consistency
    if (originalAddresses.size() != originalNames.size() || 
        originalAddresses.size() != originalDataTypes.size() ||
        originalAddresses.size() != originalDescriptions.size() ||
        originalAddresses.size() != originalMeasurements.size()) {
        qWarning() << "Inconsistent original point metadata in block" << blockPoint.name;
        return;
    }
    
    // Process each original point
    for (int pointIndex = 0; pointIndex < originalAddresses.size(); pointIndex++) {
        int originalAddress = originalAddresses[pointIndex].toInt();
        int dataTypeInt = originalDataTypes[pointIndex].toInt();
        ModbusDataType dataType = static_cast<ModbusDataType>(dataTypeInt);
        
        // Calculate offset from start address based on actual register address difference
        int offset = originalAddress - startAddress;
        
        qDebug() << "Processing point" << pointIndex << ":"
                 << "Name:" << originalNames[pointIndex]
                 << "Address:" << originalAddress
                 << "Start Address:" << startAddress
                 << "Calculated Offset:" << offset
                 << "Data Type:" << dataTypeInt
                 << "Raw data at offset:" << (offset < result.rawData.size() ? QString::number(result.rawData[offset]) : "OUT_OF_BOUNDS");
        
        // Register count validation for different data types
        
        // Validate offset bounds
        int registersNeeded = 1;
        switch (dataType) {
            case ModbusDataType::Float32:
            case ModbusDataType::Long32:
                registersNeeded = 2;
                break;
            case ModbusDataType::Double64:
            case ModbusDataType::Long64:
                registersNeeded = 4;
                break;
            default:
                registersNeeded = 1;
                break;
        }
        
        if (offset < 0 || (offset + registersNeeded - 1) >= result.rawData.size()) {
            qWarning() << "Address offset out of range:" << offset << "(needs" << registersNeeded << "registers) for address" << originalAddress << "in block of size" << result.rawData.size();
            continue;
        }
        
        // Create acquired data point with enhanced mapping
        AcquiredDataPoint dataPoint;
        dataPoint.pointName = originalNames[pointIndex];
        dataPoint.timestamp = result.timestamp;
        dataPoint.measurement = originalMeasurements[pointIndex];
        dataPoint.tags = blockPoint.tags; // Start with block tags
        dataPoint.isValid = true;
        
        // Enhanced InfluxDB mapping fields
        dataPoint.tags["device_name"] = blockPoint.tags.value("device_name", "STATION_TEST");
        dataPoint.tags["address"] = QString::number(originalAddress);
        dataPoint.tags["description"] = originalDescriptions[pointIndex];
        
        // Map read_mode based on data type
        QString readMode;
        switch (dataType) {
            case ModbusDataType::HoldingRegister:
            case ModbusDataType::InputRegister:
                readMode = "single_register";
                break;
            case ModbusDataType::Float32:
            case ModbusDataType::Long32:
                readMode = "dual_register";
                break;
            case ModbusDataType::Double64:
            case ModbusDataType::Long64:
                readMode = "quad_register";
                break;
            case ModbusDataType::Coil:
            case ModbusDataType::DiscreteInput:
                readMode = "single_bit";
                break;
            default:
                readMode = "single_register";
        }
        dataPoint.tags["read_mode"] = readMode;
        
        // Map data_type to string
        QString dataTypeStr;
        switch (dataType) {
            case ModbusDataType::HoldingRegister:
                dataTypeStr = "Int16";
                break;
            case ModbusDataType::InputRegister:
                dataTypeStr = "Int16";
                break;
            case ModbusDataType::Coil:
                dataTypeStr = "Bool";
                break;
            case ModbusDataType::DiscreteInput:
                dataTypeStr = "Bool";
                break;
            case ModbusDataType::Float32:
                dataTypeStr = "Float32";
                break;
            case ModbusDataType::Double64:
                dataTypeStr = "Double64";
                break;
            case ModbusDataType::Long32:
                dataTypeStr = "Long32";
                break;
            case ModbusDataType::Long64:
                dataTypeStr = "Long64";
                break;
            case ModbusDataType::BOOL:
                dataTypeStr = "Bool";
                break;
            default:
                dataTypeStr = "Int16";
        }
        dataPoint.tags["data_type"] = dataTypeStr;
        
        // Extract value based on data type
        switch (dataType) {
        case ModbusDataType::Float32:
            if (offset + 1 < result.rawData.size()) {
                quint16 high = result.rawData[offset];
                quint16 low = result.rawData[offset + 1];
                quint32 combined = (static_cast<quint32>(high) << 16) | low;
                float floatValue;
                memcpy(&floatValue, &combined, sizeof(float));
                dataPoint.value = floatValue;
                
#ifdef FLOAT32_DECODE_DEBUG
                qDebug() << "Float32 decode for" << originalNames[pointIndex] << ":"
                         << "High register [" << offset << "]:" << QString::number(high, 16)
                         << "Low register [" << (offset + 1) << "]:" << QString::number(low, 16)
                         << "Combined hex:" << QString::number(combined, 16)
                         << "Float value:" << floatValue;
#endif
            } else {
                qWarning() << "Float32 decode failed for" << originalNames[pointIndex] 
                          << "- insufficient data. Offset:" << offset 
                          << "Data size:" << result.rawData.size();
            }
            break;
        case ModbusDataType::Double64:
            if (offset + 3 < result.rawData.size()) {
                quint16 reg0 = result.rawData[offset];
                quint16 reg1 = result.rawData[offset + 1];
                quint16 reg2 = result.rawData[offset + 2];
                quint16 reg3 = result.rawData[offset + 3];
                
                quint64 combined = (static_cast<quint64>(reg0) << 48) |
                                 (static_cast<quint64>(reg1) << 32) |
                                 (static_cast<quint64>(reg2) << 16) |
                                 reg3;
                double doubleValue;
                memcpy(&doubleValue, &combined, sizeof(double));
                dataPoint.value = doubleValue;
                
#ifdef MODBUS_DEBUG_ENABLED
                qDebug() << "Double64 decode for" << originalNames[pointIndex] << ":"
                         << "Registers [" << offset << "-" << (offset+3) << "]:" 
                         << "[" << reg0 << ", " << reg1 << ", " << reg2 << ", " << reg3 << "]"
                         << "Hex: [0x" << QString::number(reg0, 16).toUpper()
                         << ", 0x" << QString::number(reg1, 16).toUpper()
                         << ", 0x" << QString::number(reg2, 16).toUpper()
                         << ", 0x" << QString::number(reg3, 16).toUpper() << "]"
                         << "Combined: 0x" << QString::number(combined, 16).toUpper()
                         << "Double value:" << doubleValue;
#endif
            } else {
                qWarning() << "Double64 decode failed for" << originalNames[pointIndex] 
                          << "- insufficient data. Offset:" << offset 
                          << "Data size:" << result.rawData.size();
            }
            break;
        case ModbusDataType::Long32:
            if (offset + 1 < result.rawData.size()) {
                qint32 combined = (static_cast<qint32>(result.rawData[offset]) << 16) | result.rawData[offset + 1];
                dataPoint.value = combined;
            }
            break;
        case ModbusDataType::Long64:
            if (offset + 3 < result.rawData.size()) {
                qint64 combined = (static_cast<qint64>(result.rawData[offset]) << 48) |
                                (static_cast<qint64>(result.rawData[offset + 1]) << 32) |
                                (static_cast<qint64>(result.rawData[offset + 2]) << 16) |
                                result.rawData[offset + 3];
                dataPoint.value = combined;
            }
            break;
        case ModbusDataType::BOOL:
            // Handle BOOL data type with comprehensive error handling
            if (offset < result.rawData.size()) {
                quint16 rawValue = result.rawData[offset];
                
                // Validate raw value range for BOOL conversion
                if (rawValue > 1) {
                    qWarning() << "BOOL conversion warning for" << originalNames[pointIndex] 
                              << "- raw value" << rawValue << "exceeds typical boolean range (0-1)."
                              << "Converting non-zero to true.";
                }
                
                // Convert to boolean: 0 = false, non-zero = true
                bool boolValue = (rawValue != 0);
                dataPoint.value = boolValue;
                
                // Validate QVariant conversion success
                if (!dataPoint.value.canConvert<bool>()) {
                    qWarning() << "BOOL QVariant conversion failed for" << originalNames[pointIndex]
                              << "- unable to convert to boolean type. Using default false.";
                    dataPoint.value = false;
                    dataPoint.isValid = false;
                    dataPoint.errorMessage = QString("BOOL conversion failed: QVariant cannot convert to bool");
                }
                
#ifdef MODBUS_DEBUG_ENABLED
                qDebug() << "BOOL decode for" << originalNames[pointIndex] << ":"
                         << "Raw value:" << rawValue
                         << "Boolean value:" << boolValue
                         << "Conversion valid:" << dataPoint.value.canConvert<bool>();
#endif
            } else {
                qWarning() << "BOOL decode failed for" << originalNames[pointIndex] 
                          << "- insufficient data. Offset:" << offset 
                          << "Data size:" << result.rawData.size();
                dataPoint.value = false; // Default to false on error
                dataPoint.isValid = false;
                dataPoint.errorMessage = QString("BOOL decode failed: insufficient data at offset %1").arg(offset);
            }
            break;
        default:
            // For simple types (HoldingRegister, InputRegister)
            dataPoint.value = result.rawData[offset];
            break;
        }
        
        qDebug() << "📊 Block Data Point Extracted:";
        qDebug() << "   Point Name:" << dataPoint.pointName;
        qDebug() << "   Address:" << originalAddress << "(offset" << offset << "from block start" << startAddress << ")";
        qDebug() << "   Data Type:" << dataTypeInt;
        qDebug() << "   Raw Value:" << result.rawData[offset] << "(register" << (startAddress + offset) << ")";
        qDebug() << "   Processed Value:" << dataPoint.value.toString();
        qDebug() << "   Measurement:" << dataPoint.measurement;
        qDebug() << "   Description:" << originalDescriptions[pointIndex];
        qDebug() << "   Timestamp:" << QDateTime::fromMSecsSinceEpoch(dataPoint.timestamp).toString("yyyy-MM-dd hh:mm:ss.zzz");
        qDebug() << "   ----------------------------------------";
        
        // Create a temporary source point for validation
        DataAcquisitionPoint tempSourcePoint;
        tempSourcePoint.address = originalAddress;
        tempSourcePoint.host = blockPoint.host;
        tempSourcePoint.dataType = static_cast<ModbusDataType>(dataTypeInt);
        tempSourcePoint.name = originalNames[pointIndex];
        tempSourcePoint.tags = blockPoint.tags;
        
        // Validate and ensure all required InfluxDB tags are present
        validateAndSetInfluxTags(dataPoint, tempSourcePoint);
        
        // Emit signal and send to InfluxDB
        emit dataPointAcquired(dataPoint);
        
        if (dataPoint.isValid) {
            bool sent = sendDataToInflux(dataPoint);
            
            if (sent) {
                m_statistics.totalDataPointsSent++;
            }
        }
    }
    
    m_statistics.successfulReads++;
    updateStatistics(true, 0);
    emit statisticsUpdated(m_statistics);
}

bool ScadaCoreService::isPointCoveredByBlock(const DataAcquisitionPoint &point)
{
    // Check if there's a block that covers this individual point
    for (const DataAcquisitionPoint &blockPoint : m_dataPoints) {
        // Skip if this is not a block point
        if (!blockPoint.tags.contains("block_type")) {
            continue;
        }
        
        // Check if the individual point matches the block's device and register type
        if (blockPoint.host != point.host || blockPoint.port != point.port) {
            continue;
        }
        
        // Get block information
        bool ok;
        int blockStartAddress = blockPoint.tags.value("block_start_address").toInt(&ok);
        if (!ok) continue;
        
        int blockSize = blockPoint.tags.value("block_size").toInt(&ok);
        if (!ok) continue;
        
        QString blockRegisterType = blockPoint.tags.value("register_type");
        QString pointRegisterType;
        
        // Map point data type to register type string
        switch (point.dataType) {
        case ModbusDataType::HoldingRegister:
        case ModbusDataType::Float32:
        case ModbusDataType::Double64:
        case ModbusDataType::Long32:
        case ModbusDataType::Long64:
            pointRegisterType = "HOLDING_REGISTER";
            break;
        case ModbusDataType::InputRegister:
            pointRegisterType = "INPUT_REGISTER";
            break;
        case ModbusDataType::Coil:
            pointRegisterType = "COIL";
            break;
        case ModbusDataType::DiscreteInput:
        case ModbusDataType::BOOL:
            pointRegisterType = "DISCRETE_INPUT";
            break;
        }
        
        // Check if register types match and point address is within block range
        if (blockRegisterType == pointRegisterType &&
            point.address >= blockStartAddress &&
            point.address < blockStartAddress + blockSize) {
            return true;
        }
    }
    
    return false;
}

void ScadaCoreService::validateAndSetInfluxTags(AcquiredDataPoint &dataPoint, const DataAcquisitionPoint &sourcePoint)
{
    // Ensure address tag is always set (critical for InfluxDB mapping)
    if (!dataPoint.tags.contains("address") || dataPoint.tags["address"].isEmpty()) {
        dataPoint.tags["address"] = QString::number(sourcePoint.address);
        qDebug() << "WARNING: Missing address tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Ensure device_name tag is set
    if (!dataPoint.tags.contains("device_name") || dataPoint.tags["device_name"].isEmpty()) {
        dataPoint.tags["device_name"] = sourcePoint.host;
        qDebug() << "WARNING: Missing device_name tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Ensure data_type tag is set
    if (!dataPoint.tags.contains("data_type") || dataPoint.tags["data_type"].isEmpty()) {
        QString dataTypeStr;
        switch (sourcePoint.dataType) {
            case ModbusDataType::HoldingRegister:
                dataTypeStr = "INT16";
                break;
            case ModbusDataType::InputRegister:
                dataTypeStr = "INPUT_REGISTER";
                break;
            case ModbusDataType::Coil:
                dataTypeStr = "COIL";
                break;
            case ModbusDataType::DiscreteInput:
                dataTypeStr = "DISCRETE_INPUT";
                break;
            case ModbusDataType::BOOL:
                dataTypeStr = "BOOL";
                break;
            case ModbusDataType::Float32:
                dataTypeStr = "FLOAT32";
                break;
            case ModbusDataType::Double64:
                dataTypeStr = "DOUBLE64";
                break;
            case ModbusDataType::Long32:
                dataTypeStr = "INT32";
                break;
            case ModbusDataType::Long64:
                dataTypeStr = "INT64";
                break;
            default:
                dataTypeStr = "UNKNOWN";
                break;
        }
        dataPoint.tags["data_type"] = dataTypeStr;
        qDebug() << "WARNING: Missing data_type tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Ensure description tag is set (use point name as fallback)
    if (!dataPoint.tags.contains("description") || dataPoint.tags["description"].isEmpty()) {
        dataPoint.tags["description"] = sourcePoint.name.isEmpty() ? dataPoint.pointName : sourcePoint.name;
        qDebug() << "WARNING: Missing description tag detected and fixed for point:" << dataPoint.pointName;
    }
    
    // Copy any additional tags from source point
    for (auto it = sourcePoint.tags.constBegin(); it != sourcePoint.tags.constEnd(); ++it) {
        if (!dataPoint.tags.contains(it.key()) || dataPoint.tags[it.key()].isEmpty()) {
            dataPoint.tags[it.key()] = it.value();
        }
    }
    
    // Validate that critical tags are not empty after setting
    QStringList criticalTags = {"address", "device_name", "data_type", "description"};
    for (const QString &tag : criticalTags) {
        if (!dataPoint.tags.contains(tag) || dataPoint.tags[tag].isEmpty()) {
            qWarning() << "CRITICAL: Tag validation failed for" << tag << "in point:" << dataPoint.pointName;
        }
    }
}
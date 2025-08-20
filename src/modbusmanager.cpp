#include "../include/modbusmanager.h"
#include <QDebug>
#include <QDateTime>
#include <QDataStream>
#include <QByteArray>
#include <QFileInfo>

ModbusManager::ModbusManager(QObject *parent)
    : QObject(parent)
    , m_modbusClient(nullptr)
    , m_requestTimer(nullptr)
    , m_timeoutTimer(nullptr)
    , m_requestInProgress(false)
    , m_currentReply(nullptr)
    , m_currentRequestTime(0)
    , m_settings(nullptr)
    , m_autoAdjust(true)
    , m_heartbeatInterval(30000)
    , m_retryDelay(3000)
    , m_maxRetries(8)
    , m_requestTimeout(12000)
    , m_connectionTimeout(15000)
    , m_networkType("cellular_4g")
    , m_requestInterval(100)  // 100ms between requests
{
    qDebug() << "DEBUG: ModbusManager constructor - Entry point";
    
    // QModbusTcpClient and timers will be created in initializeClient() after moveToThread()
    // to avoid threading violations
    
    qDebug() << "DEBUG: ModbusManager constructor completed successfully";
}

void ModbusManager::initializeClient()
{
    // This method should be called after moveToThread() to create Qt objects in the correct thread
    if (!m_modbusClient) {
        qDebug() << "DEBUG: Creating QModbusTcpClient in worker thread...";
        m_modbusClient = new QModbusTcpClient(this);
        qDebug() << "DEBUG: QModbusTcpClient created successfully";
        
        // Connect modbus client signals
        connect(m_modbusClient, &QModbusClient::stateChanged,
                this, &ModbusManager::onStateChanged);
        connect(m_modbusClient, &QModbusDevice::errorOccurred,
                this, &ModbusManager::onErrorOccurred);
        qDebug() << "DEBUG: Modbus client signals connected";
    }
    
    // Initialize timers
    if (!m_requestTimer) {
        qDebug() << "DEBUG: Creating request timer in worker thread...";
        m_requestTimer = new QTimer(this);
        m_requestTimer->setSingleShot(true);
        connect(m_requestTimer, &QTimer::timeout, this, &ModbusManager::processNextRequest);
        qDebug() << "DEBUG: Request timer created and connected";
    }
    
    if (!m_timeoutTimer) {
        qDebug() << "DEBUG: Creating timeout timer in worker thread...";
        m_timeoutTimer = new QTimer(this);
        m_timeoutTimer->setSingleShot(true);
        connect(m_timeoutTimer, &QTimer::timeout, this, &ModbusManager::onRequestTimeout);
        qDebug() << "DEBUG: Timeout timer created and connected";
    }
    
    qDebug() << "DEBUG: ModbusManager initialization completed successfully";
}

ModbusManager::~ModbusManager()
{
    if (m_modbusClient) {
        m_modbusClient->disconnectDevice();
    }
    if (m_settings) {
        delete m_settings;
    }
}

bool ModbusManager::loadConfigurationFromFile(const QString &configPath)
{
    if (m_settings) {
        delete m_settings;
    }
    
    QFileInfo configFile(configPath);
    if (!configFile.exists()) {
        qDebug() << "❌ Config file not found:" << configPath;
        return false;
    }
    
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    
    // Load ConnectionResilience configuration
    m_settings->beginGroup("ConnectionResilience");
    m_autoAdjust = m_settings->value("auto_adjust", true).toBool();
    m_heartbeatInterval = m_settings->value("heartbeat_interval", 30000).toInt();
    m_retryDelay = m_settings->value("retry_delay", 3000).toInt();
    m_maxRetries = m_settings->value("max_retries", 8).toInt();
    m_requestTimeout = m_settings->value("request_timeout", 12000).toInt();
    m_connectionTimeout = m_settings->value("connection_timeout", 15000).toInt();
    m_networkType = m_settings->value("network_type", "cellular_4g").toString();
    m_settings->endGroup();
    
    qDebug() << "✅ ModbusManager configuration loaded from:" << configPath;
    qDebug() << "   Auto Adjust:" << m_autoAdjust;
    qDebug() << "   Heartbeat Interval:" << m_heartbeatInterval << "ms";
    qDebug() << "   Retry Delay:" << m_retryDelay << "ms";
    qDebug() << "   Max Retries:" << m_maxRetries;
    qDebug() << "   Request Timeout:" << m_requestTimeout << "ms";
    qDebug() << "   Connection Timeout:" << m_connectionTimeout << "ms";
    qDebug() << "   Network Type:" << m_networkType;
    
    return true;
}

bool ModbusManager::connectToServer(const QString &host, int port)
{
    qDebug() << "DEBUG: ModbusManager::connectToServer() - Entry point";
    
    if (!m_modbusClient) {
        qDebug() << "DEBUG: m_modbusClient is null!";
        return false;
    }
    qDebug() << "DEBUG: m_modbusClient is valid";
        
    qDebug() << "DEBUG: Checking connection state...";
    if (m_modbusClient->state() == QModbusDevice::ConnectedState) {
        qDebug() << "DEBUG: Already connected";
        return true;
    }
    qDebug() << "DEBUG: Not connected, proceeding with connection...";
        
    qDebug() << "DEBUG: Setting NetworkPortParameter...";
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    qDebug() << "DEBUG: Setting NetworkAddressParameter...";
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, host);
    qDebug() << "DEBUG: Setting timeout...";
    m_modbusClient->setTimeout(m_connectionTimeout);  // Use config value
    qDebug() << "DEBUG: Setting retries...";
    m_modbusClient->setNumberOfRetries(m_maxRetries); // Use config value
    
    qDebug() << "🔗 Connecting to Modbus server" << host << ":" << port;
    qDebug() << "   Using timeout:" << m_connectionTimeout << "ms, retries:" << m_maxRetries;
    
    qDebug() << "DEBUG: Calling connectDevice()...";
    bool result = m_modbusClient->connectDevice();
    qDebug() << "DEBUG: connectDevice() returned:" << result;
    return result;
}

void ModbusManager::disconnectFromServer()
{
    if (m_modbusClient && m_modbusClient->state() == QModbusDevice::ConnectedState) {
        m_modbusClient->disconnectDevice();
    }
}

bool ModbusManager::isConnected() const
{
    if (!m_modbusClient) {
        qDebug() << "isConnected() check: m_modbusClient is null";
        return false;
    }
    
    QModbusDevice::State currentState = m_modbusClient->state();
    bool connected = (currentState == QModbusDevice::ConnectedState);
    
    if (!connected) {
        qDebug() << "isConnected() check: Current state is" << currentState << "(not ConnectedState)";
    }
    
    return connected;
}

bool ModbusManager::isClientInitialized() const
{
    bool initialized = (m_modbusClient != nullptr);
    if (!initialized) {
        qDebug() << "ModbusManager::isClientInitialized() - Client is not initialized";
    }
    return initialized;
}

// Single read operations
void ModbusManager::readHoldingRegister(int address, ModbusDataType dataType, int unitId)
{
    int registerCount = (dataType == ModbusDataType::Float32) ? 2 : 
                       (dataType == ModbusDataType::Double64) ? 4 :
                       (dataType == ModbusDataType::Long32) ? 2 :
                       (dataType == ModbusDataType::Long64) ? 4 : 1;
    // Use address directly (Modbus addresses are 0-based)
    readHoldingRegisters(address, registerCount, dataType, unitId);
}

void ModbusManager::readInputRegister(int address, ModbusDataType dataType, int unitId)
{
    int registerCount = (dataType == ModbusDataType::Float32) ? 2 : 
                       (dataType == ModbusDataType::Double64) ? 4 :
                       (dataType == ModbusDataType::Long32) ? 2 :
                       (dataType == ModbusDataType::Long64) ? 4 : 1;
    // Use address directly (Modbus addresses are 0-based)
    readInputRegisters(address, registerCount, dataType, unitId);
}

void ModbusManager::readCoil(int address, int unitId)
{
    // Use address directly (Modbus addresses are 0-based)
    readCoils(address, 1, unitId);
}

void ModbusManager::readDiscreteInput(int address, int unitId)
{
    // Use address directly (Modbus addresses are 0-based)
    readDiscreteInputs(address, 1, unitId);
}

// Multiple read operations
void ModbusManager::readHoldingRegisters(int startAddress, int count, ModbusDataType dataType, int unitId)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        return;
    }
    
    // Check register count limit
    if (count > 125) {
        emit errorOccurred(QString("Register count (%1) exceeds maximum limit of 125 registers").arg(count));
        return;
    }
    
    // Create request and add to queue
    ModbusRequest request;
    request.type = ModbusRequest::ReadHoldingRegisters;
    request.startAddress = startAddress;
    request.count = count;
    request.unitId = unitId;
    request.dataType = dataType;
    request.requestTime = QDateTime::currentMSecsSinceEpoch();
    
    queueRequest(request);
}

void ModbusManager::readInputRegisters(int startAddress, int count, ModbusDataType dataType, int unitId)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        return;
    }
    
    // Check register count limit
    if (count > 125) {
        emit errorOccurred(QString("Register count (%1) exceeds maximum limit of 125 registers").arg(count));
        return;
    }
    
    // Create request and add to queue
    ModbusRequest request;
    request.type = ModbusRequest::ReadInputRegisters;
    request.startAddress = startAddress;
    request.count = count;
    request.unitId = unitId;
    request.dataType = dataType;
    request.requestTime = QDateTime::currentMSecsSinceEpoch();
    
    queueRequest(request);
}

void ModbusManager::readCoils(int startAddress, int count, int unitId)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        return;
    }
    
    // Check coil count limit
    if (count > 125) {
        emit errorOccurred(QString("Coil count (%1) exceeds maximum limit of 125 coils").arg(count));
        return;
    }
    
    // Create request and add to queue
    ModbusRequest request;
    request.type = ModbusRequest::ReadCoils;
    request.startAddress = startAddress;
    request.count = count;
    request.unitId = unitId;
    request.dataType = ModbusDataType::Coil;
    request.requestTime = QDateTime::currentMSecsSinceEpoch();
    
    queueRequest(request);
}

void ModbusManager::readDiscreteInputs(int startAddress, int count, int unitId)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        return;
    }
    
    // Check discrete input count limit
    if (count > 125) {
        emit errorOccurred(QString("Discrete input count (%1) exceeds maximum limit of 125 inputs").arg(count));
        return;
    }
    
    // Create request and add to queue
    ModbusRequest request;
    request.type = ModbusRequest::ReadDiscreteInputs;
    request.startAddress = startAddress;
    request.count = count;
    request.unitId = unitId;
    request.dataType = ModbusDataType::DiscreteInput;
    request.requestTime = QDateTime::currentMSecsSinceEpoch();
    
    queueRequest(request);
}

// Single write operations
void ModbusManager::writeHoldingRegister(int address, quint16 value, int unitId)
{
    writeHoldingRegisters(address, QVector<quint16>() << value, unitId);
}

void ModbusManager::writeHoldingRegisterFloat32(int address, float value, int unitId)
{
    auto registers = float32ToRegisters(value);
    writeHoldingRegisters(address, QVector<quint16>() << registers.first << registers.second, unitId);
}

void ModbusManager::writeHoldingRegisterDouble64(int address, double value, int unitId)
{
    auto registers = double64ToRegisters(value);
    writeHoldingRegisters(address, registers, unitId);
}

void ModbusManager::writeHoldingRegisterLong32(int address, qint32 value, int unitId)
{
    auto registers = long32ToRegisters(value);
    writeHoldingRegisters(address, QVector<quint16>() << registers.first << registers.second, unitId);
}

void ModbusManager::writeHoldingRegisterLong64(int address, qint64 value, int unitId)
{
    auto registers = long64ToRegisters(value);
    writeHoldingRegisters(address, registers, unitId);
}

void ModbusManager::writeCoil(int address, bool value, int unitId)
{
    writeCoils(address, QVector<bool>() << value, unitId);
}

// Multiple write operations
void ModbusManager::writeHoldingRegisters(int startAddress, const QVector<quint16> &values, int unitId)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        return;
    }
    
    // Check register count limit
    if (values.size() > 125) {
        emit errorOccurred(QString("Register count (%1) exceeds maximum limit of 125 registers").arg(values.size()));
        return;
    }
    
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, startAddress, values.size());
    for (int i = 0; i < values.size(); ++i) {
        writeUnit.setValue(i, values[i]);
    }
    
    if (auto *reply = m_modbusClient->sendWriteRequest(writeUnit, unitId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onWriteReady);
            m_replyAddressMap[reply] = qMakePair(startAddress, values.size());
        } else {
            delete reply;
        }
    }
}

void ModbusManager::writeHoldingRegistersFloat32(int startAddress, const QVector<float> &values, int unitId)
{
    QVector<quint16> registers;
    for (const float &value : values) {
        auto regs = float32ToRegisters(value);
        registers << regs.first << regs.second;
    }
    writeHoldingRegisters(startAddress, registers, unitId);
}

void ModbusManager::writeHoldingRegistersDouble64(int startAddress, const QVector<double> &values, int unitId)
{
    QVector<quint16> registers;
    for (const double &value : values) {
        auto regs = double64ToRegisters(value);
        registers += regs;
    }
    writeHoldingRegisters(startAddress, registers, unitId);
}

void ModbusManager::writeHoldingRegistersLong32(int startAddress, const QVector<qint32> &values, int unitId)
{
    QVector<quint16> registers;
    for (const qint32 &value : values) {
        auto regs = long32ToRegisters(value);
        registers << regs.first << regs.second;
    }
    writeHoldingRegisters(startAddress, registers, unitId);
}

void ModbusManager::writeHoldingRegistersLong64(int startAddress, const QVector<qint64> &values, int unitId)
{
    QVector<quint16> registers;
    for (const qint64 &value : values) {
        auto regs = long64ToRegisters(value);
        registers += regs;
    }
    writeHoldingRegisters(startAddress, registers, unitId);
}

void ModbusManager::writeCoils(int startAddress, const QVector<bool> &values, int unitId)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        return;
    }
    
    // Check coil count limit
    if (values.size() > 125) {
        emit errorOccurred(QString("Coil count (%1) exceeds maximum limit of 125 coils").arg(values.size()));
        return;
    }
    
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils, startAddress, values.size());
    for (int i = 0; i < values.size(); ++i) {
        writeUnit.setValue(i, values[i] ? 1 : 0);
    }
    
    if (auto *reply = m_modbusClient->sendWriteRequest(writeUnit, unitId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onWriteReady);
            m_replyAddressMap[reply] = qMakePair(startAddress, values.size());
        } else {
            delete reply;
        }
    }
}

// IEEE 754 utility functions
bool ModbusManager::isFloat32Valid(float value)
{
    return std::isfinite(value);
}

bool ModbusManager::isDouble64Valid(double value)
{
    return std::isfinite(value);
}

bool ModbusManager::isFloat32NaN(float value)
{
    return std::isnan(value);
}

bool ModbusManager::isDouble64NaN(double value)
{
    return std::isnan(value);
}

bool ModbusManager::isFloat32Inf(float value)
{
    return std::isinf(value);
}

bool ModbusManager::isDouble64Inf(double value)
{
    return std::isinf(value);
}

bool ModbusManager::isFloat32Denormalized(float value)
{
    if (std::isnan(value) || std::isinf(value) || value == 0.0f)
        return false;
    return std::abs(value) < std::numeric_limits<float>::min();
}

bool ModbusManager::isDouble64Denormalized(double value)
{
    if (std::isnan(value) || std::isinf(value) || value == 0.0)
        return false;
    return std::abs(value) < std::numeric_limits<double>::min();
}

// Data conversion utilities
float ModbusManager::registersToFloat32(quint16 reg1, quint16 reg2)
{
    union {
        float f;
        quint32 i;
    } converter;
    
    converter.i = (static_cast<quint32>(reg1) << 16) | reg2;
    return converter.f;
}

double ModbusManager::registersToDouble64(quint16 reg1, quint16 reg2, quint16 reg3, quint16 reg4)
{
    union {
        double d;
        quint64 i;
    } converter;
    
    converter.i = (static_cast<quint64>(reg1) << 48) | 
                  (static_cast<quint64>(reg2) << 32) |
                  (static_cast<quint64>(reg3) << 16) | 
                  static_cast<quint64>(reg4);
    return converter.d;
}

qint32 ModbusManager::registersToLong32(quint16 reg1, quint16 reg2)
{
    union {
        qint32 l;
        quint32 i;
    } converter;
    
    converter.i = (static_cast<quint32>(reg1) << 16) | reg2;
    return converter.l;
}

qint64 ModbusManager::registersToLong64(quint16 reg1, quint16 reg2, quint16 reg3, quint16 reg4)
{
    union {
        qint64 l;
        quint64 i;
    } converter;
    
    converter.i = (static_cast<quint64>(reg1) << 48) | 
                  (static_cast<quint64>(reg2) << 32) |
                  (static_cast<quint64>(reg3) << 16) | 
                  static_cast<quint64>(reg4);
    return converter.l;
}

QPair<quint16, quint16> ModbusManager::float32ToRegisters(float value)
{
    union {
        float f;
        quint32 i;
    } converter;
    
    converter.f = value;
    quint16 reg1 = static_cast<quint16>(converter.i >> 16);
    quint16 reg2 = static_cast<quint16>(converter.i & 0xFFFF);
    
    return qMakePair(reg1, reg2);
}

QVector<quint16> ModbusManager::double64ToRegisters(double value)
{
    union {
        double d;
        quint64 i;
    } converter;
    
    converter.d = value;
    
    QVector<quint16> registers(4);
    registers[0] = static_cast<quint16>(converter.i >> 48);
    registers[1] = static_cast<quint16>((converter.i >> 32) & 0xFFFF);
    registers[2] = static_cast<quint16>((converter.i >> 16) & 0xFFFF);
    registers[3] = static_cast<quint16>(converter.i & 0xFFFF);
    
    return registers;
}

QPair<quint16, quint16> ModbusManager::long32ToRegisters(qint32 value)
{
    union {
        qint32 l;
        quint32 i;
    } converter;
    
    converter.l = value;
    quint16 reg1 = static_cast<quint16>((converter.i >> 16) & 0xFFFF);
    quint16 reg2 = static_cast<quint16>(converter.i & 0xFFFF);
    
    return qMakePair(reg1, reg2);
}

QVector<quint16> ModbusManager::long64ToRegisters(qint64 value)
{
    union {
        qint64 l;
        quint64 i;
    } converter;
    
    converter.l = value;
    QVector<quint16> registers(4);
    registers[0] = static_cast<quint16>((converter.i >> 48) & 0xFFFF);
    registers[1] = static_cast<quint16>((converter.i >> 32) & 0xFFFF);
    registers[2] = static_cast<quint16>((converter.i >> 16) & 0xFFFF);
    registers[3] = static_cast<quint16>(converter.i & 0xFFFF);
    
    return registers;
}

// Boolean conversion functions with comprehensive error handling
bool ModbusManager::registerToBool(quint16 reg)
{
    // Simple conversion: non-zero register value = true
    return reg != 0;
}

bool ModbusManager::numericToBool(double value)
{
    // Handle special IEEE 754 cases
    if (std::isnan(value)) {
        qDebug() << "Warning: Converting NaN to boolean, returning false";
        return false;
    }
    
    if (std::isinf(value)) {
        qDebug() << "Warning: Converting infinity to boolean, returning" << (value > 0 ? "true" : "false");
        return value > 0;
    }
    
    if (isDouble64Denormalized(value)) {
        qDebug() << "Warning: Converting denormalized number to boolean, treating as non-zero";
        return value != 0.0;
    }
    
    // Convert numeric value to boolean (non-zero = true)
    return value != 0.0;
}

quint16 ModbusManager::boolToRegister(bool value)
{
    // Standard boolean to register conversion: true=1, false=0
    return value ? 1 : 0;
}

double ModbusManager::boolToNumeric(bool value)
{
    // Standard boolean to numeric conversion: true=1.0, false=0.0
    return value ? 1.0 : 0.0;
}

// Queue management methods
void ModbusManager::queueRequest(const ModbusRequest &request)
{
    m_requestQueue.enqueue(request);
    
    // Start processing if no request is in progress
    if (!m_requestInProgress && !m_requestTimer->isActive()) {
        m_requestTimer->start(0); // Process immediately
    }
}

void ModbusManager::processNextRequest()
{
    if (m_requestQueue.isEmpty() || m_requestInProgress) {
        return;
    }
    
    ModbusRequest request = m_requestQueue.dequeue();
    executeRequest(request);
}

void ModbusManager::executeRequest(const ModbusRequest &request)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to Modbus server");
        // Schedule next request
        if (!m_requestQueue.isEmpty()) {
            m_requestTimer->start(m_requestInterval);
        }
        return;
    }
    
    m_requestInProgress = true;
    m_currentRequestTime = QDateTime::currentMSecsSinceEpoch();
    
    QModbusDataUnit readUnit;
    
    switch (request.type) {
        case ModbusRequest::ReadHoldingRegisters:
            readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, request.startAddress, request.count);
            break;
        case ModbusRequest::ReadInputRegisters:
            readUnit = QModbusDataUnit(QModbusDataUnit::InputRegisters, request.startAddress, request.count);
            break;
        case ModbusRequest::ReadCoils:
            readUnit = QModbusDataUnit(QModbusDataUnit::Coils, request.startAddress, request.count);
            break;
        case ModbusRequest::ReadDiscreteInputs:
            readUnit = QModbusDataUnit(QModbusDataUnit::DiscreteInputs, request.startAddress, request.count);
            break;
        case ModbusRequest::WriteHoldingRegisters:
            // Write operations will be handled separately
            break;
        case ModbusRequest::WriteCoils:
            // Write operations will be handled separately
            break;
    }
    
    if (auto *reply = m_modbusClient->sendReadRequest(readUnit, request.unitId)) {
        if (!reply->isFinished()) {
            m_currentReply = reply;
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onReadReady);
            m_pendingReads[reply] = request.dataType;
            m_replyAddressMap[reply] = qMakePair(request.startAddress, request.count);
            
            // Start timeout timer
            m_timeoutTimer->start(m_requestTimeout);
        } else {
            delete reply;
            completeCurrentRequest();
        }
    } else {
        completeCurrentRequest();
    }
}

void ModbusManager::completeCurrentRequest()
{
    m_requestInProgress = false;
    m_currentReply = nullptr;
    m_timeoutTimer->stop();
    
    // Schedule next request
    if (!m_requestQueue.isEmpty()) {
        m_requestTimer->start(m_requestInterval);
    }
}

void ModbusManager::handleRequestTimeout()
{
    if (m_currentReply) {
        m_currentReply->deleteLater();
        m_pendingReads.remove(m_currentReply);
        m_replyAddressMap.remove(m_currentReply);
        
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_currentRequestTime;
        emit errorOccurred(QString("Modbus request timeout after %1ms").arg(elapsed));
    }
    
    completeCurrentRequest();
}

// Private slots
void ModbusManager::onRequestTimeout()
{
    handleRequestTimeout();
}

void ModbusManager::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
    
    // Stop timeout timer since we got a response
    if (reply == m_currentReply) {
        m_timeoutTimer->stop();
    }
        
    ModbusDataType dataType = m_pendingReads.value(reply, ModbusDataType::HoldingRegister);
    ModbusReadResult result = processReadReply(reply, dataType);
    
    m_pendingReads.remove(reply);
    m_replyAddressMap.remove(reply);
    reply->deleteLater();
    
    // Complete current request and process next one
    if (reply == m_currentReply) {
        completeCurrentRequest();
    }
    
    emit readCompleted(result);
}

void ModbusManager::onWriteReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
        
    auto addressInfo = m_replyAddressMap.value(reply, qMakePair(0, 0));
    ModbusWriteResult result = processWriteReply(reply, addressInfo.first, addressInfo.second);
    
    m_replyAddressMap.remove(reply);
    reply->deleteLater();
    
    emit writeCompleted(result);
}

void ModbusManager::onStateChanged(QModbusDevice::State state)
{
    // Only emit connection state changes for definitive states
    // Don't emit disconnection during intermediate connecting states
    if (state == QModbusDevice::ConnectedState) {
        emit connectionStateChanged(true);
        qDebug() << "Connected to Modbus server";
    } else if (state == QModbusDevice::UnconnectedState) {
        emit connectionStateChanged(false);
        qDebug() << "Disconnected from Modbus server";
    }
    // Don't emit signals for ConnectingState or ClosingState to avoid
    // premature queue clearing in ModbusWorker
}

void ModbusManager::onErrorOccurred(QModbusDevice::Error error)
{
    QString errorString = m_modbusClient->errorString();
    qDebug() << "Modbus error (code" << static_cast<int>(error) << "):" << errorString;
    emit errorOccurred(errorString);
}

// Helper functions
ModbusReadResult ModbusManager::processReadReply(QModbusReply *reply, ModbusDataType dataType)
{
    ModbusReadResult result;
    result.timestamp = QDateTime::currentMSecsSinceEpoch();
    result.dataType = dataType;
    
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        result.success = true;
        result.startAddress = unit.startAddress();
        result.registerCount = unit.valueCount();
        result.hasValidData = true;
        
        // Extract raw data
        for (int i = 0; i < unit.valueCount(); ++i) {
            result.rawData.append(unit.value(i));
        }
        
        // Convert and validate data
        result.processedData = convertRawData(result.rawData, dataType);
        validateIEEE754Data(result);
        
    } else {
        result.success = false;
        result.errorType = reply->error();
        result.errorString = reply->errorString();
        result.hasValidData = false;
    }
    
    return result;
}

ModbusWriteResult ModbusManager::processWriteReply(QModbusReply *reply, int startAddress, int count)
{
    ModbusWriteResult result;
    result.timestamp = QDateTime::currentMSecsSinceEpoch();
    result.startAddress = startAddress;
    result.registerCount = count;
    
    if (reply->error() == QModbusDevice::NoError) {
        result.success = true;
    } else {
        result.success = false;
        result.errorType = reply->error();
        result.errorString = reply->errorString();
    }
    
    return result;
}

void ModbusManager::validateIEEE754Data(ModbusReadResult &result)
{
    if (result.dataType == ModbusDataType::Float32 || result.dataType == ModbusDataType::Double64) {
        for (auto it = result.processedData.begin(); it != result.processedData.end(); ++it) {
            if (result.dataType == ModbusDataType::Float32) {
                float value = it.value().toFloat();
                if (isFloat32NaN(value)) result.hasNaN = true;
                if (isFloat32Inf(value)) result.hasInf = true;
                if (isFloat32Denormalized(value)) result.hasDenormalized = true;
            } else if (result.dataType == ModbusDataType::Double64) {
                double value = it.value().toDouble();
                if (isDouble64NaN(value)) result.hasNaN = true;
                if (isDouble64Inf(value)) result.hasInf = true;
                if (isDouble64Denormalized(value)) result.hasDenormalized = true;
            }
        }
    } else if (result.dataType == ModbusDataType::BOOL) {
        // Validate BOOL data type conversions
        for (auto it = result.processedData.begin(); it != result.processedData.end(); ++it) {
            // Check if boolean conversion was successful
            QVariant value = it.value();
            if (!value.canConvert<bool>()) {
                qDebug() << "Warning: BOOL data type conversion failed for key:" << it.key() << "value:" << value;
                result.hasValidData = false;
            }
        }
    }
}

QVariantMap ModbusManager::convertRawData(const QVector<quint16> &rawData, ModbusDataType dataType)
{
    QVariantMap convertedData;
    
    switch (dataType) {
        case ModbusDataType::HoldingRegister:
        case ModbusDataType::InputRegister:
            for (int i = 0; i < rawData.size(); ++i) {
                convertedData[QString("register_%1").arg(i)] = rawData[i];
            }
            break;
            
        case ModbusDataType::Coil:
        case ModbusDataType::DiscreteInput:
            for (int i = 0; i < rawData.size(); ++i) {
                convertedData[QString("bit_%1").arg(i)] = (rawData[i] != 0);
            }
            break;
            
        case ModbusDataType::Float32:
            for (int i = 0; i < rawData.size(); i += 2) {
                if (i + 1 < rawData.size()) {
                    float value = registersToFloat32(rawData[i], rawData[i + 1]);
                    convertedData[QString("float32_%1").arg(i / 2)] = value;
                }
            }
            break;
            
        case ModbusDataType::Double64:
            for (int i = 0; i < rawData.size(); i += 4) {
                if (i + 3 < rawData.size()) {
                    double value = registersToDouble64(rawData[i], rawData[i + 1], 
                                                      rawData[i + 2], rawData[i + 3]);
                    convertedData[QString("double64_%1").arg(i / 4)] = value;
                }
            }
            break;
            
        case ModbusDataType::Long32:
            for (int i = 0; i < rawData.size(); i += 2) {
                if (i + 1 < rawData.size()) {
                    qint32 value = registersToLong32(rawData[i], rawData[i + 1]);
                    convertedData[QString("long32_%1").arg(i / 2)] = value;
                }
            }
            break;
            
        case ModbusDataType::Long64:
            for (int i = 0; i < rawData.size(); i += 4) {
                if (i + 3 < rawData.size()) {
                    qint64 value = registersToLong64(rawData[i], rawData[i + 1], 
                                                    rawData[i + 2], rawData[i + 3]);
                    convertedData[QString("long64_%1").arg(i / 4)] = value;
                }
            }
            break;
            
        case ModbusDataType::BOOL:
            for (int i = 0; i < rawData.size(); ++i) {
                bool value = registerToBool(rawData[i]);
                convertedData[QString("bool_%1").arg(i)] = value;
            }
            break;
    }
    
    return convertedData;
}
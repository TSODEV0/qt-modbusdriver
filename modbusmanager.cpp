#include "modbusmanager.h"
#include <QDebug>
#include <QDateTime>
#include <QDataStream>
#include <QByteArray>

ModbusManager::ModbusManager(QObject *parent)
    : QObject(parent)
    , m_modbusClient(nullptr)
{
    m_modbusClient = new QModbusTcpClient(this);
    
    connect(m_modbusClient, &QModbusClient::stateChanged,
            this, &ModbusManager::onStateChanged);
    connect(m_modbusClient, &QModbusDevice::errorOccurred,
            this, &ModbusManager::onErrorOccurred);
}

ModbusManager::~ModbusManager()
{
    if (m_modbusClient) {
        m_modbusClient->disconnectDevice();
    }
}

bool ModbusManager::connectToServer(const QString &host, int port)
{
    if (!m_modbusClient)
        return false;
        
    if (m_modbusClient->state() == QModbusDevice::ConnectedState)
        return true;
        
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, host);
    m_modbusClient->setTimeout(3000);
    m_modbusClient->setNumberOfRetries(3);
    
    return m_modbusClient->connectDevice();
}

void ModbusManager::disconnectFromServer()
{
    if (m_modbusClient && m_modbusClient->state() == QModbusDevice::ConnectedState) {
        m_modbusClient->disconnectDevice();
    }
}

bool ModbusManager::isConnected() const
{
    return m_modbusClient && m_modbusClient->state() == QModbusDevice::ConnectedState;
}

// Single read operations
void ModbusManager::readHoldingRegister(int address, ModbusDataType dataType)
{
    int registerCount = (dataType == ModbusDataType::Float32) ? 2 : 
                       (dataType == ModbusDataType::Double64) ? 4 :
                       (dataType == ModbusDataType::Long32) ? 2 :
                       (dataType == ModbusDataType::Long64) ? 4 : 1;
    // Use address directly (Modbus addresses are 0-based)
    readHoldingRegisters(address, registerCount, dataType);
}

void ModbusManager::readInputRegister(int address, ModbusDataType dataType)
{
    int registerCount = (dataType == ModbusDataType::Float32) ? 2 : 
                       (dataType == ModbusDataType::Double64) ? 4 :
                       (dataType == ModbusDataType::Long32) ? 2 :
                       (dataType == ModbusDataType::Long64) ? 4 : 1;
    // Use address directly (Modbus addresses are 0-based)
    readInputRegisters(address, registerCount, dataType);
}

void ModbusManager::readCoil(int address)
{
    // Use address directly (Modbus addresses are 0-based)
    readCoils(address, 1);
}

void ModbusManager::readDiscreteInput(int address)
{
    // Use address directly (Modbus addresses are 0-based)
    readDiscreteInputs(address, 1);
}

// Multiple read operations
void ModbusManager::readHoldingRegisters(int startAddress, int count, ModbusDataType dataType)
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
    
    // Use address directly (Modbus addresses are 0-based)
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, startAddress, count);
    
    if (auto *reply = m_modbusClient->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onReadReady);
            m_pendingReads[reply] = dataType;
            m_replyAddressMap[reply] = qMakePair(startAddress, count);
        } else {
            delete reply;
        }
    }
}

void ModbusManager::readInputRegisters(int startAddress, int count, ModbusDataType dataType)
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
    
    // Use address directly (Modbus addresses are 0-based)
    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters, startAddress, count);
    
    if (auto *reply = m_modbusClient->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onReadReady);
            m_pendingReads[reply] = dataType;
            m_replyAddressMap[reply] = qMakePair(startAddress, count);
        } else {
            delete reply;
        }
    }
}

void ModbusManager::readCoils(int startAddress, int count)
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
    
    QModbusDataUnit readUnit(QModbusDataUnit::Coils, startAddress, count);
    
    if (auto *reply = m_modbusClient->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onReadReady);
            m_pendingReads[reply] = ModbusDataType::Coil;
            m_replyAddressMap[reply] = qMakePair(startAddress, count);
        } else {
            delete reply;
        }
    }
}

void ModbusManager::readDiscreteInputs(int startAddress, int count)
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
    
    QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs, startAddress, count);
    
    if (auto *reply = m_modbusClient->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onReadReady);
            m_pendingReads[reply] = ModbusDataType::DiscreteInput;
            m_replyAddressMap[reply] = qMakePair(startAddress, count);
        } else {
            delete reply;
        }
    }
}

// Single write operations
void ModbusManager::writeHoldingRegister(int address, quint16 value)
{
    writeHoldingRegisters(address, QVector<quint16>() << value);
}

void ModbusManager::writeHoldingRegisterFloat32(int address, float value)
{
    auto registers = float32ToRegisters(value);
    writeHoldingRegisters(address, QVector<quint16>() << registers.first << registers.second);
}

void ModbusManager::writeHoldingRegisterDouble64(int address, double value)
{
    auto registers = double64ToRegisters(value);
    writeHoldingRegisters(address, registers);
}

void ModbusManager::writeHoldingRegisterLong32(int address, qint32 value)
{
    auto registers = long32ToRegisters(value);
    writeHoldingRegisters(address, QVector<quint16>() << registers.first << registers.second);
}

void ModbusManager::writeHoldingRegisterLong64(int address, qint64 value)
{
    auto registers = long64ToRegisters(value);
    writeHoldingRegisters(address, registers);
}

void ModbusManager::writeCoil(int address, bool value)
{
    writeCoils(address, QVector<bool>() << value);
}

// Multiple write operations
void ModbusManager::writeHoldingRegisters(int startAddress, const QVector<quint16> &values)
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
    
    if (auto *reply = m_modbusClient->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &ModbusManager::onWriteReady);
            m_replyAddressMap[reply] = qMakePair(startAddress, values.size());
        } else {
            delete reply;
        }
    }
}

void ModbusManager::writeHoldingRegistersFloat32(int startAddress, const QVector<float> &values)
{
    QVector<quint16> registers;
    for (const float &value : values) {
        auto regs = float32ToRegisters(value);
        registers << regs.first << regs.second;
    }
    writeHoldingRegisters(startAddress, registers);
}

void ModbusManager::writeHoldingRegistersDouble64(int startAddress, const QVector<double> &values)
{
    QVector<quint16> registers;
    for (const double &value : values) {
        auto regs = double64ToRegisters(value);
        registers += regs;
    }
    writeHoldingRegisters(startAddress, registers);
}

void ModbusManager::writeHoldingRegistersLong32(int startAddress, const QVector<qint32> &values)
{
    QVector<quint16> registers;
    for (const qint32 &value : values) {
        auto regs = long32ToRegisters(value);
        registers << regs.first << regs.second;
    }
    writeHoldingRegisters(startAddress, registers);
}

void ModbusManager::writeHoldingRegistersLong64(int startAddress, const QVector<qint64> &values)
{
    QVector<quint16> registers;
    for (const qint64 &value : values) {
        auto regs = long64ToRegisters(value);
        registers += regs;
    }
    writeHoldingRegisters(startAddress, registers);
}

void ModbusManager::writeCoils(int startAddress, const QVector<bool> &values)
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
    
    if (auto *reply = m_modbusClient->sendWriteRequest(writeUnit, 1)) {
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

// Private slots
void ModbusManager::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
        
    ModbusDataType dataType = m_pendingReads.value(reply, ModbusDataType::HoldingRegister);
    ModbusReadResult result = processReadReply(reply, dataType);
    
    m_pendingReads.remove(reply);
    m_replyAddressMap.remove(reply);
    reply->deleteLater();
    
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
    bool connected = (state == QModbusDevice::ConnectedState);
    emit connectionStateChanged(connected);
    
    if (connected) {
        qDebug() << "Connected to Modbus server";
    } else {
        qDebug() << "Disconnected from Modbus server";
    }
}

void ModbusManager::onErrorOccurred(QModbusDevice::Error error)
{
    QString errorString = m_modbusClient->errorString();
    qDebug() << "Modbus error:" << errorString;
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
    }
    
    return convertedData;
}
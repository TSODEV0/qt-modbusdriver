#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QObject>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QTimer>
#include <QVariant>
#include <QVector>
#include <QMap>
#include <cmath>
#include <limits>

// Data types supported by the Modbus manager
enum class ModbusDataType {
    HoldingRegister,
    InputRegister,
    Coil,
    DiscreteInput,
    Float32,
    Double64
};

// Operation modes for Modbus communication
enum class ModbusOperationMode {
    SingleRead,
    MultipleRead,
    SingleWrite,
    MultipleWrite
};

// Operation types for data access patterns
enum class ModbusOperationType {
    ReadOnly,
    WriteOnly,
    ReadWrite
};

// Comprehensive result structure for Modbus operations
struct ModbusReadResult {
    bool success;
    QString errorString;
    QModbusDevice::Error errorType;
    QVector<quint16> rawData;
    QVariantMap processedData;
    int startAddress;
    int registerCount;
    ModbusDataType dataType;
    qint64 timestamp;
    bool hasValidData;
    
    // IEEE 754 specific validation flags
    bool hasNaN;
    bool hasInf;
    bool hasDenormalized;
    
    ModbusReadResult() : success(false), errorType(QModbusDevice::NoError), 
                        startAddress(0), registerCount(0), 
                        dataType(ModbusDataType::HoldingRegister),
                        timestamp(0), hasValidData(false),
                        hasNaN(false), hasInf(false), hasDenormalized(false) {}
};

// Write operation result structure
struct ModbusWriteResult {
    bool success;
    QString errorString;
    QModbusDevice::Error errorType;
    int startAddress;
    int registerCount;
    qint64 timestamp;
    
    ModbusWriteResult() : success(false), errorType(QModbusDevice::NoError),
                         startAddress(0), registerCount(0), timestamp(0) {}
};

class ModbusManager : public QObject
{
    Q_OBJECT

public:
    explicit ModbusManager(QObject *parent = nullptr);
    ~ModbusManager();
    
    // Connection management
    bool connectToServer(const QString &host, int port = 502);
    void disconnectFromServer();
    bool isConnected() const;
    
    // Single read operations
    void readHoldingRegister(int address, ModbusDataType dataType = ModbusDataType::HoldingRegister);
    void readInputRegister(int address, ModbusDataType dataType = ModbusDataType::InputRegister);
    void readCoil(int address);
    void readDiscreteInput(int address);
    
    // Multiple read operations
    void readHoldingRegisters(int startAddress, int count, ModbusDataType dataType = ModbusDataType::HoldingRegister);
    void readInputRegisters(int startAddress, int count, ModbusDataType dataType = ModbusDataType::InputRegister);
    void readCoils(int startAddress, int count);
    void readDiscreteInputs(int startAddress, int count);
    
    // Single write operations
    void writeHoldingRegister(int address, quint16 value);
    void writeHoldingRegisterFloat32(int address, float value);
    void writeHoldingRegisterDouble64(int address, double value);
    void writeCoil(int address, bool value);
    
    // Multiple write operations
    void writeHoldingRegisters(int startAddress, const QVector<quint16> &values);
    void writeHoldingRegistersFloat32(int startAddress, const QVector<float> &values);
    void writeHoldingRegistersDouble64(int startAddress, const QVector<double> &values);
    void writeCoils(int startAddress, const QVector<bool> &values);
    
    // IEEE 754 utility functions
    static bool isFloat32Valid(float value);
    static bool isDouble64Valid(double value);
    static bool isFloat32NaN(float value);
    static bool isDouble64NaN(double value);
    static bool isFloat32Inf(float value);
    static bool isDouble64Inf(double value);
    static bool isFloat32Denormalized(float value);
    static bool isDouble64Denormalized(double value);
    
    // Data conversion utilities
    static float registersToFloat32(quint16 reg1, quint16 reg2);
    static double registersToDouble64(quint16 reg1, quint16 reg2, quint16 reg3, quint16 reg4);
    static QPair<quint16, quint16> float32ToRegisters(float value);
    static QVector<quint16> double64ToRegisters(double value);
    
signals:
    void readCompleted(const ModbusReadResult &result);
    void writeCompleted(const ModbusWriteResult &result);
    void connectionStateChanged(bool connected);
    void errorOccurred(const QString &error);
    
private slots:
    void onReadReady();
    void onWriteReady();
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    
private:
    QModbusTcpClient *m_modbusClient;
    QMap<QModbusReply*, ModbusDataType> m_pendingReads;
    QMap<QModbusReply*, QPair<int, int>> m_replyAddressMap; // startAddress, count
    
    // Helper functions
    ModbusReadResult processReadReply(QModbusReply *reply, ModbusDataType dataType);
    ModbusWriteResult processWriteReply(QModbusReply *reply, int startAddress, int count);
    void validateIEEE754Data(ModbusReadResult &result);
    QVariantMap convertRawData(const QVector<quint16> &rawData, ModbusDataType dataType);
};

#endif // MODBUSMANAGER_H
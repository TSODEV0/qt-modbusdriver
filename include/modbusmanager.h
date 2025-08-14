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
#include <QQueue>
#include <QSettings>
#include <cmath>
#include <limits>

// Data types supported by the Modbus manager
enum class ModbusDataType {
    HoldingRegister,
    InputRegister,
    Coil,
    DiscreteInput,
    Float32,
    Double64,
    Long32,
    Long64
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

// Request queue structure for proper sequencing
struct ModbusRequest {
    enum RequestType {
        ReadHoldingRegisters,
        ReadInputRegisters,
        ReadCoils,
        ReadDiscreteInputs,
        WriteHoldingRegisters,
        WriteCoils
    };
    
    RequestType type;
    int startAddress;
    int count;
    int unitId;
    ModbusDataType dataType;
    QVector<quint16> writeData;
    QVector<bool> writeBoolData;
    qint64 requestTime;
    
    ModbusRequest() : type(ReadHoldingRegisters), startAddress(0), count(1), unitId(1),
                     dataType(ModbusDataType::HoldingRegister), requestTime(0) {}
};

class ModbusManager : public QObject
{
    Q_OBJECT

public:
    explicit ModbusManager(QObject *parent = nullptr);
    ~ModbusManager();
    
    // Configuration loading
    bool loadConfigurationFromFile(const QString &configPath = "/home/Pttaov1/TSO_SCADA/qtworkplace/modbusdriver/config/config.ini");
    
    // Connection management
    bool connectToServer(const QString &host, int port = 502);
    void disconnectFromServer();
    bool isConnected() const;
    
    // Single register/coil operations
    void readHoldingRegister(int address, ModbusDataType dataType = ModbusDataType::HoldingRegister, int unitId = 1);
    void readInputRegister(int address, ModbusDataType dataType = ModbusDataType::InputRegister, int unitId = 1);
    void readCoil(int address, int unitId = 1);
    void readDiscreteInput(int address, int unitId = 1);
    
    // Multiple register/coil operations
    void readHoldingRegisters(int startAddress, int count, ModbusDataType dataType = ModbusDataType::HoldingRegister, int unitId = 1);
    void readInputRegisters(int startAddress, int count, ModbusDataType dataType = ModbusDataType::InputRegister, int unitId = 1);
    void readCoils(int startAddress, int count, int unitId = 1);
    void readDiscreteInputs(int startAddress, int count, int unitId = 1);
    
    // Single register/coil write operations
    void writeHoldingRegister(int address, quint16 value, int unitId = 1);
    void writeHoldingRegisterFloat32(int address, float value, int unitId = 1);
    void writeHoldingRegisterDouble64(int address, double value, int unitId = 1);
    void writeHoldingRegisterLong32(int address, qint32 value, int unitId = 1);
    void writeHoldingRegisterLong64(int address, qint64 value, int unitId = 1);
    void writeCoil(int address, bool value, int unitId = 1);
    
    // Multiple register/coil write operations
    void writeHoldingRegisters(int startAddress, const QVector<quint16> &values, int unitId = 1);
    void writeHoldingRegistersFloat32(int startAddress, const QVector<float> &values, int unitId = 1);
    void writeHoldingRegistersDouble64(int startAddress, const QVector<double> &values, int unitId = 1);
    void writeHoldingRegistersLong32(int startAddress, const QVector<qint32> &values, int unitId = 1);
    void writeHoldingRegistersLong64(int startAddress, const QVector<qint64> &values, int unitId = 1);
    void writeCoils(int startAddress, const QVector<bool> &values, int unitId = 1);
    
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
    static qint32 registersToLong32(quint16 reg1, quint16 reg2);
    static qint64 registersToLong64(quint16 reg1, quint16 reg2, quint16 reg3, quint16 reg4);
    static QPair<quint16, quint16> float32ToRegisters(float value);
    static QVector<quint16> double64ToRegisters(double value);
    static QPair<quint16, quint16> long32ToRegisters(qint32 value);
    static QVector<quint16> long64ToRegisters(qint64 value);
    
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
    void processNextRequest();
    void onRequestTimeout();
    
private:
    QModbusTcpClient *m_modbusClient;
    QMap<QModbusReply*, ModbusDataType> m_pendingReads;
    QMap<QModbusReply*, QPair<int, int>> m_replyAddressMap; // startAddress, count
    
    // Configuration settings
    QSettings *m_settings;
    
    // Connection resilience parameters from config.ini
    bool m_autoAdjust;
    int m_heartbeatInterval;
    int m_retryDelay;
    int m_maxRetries;
    int m_requestTimeout;
    int m_connectionTimeout;
    QString m_networkType;
    
    // Request queue management
    QQueue<ModbusRequest> m_requestQueue;
    QTimer *m_requestTimer;
    QTimer *m_timeoutTimer;
    bool m_requestInProgress;
    QModbusReply *m_currentReply;
    qint64 m_currentRequestTime;
    int m_requestInterval; // milliseconds between requests
    
    // Helper methods for data processing
    ModbusReadResult processReadReply(QModbusReply *reply, ModbusDataType dataType);
    ModbusWriteResult processWriteReply(QModbusReply *reply, int startAddress, int count);
    void validateIEEE754Data(ModbusReadResult &result);
    QVariantMap convertRawData(const QVector<quint16> &rawData, ModbusDataType dataType);
    
    // Request queue management
    void queueRequest(const ModbusRequest &request);
    void executeRequest(const ModbusRequest &request);
    void completeCurrentRequest();
    void handleRequestTimeout();
};

#endif // MODBUSMANAGER_H
#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QObject>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QTimer>
#include <QQueue>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QVariantMap>
#include <QSettings>
#include <QDateTime>

// Modbus data type enumeration
enum class ModbusDataType {
    HoldingRegister,
    InputRegister,
    Coil,
    DiscreteInput,
    Float32,
    Double64,
    Long32,
    Long64,
    BOOL
};

// Modbus request structure
struct ModbusRequest {
    enum Type {
        ReadHoldingRegisters,
        ReadInputRegisters,
        ReadCoils,
        ReadDiscreteInputs,
        WriteHoldingRegisters,
        WriteCoils
    };
    
    Type type;
    int startAddress;
    int count;
    int unitId;
    ModbusDataType dataType;
    qint64 requestTime;
    QVector<quint16> writeData;
    QVector<bool> writeBoolData;
    
    // Retry mechanism fields
    int retryCount;
    qint64 nextRetryTime;
    QString lastError;
    
    ModbusRequest() : type(ReadHoldingRegisters), startAddress(0), count(1), 
                     unitId(1), dataType(ModbusDataType::HoldingRegister), requestTime(0),
                     retryCount(0), nextRetryTime(0) {}
};

// Modbus read result structure
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
    
    // IEEE 754 validation flags
    bool hasNaN;
    bool hasInf;
    bool hasDenormalized;
    
    ModbusReadResult() : success(false), errorType(QModbusDevice::NoError), 
                        startAddress(0), registerCount(0), 
                        dataType(ModbusDataType::HoldingRegister), timestamp(0), 
                        hasValidData(false), hasNaN(false), hasInf(false), 
                        hasDenormalized(false) {}
};

// Modbus write result structure
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
    
    // Configuration
    bool loadConfigurationFromFile(const QString &configPath);
    
    // Connection management
    bool connectToServer(const QString &host, int port = 502);
    void disconnectFromServer();
    bool isConnected() const;
    
    // Single read operations
    void readHoldingRegister(int address, ModbusDataType dataType, int unitId = 1);
    void readInputRegister(int address, ModbusDataType dataType, int unitId = 1);
    void readCoil(int address, int unitId = 1);
    void readDiscreteInput(int address, int unitId = 1);
    
    // Multiple read operations
    void readHoldingRegisters(int startAddress, int count, ModbusDataType dataType, int unitId = 1);
    void readInputRegisters(int startAddress, int count, ModbusDataType dataType, int unitId = 1);
    void readCoils(int startAddress, int count, int unitId = 1);
    void readDiscreteInputs(int startAddress, int count, int unitId = 1);
    
    // Single write operations
    void writeHoldingRegister(int address, quint16 value, int unitId = 1);
    void writeHoldingRegisterFloat32(int address, float value, int unitId = 1);
    void writeHoldingRegisterDouble64(int address, double value, int unitId = 1);
    void writeHoldingRegisterLong32(int address, qint32 value, int unitId = 1);
    void writeHoldingRegisterLong64(int address, qint64 value, int unitId = 1);
    void writeCoil(int address, bool value, int unitId = 1);
    
    // Multiple write operations
    void writeHoldingRegisters(int startAddress, const QVector<quint16> &values, int unitId = 1);
    void writeHoldingRegistersFloat32(int startAddress, const QVector<float> &values, int unitId = 1);
    void writeHoldingRegistersDouble64(int startAddress, const QVector<double> &values, int unitId = 1);
    void writeHoldingRegistersLong32(int startAddress, const QVector<qint32> &values, int unitId = 1);
    void writeHoldingRegistersLong64(int startAddress, const QVector<qint64> &values, int unitId = 1);
    void writeCoils(int startAddress, const QVector<bool> &values, int unitId = 1);
    
    // IEEE 754 validation functions
    static bool isFloat32Valid(float value);
    static bool isDouble64Valid(double value);
    static bool isFloat32NaN(float value);
    static bool isDouble64NaN(double value);
    static bool isFloat32Inf(float value);
    static bool isDouble64Inf(double value);
    static bool isFloat32Denormalized(float value);
    static bool isDouble64Denormalized(double value);
    
    // Data conversion functions
    static float registersToFloat32(quint16 reg1, quint16 reg2);
    static double registersToDouble64(quint16 reg1, quint16 reg2, quint16 reg3, quint16 reg4);
    static qint32 registersToLong32(quint16 reg1, quint16 reg2);
    static qint64 registersToLong64(quint16 reg1, quint16 reg2, quint16 reg3, quint16 reg4);
    static QPair<quint16, quint16> float32ToRegisters(float value);
    static QVector<quint16> double64ToRegisters(double value);
    static QPair<quint16, quint16> long32ToRegisters(qint32 value);
    static QVector<quint16> long64ToRegisters(qint64 value);
    
    // Boolean conversion functions
    static bool registerToBool(quint16 reg);
    static bool numericToBool(double value);
    static quint16 boolToRegister(bool value);
    static double boolToNumeric(bool value);
    
    // Data processing functions
    static QVariantMap convertRawData(const QVector<quint16> &rawData, ModbusDataType dataType);

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
    void onRetryTimer();

private:
    // Core Modbus components
    QModbusTcpClient *m_modbusClient;
    QTimer *m_requestTimer;
    QTimer *m_timeoutTimer;
    QTimer *m_retryTimer;
    
    // Request queue management
    QQueue<ModbusRequest> m_requestQueue;
    QQueue<ModbusRequest> m_retryQueue;
    bool m_requestInProgress;
    QModbusReply *m_currentReply;
    ModbusRequest m_currentRequest;
    qint64 m_currentRequestTime;
    QMap<QModbusReply*, QPair<int, int>> m_replyAddressMap;
    QMap<QModbusReply*, ModbusDataType> m_pendingReads;
    
    // Configuration
    QSettings *m_settings;
    bool m_autoAdjust;
    int m_heartbeatInterval;
    int m_retryDelay;
    int m_maxRetries;
    int m_requestTimeout;
    int m_connectionTimeout;
    QString m_networkType;
    int m_requestInterval;
    
    // Helper methods
    void queueRequest(const ModbusRequest &request);
    void executeRequest(const ModbusRequest &request);
    void completeCurrentRequest();
    void handleRequestTimeout();
    void retryFailedRequest(const ModbusRequest &request, const QString &error);
    void processRetryQueue();
    qint64 calculateRetryDelay(int retryCount);
    ModbusReadResult processReadReply(QModbusReply *reply, ModbusDataType dataType);
    ModbusWriteResult processWriteReply(QModbusReply *reply, int startAddress, int count);
    void validateIEEE754Data(ModbusReadResult &result);
};

#endif // MODBUSMANAGER_H
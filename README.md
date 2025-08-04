# Modbus Driver - IEEE 754 Compliant

A comprehensive Qt C++ application implementing Modbus communication with full IEEE 754 standard compliance for floating-point operations. This project demonstrates various Modbus operation modes including single/multiple read/write operations for different data types.

## Features

### Modbus Communication
- **TCP/IP Communication**: Uses Qt SerialBus for reliable Modbus TCP communication
- **Connection Management**: Robust connection handling with status monitoring
- **Error Handling**: Comprehensive error detection and reporting
- **Timeout and Retry**: Configurable timeout and retry mechanisms

### Data Type Support
- **Holding Registers**: 16-bit register read/write operations
- **Input Registers**: 16-bit register read-only operations
- **Coils**: Boolean (bit) read/write operations
- **Discrete Inputs**: Boolean (bit) read-only operations
- **Float32**: IEEE 754 single-precision floating-point numbers
- **Double64**: IEEE 754 double-precision floating-point numbers

### Operation Modes

#### Single Operations
- **Single Read**: Individual reads for specific addresses
- **Single Write**: Individual writes for specific addresses

#### Multiple Operations
- **Multiple Read**: Bulk reads for multiple consecutive addresses
- **Multiple Write**: Bulk writes for multiple consecutive addresses

### IEEE 754 Compliance
- **NaN Detection**: Identifies Not-a-Number values
- **Infinity Detection**: Detects positive and negative infinity
- **Denormalized Number Detection**: Identifies subnormal floating-point values
- **Proper Conversion**: Accurate conversion between registers and floating-point values
- **Validation**: Comprehensive validation of floating-point data integrity

## Architecture

### Core Components

#### ModbusManager Class
The central component handling all Modbus communication:

```cpp
class ModbusManager : public QObject
{
    // Connection management
    bool connectToServer(const QString &host, int port = 502);
    void disconnectFromServer();
    bool isConnected() const;
    
    // Single read operations
    void readHoldingRegister(int address, ModbusDataType dataType);
    void readInputRegister(int address, ModbusDataType dataType);
    void readCoil(int address);
    void readDiscreteInput(int address);
    
    // Multiple read operations
    void readHoldingRegisters(int startAddress, int count, ModbusDataType dataType);
    void readInputRegisters(int startAddress, int count, ModbusDataType dataType);
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
};
```

#### Data Structures

**ModbusReadResult**: Comprehensive read operation result
```cpp
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
};
```

**ModbusWriteResult**: Write operation result
```cpp
struct ModbusWriteResult {
    bool success;
    QString errorString;
    QModbusDevice::Error errorType;
    int startAddress;
    int registerCount;
    qint64 timestamp;
};
```

#### Enumerations

**ModbusDataType**: Supported data types
```cpp
enum class ModbusDataType {
    HoldingRegister,
    InputRegister,
    Coil,
    DiscreteInput,
    Float32,
    Double64
};
```

**ModbusOperationMode**: Operation patterns
```cpp
enum class ModbusOperationMode {
    SingleRead,
    MultipleRead,
    SingleWrite,
    MultipleWrite
};
```

**ModbusOperationType**: Access patterns
```cpp
enum class ModbusOperationType {
    ReadOnly,
    WriteOnly,
    ReadWrite
};
```

### IEEE 754 Implementation

#### Utility Functions
- `isFloat32Valid()` / `isDouble64Valid()`: Validate finite numbers
- `isFloat32NaN()` / `isDouble64NaN()`: Detect NaN values
- `isFloat32Inf()` / `isDouble64Inf()`: Detect infinite values
- `isFloat32Denormalized()` / `isDouble64Denormalized()`: Detect subnormal numbers

#### Data Conversion
- `registersToFloat32()`: Convert two 16-bit registers to Float32
- `registersToDouble64()`: Convert four 16-bit registers to Double64
- `float32ToRegisters()`: Convert Float32 to two 16-bit registers
- `double64ToRegisters()`: Convert Double64 to four 16-bit registers

## User Interface

### Connection Tab
- **Host Configuration**: IP address input (default: 127.0.0.1)
- **Port Configuration**: Port number input (default: 502)
- **Connection Controls**: Connect/Disconnect buttons
- **Status Display**: Real-time connection status with color coding

### Read Operations Tab
- **Address Selection**: Target register/coil address
- **Count Selection**: Number of consecutive addresses to read
- **Data Type Selection**: Choose from available data types
- **Operation Buttons**: Various read operation modes

### Write Operations Tab
- **Address Selection**: Target register/coil address
- **Value Inputs**: Different input fields for various data types
  - Integer values (0-65535)
  - Float32 values (with 6 decimal precision)
  - Double64 values (with 10 decimal precision)
  - Boolean values (checkbox)
  - Multiple values (comma-separated)
- **Operation Buttons**: Various write operation modes

### Communication Log
- **Real-time Logging**: Timestamped operation logs
- **Detailed Results**: Complete operation results with data values
- **IEEE 754 Warnings**: Automatic detection and reporting of special values
- **Error Reporting**: Comprehensive error information
- **Log Management**: Clear log functionality

## Building and Running

### Prerequisites
- Qt 6.6.3 or later
- Qt SerialBus module
- Qt Network module
- C++17 compatible compiler
- CMake or qmake build system

### Build Instructions

#### Using Qt Creator
1. Open `modbusdriver.pro` in Qt Creator
2. Configure the project with Qt 6.6.3 kit
3. Build the project (Ctrl+B)
4. Run the application (Ctrl+R)

#### Using Command Line (qmake)
```bash
cd /path/to/modbusdriver
qmake modbusdriver.pro
make
./modbusdriver
```

### Testing

#### Modbus Server Setup
For testing, you can use various Modbus server simulators:

1. **ModbusPal**: Free Modbus simulator
2. **QModMaster**: Qt-based Modbus master/slave
3. **pymodbus**: Python-based Modbus server

#### Example Test Scenarios

**Basic Register Operations**:
1. Connect to Modbus server (127.0.0.1:502)
2. Write integer value 12345 to address 0
3. Read holding register from address 0
4. Verify the returned value matches

**Float32 Operations**:
1. Write Float32 value 3.14159 to address 10
2. Read Float32 from address 10
3. Verify IEEE 754 compliance and precision

**Multiple Operations**:
1. Write multiple values: 1,2,3,4,5 to address 20
2. Read 5 registers starting from address 20
3. Verify all values match

**IEEE 754 Special Values**:
1. Write NaN, +Inf, -Inf values
2. Read and verify proper detection of special values
3. Check warning flags in the log

## Code Examples

### Basic Usage

```cpp
// Initialize Modbus manager
ModbusManager *manager = new ModbusManager(this);

// Connect to server
manager->connectToServer("192.168.1.100", 502);

// Read single holding register
manager->readHoldingRegister(0, ModbusDataType::HoldingRegister);

// Write Float32 value
manager->writeHoldingRegisterFloat32(10, 3.14159f);

// Read multiple registers
manager->readHoldingRegisters(0, 10, ModbusDataType::HoldingRegister);

// Handle results
connect(manager, &ModbusManager::readCompleted, 
        [](const ModbusReadResult &result) {
    if (result.success) {
        qDebug() << "Read successful:" << result.processedData;
        if (result.hasNaN) qDebug() << "Warning: NaN detected";
        if (result.hasInf) qDebug() << "Warning: Infinity detected";
    } else {
        qDebug() << "Read failed:" << result.errorString;
    }
});
```

### IEEE 754 Validation

```cpp
// Check if a float value is valid
float testValue = 3.14159f;
if (ModbusManager::isFloat32Valid(testValue)) {
    qDebug() << "Value is finite and valid";
}

// Detect special values
if (ModbusManager::isFloat32NaN(testValue)) {
    qDebug() << "Value is NaN";
}

if (ModbusManager::isFloat32Inf(testValue)) {
    qDebug() << "Value is infinite";
}

if (ModbusManager::isFloat32Denormalized(testValue)) {
    qDebug() << "Value is denormalized";
}
```

## Error Handling

The application provides comprehensive error handling:

- **Connection Errors**: Network connectivity issues
- **Timeout Errors**: Communication timeouts
- **Protocol Errors**: Modbus protocol violations
- **Data Validation Errors**: Invalid data ranges or formats
- **IEEE 754 Warnings**: Special floating-point value detection

## Performance Considerations

- **Asynchronous Operations**: All Modbus operations are non-blocking
- **Efficient Data Conversion**: Optimized register-to-float conversions
- **Memory Management**: Proper cleanup of Qt objects and replies
- **Connection Pooling**: Reuse of TCP connections when possible

## Compliance and Standards

- **Modbus TCP/IP**: Full compliance with Modbus Application Protocol v1.1b3
- **IEEE 754-2008**: Complete implementation of binary floating-point arithmetic
- **Qt Standards**: Follows Qt coding conventions and best practices
- **C++17**: Modern C++ features for improved performance and safety

## License

This project is provided as an educational and demonstration tool for Modbus communication with IEEE 754 compliance.

## Contributing

Contributions are welcome! Please ensure:
- Code follows Qt coding standards
- IEEE 754 compliance is maintained
- Comprehensive testing of new features
- Documentation updates for new functionality

## Support

For issues, questions, or contributions, please refer to the project documentation or create an issue in the project repository.# qt-modbusdriver

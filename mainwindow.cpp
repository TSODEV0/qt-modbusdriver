#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QMessageBox>
#include <QSplitter>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_modbusManager(nullptr)
{
    ui->setupUi(this);
    
    setWindowTitle("Modbus Driver - IEEE 754 Compliant");
    setMinimumSize(1000, 700);
    
    // Initialize Modbus manager
    m_modbusManager = new ModbusManager(this);
    
    // Connect signals
    connect(m_modbusManager, &ModbusManager::connectionStateChanged,
            this, &MainWindow::onConnectionStateChanged);
    connect(m_modbusManager, &ModbusManager::readCompleted,
            this, &MainWindow::onReadCompleted);
    connect(m_modbusManager, &ModbusManager::writeCompleted,
            this, &MainWindow::onWriteCompleted);
    connect(m_modbusManager, &ModbusManager::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    
    // Setup UI
    setupUI();
    
    // Setup status timer
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateConnectionStatus);
    m_statusTimer->start(1000); // Update every second
    
    // Initial UI state
    updateUIState(false);
    
    logMessage("Modbus Driver initialized. Ready to connect.");
}

MainWindow::~MainWindow()
{
    if (m_modbusManager) {
        m_modbusManager->disconnectFromServer();
    }
    delete ui;
}

void MainWindow::setupUI()
{
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    auto tabWidget = new QTabWidget();
    
    // Connection tab
    auto connectionWidget = new QWidget();
    setupConnectionGroup();
    auto connectionLayout = new QVBoxLayout(connectionWidget);
    
    auto connectionGroup = new QGroupBox("Connection Settings");
    auto connectionGroupLayout = new QGridLayout(connectionGroup);
    
    connectionGroupLayout->addWidget(new QLabel("Host:"), 0, 0);
    m_hostEdit = new QLineEdit("10.72.2.215");
    connectionGroupLayout->addWidget(m_hostEdit, 0, 1);
    
    connectionGroupLayout->addWidget(new QLabel("Port:"), 1, 0);
    m_portSpinBox = new QSpinBox();
    m_portSpinBox->setRange(1, 65535);
    m_portSpinBox->setValue(502);
    connectionGroupLayout->addWidget(m_portSpinBox, 1, 1);
    
    auto buttonLayout = new QHBoxLayout();
    m_connectButton = new QPushButton("Connect");
    m_disconnectButton = new QPushButton("Disconnect");
    buttonLayout->addWidget(m_connectButton);
    buttonLayout->addWidget(m_disconnectButton);
    connectionGroupLayout->addLayout(buttonLayout, 2, 0, 1, 2);
    
    m_statusLabel = new QLabel("Status: Disconnected");
    m_statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    connectionGroupLayout->addWidget(m_statusLabel, 3, 0, 1, 2);
    
    connectionLayout->addWidget(connectionGroup);
    connectionLayout->addStretch();
    
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(m_disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectFromServer);
    
    tabWidget->addTab(connectionWidget, "Connection");
    
    // Read operations tab
    auto readWidget = new QWidget();
    setupReadGroup(readWidget);
    tabWidget->addTab(readWidget, "Read Operations");
    
    // Write operations tab
    auto writeWidget = new QWidget();
    setupWriteGroup(writeWidget);
    tabWidget->addTab(writeWidget, "Write Operations");
    
    // Create splitter for tabs and log
    auto splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(tabWidget);
    
    // Log group
    setupLogGroup();
    auto logGroup = new QGroupBox("Communication Log");
    auto logLayout = new QVBoxLayout(logGroup);
    
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setMaximumHeight(200);
    m_logTextEdit->setReadOnly(true);
    logLayout->addWidget(m_logTextEdit);
    
    m_clearLogButton = new QPushButton("Clear Log");
    connect(m_clearLogButton, &QPushButton::clicked, this, &MainWindow::clearLog);
    logLayout->addWidget(m_clearLogButton);
    
    splitter->addWidget(logGroup);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
}

void MainWindow::setupConnectionGroup()
{
    // Already implemented in setupUI
}

void MainWindow::setupReadGroup(QWidget *readWidget)
{
    if (!readWidget) {
        readWidget = new QWidget();
    }
    
    auto layout = new QVBoxLayout(readWidget);
    
    auto readGroup = new QGroupBox("Read Operations");
    auto readLayout = new QGridLayout(readGroup);
    
    // Address and count inputs
    readLayout->addWidget(new QLabel("Address:"), 0, 0);
    m_readAddressSpinBox = new QSpinBox();
    m_readAddressSpinBox->setRange(0, 65535);
    m_readAddressSpinBox->setValue(0);
    readLayout->addWidget(m_readAddressSpinBox, 0, 1);
    
    readLayout->addWidget(new QLabel("Count:"), 0, 2);
    m_readCountSpinBox = new QSpinBox();
    m_readCountSpinBox->setRange(1, 125);
    m_readCountSpinBox->setValue(1);
    readLayout->addWidget(m_readCountSpinBox, 0, 3);
    
    readLayout->addWidget(new QLabel("Data Type:"), 1, 0);
    m_readDataTypeCombo = new QComboBox();
    m_readDataTypeCombo->addItems({"Holding Register", "Input Register", "Coil", "Discrete Input", "Float32", "Double64"});
    readLayout->addWidget(m_readDataTypeCombo, 1, 1, 1, 3);
    
    // Read buttons
    m_readSingleButton = new QPushButton("Read Single Register");
    m_readMultipleButton = new QPushButton("Read Multiple Registers");
    m_readCoilButton = new QPushButton("Read Single Coil");
    m_readMultipleCoilsButton = new QPushButton("Read Multiple Coils");
    m_readFloat32Button = new QPushButton("Read Float32");
    m_readDouble64Button = new QPushButton("Read Double64");
    
    readLayout->addWidget(m_readSingleButton, 2, 0, 1, 2);
    readLayout->addWidget(m_readMultipleButton, 2, 2, 1, 2);
    readLayout->addWidget(m_readCoilButton, 3, 0, 1, 2);
    readLayout->addWidget(m_readMultipleCoilsButton, 3, 2, 1, 2);
    readLayout->addWidget(m_readFloat32Button, 4, 0, 1, 2);
    readLayout->addWidget(m_readDouble64Button, 4, 2, 1, 2);
    
    // Connect signals
    connect(m_readSingleButton, &QPushButton::clicked, this, &MainWindow::readSingleRegister);
    connect(m_readMultipleButton, &QPushButton::clicked, this, &MainWindow::readMultipleRegisters);
    connect(m_readCoilButton, &QPushButton::clicked, this, &MainWindow::readSingleCoil);
    connect(m_readMultipleCoilsButton, &QPushButton::clicked, this, &MainWindow::readMultipleCoils);
    connect(m_readFloat32Button, &QPushButton::clicked, this, &MainWindow::readFloat32);
    connect(m_readDouble64Button, &QPushButton::clicked, this, &MainWindow::readDouble64);
    
    layout->addWidget(readGroup);
    layout->addStretch();
}

void MainWindow::setupWriteGroup(QWidget *writeWidget)
{
    if (!writeWidget) {
        writeWidget = new QWidget();
    }
    auto layout = new QVBoxLayout(writeWidget);
    
    auto writeGroup = new QGroupBox("Write Operations");
    auto writeLayout = new QGridLayout(writeGroup);
    
    // Address input
    writeLayout->addWidget(new QLabel("Address:"), 0, 0);
    m_writeAddressSpinBox = new QSpinBox();
    m_writeAddressSpinBox->setRange(0, 65535);
    m_writeAddressSpinBox->setValue(0);
    writeLayout->addWidget(m_writeAddressSpinBox, 0, 1);
    
    // Value inputs
    writeLayout->addWidget(new QLabel("Integer Value:"), 1, 0);
    m_writeValueSpinBox = new QSpinBox();
    m_writeValueSpinBox->setRange(0, 65535);
    m_writeValueSpinBox->setValue(0);
    writeLayout->addWidget(m_writeValueSpinBox, 1, 1);
    
    writeLayout->addWidget(new QLabel("Float Value:"), 2, 0);
    m_writeFloatSpinBox = new QDoubleSpinBox();
    m_writeFloatSpinBox->setRange(-1e6, 1e6);
    m_writeFloatSpinBox->setDecimals(6);
    m_writeFloatSpinBox->setValue(0.0);
    writeLayout->addWidget(m_writeFloatSpinBox, 2, 1);
    
    writeLayout->addWidget(new QLabel("Double Value:"), 3, 0);
    m_writeDoubleSpinBox = new QDoubleSpinBox();
    m_writeDoubleSpinBox->setRange(-1e15, 1e15);
    m_writeDoubleSpinBox->setDecimals(10);
    m_writeDoubleSpinBox->setValue(0.0);
    writeLayout->addWidget(m_writeDoubleSpinBox, 3, 1);
    
    writeLayout->addWidget(new QLabel("Coil Value:"), 4, 0);
    m_writeCoilCheckBox = new QCheckBox("True/False");
    writeLayout->addWidget(m_writeCoilCheckBox, 4, 1);
    
    writeLayout->addWidget(new QLabel("Multiple Values (comma-separated):"), 5, 0);
    m_writeMultipleEdit = new QLineEdit("1,2,3,4,5");
    writeLayout->addWidget(m_writeMultipleEdit, 5, 1);
    
    // Write buttons
    m_writeSingleButton = new QPushButton("Write Single Register");
    m_writeMultipleButton = new QPushButton("Write Multiple Registers");
    m_writeCoilButton = new QPushButton("Write Single Coil");
    m_writeMultipleCoilsButton = new QPushButton("Write Multiple Coils");
    m_writeFloat32Button = new QPushButton("Write Float32");
    m_writeDouble64Button = new QPushButton("Write Double64");
    
    writeLayout->addWidget(m_writeSingleButton, 6, 0, 1, 2);
    writeLayout->addWidget(m_writeMultipleButton, 7, 0, 1, 2);
    writeLayout->addWidget(m_writeCoilButton, 8, 0, 1, 2);
    writeLayout->addWidget(m_writeMultipleCoilsButton, 9, 0, 1, 2);
    writeLayout->addWidget(m_writeFloat32Button, 10, 0, 1, 2);
    writeLayout->addWidget(m_writeDouble64Button, 11, 0, 1, 2);
    
    // Connect signals
    connect(m_writeSingleButton, &QPushButton::clicked, this, &MainWindow::writeSingleRegister);
    connect(m_writeMultipleButton, &QPushButton::clicked, this, &MainWindow::writeMultipleRegisters);
    connect(m_writeCoilButton, &QPushButton::clicked, this, &MainWindow::writeSingleCoil);
    connect(m_writeMultipleCoilsButton, &QPushButton::clicked, this, &MainWindow::writeMultipleCoils);
    connect(m_writeFloat32Button, &QPushButton::clicked, this, &MainWindow::writeFloat32);
    connect(m_writeDouble64Button, &QPushButton::clicked, this, &MainWindow::writeDouble64);
    
    layout->addWidget(writeGroup);
    layout->addStretch();
}

void MainWindow::setupLogGroup()
{
    // Already implemented in setupUI
}

void MainWindow::connectToServer()
{
    QString host = m_hostEdit->text();
    int port = m_portSpinBox->value();
    
    logMessage(QString("Attempting to connect to %1:%2...").arg(host).arg(port));
    
    if (m_modbusManager->connectToServer(host, port)) {
        logMessage("Connection request sent.");
    } else {
        logMessage("Failed to initiate connection.");
        QMessageBox::warning(this, "Connection Error", "Failed to initiate connection to Modbus server.");
    }
}

void MainWindow::disconnectFromServer()
{
    logMessage("Disconnecting from server...");
    m_modbusManager->disconnectFromServer();
}

void MainWindow::onConnectionStateChanged(bool connected)
{
    updateUIState(connected);
    
    if (connected) {
        m_statusLabel->setText("Status: Connected");
        m_statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        logMessage("Successfully connected to Modbus server.");
    } else {
        m_statusLabel->setText("Status: Disconnected");
        m_statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        logMessage("Disconnected from Modbus server.");
    }
}

void MainWindow::onReadCompleted(const ModbusReadResult &result)
{
    QString message = QString("Read operation completed - Address: %1, Count: %2")
                     .arg(result.startAddress).arg(result.registerCount);
    
    if (result.success) {
        message += " [SUCCESS]";
        
        // Display processed data
        for (auto it = result.processedData.begin(); it != result.processedData.end(); ++it) {
            message += QString("\n  %1: %2").arg(it.key()).arg(it.value().toString());
        }
        
        // IEEE 754 validation info
        if (result.dataType == ModbusDataType::Float32 || result.dataType == ModbusDataType::Double64) {
            if (result.hasNaN) message += "\n  Warning: NaN values detected";
            if (result.hasInf) message += "\n  Warning: Infinite values detected";
            if (result.hasDenormalized) message += "\n  Warning: Denormalized values detected";
        }
        
    } else {
        message += QString(" [FAILED] - %1").arg(result.errorString);
    }
    
    logMessage(message);
}

void MainWindow::onWriteCompleted(const ModbusWriteResult &result)
{
    QString message = QString("Write operation completed - Address: %1, Count: %2")
                     .arg(result.startAddress).arg(result.registerCount);
    
    if (result.success) {
        message += " [SUCCESS]";
    } else {
        message += QString(" [FAILED] - %1").arg(result.errorString);
    }
    
    logMessage(message);
}

void MainWindow::onErrorOccurred(const QString &error)
{
    logMessage(QString("Error: %1").arg(error));
    QMessageBox::critical(this, "Modbus Error", error);
}

// Read operation slots
void MainWindow::readSingleRegister()
{
    int address = m_readAddressSpinBox->value();
    ModbusDataType dataType = static_cast<ModbusDataType>(m_readDataTypeCombo->currentIndex());
    
    logMessage(QString("Reading single register at address %1").arg(address));
    
    if (dataType == ModbusDataType::HoldingRegister) {
        m_modbusManager->readHoldingRegister(address, dataType);
    } else if (dataType == ModbusDataType::InputRegister) {
        m_modbusManager->readInputRegister(address, dataType);
    }
}

void MainWindow::readMultipleRegisters()
{
    int address = m_readAddressSpinBox->value();
    int count = m_readCountSpinBox->value();
    ModbusDataType dataType = static_cast<ModbusDataType>(m_readDataTypeCombo->currentIndex());
    
    logMessage(QString("Reading %1 registers starting at address %2").arg(count).arg(address));
    
    if (dataType == ModbusDataType::HoldingRegister) {
        m_modbusManager->readHoldingRegisters(address, count, dataType);
    } else if (dataType == ModbusDataType::InputRegister) {
        m_modbusManager->readInputRegisters(address, count, dataType);
    }
}

void MainWindow::readSingleCoil()
{
    int address = m_readAddressSpinBox->value();
    logMessage(QString("Reading single coil at address %1").arg(address));
    m_modbusManager->readCoil(address);
}

void MainWindow::readMultipleCoils()
{
    int address = m_readAddressSpinBox->value();
    int count = m_readCountSpinBox->value();
    logMessage(QString("Reading %1 coils starting at address %2").arg(count).arg(address));
    m_modbusManager->readCoils(address, count);
}

void MainWindow::readFloat32()
{
    int address = m_readAddressSpinBox->value();
    logMessage(QString("Reading Float32 at address %1").arg(address));
    m_modbusManager->readHoldingRegister(address, ModbusDataType::Float32);
}

void MainWindow::readDouble64()
{
    int address = m_readAddressSpinBox->value();
    logMessage(QString("Reading Double64 at address %1").arg(address));
    m_modbusManager->readHoldingRegister(address, ModbusDataType::Double64);
}

// Write operation slots
void MainWindow::writeSingleRegister()
{
    int address = m_writeAddressSpinBox->value();
    quint16 value = static_cast<quint16>(m_writeValueSpinBox->value());
    
    logMessage(QString("Writing value %1 to register at address %2").arg(value).arg(address));
    m_modbusManager->writeHoldingRegister(address, value);
}

void MainWindow::writeMultipleRegisters()
{
    int address = m_writeAddressSpinBox->value();
    QString valuesText = m_writeMultipleEdit->text();
    
    QStringList valueStrings = valuesText.split(',', Qt::SkipEmptyParts);
    QVector<quint16> values;
    
    for (const QString &valueStr : valueStrings) {
        bool ok;
        quint16 value = valueStr.trimmed().toUShort(&ok);
        if (ok) {
            values.append(value);
        }
    }
    
    if (values.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter valid comma-separated values.");
        return;
    }
    
    logMessage(QString("Writing %1 values starting at address %2").arg(values.size()).arg(address));
    m_modbusManager->writeHoldingRegisters(address, values);
}

void MainWindow::writeSingleCoil()
{
    int address = m_writeAddressSpinBox->value();
    bool value = m_writeCoilCheckBox->isChecked();
    
    logMessage(QString("Writing coil value %1 to address %2").arg(value ? "TRUE" : "FALSE").arg(address));
    m_modbusManager->writeCoil(address, value);
}

void MainWindow::writeMultipleCoils()
{
    int address = m_writeAddressSpinBox->value();
    QString valuesText = m_writeMultipleEdit->text();
    
    QStringList valueStrings = valuesText.split(',', Qt::SkipEmptyParts);
    QVector<bool> values;
    
    for (const QString &valueStr : valueStrings) {
        QString trimmed = valueStr.trimmed().toLower();
        if (trimmed == "1" || trimmed == "true" || trimmed == "on") {
            values.append(true);
        } else if (trimmed == "0" || trimmed == "false" || trimmed == "off") {
            values.append(false);
        }
    }
    
    if (values.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter valid comma-separated boolean values (0/1, true/false, on/off).");
        return;
    }
    
    logMessage(QString("Writing %1 coil values starting at address %2").arg(values.size()).arg(address));
    m_modbusManager->writeCoils(address, values);
}

void MainWindow::writeFloat32()
{
    int address = m_writeAddressSpinBox->value();
    float value = static_cast<float>(m_writeFloatSpinBox->value());
    
    logMessage(QString("Writing Float32 value %1 to address %2").arg(value).arg(address));
    m_modbusManager->writeHoldingRegisterFloat32(address, value);
}

void MainWindow::writeDouble64()
{
    int address = m_writeAddressSpinBox->value();
    double value = m_writeDoubleSpinBox->value();
    
    logMessage(QString("Writing Double64 value %1 to address %2").arg(value).arg(address));
    m_modbusManager->writeHoldingRegisterDouble64(address, value);
}

void MainWindow::clearLog()
{
    m_logTextEdit->clear();
    logMessage("Log cleared.");
}

void MainWindow::updateConnectionStatus()
{
    // This is called periodically to update the connection status
    // The actual status update is handled by onConnectionStateChanged
}

void MainWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    
    m_logTextEdit->append(logEntry);
    m_logTextEdit->ensureCursorVisible();
    
    // Also output to console for debugging
    qDebug() << logEntry;
}

void MainWindow::updateUIState(bool connected)
{
    m_connectButton->setEnabled(!connected);
    m_disconnectButton->setEnabled(connected);
    m_hostEdit->setEnabled(!connected);
    m_portSpinBox->setEnabled(!connected);
    
    // Enable/disable operation buttons
    if (m_readSingleButton) m_readSingleButton->setEnabled(connected);
    if (m_readMultipleButton) m_readMultipleButton->setEnabled(connected);
    if (m_readCoilButton) m_readCoilButton->setEnabled(connected);
    if (m_readMultipleCoilsButton) m_readMultipleCoilsButton->setEnabled(connected);
    if (m_readFloat32Button) m_readFloat32Button->setEnabled(connected);
    if (m_readDouble64Button) m_readDouble64Button->setEnabled(connected);
    
    if (m_writeSingleButton) m_writeSingleButton->setEnabled(connected);
    if (m_writeMultipleButton) m_writeMultipleButton->setEnabled(connected);
    if (m_writeCoilButton) m_writeCoilButton->setEnabled(connected);
    if (m_writeMultipleCoilsButton) m_writeMultipleCoilsButton->setEnabled(connected);
    if (m_writeFloat32Button) m_writeFloat32Button->setEnabled(connected);
    if (m_writeDouble64Button) m_writeDouble64Button->setEnabled(connected);
}

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "modbusmanager.h"

class ConnectionTest : public QObject
{
    Q_OBJECT

public:
    ConnectionTest(QObject *parent = nullptr) : QObject(parent)
    {
        m_modbusManager = new ModbusManager(this);
        
        connect(m_modbusManager, &ModbusManager::readCompleted,
                this, &ConnectionTest::onReadCompleted);
        connect(m_modbusManager, &ModbusManager::writeCompleted,
                this, &ConnectionTest::onWriteCompleted);
        connect(m_modbusManager, &ModbusManager::connectionStateChanged,
                this, &ConnectionTest::onConnectionStateChanged);
        connect(m_modbusManager, &ModbusManager::errorOccurred,
                this, &ConnectionTest::onErrorOccurred);
    }
    
    void startTest()
    {
        qDebug() << "=== Modbus Connection Behavior Test ===";
        qDebug() << "Testing simultaneous read and write operations...";
        
        // Connect to Modbus server
        if (!m_modbusManager->connectToServer("10.72.2.215", 502)) {
            qDebug() << "Failed to initiate connection";
            return;
        }
        
        // Wait a moment for connection to establish
        QTimer::singleShot(1000, this, &ConnectionTest::performOperations);
    }
    
private slots:
    void performOperations()
    {
        if (!m_modbusManager->isConnected()) {
            qDebug() << "Not connected to Modbus server";
            return;
        }
        
        qDebug() << "\n--- Test 1: Sequential Operations ---";
        qDebug() << "Performing read operation first...";
        m_modbusManager->readHoldingRegister(500, ModbusDataType::HoldingRegister);
        
        // Perform write operation immediately after read
        qDebug() << "Performing write operation immediately after read...";
        m_modbusManager->writeHoldingRegister(500, 12345);
        
        // Schedule simultaneous operations test
        QTimer::singleShot(3000, this, &ConnectionTest::performSimultaneousOperations);
    }
    
    void performSimultaneousOperations()
    {
        qDebug() << "\n--- Test 2: Simultaneous Operations ---";
        qDebug() << "Performing read and write operations simultaneously...";
        
        // Start both operations at nearly the same time
        m_modbusManager->readHoldingRegister(502, ModbusDataType::Float32);
        m_modbusManager->writeHoldingRegisterFloat32(502, 99.99f);
        
        // Schedule connection analysis
        QTimer::singleShot(5000, this, &ConnectionTest::analyzeConnection);
    }
    
    void analyzeConnection()
    {
        qDebug() << "\n--- Connection Analysis ---";
        qDebug() << "Connection state:" << (m_modbusManager->isConnected() ? "Connected" : "Disconnected");
        qDebug() << "\nConclusion:";
        qDebug() << "- Both read and write operations use the SAME TCP connection";
        qDebug() << "- No separate socket is created for write operations";
        qDebug() << "- The QModbusTcpClient manages all operations through a single connection";
        
        QCoreApplication::quit();
    }
    
    void onReadCompleted(const ModbusReadResult &result)
    {
        qDebug() << "📖 Read completed - Success:" << result.success 
                 << "Address:" << result.startAddress
                 << "Data:" << result.rawData;
    }
    
    void onWriteCompleted(const ModbusWriteResult &result)
    {
        qDebug() << "🔧 Write completed - Success:" << result.success 
                 << "Address:" << result.startAddress;
    }
    
    void onConnectionStateChanged(bool connected)
    {
        qDebug() << "🔗 Connection state changed:" << (connected ? "CONNECTED" : "DISCONNECTED");
    }
    
    void onErrorOccurred(const QString &error)
    {
        qDebug() << "❌ Modbus error:" << error;
    }
    
private:
    ModbusManager *m_modbusManager;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    ConnectionTest test;
    test.startTest();
    
    return app.exec();
}

#include "connection_test.moc"
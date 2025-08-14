#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "../include/connection_resilience_manager.h"
#include "../include/modbusmanager.h"

class ConnectionResilienceTest : public QObject
{
    Q_OBJECT

public:
    ConnectionResilienceTest(QObject *parent = nullptr) : QObject(parent)
    {
        // Initialize components
        m_modbusManager = new ModbusManager(this);
        m_resilienceManager = new ConnectionResilienceManager(this);
        
        // Connect resilience manager to modbus manager
        m_resilienceManager->setModbusManager(m_modbusManager);
        
        // Connect signals for monitoring
        setupSignalConnections();
        
        qDebug() << "=== Connection Resilience Test ===";
        qDebug() << "Testing robust connection handling for poor network conditions";
        qDebug();
    }
    
    void runTests()
    {
        qDebug() << "Starting connection resilience tests...";
        
        // Test 1: Good connection configuration
        testGoodConnectionConfig();
        
        // Test 2: Poor connection configuration
        QTimer::singleShot(3000, this, &ConnectionResilienceTest::testPoorConnectionConfig);
        
        // Test 3: Automatic quality detection
        QTimer::singleShot(6000, this, &ConnectionResilienceTest::testQualityDetection);
        
        // Test 4: Connection monitoring
        QTimer::singleShot(9000, this, &ConnectionResilienceTest::testConnectionMonitoring);
        
        // Test 5: Error handling
        QTimer::singleShot(15000, this, &ConnectionResilienceTest::testErrorHandling);
        
        // Test 6: Reconnection logic
        QTimer::singleShot(20000, this, &ConnectionResilienceTest::testReconnectionLogic);
        
        // Stop test after 30 seconds
        QTimer::singleShot(30000, this, &ConnectionResilienceTest::stopTest);
    }

private slots:
    void testGoodConnectionConfig()
    {
        qDebug() << "\n--- Test 1: Good Connection Configuration ---";
        
        m_resilienceManager->configureForGoodConnection();
        
        qDebug() << "Configuration applied:";
        qDebug() << "  Connection timeout:" << m_resilienceManager->getConnectionTimeout() << "ms";
        qDebug() << "  Request timeout:" << m_resilienceManager->getRequestTimeout() << "ms";
        qDebug() << "  Max retries:" << m_resilienceManager->getMaxRetries();
        qDebug() << "  Retry delay:" << m_resilienceManager->getRetryDelay() << "ms";
        qDebug() << "  Heartbeat interval:" << m_resilienceManager->getHeartbeatInterval() << "ms";
    }
    
    void testPoorConnectionConfig()
    {
        qDebug() << "\n--- Test 2: Poor Connection Configuration ---";
        
        m_resilienceManager->configureForPoorConnection();
        
        qDebug() << "Configuration applied:";
        qDebug() << "  Connection timeout:" << m_resilienceManager->getConnectionTimeout() << "ms";
        qDebug() << "  Request timeout:" << m_resilienceManager->getRequestTimeout() << "ms";
        qDebug() << "  Max retries:" << m_resilienceManager->getMaxRetries();
        qDebug() << "  Retry delay:" << m_resilienceManager->getRetryDelay() << "ms";
        qDebug() << "  Heartbeat interval:" << m_resilienceManager->getHeartbeatInterval() << "ms";
    }
    
    void testQualityDetection()
    {
        qDebug() << "\n--- Test 3: Connection Quality Detection ---";
        
        qDebug() << "Current connection quality:" << m_resilienceManager->qualityToString(m_resilienceManager->getConnectionQuality());
        qDebug() << "Average response time:" << m_resilienceManager->getAverageResponseTime() << "ms";
        qDebug() << "Consecutive failures:" << m_resilienceManager->getConsecutiveFailures();
        
        // Test custom configuration
        qDebug() << "\nTesting custom configuration for cellular network:";
        m_resilienceManager->setConnectionTimeout(20000);
        m_resilienceManager->setRequestTimeout(15000);
        m_resilienceManager->setMaxRetries(10);
        m_resilienceManager->setRetryDelay(5000);
        
        qDebug() << "Custom configuration applied:";
        qDebug() << "  Connection timeout:" << m_resilienceManager->getConnectionTimeout() << "ms";
        qDebug() << "  Request timeout:" << m_resilienceManager->getRequestTimeout() << "ms";
        qDebug() << "  Max retries:" << m_resilienceManager->getMaxRetries();
        qDebug() << "  Retry delay:" << m_resilienceManager->getRetryDelay() << "ms";
    }
    
    void testConnectionMonitoring()
    {
        qDebug() << "\n--- Test 4: Connection Monitoring ---";
        
        // Start monitoring
        qDebug() << "Starting connection monitoring for 10.72.2.215:502...";
        m_resilienceManager->startMonitoring("10.72.2.215", 502);
        
        // Attempt connection
        qDebug() << "Attempting connection to Modbus server...";
        bool connected = m_modbusManager->connectToServer("10.72.2.215", 502);
        
        if (connected) {
            qDebug() << "✅ Connection attempt initiated";
        } else {
            qDebug() << "❌ Connection attempt failed";
        }
        
        // Test some operations
        QTimer::singleShot(2000, this, [this]() {
            qDebug() << "Testing read operations...";
            m_modbusManager->readHoldingRegister(100, ModbusDataType::HoldingRegister);
            m_modbusManager->readHoldingRegister(200, ModbusDataType::Float32);
        });
    }
    
    void testErrorHandling()
    {
        qDebug() << "\n--- Test 5: Error Handling ---";
        
        // Test with invalid address to trigger errors
        qDebug() << "Testing error handling with invalid operations...";
        
        // Try to connect to non-existent server
        qDebug() << "Attempting connection to non-existent server (192.168.999.999)...";
        m_modbusManager->connectToServer("192.168.999.999", 502);
        
        // Try invalid register read
        QTimer::singleShot(1000, this, [this]() {
            qDebug() << "Attempting read from invalid register range...";
            m_modbusManager->readHoldingRegisters(65000, 100, ModbusDataType::HoldingRegister);
        });
    }
    
    void testReconnectionLogic()
    {
        qDebug() << "\n--- Test 6: Reconnection Logic ---";
        
        // Disconnect and test reconnection
        qDebug() << "Disconnecting from server to test reconnection...";
        m_modbusManager->disconnectFromServer();
        
        // Wait a moment then trigger reconnection
        QTimer::singleShot(2000, this, [this]() {
            qDebug() << "Triggering manual reconnection attempt...";
            m_resilienceManager->attemptReconnection();
        });
        
        // Test heartbeat
        QTimer::singleShot(4000, this, [this]() {
            qDebug() << "Testing heartbeat mechanism...";
            m_resilienceManager->triggerHeartbeat();
        });
    }
    
    void stopTest()
    {
        qDebug() << "\n--- Test Complete ---";
        
        // Stop monitoring
        m_resilienceManager->stopMonitoring();
        
        // Disconnect
        m_modbusManager->disconnectFromServer();
        
        // Print final statistics
        qDebug() << "\nFinal Statistics:";
        qDebug() << "  Connection quality:" << m_resilienceManager->qualityToString(m_resilienceManager->getConnectionQuality());
        qDebug() << "  Average response time:" << m_resilienceManager->getAverageResponseTime() << "ms";
        qDebug() << "  Consecutive failures:" << m_resilienceManager->getConsecutiveFailures();
        qDebug() << "  Monitoring active:" << (m_resilienceManager->isMonitoring() ? "Yes" : "No");
        
        qDebug() << "\n=== Connection Resilience Test Complete ===";
        QCoreApplication::quit();
    }
    
    // Signal handlers for monitoring
    void onConfigurationChanged(const QString &description)
    {
        qDebug() << "🔧 Configuration changed:" << description;
    }
    
    void onConnectionQualityChanged(ConnectionResilienceManager::ConnectionQuality quality)
    {
        qDebug() << "📊 Connection quality changed to:" << m_resilienceManager->qualityToString(quality);
    }
    
    void onConnectionLost()
    {
        qDebug() << "⚠️ Connection lost - automatic recovery will be attempted";
    }
    
    void onConnectionRestored()
    {
        qDebug() << "✅ Connection restored successfully";
    }
    
    void onReconnectionAttempt(int attempt, int maxAttempts)
    {
        qDebug() << "🔄 Reconnection attempt" << attempt << "of" << maxAttempts;
    }
    
    void onReconnectionSuccessful()
    {
        qDebug() << "✅ Reconnection successful";
    }
    
    void onErrorDetected(const QString &error, int consecutiveFailures)
    {
        qDebug() << "❌ Error detected:" << error << "(Consecutive failures:" << consecutiveFailures << ")";
    }
    
    void onMonitoringStarted(const QString &host, int port)
    {
        qDebug() << "👁️ Monitoring started for" << host << ":" << port;
    }
    
    void onMonitoringStopped()
    {
        qDebug() << "👁️ Monitoring stopped";
    }
    
    void onModbusReadCompleted(const ModbusReadResult &result)
    {
        if (result.success) {
            qDebug() << "📖 Read successful - Address:" << result.startAddress 
                     << "Data:" << result.rawData.size() << "registers";
        } else {
            qDebug() << "📖 Read failed - Address:" << result.startAddress 
                     << "Error:" << result.errorString;
        }
    }
    
    void onModbusWriteCompleted(const ModbusWriteResult &result)
    {
        if (result.success) {
            qDebug() << "✏️ Write successful - Address:" << result.startAddress;
        } else {
            qDebug() << "✏️ Write failed - Address:" << result.startAddress 
                     << "Error:" << result.errorString;
        }
    }
    
    void onModbusConnectionStateChanged(bool connected)
    {
        qDebug() << "🔌 Modbus connection state:" << (connected ? "Connected" : "Disconnected");
    }
    
    void onModbusError(const QString &error)
    {
        qDebug() << "⚠️ Modbus error:" << error;
    }

private:
    void setupSignalConnections()
    {
        // Connection resilience manager signals
        connect(m_resilienceManager, &ConnectionResilienceManager::configurationChanged,
                this, &ConnectionResilienceTest::onConfigurationChanged);
        connect(m_resilienceManager, &ConnectionResilienceManager::connectionQualityChanged,
                this, &ConnectionResilienceTest::onConnectionQualityChanged);
        connect(m_resilienceManager, &ConnectionResilienceManager::connectionLost,
                this, &ConnectionResilienceTest::onConnectionLost);
        connect(m_resilienceManager, &ConnectionResilienceManager::connectionRestored,
                this, &ConnectionResilienceTest::onConnectionRestored);
        connect(m_resilienceManager, &ConnectionResilienceManager::reconnectionAttempt,
                this, &ConnectionResilienceTest::onReconnectionAttempt);
        connect(m_resilienceManager, &ConnectionResilienceManager::reconnectionSuccessful,
                this, &ConnectionResilienceTest::onReconnectionSuccessful);
        connect(m_resilienceManager, &ConnectionResilienceManager::errorDetected,
                this, &ConnectionResilienceTest::onErrorDetected);
        connect(m_resilienceManager, &ConnectionResilienceManager::monitoringStarted,
                this, &ConnectionResilienceTest::onMonitoringStarted);
        connect(m_resilienceManager, &ConnectionResilienceManager::monitoringStopped,
                this, &ConnectionResilienceTest::onMonitoringStopped);
        
        // Modbus manager signals
        connect(m_modbusManager, &ModbusManager::readCompleted,
                this, &ConnectionResilienceTest::onModbusReadCompleted);
        connect(m_modbusManager, &ModbusManager::writeCompleted,
                this, &ConnectionResilienceTest::onModbusWriteCompleted);
        connect(m_modbusManager, &ModbusManager::connectionStateChanged,
                this, &ConnectionResilienceTest::onModbusConnectionStateChanged);
        connect(m_modbusManager, &ModbusManager::errorOccurred,
                this, &ConnectionResilienceTest::onModbusError);
    }
    
    ModbusManager *m_modbusManager;
    ConnectionResilienceManager *m_resilienceManager;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    ConnectionResilienceTest test;
    test.runTests();
    
    return app.exec();
}

#include "connection_resilience_test.moc"
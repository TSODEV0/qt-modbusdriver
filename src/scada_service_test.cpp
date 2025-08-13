#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QLoggingCategory>
#include "../include/scada_core_service.h"
#include "../include/database_manager.h"

class ScadaServiceTest : public QObject
{
    Q_OBJECT

public:
    ScadaServiceTest(ScadaCoreService *service, QObject *parent = nullptr) 
        : QObject(parent), m_service(service), m_dbManager(nullptr) {}
    
    void runTest() {
        qDebug() << "=== SCADA Core Service Test with Database Integration ===";
        qDebug() << "Testing data acquisition with PostgreSQL configuration and InfluxDB integration";
        qDebug();
        
        // Create database manager
        m_dbManager = new DatabaseManager(this);
        
        // Connect database signals
        connect(m_dbManager, &DatabaseManager::databaseConnected,
                this, &ScadaServiceTest::onDatabaseConnected);
        connect(m_dbManager, &DatabaseManager::errorOccurred,
                this, &ScadaServiceTest::onDatabaseError);
        
        // Load configuration from file
        qDebug() << "Loading configuration from file...";
        if (!m_dbManager->loadConfigurationFromFile("/home/Pttaov1/TSO_SCADA/qtworkplace/modbusdriver/config/config.ini")) {
            qDebug() << "❌ Failed to load configuration:" << m_dbManager->lastError();
            QCoreApplication::quit();
            return;
        }
        
        // Connect to PostgreSQL database using configuration
        qDebug() << "Connecting to PostgreSQL database using configuration...";
        if (!m_dbManager->connectToDatabase()) {
            qDebug() << "❌ Failed to connect to database:" << m_dbManager->lastError();
            QCoreApplication::quit();
            return;
        }
    }
    
private slots:
    void onDatabaseConnected() {
        qDebug() << "✅ Database connected successfully";
        
        // Load Modbus device configurations from database
        QVector<ModbusDeviceConfig> devices = m_dbManager->loadModbusDevices();
        if (devices.isEmpty()) {
            qDebug() << "❌ No Modbus devices found in database";
            QCoreApplication::quit();
            return;
        }
        
        // Connect service signals (service already created and configured in main)
        connect(m_service, &ScadaCoreService::serviceStarted,
                this, &ScadaServiceTest::onServiceStarted);
        connect(m_service, &ScadaCoreService::serviceStopped,
                this, &ScadaServiceTest::onServiceStopped);
        connect(m_service, &ScadaCoreService::dataPointAcquired,
                this, &ScadaServiceTest::onDataPointAcquired);
        connect(m_service, &ScadaCoreService::dataPointSentToInflux,
                this, &ScadaServiceTest::onDataPointSentToInflux);
        connect(m_service, &ScadaCoreService::errorOccurred,
                this, &ScadaServiceTest::onErrorOccurred);
        connect(m_service, &ScadaCoreService::statisticsUpdated,
                this, &ScadaServiceTest::onStatisticsUpdated);
        connect(m_service, &ScadaCoreService::writeCompleted,
                this, &ScadaServiceTest::onWriteCompleted);
        
        // Configure Telegraf socket
        m_service->setTelegrafSocketPath("/tmp/telegraf.sock");
        qDebug() << "Telegraf socket configured:" << m_service->getTelegrafSocketPath();
        
        // Data points already configured in main() with optimization
        qDebug() << "✅ Using optimized data points configured in main():" << m_service->getDataPoints().size() << "points";
        
        // Start service
        qDebug() << "Starting SCADA Core Service with database configuration...";
        if (m_service->startService()) {
            qDebug() << "Service started successfully";
        } else {
            qDebug() << "Failed to start service";
            QCoreApplication::quit();
            return;
        }
        
        // Run for 60 seconds then stop
        QTimer::singleShot(60000, this, &ScadaServiceTest::stopTest);
    }
    
    void onDatabaseError(const QString &error) {
        qDebug() << "❌ Database error:" << error;
    }
    
    void onServiceStarted() {
        qDebug() << "✅ SCADA Core Service started with database configuration";
        qDebug() << "Data acquisition and InfluxDB integration active";
        qDebug();
        
        // Start write operations test after 5 seconds
        //QTimer::singleShot(5000, this, &ScadaServiceTest::testWriteOperationsFromDatabase);
    }
    
    void onServiceStopped() {
        qDebug() << "🛑 SCADA Core Service stopped";
        
        // Print final statistics
        auto stats = m_service->getStatistics();
        qDebug();
        qDebug() << "=== Final Service Statistics ===";
        qDebug() << "Total read operations:" << stats.totalReadOperations;
        qDebug() << "Successful reads:" << stats.successfulReads;
        qDebug() << "Failed reads:" << stats.failedReads;
        qDebug() << "Data points sent to InfluxDB:" << stats.totalDataPointsSent;
        qDebug() << "Socket errors:" << stats.socketErrors;
        qDebug() << "Average response time:" << QString::number(stats.averageResponseTime, 'f', 2) << "ms";
        
        qint64 runtime = QDateTime::currentMSecsSinceEpoch() - stats.serviceStartTime;
        qDebug() << "Total runtime:" << QString::number(runtime / 1000.0, 'f', 1) << "seconds";
        
        if (stats.totalReadOperations > 0) {
            double successRate = (double)stats.successfulReads / stats.totalReadOperations * 100.0;
            qDebug() << "Success rate:" << QString::number(successRate, 'f', 1) << "%";
        }
        
        qDebug();
        qDebug() << "=== SCADA Core Service Test Complete ===";
        QCoreApplication::quit();
    }
    
    void onDataPointAcquired(const AcquiredDataPoint &dataPoint) {
        if (dataPoint.isValid) {
            qDebug() << "📊 Data acquired:" << dataPoint.pointName 
                     << "=" << dataPoint.value.toString()
                     << "(" << dataPoint.measurement << ")"
                     << "Unit ID:" << dataPoint.tags.value("unit_id", "N/A");
        } else {
            qDebug() << "❌ Data acquisition failed:" << dataPoint.pointName 
                     << "Error:" << dataPoint.errorMessage;
        }
    }
    
    void onDataPointSentToInflux(const QString &pointName, bool success) {
        if (success) {
            qDebug() << "📤 Data sent to InfluxDB:" << pointName;
        } else {
            qDebug() << "❌ Failed to send to InfluxDB:" << pointName;
        }
    }
    
    void onErrorOccurred(const QString &error) {
        qDebug() << "⚠️  Service error:" << error;
    }
    
    void onStatisticsUpdated(const ScadaCoreService::ServiceStatistics &stats) {
        static int updateCount = 0;
        updateCount++;
        
        // Print statistics every 10 updates
        if (updateCount % 10 == 0) {
            qDebug() << "📈 Statistics update #" << updateCount;
            qDebug() << "   Total operations:" << stats.totalReadOperations;
            qDebug() << "   Success rate:" << 
                        (stats.totalReadOperations > 0 ? 
                         QString::number((double)stats.successfulReads / stats.totalReadOperations * 100.0, 'f', 1) + "%" :
                         "N/A");
            qDebug() << "   Data points sent:" << stats.totalDataPointsSent;
            qDebug() << "   Avg response time:" << QString::number(stats.averageResponseTime, 'f', 2) << "ms";
        }
    }
    
    void onWriteCompleted(const QString &operation, bool success, const QString &error) {
        if (success) {
            qDebug() << "✅ Write operation completed:" << operation;
        } else {
            qDebug() << "❌ Write operation failed:" << operation << "Error:" << error;
        }
    }
    
    void testWriteOperationsFromDatabase() {
        qDebug();
        qDebug() << "=== Starting Database-Driven Modbus Write & Read Test Operations ===";
        
        // Get first device from database for testing
        QVector<ModbusDeviceConfig> devices = m_dbManager->loadModbusDevices();
        if (devices.isEmpty()) {
            qDebug() << "❌ No devices available for write testing";
            return;
        }
        
        const ModbusDeviceConfig &device = devices.first();
        const QString host = device.ipAddress;
        const int port = device.port;
        
        qDebug() << "🎯 Using device:" << device.deviceName << "(" << host << ":" << port << ")" 
                 << "Unit ID:" << device.unitId;
        
        // // Test write operations using database configuration
        // qDebug() << "🔧 Test 1: Writing holding register (address 500, value 1234)";
        // m_service->writeHoldingRegister(host, port, 500, 1234);
        
        // QTimer::singleShot(2000, [this, host, port, device]() {
        //     qDebug() << "🔧 Test 2: Writing Float32 (address 502, value 25.75)";
        //     m_service->writeHoldingRegisterFloat32(host, port, 502, 25.75f);
        // });
        
        // QTimer::singleShot(4000, [this, host, port, device]() {
        //     qDebug() << "🔧 Test 3: Writing coil (address 10, value true)";
        //     m_service->writeCoil(host, port, 10, true);
        // });
        
        // qDebug() << "Write operations test scheduled using database configuration.";
        // qDebug();
    }
    
    void stopTest() {
        qDebug();
        qDebug() << "Stopping SCADA Core Service test...";
        if (m_service) {
            m_service->stopService();
        }
        if (m_dbManager) {
            m_dbManager->disconnectFromDatabase();
        }
    }
    
private:
    void setupDataPointsFromDatabase() {
        qDebug() << "Loading data acquisition points from database...";
        
        QVector<DataAcquisitionPoint> dataPoints = m_dbManager->loadDataPoints();
        
        if (dataPoints.isEmpty()) {
            qDebug() << "❌ No data points loaded from database";
            return;
        }
        
        // Add all data points to the service
        for (const auto &point : dataPoints) {
            m_service->addDataPoint(point);
        }
        
        qDebug() << "✅ Configured" << m_service->getDataPoints().size() << "data acquisition points from database:";
        for (const auto &point : m_service->getDataPoints()) {
            qDebug() << "  -" << point.name << "at" << point.host << ":" << point.port 
                     << "address" << point.address << "(" << point.measurement << ")"
                     << "Poll:" << point.pollInterval << "ms"
                     << "Unit ID:" << point.tags.value("unit_id", "N/A");
        }
        qDebug();
    }
    
    ScadaCoreService *m_service;
    DatabaseManager *m_dbManager;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    
    qDebug() << "🚀 Starting SCADA Service Test with Configuration File...";
    
    // Create database manager and load configuration
    DatabaseManager dbManager;
    if (!dbManager.loadConfigurationFromFile("/home/Pttaov1/TSO_SCADA/qtworkplace/modbusdriver/config/config.ini")) {
        qDebug() << "❌ Failed to load configuration:" << dbManager.lastError();
        return -1;
    }
    
    // Connect to database using configuration
    if (!dbManager.connectToDatabase()) {
        qDebug() << "❌ Database connection failed:" << dbManager.lastError();
        return -1;
    }
    
    // Load Modbus devices from database
    QVector<ModbusDeviceConfig> devices = dbManager.loadModbusDevices();
    if (devices.isEmpty()) {
        qDebug() << "⚠️  No Modbus devices found in database";
        return -1;
    }
    
    qDebug() << "📋 Loaded" << devices.size() << "Modbus devices from database";
    
    // Create SCADA core service
    ScadaCoreService scadaService;
    
    // Load data points from database
    QVector<DataAcquisitionPoint> dataPoints = dbManager.loadDataPoints();
    qDebug() << "📊 Loaded" << dataPoints.size() << "data acquisition points";
    
    // Optimize Modbus read blocks to reduce TCP connections
    QVector<DataAcquisitionPoint> optimizedDataPoints = dbManager.optimizeModbusReadBlocks(dataPoints);
    qDebug() << "Optimized to" << optimizedDataPoints.size() << "read blocks";
    
    // Use optimized data points for service
    dataPoints = optimizedDataPoints;
    
    // Configure SCADA service with loaded data points
    for (const auto &point : dataPoints) {
        scadaService.addDataPoint(point);
    }
    
    qDebug() << "SCADA Core Service Test Application with PostgreSQL Integration";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Testing database-driven configuration and InfluxDB integration";
    qDebug();
    
    ScadaServiceTest test(&scadaService);
    
    // Start test after event loop starts
    QTimer::singleShot(100, &test, &ScadaServiceTest::runTest);
    
    return app.exec();
}

#include "scada_service_test.moc"
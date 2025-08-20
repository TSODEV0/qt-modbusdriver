#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include "include/database_manager.h"
#include "include/scada_core_service.h"
#include "include/modbus_worker.h"

class EnhancedScadaTest : public QObject
{
    Q_OBJECT

public:
    explicit EnhancedScadaTest(QObject *parent = nullptr)
        : QObject(parent)
        , m_databaseManager(nullptr)
        , m_scadaService(nullptr)
        , m_workerManager(nullptr)
        , m_testRunning(false)
        , m_mockMode(false)
    {
        qDebug() << "Enhanced SCADA Test initialized at" << QDateTime::currentDateTime().toString();
    }

    ~EnhancedScadaTest()
    {
        cleanup();
    }

public slots:
    void startTest()
    {
        qDebug() << "\n=== Enhanced SCADA Test Starting ===";
        qDebug() << "Timestamp:" << QDateTime::currentDateTime().toString();
        
        // Initialize database manager
        m_databaseManager = new DatabaseManager(this);
        
        // Connect database signals
        // Note: DatabaseManager may not have these signals, so we'll check connection status directly
        
        // Attempt database connection
        qDebug() << "Connecting to database...";
        if (!m_databaseManager->connectToDatabase()) {
            qWarning() << "Failed to connect to database, switching to mock mode";
            m_mockMode = true;
            startMockTest();
            return;
        }
        
        qDebug() << "Database connected successfully";
        
        // Load data points from database
        QVector<DataAcquisitionPoint> dataPoints = m_databaseManager->loadDataPoints();
        qDebug() << "Loaded" << dataPoints.size() << "data acquisition points";
        
        if (dataPoints.isEmpty()) {
            qWarning() << "No data points found in database, switching to mock mode";
            m_mockMode = true;
            startMockTest();
            return;
        }
        
        // Initialize SCADA service
        m_scadaService = new ScadaCoreService(this);
        
        // Connect SCADA service signals
        connect(m_scadaService, &ScadaCoreService::dataPointAcquired, this, &EnhancedScadaTest::onDataAcquired);
        connect(m_scadaService, &ScadaCoreService::errorOccurred, this, &EnhancedScadaTest::onScadaError);
        
        // Configure SCADA service with database data points
        for (const auto &point : dataPoints) {
            m_scadaService->addDataPoint(point);
            qDebug() << "Data Point:" << point.name 
                     << "Device:" << point.tags.value("device_name", "Unknown")
                     << "Address:" << point.address
                     << "Type:" << static_cast<int>(point.dataType)
                     << "Poll Interval:" << point.pollInterval << "ms";
        }
        
        // Start the SCADA service
        if (!m_scadaService->startService()) {
            qWarning() << "Failed to start SCADA service";
            QTimer::singleShot(1000, this, &EnhancedScadaTest::completeTest);
            return;
        }
        
        qDebug() << "SCADA service started successfully";
        m_testRunning = true;
        
        // Start monitoring timer
        QTimer::singleShot(5000, this, &EnhancedScadaTest::printStatus);
        
        // Schedule test completion
        QTimer::singleShot(60000, this, &EnhancedScadaTest::completeTest); // Run for 1 minute
    }

private slots:

    

    

    
    void startMockTest()
    {
        qDebug() << "\n=== Starting Mock Mode Test ===";
        
        // Create mock data points
        QVector<DataAcquisitionPoint> mockPoints = createMockDataPoints();
        
        // Initialize SCADA service
        m_scadaService = new ScadaCoreService(this);
        
        // Connect signals
        connect(m_scadaService, &ScadaCoreService::dataPointAcquired, this, &EnhancedScadaTest::onDataAcquired);
        connect(m_scadaService, &ScadaCoreService::errorOccurred, this, &EnhancedScadaTest::onScadaError);
        
        // Add mock data points
        for (const auto &point : mockPoints) {
            m_scadaService->addDataPoint(point);
        }
        
        // Start service
        if (m_scadaService->startService()) {
            qDebug() << "Mock SCADA service started successfully";
            m_testRunning = true;
            
            // Generate mock data
            startMockDataGeneration();
            
            // Start monitoring
            QTimer::singleShot(5000, this, &EnhancedScadaTest::printStatus);
            
            // Schedule completion
            QTimer::singleShot(30000, this, &EnhancedScadaTest::completeTest); // Run for 30 seconds in mock mode
        } else {
            qWarning() << "Failed to start mock SCADA service";
            QTimer::singleShot(1000, this, &EnhancedScadaTest::completeTest);
        }
    }
    
    void onDataAcquired(const AcquiredDataPoint &dataPoint)
    {
        static int dataCount = 0;
        dataCount++;
        
        if (dataCount % 10 == 0) { // Print every 10th data point to avoid spam
            qDebug() << "Data acquired [" << dataCount << "]:"
                     << "Point:" << dataPoint.pointName
                     << "Value:" << dataPoint.value.toString()
                     << "Valid:" << dataPoint.isValid
                     << "Timestamp:" << dataPoint.timestamp;
        }
    }
    
    void onScadaError(const QString &error)
    {
        qWarning() << "SCADA Error:" << error;
    }
    
    void printStatus()
    {
        if (!m_testRunning) return;
        
        qDebug() << "\n=== Status Report ===";
        qDebug() << "Timestamp:" << QDateTime::currentDateTime().toString();
        qDebug() << "Mode:" << (m_mockMode ? "Mock" : "Real");
        
        if (m_scadaService) {
            auto stats = m_scadaService->getStatistics();
            qDebug() << "Total Requests:" << stats.totalReadOperations;
            qDebug() << "Successful Requests:" << stats.successfulReads;
            qDebug() << "Failed Requests:" << stats.failedReads;
            qDebug() << "Average Response Time:" << stats.averageResponseTime << "ms";
            
            auto perfMetrics = m_scadaService->getPerformanceMetrics();
            qDebug() << "Single-threaded Operations:" << perfMetrics.singleThreadedOperations;
            qDebug() << "Multi-threaded Operations:" << perfMetrics.multiThreadedOperations;
        }
        
        // Schedule next status report
        if (m_testRunning) {
            QTimer::singleShot(10000, this, &EnhancedScadaTest::printStatus);
        }
    }
    
    void completeTest()
    {
        qDebug() << "\n=== Test Completion ===";
        m_testRunning = false;
        
        if (m_scadaService) {
            auto finalStats = m_scadaService->getStatistics();
            qDebug() << "Final Statistics:";
            qDebug() << "  Total Requests:" << finalStats.totalReadOperations;
            qDebug() << "  Successful:" << finalStats.successfulReads;
            qDebug() << "  Failed:" << finalStats.failedReads;
            qDebug() << "  Success Rate:" << (finalStats.totalReadOperations > 0 ?
                        (double)finalStats.successfulReads / finalStats.totalReadOperations * 100 : 0) << "%";
            qDebug() << "  Average Response Time:" << finalStats.averageResponseTime << "ms";
            
            auto perfReport = m_scadaService->getPerformanceReport();
            qDebug() << "Performance Report:" << perfReport;
        }
        
        qDebug() << "Test completed at" << QDateTime::currentDateTime().toString();
        
        // Cleanup and exit
        QTimer::singleShot(1000, this, &EnhancedScadaTest::cleanup);
    }
    
    void cleanup()
    {
        if (m_scadaService) {
            m_scadaService->stopService();
            m_scadaService->deleteLater();
            m_scadaService = nullptr;
        }
        
        if (m_databaseManager) {
            m_databaseManager->disconnectFromDatabase();
            m_databaseManager->deleteLater();
            m_databaseManager = nullptr;
        }
        
        qDebug() << "Cleanup completed";
        QCoreApplication::quit();
    }

private:
    QVector<DataAcquisitionPoint> createMockDataPoints()
    {
        QVector<DataAcquisitionPoint> points;
        
        // Create mock data points for testing - using simulation mode
        DataAcquisitionPoint point1;
        point1.name = "MOCK_TEMP_001";
        point1.host = "SIMULATION";
        point1.port = 0;
        point1.unitId = 1;
        point1.address = 1001;
        point1.dataType = ModbusDataType::HoldingRegister;
        point1.pollInterval = 2000;
        point1.enabled = true;
        point1.measurement = "temperature";
        point1.tags["device_name"] = "MockDevice1";
        point1.tags["description"] = "Mock temperature sensor";
        point1.tags["simulation_mode"] = "true";
        points.append(point1);
        
        DataAcquisitionPoint point2;
        point2.name = "MOCK_PRESSURE_001";
        point2.host = "SIMULATION";
        point2.port = 0;
        point2.unitId = 1;
        point2.address = 1002;
        point2.dataType = ModbusDataType::Float32;
        point2.pollInterval = 3000;
        point2.enabled = true;
        point2.measurement = "pressure";
        point2.tags["device_name"] = "MockDevice1";
        point2.tags["description"] = "Mock pressure sensor";
        point2.tags["simulation_mode"] = "true";
        points.append(point2);
        
        DataAcquisitionPoint point3;
        point3.name = "MOCK_STATUS_001";
        point3.host = "SIMULATION";
        point3.port = 0;
        point3.unitId = 2;
        point3.address = 2001;
        point3.dataType = ModbusDataType::BOOL;
        point3.pollInterval = 1000;
        point3.enabled = true;
        point3.measurement = "status";
        point3.tags["device_name"] = "MockDevice2";
        point3.tags["description"] = "Mock status indicator";
        point3.tags["simulation_mode"] = "true";
        points.append(point3);
        
        return points;
    }
    
    void startMockDataGeneration()
    {
        // This would simulate data acquisition in a real implementation
        // For now, we rely on the existing polling mechanism
        qDebug() << "Mock data generation started (simulated by polling mechanism)";
    }

private:
    DatabaseManager *m_databaseManager;
    ScadaCoreService *m_scadaService;
    ModbusWorkerManager *m_workerManager;
    bool m_testRunning;
    bool m_mockMode;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "Enhanced SCADA Test Application";
    qDebug() << "Version: 1.0";
    qDebug() << "Build Date:" << __DATE__ << __TIME__;
    
    EnhancedScadaTest test;
    
    // Start test after event loop starts
    QTimer::singleShot(100, &test, &EnhancedScadaTest::startTest);
    
    return app.exec();
}

#include "enhanced_scada_test.moc"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QLoggingCategory>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QThreadPool>
#include <QFuture>
#include <QtConcurrent>
#include <QElapsedTimer>
#include <QAtomicInteger>
#include <QTest>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "../include/scada_core_service.h"
#include "../include/database_manager.h"
#include "../include/modbus_worker.h"
#include "../include/modbus_worker_manager.h"
#include "../include/connection_resilience_manager.h"

/**
 * Enhanced ScadaServiceTest with Multithreading Support
 * 
 * This class provides comprehensive testing of SCADA functionality with:
 * - Concurrent data acquisition using ModbusWorker threads
 * - Performance benchmarking and statistics collection
 * - Thread-safe operations and synchronization
 * - Backward compatibility with existing functionality
 */
class ScadaServiceTest : public QObject
{
    Q_OBJECT

public:
    struct PerformanceMetrics {
        QAtomicInteger<qint64> totalOperations{0};
        QAtomicInteger<qint64> successfulOperations{0};
        QAtomicInteger<qint64> failedOperations{0};
        QAtomicInteger<qint64> concurrentOperations{0};
        double averageResponseTime{0.0};
        qint64 testStartTime{0};
        qint64 testEndTime{0};
        
        void reset() {
            totalOperations = 0;
            successfulOperations = 0;
            failedOperations = 0;
            concurrentOperations = 0;
            averageResponseTime = 0.0;
            testStartTime = QDateTime::currentMSecsSinceEpoch();
            testEndTime = 0;
        }
    };

    ScadaServiceTest(ScadaCoreService *service, const QString &executionMode = "multiple", QObject *parent = nullptr) 
        : QObject(parent), m_service(service), m_dbManager(nullptr), 
          m_workerManager(nullptr), m_multithreadingEnabled(true),
          m_maxConcurrentWorkers(4), m_performanceTestDuration(60000),
          m_testRunning(false), m_mockMode(false), m_resilienceManager(nullptr),
          m_executionMode(executionMode) {
        // Initialize performance metrics
        m_performanceMetrics.reset();
        qDebug() << "Enhanced SCADA Test initialized at" << QDateTime::currentDateTime().toString();
        qDebug() << "Execution mode:" << m_executionMode;
        
        // Initialize connection resilience manager
        m_resilienceManager = new ConnectionResilienceManager(this);
        setupConnectionResilience();
    }
    
    // Constructor with multithreading support
    ScadaServiceTest(bool enableMultithreading = false, QObject *parent = nullptr)
        : QObject(parent), m_service(nullptr), m_dbManager(nullptr),
          m_workerManager(nullptr), m_multithreadingEnabled(enableMultithreading),
          m_maxConcurrentWorkers(4), m_performanceTestDuration(60000),
          m_testRunning(false), m_mockMode(false), m_resilienceManager(nullptr)
    {
        // Initialize performance metrics
        m_performanceMetrics.reset();
        qDebug() << "Enhanced SCADA Test initialized at" << QDateTime::currentDateTime().toString();
        
        // Initialize connection resilience manager
        m_resilienceManager = new ConnectionResilienceManager(this);
        setupConnectionResilience();
        
        // Create service if multithreading is enabled
        if (m_multithreadingEnabled) {
            m_service = new ScadaCoreService(this);
        }
    }
    
    void runTest() {
        qDebug() << "=== Enhanced SCADA Core Service Test with Multithreading Support ===";
        qDebug() << "Testing data acquisition with PostgreSQL configuration, InfluxDB integration, and concurrent operations";
        qDebug() << "Multithreading enabled:" << (m_multithreadingEnabled ? "YES" : "NO");
        qDebug() << "Max concurrent workers:" << m_maxConcurrentWorkers;
        qDebug();
        
        // Initialize performance metrics
        m_performanceMetrics.reset();
        m_performanceMetrics.testStartTime = QDateTime::currentMSecsSinceEpoch();
        
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
            qWarning() << "Failed to connect to database. Test cannot proceed without database connection.";
            QTimer::singleShot(1000, []() {
                QCoreApplication::quit();
            });
            return;
        }
        
        // Initialize multithreading components if enabled
        if (m_multithreadingEnabled) {
            initializeMultithreadingComponents();
        }
    }
    
    // New multithreading methods
    void initializeMultithreadingComponents() {
        qDebug() << "🔧 Initializing multithreading components...";
        
        // Create ModbusWorkerManager
        m_workerManager = new ModbusWorkerManager(this);
        
        // Connect worker manager signals
        connect(m_workerManager, &ModbusWorkerManager::workerCreated,
                this, &ScadaServiceTest::onWorkerCreated);
        connect(m_workerManager, &ModbusWorkerManager::workerRemoved,
                this, &ScadaServiceTest::onWorkerRemoved);
        connect(m_workerManager, &ModbusWorkerManager::globalStatisticsUpdated,
                this, &ScadaServiceTest::onGlobalStatisticsUpdated);
        
        // Set default poll interval for workers (increased to reduce connection drops)
        m_workerManager->setDefaultPollInterval(2000);
        
        qDebug() << "✅ Multithreading components initialized";
    }
    
    void runConcurrentDataAcquisitionTest() {
        qDebug() << "🚀 Starting concurrent data acquisition test...";
        
        // Load Modbus devices from database
        QVector<ModbusDeviceConfig> devices = m_dbManager->loadModbusDevices();
        if (devices.isEmpty()) {
            qDebug() << "❌ No Modbus devices found for concurrent testing";
            return;
        }
        
        // Create workers for each device
        QList<QFuture<void>> futures;
        for (const auto &device : devices) {
            if (m_workerManager->getWorkerCount() >= m_maxConcurrentWorkers) {
                qDebug() << "⚠️  Maximum concurrent workers reached:" << m_maxConcurrentWorkers;
                break;
            }
            
            // Create worker for device
            ModbusWorker* worker = m_workerManager->getOrCreateWorker(
                device.ipAddress, device.port, device.unitId);
            
            if (worker) {
                // Connect worker signals for performance monitoring
                connect(worker, &ModbusWorker::readCompleted,
                        this, &ScadaServiceTest::onWorkerReadCompleted);
                connect(worker, &ModbusWorker::writeCompleted,
                        this, &ScadaServiceTest::onWorkerWriteCompleted);
                connect(worker, &ModbusWorker::errorOccurred,
                        this, &ScadaServiceTest::onWorkerError);
                
                // Note: Worker will be started and connected via startAllWorkers() and connectAllDevices()
                // to avoid race conditions and duplicate initialization
                
                qDebug() << "🔧 Created worker for device:" << device.deviceName 
                         << "(" << device.ipAddress << ":" << device.port << ")";
            }
        }
        
        // Start all workers
        m_workerManager->startAllWorkers();
        m_workerManager->connectAllDevices();
        
        qDebug() << "✅ Concurrent data acquisition test started with" 
                 << m_workerManager->getWorkerCount() << "workers";
    }
    
    void runPerformanceBenchmark() {
        qDebug() << "📊 Starting performance benchmark...";
        
        QElapsedTimer benchmarkTimer;
        benchmarkTimer.start();
        
        // Load data points for testing
        QVector<DataAcquisitionPoint> dataPoints = m_dbManager->loadDataPoints();
        if (dataPoints.isEmpty()) {
            qDebug() << "❌ No data points available for benchmark";
            return;
        }
        
        // Run concurrent read operations
        QList<QFuture<void>> readFutures;
        for (const auto &point : dataPoints) {
            if (readFutures.size() >= m_maxConcurrentWorkers * 2) break;
            
            QFuture<void> future = QtConcurrent::run([this, point]() {
                performConcurrentRead(point);
            });
            readFutures.append(future);
        }
        
        // Wait for all operations to complete
        for (auto &future : readFutures) {
            future.waitForFinished();
        }
        
        qint64 benchmarkTime = benchmarkTimer.elapsed();
        qDebug() << "📈 Performance benchmark completed in" << benchmarkTime << "ms";
        
        // Calculate and display performance metrics
        displayPerformanceMetrics();
    }
    
    void performConcurrentRead(const DataAcquisitionPoint &point) {
        QElapsedTimer operationTimer;
        operationTimer.start();
        
        m_performanceMetrics.totalOperations.fetchAndAddOrdered(1);
        m_performanceMetrics.concurrentOperations.fetchAndAddOrdered(1);
        
        // Get or create worker for this device
        ModbusWorker* worker = m_workerManager->getOrCreateWorker(
            point.host, point.port, point.tags.value("unit_id", "1").toInt());
        
        if (worker && worker->isConnected()) {
            // Create Modbus request
            ModbusRequest request;
            request.startAddress = point.address;
            request.count = 1;
            request.type = ModbusRequest::ReadHoldingRegisters;
            request.unitId = point.unitId;
            request.dataType = ModbusDataType::HoldingRegister;
            
            // Queue read request with high priority for benchmark
            qint64 requestId = worker->queueReadRequest(request, RequestPriority::High);
            
            // Simulate processing time
            QThread::msleep(10);
            
            qint64 operationTime = operationTimer.elapsed();
            updatePerformanceMetrics(true, operationTime);
        } else {
            updatePerformanceMetrics(false, operationTimer.elapsed());
        }
        
        m_performanceMetrics.concurrentOperations.fetchAndSubOrdered(1);
    }
    
    void updatePerformanceMetrics(bool success, qint64 responseTime) {
        QMutexLocker locker(&m_metricsMutex);
        
        if (success) {
            m_performanceMetrics.successfulOperations.fetchAndAddOrdered(1);
        } else {
            m_performanceMetrics.failedOperations.fetchAndAddOrdered(1);
        }
        
        // Update average response time
        qint64 totalOps = m_performanceMetrics.totalOperations.loadAcquire();
        if (totalOps > 0) {
            m_performanceMetrics.averageResponseTime = 
                (m_performanceMetrics.averageResponseTime * (totalOps - 1) + responseTime) / totalOps;
        }
    }
    
    void displayPerformanceMetrics() {
        qDebug() << "";
        qDebug() << "=== Performance Metrics ===";
        qDebug() << "Total operations:" << m_performanceMetrics.totalOperations.loadAcquire();
        qDebug() << "Successful operations:" << m_performanceMetrics.successfulOperations.loadAcquire();
        qDebug() << "Failed operations:" << m_performanceMetrics.failedOperations.loadAcquire();
        qDebug() << "Average response time:" << QString::number(m_performanceMetrics.averageResponseTime, 'f', 2) << "ms";
        
        qint64 totalOps = m_performanceMetrics.totalOperations.loadAcquire();
        qint64 successOps = m_performanceMetrics.successfulOperations.loadAcquire();
        if (totalOps > 0) {
            double successRate = (double)successOps / totalOps * 100.0;
            qDebug() << "Success rate:" << QString::number(successRate, 'f', 1) << "%";
        }
        
        qint64 testDuration = QDateTime::currentMSecsSinceEpoch() - m_performanceMetrics.testStartTime;
        qDebug() << "Test duration:" << QString::number(testDuration / 1000.0, 'f', 1) << "seconds";
        
        if (testDuration > 0) {
            double throughput = (double)totalOps / (testDuration / 1000.0);
            qDebug() << "Throughput:" << QString::number(throughput, 'f', 2) << "operations/second";
        }
        qDebug() << "";
    }
    

    

    

    

    

    


private slots:
    void onDatabaseConnected() {
        qDebug() << "✅ Database connected successfully";
        
        // Enhanced database connection handling with multithreading support
        
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
       // QTimer::singleShot(300000, this, &ScadaServiceTest::stopTest);
    }
    
    void onDatabaseError(const QString &error) {
        qDebug() << "❌ Database error:" << error;
    }
    
    void onServiceStarted() {
        qDebug() << "✅ SCADA Core Service started with database configuration";
        qDebug() << "Data acquisition and InfluxDB integration active";
        qDebug();
        
        // Start multithreading tests if enabled
        if (m_multithreadingEnabled && m_workerManager) {
            qDebug() << "🚀 Starting multithreaded performance tests...";
            
            // Start concurrent data acquisition test
            QTimer::singleShot(2000, this, &ScadaServiceTest::runConcurrentDataAcquisitionTest);
            
            // Start performance benchmark after concurrent test
            QTimer::singleShot(10000, this, &ScadaServiceTest::runPerformanceBenchmark);
            
            // Schedule performance metrics display
            QTimer::singleShot(30000, this, &ScadaServiceTest::displayPerformanceMetrics);
        }
        
        // Start write operations test after 5 seconds (backward compatibility)
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
        static int dataCount = 0;
        dataCount++;
        
        if (dataCount % 10 == 0) { // Print every 10th data point to avoid spam
            if (dataPoint.isValid) {
                qDebug() << "📊 Data acquired [" << dataCount << "]:" << dataPoint.pointName 
                         << "=" << dataPoint.value.toString()
                         << "(" << dataPoint.measurement << ")"
                         << "Unit ID:" << dataPoint.tags.value("unit_id", "N/A");
            } else {
                qDebug() << "❌ Data acquisition failed [" << dataCount << "]:" << dataPoint.pointName 
                         << "Error:" << dataPoint.errorMessage;
            }
        }
        
        // Update performance metrics
        updatePerformanceMetrics(dataPoint.isValid, 0); // Response time not available here
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
            qDebug() << "📈 SCADA Service Statistics update #" << updateCount;
            qDebug() << "   Total operations:" << stats.totalReadOperations;
            qDebug() << "   Success rate:" << 
                        (stats.totalReadOperations > 0 ? 
                         QString::number((double)stats.successfulReads / stats.totalReadOperations * 100.0, 'f', 1) + "%" :
                         "N/A");
            qDebug() << "   Data points sent:" << stats.totalDataPointsSent;
            qDebug() << "   Avg response time:" << QString::number(stats.averageResponseTime, 'f', 2) << "ms";
            
            // Display multithreading statistics if enabled
            if (m_multithreadingEnabled && m_workerManager) {
                auto globalStats = m_workerManager->getGlobalStatistics();
                qDebug() << "🔧 Worker Manager Statistics:";
                qDebug() << "   Active workers:" << globalStats.activeWorkers;
                qDebug() << "   Connected devices:" << globalStats.connectedDevices;
                qDebug() << "   Total worker requests:" << globalStats.totalRequests;
                qDebug() << "   Worker success rate:" << 
                            (globalStats.totalRequests > 0 ? 
                             QString::number((double)globalStats.totalSuccessfulRequests / globalStats.totalRequests * 100.0, 'f', 1) + "%" :
                             "N/A");
            }
        }
    }
    
    void onWriteCompleted(qint64 requestId, const QString &operation, bool success, const QString &error) {
        if (success) {
            qDebug() << "✅ Write operation completed - Request ID:" << requestId << "Operation:" << operation;
        } else {
            qDebug() << "❌ Write operation failed - Request ID:" << requestId << "Operation:" << operation << "Error:" << error;
        }
    }
    

    
    // New worker event handlers
    void onWorkerCreated(const QString &deviceKey) {
        qDebug() << "🔧 Worker created for device:" << deviceKey;
    }
    
    void onWorkerRemoved(const QString &deviceKey) {
        qDebug() << "🗑️  Worker removed for device:" << deviceKey;
    }
    
    void onGlobalStatisticsUpdated(const ModbusWorkerManager::GlobalStatistics &stats) {
        static int globalUpdateCount = 0;
        globalUpdateCount++;
        
        // Print global statistics every 5 updates
        if (globalUpdateCount % 5 == 0) {
            qDebug() << "🌐 Global Worker Statistics update #" << globalUpdateCount;
            qDebug() << "   Active workers:" << stats.activeWorkers;
            qDebug() << "   Connected devices:" << stats.connectedDevices;
            qDebug() << "   Total requests:" << stats.totalRequests;
            qDebug() << "   Global avg response time:" << QString::number(stats.globalAverageResponseTime, 'f', 2) << "ms";
        }
    }
    
    void onWorkerReadCompleted(qint64 requestId, const ModbusReadResult &result) {
        updatePerformanceMetrics(result.success, 0); // Response time handled by worker
        
        if (result.success) {
            qDebug() << "✅ Worker read completed - Request ID:" << requestId 
                     << "Value:" << (result.rawData.isEmpty() ? 0 : result.rawData.value(0, 0));
        } else {
            qDebug() << "❌ Worker read failed - Request ID:" << requestId 
                     << "Error:" << result.errorString;
        }
    }
    
    void onWorkerWriteCompleted(qint64 requestId, const ModbusWriteResult &result) {
        updatePerformanceMetrics(result.success, 0); // Response time handled by worker
        
        if (result.success) {
            qDebug() << "✅ Worker write completed - Request ID:" << requestId;
        } else {
            qDebug() << "❌ Worker write failed - Request ID:" << requestId 
                     << "Error:" << result.errorString;
        }
    }
    
    void onWorkerError(const QString &deviceKey, const QString &error) {
        qDebug() << "⚠️  Worker error for device" << deviceKey << ":" << error;
        updatePerformanceMetrics(false, 0);
    }

    void stopTest() {
        qDebug();
        qDebug() << "Stopping Enhanced SCADA Core Service test...";
        
        // Stop multithreading components
        if (m_multithreadingEnabled && m_workerManager) {
            qDebug() << "🛑 Stopping all workers...";
            m_workerManager->stopAllWorkers();
            m_workerManager->disconnectAllDevices();
            
            // Final performance metrics
            m_performanceMetrics.testEndTime = QDateTime::currentMSecsSinceEpoch();
            displayPerformanceMetrics();
        }
        
        if (m_service) {
            m_service->stopService();
        }
        if (m_dbManager) {
            m_dbManager->disconnectFromDatabase();
        }
    }
    
private:
    void setupConnectionResilience() {
        // Configure connection resilience settings
        if (m_resilienceManager) {
            // Set default retry parameters
            m_resilienceManager->setMaxRetries(3);
            m_resilienceManager->setRetryDelay(1000);
            m_resilienceManager->setConnectionTimeout(5000);
            
            qDebug() << "Connection resilience manager configured";
        }
    };
    
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
    
    // Original members (backward compatibility)
    ScadaCoreService *m_service;
    DatabaseManager *m_dbManager;
    
    // New multithreading members
    ModbusWorkerManager *m_workerManager;
    bool m_multithreadingEnabled;
    int m_maxConcurrentWorkers;
    int m_performanceTestDuration;
    
    // Performance tracking
    PerformanceMetrics m_performanceMetrics;
    mutable QMutex m_metricsMutex;
    
    // Thread synchronization
    QMutex m_testMutex;
    QWaitCondition m_testCondition;
    
    // Mock mode support
    bool m_testRunning;
    bool m_mockMode;
    ConnectionResilienceManager *m_resilienceManager;
    QString m_executionMode;
};



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("SCADA Service Test");
    app.setApplicationVersion("2.0");
    
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    
    // Setup command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Enhanced SCADA Service Test Application with device mode selection");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add execution mode options
    QCommandLineOption singleModeOption(QStringList() << "s" << "single",
                                        "Run in single device mode (device ID 2 only)");
    QCommandLineOption multipleModeOption(QStringList() << "m" << "multiple", 
                                         "Run in multiple device mode (device IDs 2,3)");
    
    parser.addOption(singleModeOption);
    parser.addOption(multipleModeOption);
    
    // Process command line arguments
    parser.process(app);
    
    // Determine execution mode
    QString executionMode = "multiple"; // default
    if (parser.isSet(singleModeOption)) {
        executionMode = "single";
    } else if (parser.isSet(multipleModeOption)) {
        executionMode = "multiple";
    }
    
    qDebug() << "Enhanced SCADA Service Test Application";
    qDebug() << "Version: 2.0";
    qDebug() << "Build Date:" << __DATE__ << __TIME__;
    qDebug() << "🚀 Starting SCADA Service Test with Configuration File...";
    qDebug() << "📋 Execution Mode:" << executionMode;
    
    // Create database manager and load configuration
    DatabaseManager dbManager;
    if (!dbManager.loadConfigurationFromFile("/home/Pttaov1/TSO_SCADA/qtworkplace/modbusdriver/config/config.ini")) {
        qDebug() << "❌ Failed to load configuration:" << dbManager.lastError();
        return -1;
    }
    
    // Set execution mode for database queries
    dbManager.setExecutionMode(executionMode);
    
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
    
    ScadaServiceTest test(&scadaService, executionMode);
    
    // Start test after event loop starts
    QTimer::singleShot(100, &test, &ScadaServiceTest::runTest);
    
    return app.exec();
}



#include "scada_service_test.moc"
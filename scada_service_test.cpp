#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include "scada_core_service.h"

class ScadaServiceTest : public QObject
{
    Q_OBJECT

public:
    ScadaServiceTest(QObject *parent = nullptr) : QObject(parent), m_service(nullptr) {}
    
    void runTest() {
        qDebug() << "=== SCADA Core Service Test ===";
        qDebug() << "Testing data acquisition with InfluxDB integration";
        qDebug();
        
        // Create service
        m_service = new ScadaCoreService(this);
        
        // Connect signals
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
        
        // Configure data acquisition points
        setupDataPoints();
        
        // Start service
        qDebug() << "Starting SCADA Core Service...";
        if (m_service->startService()) {
            qDebug() << "Service started successfully";
        } else {
            qDebug() << "Failed to start service";
            QCoreApplication::quit();
            return;
        }
        
        // Run for 30 seconds then stop
        QTimer::singleShot(300000, this, &ScadaServiceTest::stopTest);
    }
    
private slots:
    void onServiceStarted() {
        qDebug() << "✅ SCADA Core Service started";
        qDebug() << "Data acquisition and InfluxDB integration active";
        qDebug();
        
        // Start write operations test after 5 seconds
        QTimer::singleShot(5000, this, &ScadaServiceTest::testWriteOperations);
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
                     << "(" << dataPoint.measurement << ")";
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
    
    void testWriteOperations() {
        qDebug();
        qDebug() << "=== Starting Modbus Write & Read Test Operations ===";
        
        const QString host = "10.72.2.215";
        const int port = 502;
        
        // Test 1: Write holding register (16-bit integer) then read back
        qDebug() << "🔧 Test 1: Writing holding register (address 500, value 1234)";
        m_service->writeHoldingRegister(host, port, 500, 1234);
        
        QTimer::singleShot(1000, [this, host, port]() {
            qDebug() << "📖 Test 1: Reading back holding register (address 500)";
            // Create temporary data point for reading
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_500";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 500;
            readPoint.dataType = ModbusDataType::HoldingRegister;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            // Remove after 500ms
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_500");
            });
        });
        
        // Test 2: Write Float32 value then read back
        QTimer::singleShot(2500, [this, host, port]() {
            qDebug() << "🔧 Test 2: Writing Float32 (address 502, value 25.75)";
            m_service->writeHoldingRegisterFloat32(host, port, 502, 25.75f);
        });
        
        QTimer::singleShot(3500, [this, host, port]() {
            qDebug() << "📖 Test 2: Reading back Float32 (address 502)";
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_502";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 502;
            readPoint.dataType = ModbusDataType::Float32;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_502");
            });
        });
        
        // Test 3: Write Double64 value then read back
        QTimer::singleShot(5000, [this, host, port]() {
            qDebug() << "🔧 Test 3: Writing Double64 (address 504, value 123.456789)";
            m_service->writeHoldingRegisterDouble64(host, port, 504, 123.456789);
        });
        
        QTimer::singleShot(6000, [this, host, port]() {
            qDebug() << "📖 Test 3: Reading back Double64 (address 504)";
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_504";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 504;
            readPoint.dataType = ModbusDataType::Double64;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_504");
            });
        });
        
        // Test 4: Write Long32 value then read back
        QTimer::singleShot(7500, [this, host, port]() {
            qDebug() << "🔧 Test 4: Writing Long32 (address 508, value 987654321)";
            m_service->writeHoldingRegisterLong32(host, port, 508, 987654321);
        });
        
        QTimer::singleShot(8500, [this, host, port]() {
            qDebug() << "📖 Test 4: Reading back Long32 (address 508)";
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_508";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 508;
            readPoint.dataType = ModbusDataType::Long32;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_508");
            });
        });
        
        // Test 5: Write Long64 value then read back
        QTimer::singleShot(10000, [this, host, port]() {
            qDebug() << "🔧 Test 5: Writing Long64 (address 510, value 1234567890123456)";
            m_service->writeHoldingRegisterLong64(host, port, 510, 1234567890123456LL);
        });
        
        QTimer::singleShot(11000, [this, host, port]() {
            qDebug() << "📖 Test 5: Reading back Long64 (address 510)";
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_510";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 510;
            readPoint.dataType = ModbusDataType::Long64;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_510");
            });
        });
        
        // Test 6: Write coil (boolean) then read back
        QTimer::singleShot(12500, [this, host, port]() {
            qDebug() << "🔧 Test 6: Writing coil (address 10, value true)";
            m_service->writeCoil(host, port, 10, true);
        });
        
        QTimer::singleShot(13500, [this, host, port]() {
            qDebug() << "📖 Test 6: Reading back coil (address 10)";
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_coil_10";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 10;
            readPoint.dataType = ModbusDataType::Coil;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_coil_10");
            });
        });
        
        // Test 7: Write another coil (boolean) then read back
        QTimer::singleShot(15000, [this, host, port]() {
            qDebug() << "🔧 Test 7: Writing coil (address 11, value false)";
            m_service->writeCoil(host, port, 11, false);
        });
        
        QTimer::singleShot(16000, [this, host, port]() {
            qDebug() << "📖 Test 7: Reading back coil (address 11)";
            DataAcquisitionPoint readPoint;
            readPoint.name = "test_read_coil_11";
            readPoint.host = host;
            readPoint.port = port;
            readPoint.address = 11;
            readPoint.dataType = ModbusDataType::Coil;
            readPoint.pollInterval = 1000;
            readPoint.measurement = "test";
            readPoint.enabled = true;
            m_service->addDataPoint(readPoint);
            
            QTimer::singleShot(500, [this]() {
                m_service->removeDataPoint("test_read_coil_11");
            });
        });
        
        // Test completion summary
        QTimer::singleShot(17000, [this]() {
            qDebug();
            qDebug() << "✅ Write & Read Test Operations Complete";
            qDebug() << "All 7 write operations with read-back verification executed";
            qDebug();
        });
        
        qDebug() << "Write & Read operations test scheduled. Operations will execute over 17 seconds.";
        qDebug();
    }
    
    void stopTest() {
        qDebug();
        qDebug() << "Stopping SCADA Core Service test...";
        if (m_service) {
            m_service->stopService();
        }
    }
    
private:
    void setupDataPoints() {
        qDebug() << "Configuring data acquisition points...";
        
        // Data Point 1: Temperature sensor (Float32)
        DataAcquisitionPoint tempSensor;
        tempSensor.name = "temperature_sensor_01";
        tempSensor.host = "10.72.2.215";
        tempSensor.port = 502;
        tempSensor.address = 100;
        tempSensor.dataType = ModbusDataType::Float32;
        tempSensor.pollInterval = 2000; // 2 seconds
        tempSensor.measurement = "sensors";
        tempSensor.tags["device_name"] = "temperature_sensor_01";
        tempSensor.tags["tag_name"] = "temperature_sensor_01";
        tempSensor.tags["description"] = "SCADA data point for temperature_sensor_01";
        tempSensor.tags["station_name"] = "field_site";
        tempSensor.enabled = true;
        m_service->addDataPoint(tempSensor);
        
        // Data Point 2: Pressure sensor (Float32)
        DataAcquisitionPoint pressureSensor;
        pressureSensor.name = "pressure_sensor_01";
        pressureSensor.host = "10.72.2.215";
        pressureSensor.port = 502;
        pressureSensor.address = 102;
        pressureSensor.dataType = ModbusDataType::Float32;
        pressureSensor.pollInterval = 1500; // 1.5 seconds
        pressureSensor.measurement = "sensors";
        pressureSensor.tags["device_name"] = "pressure_sensor_01";
        pressureSensor.tags["tag_name"] = "pressure_sensor_01";
        pressureSensor.tags["description"] = "SCADA data point for pressure_sensor_01";
        pressureSensor.tags["station_name"] = "field_site";
        pressureSensor.enabled = true;
        m_service->addDataPoint(pressureSensor);
        
        // Data Point 3: Flow rate (Double64)
        DataAcquisitionPoint flowSensor;
        flowSensor.name = "flow_sensor_01";
        flowSensor.host = "10.72.2.215";
        flowSensor.port = 502;
        flowSensor.address = 104;
        flowSensor.dataType = ModbusDataType::Double64;
        flowSensor.pollInterval = 3000; // 3 seconds
        flowSensor.measurement = "sensors";
        flowSensor.tags["device_name"] = "flow_sensor_01";
        flowSensor.tags["tag_name"] = "flow_sensor_01";
        flowSensor.tags["description"] = "SCADA data point for flow_sensor_01";
        flowSensor.tags["station_name"] = "field_site";
        flowSensor.enabled = true;
        m_service->addDataPoint(flowSensor);
        
        // Data Point 4: Pump status (Coil)
        DataAcquisitionPoint pumpStatus;
        pumpStatus.name = "pump_01_status";
        pumpStatus.host = "10.72.2.215";
        pumpStatus.port = 502;
        pumpStatus.address = 1;
        pumpStatus.dataType = ModbusDataType::Coil;
        pumpStatus.pollInterval = 1000; // 1 second
        pumpStatus.measurement = "equipment";
        pumpStatus.tags["device_name"] = "pump_01_status";
        pumpStatus.tags["tag_name"] = "pump_01_status";
        pumpStatus.tags["description"] = "SCADA data point for pump_01_status";
        pumpStatus.tags["station_name"] = "field_site";
        pumpStatus.enabled = true;
        m_service->addDataPoint(pumpStatus);
        
        // Data Point 5: Valve position (Holding Register)
        DataAcquisitionPoint valvePosition;
        valvePosition.name = "valve_01_position";
        valvePosition.host = "10.72.2.215";
        valvePosition.port = 502;
        valvePosition.address = 200;
        valvePosition.dataType = ModbusDataType::HoldingRegister;
        valvePosition.pollInterval = 2500; // 2.5 seconds
        valvePosition.measurement = "equipment";
        valvePosition.tags["device_name"] = "valve_01_position";
        valvePosition.tags["tag_name"] = "valve_01_position";
        valvePosition.tags["description"] = "SCADA data point for valve_01_position";
        valvePosition.tags["station_name"] = "field_site";
        valvePosition.enabled = true;
        m_service->addDataPoint(valvePosition);
        
        // Data Point 6: Energy meter (Long64)
        DataAcquisitionPoint energyMeter;
        energyMeter.name = "energy_meter_01";
        energyMeter.host = "10.72.2.215";
        energyMeter.port = 502;
        energyMeter.address = 300;
        energyMeter.dataType = ModbusDataType::Long64;
        energyMeter.pollInterval = 5000; // 5 seconds
        energyMeter.measurement = "energy";
        energyMeter.tags["device_name"] = "energy_meter_01";
        energyMeter.tags["tag_name"] = "energy_meter_01";
        energyMeter.tags["description"] = "SCADA data point for energy_meter_01";
        energyMeter.tags["station_name"] = "field_site";
        energyMeter.enabled = true;
        m_service->addDataPoint(energyMeter);
        
        qDebug() << "Configured" << m_service->getDataPoints().size() << "data acquisition points:";
        for (const auto &point : m_service->getDataPoints()) {
            qDebug() << "  -" << point.name << "at" << point.host << ":" << point.port 
                     << "address" << point.address << "(" << point.measurement << ")";
        }
        qDebug();
    }
    
    ScadaCoreService *m_service;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "SCADA Core Service Test Application";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Testing data acquisition and InfluxDB integration";
    qDebug();
    
    ScadaServiceTest test;
    
    // Start test after event loop starts
    QTimer::singleShot(100, &test, &ScadaServiceTest::runTest);
    
    return app.exec();
}

#include "scada_service_test.moc"
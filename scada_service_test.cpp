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
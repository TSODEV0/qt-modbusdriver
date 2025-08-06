#include "scada_core_service.h"
#include <QDebug>
#include <QJsonArray>
#include <QThread>
#include <QMetaType>

ScadaCoreService::ScadaCoreService(QObject *parent)
    : QObject(parent)
    , m_pollTimer(nullptr)
    , m_modbusManager(nullptr)
    , m_telegrafSocketPath("/tmp/telegraf.sock")
    , m_serviceRunning(false)
    , m_currentPointIndex(0)
{
    // Initialize components
    m_pollTimer = new QTimer(this);
    m_modbusManager = new ModbusManager(this);
    
    // Connect signals
    connect(m_pollTimer, &QTimer::timeout, this, &ScadaCoreService::onPollTimer);
    connect(m_modbusManager, &ModbusManager::readCompleted, 
            this, &ScadaCoreService::onModbusReadCompleted);
    connect(m_modbusManager, &ModbusManager::connectionStateChanged,
            this, &ScadaCoreService::onModbusConnectionStateChanged);
    connect(m_modbusManager, &ModbusManager::errorOccurred,
            this, &ScadaCoreService::onModbusError);
    
    // Initialize statistics
    resetStatistics();
}

ScadaCoreService::~ScadaCoreService()
{
    stopService();
}

bool ScadaCoreService::startService()
{
    if (m_serviceRunning) {
        qDebug() << "SCADA Core Service is already running";
        return true;
    }
    
    if (m_dataPoints.isEmpty()) {
        emit errorOccurred("No data points configured for acquisition");
        return false;
    }
    
    qDebug() << "Starting SCADA Core Service...";
    qDebug() << "Configured data points:" << m_dataPoints.size();
    qDebug() << "Telegraf socket path:" << m_telegrafSocketPath;
    
    // Note: Using direct Unix socket calls for each data transmission
    
    // Initialize service state
    m_serviceRunning = true;
    m_currentPointIndex = 0;
    m_statistics.serviceStartTime = QDateTime::currentMSecsSinceEpoch();
    
    // Start polling timer (100ms for responsive polling)
    m_pollTimer->start(100);
    
    emit serviceStarted();
    qDebug() << "SCADA Core Service started successfully";
    return true;
}

void ScadaCoreService::stopService()
{
    if (!m_serviceRunning) {
        return;
    }
    
    qDebug() << "Stopping SCADA Core Service...";
    
    m_serviceRunning = false;
    m_pollTimer->stop();
    
    // Disconnect from Modbus
    if (m_modbusManager) {
        m_modbusManager->disconnectFromServer();
    }
    
    // Note: Using direct Unix socket calls, no persistent connection needed
    
    emit serviceStopped();
    qDebug() << "SCADA Core Service stopped";
}

bool ScadaCoreService::isRunning() const
{
    return m_serviceRunning;
}

void ScadaCoreService::addDataPoint(const DataAcquisitionPoint &point)
{
    // Check if point already exists
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == point.name) {
            m_dataPoints[i] = point;
            qDebug() << "Updated existing data point:" << point.name;
            return;
        }
    }
    
    m_dataPoints.append(point);
    m_lastPollTimes[point.name] = 0; // Initialize last poll time
    qDebug() << "Added new data point:" << point.name << "at" << point.host << ":" << point.port;
}

void ScadaCoreService::removeDataPoint(const QString &pointName)
{
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == pointName) {
            m_dataPoints.removeAt(i);
            m_lastPollTimes.remove(pointName);
            qDebug() << "Removed data point:" << pointName;
            return;
        }
    }
}

void ScadaCoreService::updateDataPoint(const QString &pointName, const DataAcquisitionPoint &point)
{
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        if (m_dataPoints[i].name == pointName) {
            m_dataPoints[i] = point;
            qDebug() << "Updated data point:" << pointName;
            return;
        }
    }
}

QVector<DataAcquisitionPoint> ScadaCoreService::getDataPoints() const
{
    return m_dataPoints;
}

void ScadaCoreService::clearDataPoints()
{
    m_dataPoints.clear();
    m_lastPollTimes.clear();
    qDebug() << "Cleared all data points";
}

void ScadaCoreService::setTelegrafSocketPath(const QString &socketPath)
{
    m_telegrafSocketPath = socketPath;
    qDebug() << "Telegraf socket path set to:" << socketPath;
}

QString ScadaCoreService::getTelegrafSocketPath() const
{
    return m_telegrafSocketPath;
}

ScadaCoreService::ServiceStatistics ScadaCoreService::getStatistics() const
{
    return m_statistics;
}

void ScadaCoreService::resetStatistics()
{
    m_statistics = ServiceStatistics();
    m_responseTimers.clear();
}

void ScadaCoreService::onPollTimer()
{
    if (!m_serviceRunning || m_dataPoints.isEmpty()) {
        return;
    }
    
    processNextDataPoint();
}

void ScadaCoreService::processNextDataPoint()
{
    if (m_currentPointIndex >= m_dataPoints.size()) {
        m_currentPointIndex = 0;
    }
    
    if (m_dataPoints.isEmpty()) {
        return;
    }
    
    const DataAcquisitionPoint &point = m_dataPoints[m_currentPointIndex];
    
    // Check if this point is enabled and ready for polling
    if (!point.enabled) {
        m_currentPointIndex++;
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 lastPollTime = m_lastPollTimes.value(point.name, 0);
    
    if (currentTime - lastPollTime < point.pollInterval) {
        m_currentPointIndex++;
        return;
    }
    
    // Connect to Modbus host if needed
    QString hostKey = QString("%1:%2").arg(point.host).arg(point.port);
    if (m_currentHost != hostKey) {
        if (!connectToModbusHost(point.host, point.port)) {
            emit errorOccurred(QString("Failed to connect to Modbus host: %1:%2")
                             .arg(point.host).arg(point.port));
            m_currentPointIndex++;
            return;
        }
        m_currentHost = hostKey;
    }
    
    // Start data acquisition
    m_lastPollTimes[point.name] = currentTime;
    m_responseTimers[point.name] = currentTime;
    
    qDebug() << "Polling data point:" << point.name << "at address" << point.address;
    
    // Read data based on type
    switch (point.dataType) {
    case ModbusDataType::HoldingRegister:
    case ModbusDataType::Float32:
    case ModbusDataType::Double64:
    case ModbusDataType::Long32:
    case ModbusDataType::Long64:
        m_modbusManager->readHoldingRegister(point.address, point.dataType);
        break;
    case ModbusDataType::InputRegister:
        m_modbusManager->readInputRegister(point.address, point.dataType);
        break;
    case ModbusDataType::Coil:
        m_modbusManager->readCoil(point.address);
        break;
    case ModbusDataType::DiscreteInput:
        m_modbusManager->readDiscreteInput(point.address);
        break;
    }
    
    m_statistics.totalReadOperations++;
    m_currentPointIndex++;
}

void ScadaCoreService::onModbusReadCompleted(const ModbusReadResult &result)
{
    // Find the corresponding data point
    DataAcquisitionPoint *targetPoint = nullptr;
    for (auto &point : m_dataPoints) {
        if (point.address == result.startAddress) {
            targetPoint = &point;
            break;
        }
    }
    
    if (!targetPoint) {
        qWarning() << "Received read result for unknown address:" << result.startAddress;
        return;
    }
    
    // Calculate response time
    qint64 responseTime = 0;
    if (m_responseTimers.contains(targetPoint->name)) {
        responseTime = QDateTime::currentMSecsSinceEpoch() - m_responseTimers[targetPoint->name];
        m_responseTimers.remove(targetPoint->name);
    }
    
    // Create acquired data point
    AcquiredDataPoint dataPoint;
    dataPoint.pointName = targetPoint->name;
    dataPoint.timestamp = result.timestamp;
    dataPoint.measurement = targetPoint->measurement;
    dataPoint.tags = targetPoint->tags;
    dataPoint.isValid = result.success;
    
    if (result.success) {
        // Extract value based on data type
        if (!result.processedData.isEmpty()) {
            QString key = result.processedData.keys().first();
            dataPoint.value = result.processedData[key];
        } else if (!result.rawData.isEmpty()) {
            dataPoint.value = result.rawData.first();
        }
        
        m_statistics.successfulReads++;
        updateStatistics(true, responseTime);
        
        qDebug() << "Successfully read data point:" << targetPoint->name 
                 << "Value:" << dataPoint.value.toString();
    } else {
        dataPoint.errorMessage = result.errorString;
        m_statistics.failedReads++;
        updateStatistics(false, responseTime);
        
        qWarning() << "Failed to read data point:" << targetPoint->name 
                   << "Error:" << result.errorString;
    }
    
    // Emit signal
    emit dataPointAcquired(dataPoint);
    
    // Send to InfluxDB via Telegraf
    if (dataPoint.isValid) {
        bool sent = sendDataToInflux(dataPoint);
        emit dataPointSentToInflux(dataPoint.pointName, sent);
        
        if (sent) {
            m_statistics.totalDataPointsSent++;
        }
    }
    
    // Update statistics
    emit statisticsUpdated(m_statistics);
}

void ScadaCoreService::onModbusConnectionStateChanged(bool connected)
{
    if (connected) {
        qDebug() << "Modbus connection established to:" << m_currentHost;
    } else {
        qDebug() << "Modbus connection lost to:" << m_currentHost;
        m_currentHost.clear();
    }
}

void ScadaCoreService::onModbusError(const QString &error)
{
    qWarning() << "Modbus error:" << error;
    emit errorOccurred(QString("Modbus error: %1").arg(error));
}

bool ScadaCoreService::writeToTelegrafSocket(const QString& socketPath, const QByteArray& message)
{
    if (!QFileInfo::exists(socketPath)) {
        qCritical() << "Unix socket does not exist:" << socketPath;
        return false;
    }
    
    int sockfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return false;
    }
    
    struct sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.toUtf8().constData(), sizeof(addr.sun_path) - 1);
    
    int result = ::sendto(sockfd, message.constData(), message.size(), 0,
                          reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    ::close(sockfd);
    
    if (result < 0) {
        perror("sendto");
        return false;
    }
    
    qDebug() << "Sent InfluxDB line protocol to Telegraf via UNIX socket:" << message.trimmed();
    return true;
}

bool ScadaCoreService::writeToInflux(const QString& measurement, const QString& device, const QVariant& value)
{
    if (!value.isValid()) {
        qDebug() << "Invalid value for" << measurement << "on device" << device;
        return false;
    }
    
    // Sanitize tag values by removing spaces
    QString sanitizedMeasurement = measurement;
    sanitizedMeasurement.replace(" ", "_");
    
    QString sanitizedDevice = device;
    sanitizedDevice.replace(" ", "_");
    
    QString sanitizedDescription = QString("SCADA_data_point_for_%1").arg(sanitizedDevice);
    
    QString sanitizedStationName = "field_site";
    
    // Create InfluxDB line protocol with required tags only (no spaces in tag values)
    QString line = QString("%1,device_name=%2,tag_name=%3,description=%4,station_name=%5 value=%6")
                   .arg(sanitizedMeasurement, sanitizedDevice, sanitizedDevice, sanitizedDescription, sanitizedStationName, value.toString());
    
    line += "\n";
    
    bool success = writeToTelegrafSocket(m_telegrafSocketPath, line.toUtf8());
    qDebug() << "Line to write:" << line;
    
    if (!success) {
        qDebug() << "Failed to write to InfluxDB for" << measurement << "on device" << device;
        m_statistics.socketErrors++;
    } else {
        qDebug() << "Successfully wrote to InfluxDB:" << line;
        m_statistics.totalDataPointsSent++;
    }
    
    return success;
}

bool ScadaCoreService::sendDataToInflux(const AcquiredDataPoint &dataPoint)
{
    if (!dataPoint.isValid || dataPoint.measurement.isEmpty()) {
        qWarning() << "Invalid data point:" << dataPoint.pointName;
        return false;
    }
    
    // Use the device name from tags or point name as fallback
    QString device = dataPoint.tags.value("device", dataPoint.pointName);
    
    // Send data using the new writeToInflux method
    bool success = writeToInflux(dataPoint.measurement, device, dataPoint.value);
    
    if (success) {
        emit dataPointSentToInflux(dataPoint.pointName, true);
    } else {
        emit dataPointSentToInflux(dataPoint.pointName, false);
        emit errorOccurred(QString("Failed to send data point to InfluxDB: %1").arg(dataPoint.pointName));
    }
    
    return success;
}



bool ScadaCoreService::connectToModbusHost(const QString &host, int port)
{
    if (m_modbusManager->isConnected()) {
        m_modbusManager->disconnectFromServer();
        // Give some time for disconnection
        QThread::msleep(100);
    }
    
    return m_modbusManager->connectToServer(host, port);
}

void ScadaCoreService::updateStatistics(bool success, qint64 responseTime)
{
    if (responseTime > 0) {
        // Update average response time
        qint64 totalOps = m_statistics.successfulReads + m_statistics.failedReads;
        if (totalOps > 0) {
            m_statistics.averageResponseTime = 
                (m_statistics.averageResponseTime * (totalOps - 1) + responseTime) / totalOps;
        } else {
            m_statistics.averageResponseTime = responseTime;
        }
    }
}

QJsonObject ScadaCoreService::dataPointToJson(const AcquiredDataPoint &dataPoint)
{
    QJsonObject json;
    json["pointName"] = dataPoint.pointName;
    json["value"] = QJsonValue::fromVariant(dataPoint.value);
    json["timestamp"] = dataPoint.timestamp;
    json["measurement"] = dataPoint.measurement;
    json["isValid"] = dataPoint.isValid;
    
    if (!dataPoint.errorMessage.isEmpty()) {
        json["error"] = dataPoint.errorMessage;
    }
    
    if (!dataPoint.tags.isEmpty()) {
        QJsonObject tagsObj;
        for (auto it = dataPoint.tags.constBegin(); it != dataPoint.tags.constEnd(); ++it) {
            tagsObj[it.key()] = it.value();
        }
        json["tags"] = tagsObj;
    }
    
    return json;
}
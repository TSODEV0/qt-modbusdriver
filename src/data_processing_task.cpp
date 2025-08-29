#include "../include/data_processing_task.h"
#include "../include/scada_core_service.h"
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <QMutexLocker>
#include <cstring>

// DataProcessingTask Implementation

DataProcessingTask::DataProcessingTask(qint64 requestId,
                                     const ModbusReadResult &result,
                                     const DataAcquisitionPoint &point,
                                     ScadaCoreService *service)
    : m_requestId(requestId)
    , m_result(result)
    , m_point(point)
    , m_service(service)
    , m_deviceKey(QString("%1:%2").arg(point.host).arg(point.port))
{
    // Enable auto-deletion when task completes
    setAutoDelete(true);
}

void DataProcessingTask::run()
{
    qDebug() << "[DataProcessingTask] 🔧 Multi-threaded processing for request" << m_requestId 
             << "device" << m_deviceKey << "point" << m_point.name
             << "address" << m_point.address << "Unit ID:" << m_point.tags.value("unit_id", "1");
    
    try {
        // Validate input data
        if (!m_result.success || !m_result.hasValidData) {
            emit dataProcessingFailed(m_requestId, 
                                    QString("Invalid Modbus result: %1").arg(m_result.errorString),
                                    m_deviceKey);
            return;
        }
        
        if (m_result.rawData.isEmpty()) {
            emit dataProcessingFailed(m_requestId, "Empty raw data in Modbus result", m_deviceKey);
            return;
        }
        
        // Log raw Modbus data for debugging (similar to single-threaded mode)
        QString rawDataStr;
        for (int i = 0; i < m_result.rawData.size(); ++i) {
            if (i > 0) rawDataStr += ", ";
            rawDataStr += QString("0x%1").arg(m_result.rawData[i], 4, 16, QChar('0'));
        }
        qDebug() << "[DataProcessingTask] 📊 Modbus decode - Point:" << m_point.name
                 << "Raw data:" << rawDataStr
                 << "Data type:" << static_cast<int>(m_point.dataType)
                 << "Read mode:" << getReadModeString(m_point.dataType);
        
        // Create acquired data point
        AcquiredDataPoint acquiredPoint;
        acquiredPoint.pointName = m_point.name;
        acquiredPoint.timestamp = m_result.timestamp;
        acquiredPoint.measurement = m_point.measurement;
        acquiredPoint.tags = createInfluxTags(m_point);
        acquiredPoint.isValid = false; // Will be set to true if processing succeeds
        
        // Decode raw Modbus data based on data type
        QVariant decodedValue = decodeModbusData(m_result.rawData, m_point.dataType);
        
        if (!decodedValue.isValid()) {
            emit dataProcessingFailed(m_requestId, 
                                    QString("Failed to decode data for type %1")
                                    .arg(static_cast<int>(m_point.dataType)),
                                    m_deviceKey);
            return;
        }
        
        // Log decoded value details
        qDebug() << "[DataProcessingTask] 🔍 Decoded value:" << decodedValue.toString()
                 << "Type:" << decodedValue.typeName()
                 << "Valid:" << decodedValue.isValid();
        
        // Validate decoded value
        if (!validateDataValue(decodedValue, m_point.dataType)) {
            emit dataProcessingFailed(m_requestId,
                                    QString("Data validation failed for value %1")
                                    .arg(decodedValue.toString()),
                                    m_deviceKey);
            return;
        }
        
        // Set the decoded value and mark as valid
        acquiredPoint.value = decodedValue;
        acquiredPoint.isValid = true;
        
        // Log InfluxDB formatting details (similar to single-threaded mode)
        QString tagsStr;
        for (auto it = acquiredPoint.tags.begin(); it != acquiredPoint.tags.end(); ++it) {
            if (!tagsStr.isEmpty()) tagsStr += ", ";
            tagsStr += QString("%1=%2").arg(it.key()).arg(it.value());
        }
        qDebug() << "[DataProcessingTask] 📈 InfluxDB format - Measurement:" << acquiredPoint.measurement
                 << "Point:" << acquiredPoint.pointName
                 << "Value:" << acquiredPoint.value.toString()
                 << "Tags:" << tagsStr
                 << "Timestamp:" << acquiredPoint.timestamp;
        
        qDebug() << "[DataProcessingTask] ✅ Successfully processed:" 
                 << "Point:" << acquiredPoint.pointName
                 << "Value:" << acquiredPoint.value.toString()
                 << "Type:" << static_cast<int>(m_point.dataType)
                 << "Device:" << m_deviceKey
                 << "Thread:" << QThread::currentThreadId();
        
        // Emit successful completion
        emit dataProcessingCompleted(m_requestId, acquiredPoint, m_deviceKey);
        
    } catch (const std::exception &e) {
        emit dataProcessingFailed(m_requestId, 
                                QString("Exception during processing: %1").arg(e.what()),
                                m_deviceKey);
    } catch (...) {
        emit dataProcessingFailed(m_requestId, "Unknown exception during processing", m_deviceKey);
    }
}

QString DataProcessingTask::getDeviceKey() const
{
    return m_deviceKey;
}

qint64 DataProcessingTask::getRequestId() const
{
    return m_requestId;
}

QVariant DataProcessingTask::decodeModbusData(const QVector<quint16> &rawData, ModbusDataType dataType, int offset)
{
    if (offset < 0 || offset >= rawData.size()) {
        qWarning() << "[DataProcessingTask] Invalid offset" << offset << "for data size" << rawData.size();
        return QVariant();
    }
    
    switch (dataType) {
    case ModbusDataType::Float32:
        if (offset + 1 < rawData.size()) {
            quint16 high = rawData[offset];
            quint16 low = rawData[offset + 1];
            quint32 combined = (static_cast<quint32>(high) << 16) | low;
            float floatValue;
            memcpy(&floatValue, &combined, sizeof(float));
            
            // Validate IEEE 754 float
            if (std::isnan(floatValue) || std::isinf(floatValue)) {
                qWarning() << "[DataProcessingTask] Invalid Float32 value detected:" << floatValue;
                return QVariant();
            }
            
            return QVariant(floatValue);
        }
        break;
        
    case ModbusDataType::Double64:
        if (offset + 3 < rawData.size()) {
            quint16 reg0 = rawData[offset];
            quint16 reg1 = rawData[offset + 1];
            quint16 reg2 = rawData[offset + 2];
            quint16 reg3 = rawData[offset + 3];
            
            quint64 combined = (static_cast<quint64>(reg0) << 48) |
                             (static_cast<quint64>(reg1) << 32) |
                             (static_cast<quint64>(reg2) << 16) |
                             reg3;
            double doubleValue;
            memcpy(&doubleValue, &combined, sizeof(double));
            
            // Validate IEEE 754 double
            if (std::isnan(doubleValue) || std::isinf(doubleValue)) {
                qWarning() << "[DataProcessingTask] Invalid Double64 value detected:" << doubleValue;
                return QVariant();
            }
            
            return QVariant(doubleValue);
        }
        break;
        
    case ModbusDataType::Long32:
        if (offset + 1 < rawData.size()) {
            qint32 combined = (static_cast<qint32>(rawData[offset]) << 16) | rawData[offset + 1];
            return QVariant(combined);
        }
        break;
        
    case ModbusDataType::Long64:
        if (offset + 3 < rawData.size()) {
            qint64 combined = (static_cast<qint64>(rawData[offset]) << 48) |
                            (static_cast<qint64>(rawData[offset + 1]) << 32) |
                            (static_cast<qint64>(rawData[offset + 2]) << 16) |
                            rawData[offset + 3];
            return QVariant(combined);
        }
        break;
        
    case ModbusDataType::BOOL:
        {
            quint16 rawValue = rawData[offset];
            
            // Validate boolean range
            if (rawValue > 1) {
                qWarning() << "[DataProcessingTask] BOOL value" << rawValue 
                          << "exceeds typical range (0-1), converting non-zero to true";
            }
            
            bool boolValue = (rawValue != 0);
            return QVariant(boolValue);
        }
        
    case ModbusDataType::HoldingRegister:
    case ModbusDataType::InputRegister:
    case ModbusDataType::Coil:
    case ModbusDataType::DiscreteInput:
    default:
        // Simple 16-bit register value
        return QVariant(rawData[offset]);
    }
    
    qWarning() << "[DataProcessingTask] Insufficient data for type" << static_cast<int>(dataType)
               << "at offset" << offset << "(need more registers)";
    return QVariant();
}

bool DataProcessingTask::validateDataValue(const QVariant &value, ModbusDataType dataType)
{
    if (!value.isValid()) {
        return false;
    }
    
    switch (dataType) {
    case ModbusDataType::Float32:
        {
            bool ok;
            float floatVal = value.toFloat(&ok);
            return ok && !std::isnan(floatVal) && !std::isinf(floatVal);
        }
        
    case ModbusDataType::Double64:
        {
            bool ok;
            double doubleVal = value.toDouble(&ok);
            return ok && !std::isnan(doubleVal) && !std::isinf(doubleVal);
        }
        
    case ModbusDataType::BOOL:
        return value.canConvert<bool>();
        
    case ModbusDataType::Long32:
        return value.canConvert<qint32>();
        
    case ModbusDataType::Long64:
        return value.canConvert<qint64>();
        
    case ModbusDataType::HoldingRegister:
    case ModbusDataType::InputRegister:
    case ModbusDataType::Coil:
    case ModbusDataType::DiscreteInput:
    default:
        return value.canConvert<quint16>();
    }
}

QMap<QString, QString> DataProcessingTask::createInfluxTags(const DataAcquisitionPoint &point)
{
    QMap<QString, QString> tags = point.tags; // Start with existing tags
    
    // Enhanced InfluxDB mapping fields
    tags["device_name"] = point.tags.value("device_name", "STATION_TEST");
    tags["address"] = QString::number(point.address);
    tags["read_mode"] = getReadModeString(point.dataType);
    
    // Add data type information
    QString dataTypeStr;
    switch (point.dataType) {
    case ModbusDataType::Float32:
        dataTypeStr = "FLOAT32";
        break;
    case ModbusDataType::Double64:
        dataTypeStr = "DOUBLE64";
        break;
    case ModbusDataType::Long32:
        dataTypeStr = "INT32";
        break;
    case ModbusDataType::Long64:
        dataTypeStr = "INT64";
        break;
    case ModbusDataType::BOOL:
        dataTypeStr = "BOOL";
        break;
    case ModbusDataType::HoldingRegister:
        dataTypeStr = "INT16";
        break;
    case ModbusDataType::InputRegister:
        dataTypeStr = "INPUT_REGISTER";
        break;
    case ModbusDataType::Coil:
        dataTypeStr = "COIL";
        break;
    case ModbusDataType::DiscreteInput:
        dataTypeStr = "DISCRETE_INPUT";
        break;
    default:
        dataTypeStr = "UNKNOWN";
        break;
    }
    tags["data_type"] = dataTypeStr;
    
    // Add processing metadata
    tags["processing_thread"] = QString::number(reinterpret_cast<qintptr>(QThread::currentThreadId()));
    tags["processing_timestamp"] = QString::number(QDateTime::currentMSecsSinceEpoch());
    
    return tags;
}

QString DataProcessingTask::getReadModeString(ModbusDataType dataType)
{
    switch (dataType) {
    case ModbusDataType::HoldingRegister:
    case ModbusDataType::InputRegister:
        return "single_register";
    case ModbusDataType::Float32:
    case ModbusDataType::Long32:
        return "dual_register";
    case ModbusDataType::Double64:
    case ModbusDataType::Long64:
        return "quad_register";
    case ModbusDataType::Coil:
    case ModbusDataType::DiscreteInput:
    case ModbusDataType::BOOL:
        return "single_bit";
    default:
        return "single_register";
    }
}

// ParallelDataProcessor Implementation

ParallelDataProcessor::ParallelDataProcessor(QObject *parent)
    : QObject(parent)
    , m_threadPool(new QThreadPool(this))
    , m_activeTaskCount(0)
{
    // Set optimal thread count (typically CPU cores * 2 for I/O bound tasks)
    int optimalThreads = QThread::idealThreadCount() * 2;
    m_threadPool->setMaxThreadCount(qMax(2, qMin(optimalThreads, 8))); // Between 2-8 threads
    
    qDebug() << "[ParallelDataProcessor] Initialized with" << m_threadPool->maxThreadCount() << "threads";
}

ParallelDataProcessor::~ParallelDataProcessor()
{
    // Wait for all tasks to complete before destruction
    waitForCompletion(10000); // 10 second timeout
}

void ParallelDataProcessor::submitProcessingTask(qint64 requestId,
                                               const ModbusReadResult &result,
                                               const DataAcquisitionPoint &point,
                                               ScadaCoreService *service)
{
    // Create processing task
    DataProcessingTask *task = new DataProcessingTask(requestId, result, point, service);
    
    // Connect signals for result handling
    connect(task, &DataProcessingTask::dataProcessingCompleted,
            this, &ParallelDataProcessor::onTaskCompleted,
            Qt::QueuedConnection);
    
    connect(task, &DataProcessingTask::dataProcessingFailed,
            this, &ParallelDataProcessor::onTaskFailed,
            Qt::QueuedConnection);
    
    // Increment active task count
    {
        QMutexLocker locker(&m_taskCountMutex);
        m_activeTaskCount++;
    }
    
    // Submit to thread pool
    m_threadPool->start(task);
    
    qDebug() << "[ParallelDataProcessor] 🚀 Submitted task for request" << requestId 
             << "point:" << point.name << "device:" << task->getDeviceKey() 
             << "active tasks:" << m_activeTaskCount << "/" << m_threadPool->maxThreadCount() << "threads";
}

int ParallelDataProcessor::getActiveTaskCount() const
{
    QMutexLocker locker(&m_taskCountMutex);
    return m_activeTaskCount;
}

void ParallelDataProcessor::setMaxThreads(int maxThreads)
{
    m_threadPool->setMaxThreadCount(qMax(1, qMin(maxThreads, 16))); // Between 1-16 threads
    qDebug() << "[ParallelDataProcessor] Thread count set to" << m_threadPool->maxThreadCount();
}

bool ParallelDataProcessor::waitForCompletion(int timeoutMs)
{
    QMutexLocker locker(&m_taskCountMutex);
    
    if (m_activeTaskCount == 0) {
        return true;
    }
    
    return m_completionCondition.wait(&m_taskCountMutex, timeoutMs);
}

void ParallelDataProcessor::onTaskCompleted(qint64 requestId, const AcquiredDataPoint &dataPoint, const QString &deviceKey)
{
    // Decrement active task count
    {
        QMutexLocker locker(&m_taskCountMutex);
        m_activeTaskCount--;
        
        if (m_activeTaskCount == 0) {
            m_completionCondition.wakeAll();
        }
    }
    
    qDebug() << "[ParallelDataProcessor] ✅ Task completed for request" << requestId 
             << "point:" << dataPoint.pointName << "value:" << dataPoint.value.toString()
             << "device:" << deviceKey << "remaining tasks:" << m_activeTaskCount;
    
    // Forward the signal
    emit taskCompleted(requestId, dataPoint, deviceKey);
}

void ParallelDataProcessor::onTaskFailed(qint64 requestId, const QString &errorMessage, const QString &deviceKey)
{
    // Decrement active task count
    {
        QMutexLocker locker(&m_taskCountMutex);
        m_activeTaskCount--;
        
        if (m_activeTaskCount == 0) {
            m_completionCondition.wakeAll();
        }
    }
    
    qWarning() << "[ParallelDataProcessor] ❌ Task failed for request" << requestId 
               << "device:" << deviceKey << "error:" << errorMessage 
               << "remaining tasks:" << m_activeTaskCount;
    
    // Forward the signal
    emit taskFailed(requestId, errorMessage, deviceKey);
}
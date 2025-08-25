#include "../include/influxdb_line_protocol.h"
#include <QDebug>
#include <QRegularExpression>
#include <QStringList>
#include <cmath>

QByteArray InfluxDBLineProtocol::formatLineProtocol(const DataPoint &dataPoint)
{
    QString errorMessage;
    if (!validateDataPoint(dataPoint, errorMessage)) {
        qWarning() << "[InfluxDB Line Protocol] Invalid data point:" << errorMessage;
        return QByteArray();
    }

    QString lineProtocol;
    lineProtocol.reserve(512); // Pre-allocate to reduce memory allocations

    // 1. Measurement name (required)
    lineProtocol += escapeMeasurement(dataPoint.measurement);

    // 2. Tag set (optional)
    // Sort tags by key for optimal performance as recommended by InfluxDB
    QStringList sortedTagKeys = dataPoint.tags.keys();
    sortedTagKeys.sort();
    
    for (const QString &tagKey : sortedTagKeys) {
        const QString &tagValue = dataPoint.tags[tagKey];
        if (!tagValue.isEmpty()) { // Skip empty tag values as per InfluxDB spec
            lineProtocol += ',';
            lineProtocol += escapeTagKey(tagKey);
            lineProtocol += '=';
            lineProtocol += escapeTagValue(tagValue);
        }
    }

    // 3. First whitespace separator (required)
    lineProtocol += ' ';

    // 4. Field set (required - at least one field)
    QStringList fieldParts;
    for (auto it = dataPoint.fields.constBegin(); it != dataPoint.fields.constEnd(); ++it) {
        const QString &fieldKey = it.key();
        const QVariant &fieldValue = it.value();
        
        FieldType fieldType = detectFieldType(fieldValue);
        QString formattedValue = formatFieldValue(fieldValue, fieldType);
        
        if (!formattedValue.isEmpty()) {
            QString fieldPart = escapeFieldKey(fieldKey) + '=' + formattedValue;
            fieldParts.append(fieldPart);
        }
    }
    
    if (fieldParts.isEmpty()) {
        qWarning() << "[InfluxDB Line Protocol] No valid fields found for data point";
        return QByteArray();
    }
    
    lineProtocol += fieldParts.join(',');

    // 5. Timestamp (optional)
    if (dataPoint.timestamp > 0) {
        lineProtocol += ' ';
        qint64 convertedTimestamp = convertTimestamp(dataPoint.timestamp, dataPoint.precision);
        lineProtocol += QString::number(convertedTimestamp);
    }

    // 6. Newline terminator
    lineProtocol += '\n';

    return lineProtocol.toUtf8();
}

QByteArray InfluxDBLineProtocol::formatLineProtocol(const QVector<DataPoint> &dataPoints)
{
    QByteArray result;
    result.reserve(dataPoints.size() * 512); // Pre-allocate estimated size
    
    for (const DataPoint &dataPoint : dataPoints) {
        QByteArray lineData = formatLineProtocol(dataPoint);
        if (!lineData.isEmpty()) {
            result.append(lineData);
        }
    }
    
    return result;
}

bool InfluxDBLineProtocol::validateDataPoint(const DataPoint &dataPoint, QString &errorMessage)
{
    // 1. Measurement name is required and cannot be empty
    if (dataPoint.measurement.isEmpty()) {
        errorMessage = "Measurement name is required and cannot be empty";
        return false;
    }
    
    // 2. Measurement name cannot start with underscore (reserved for InfluxDB system use)
    if (dataPoint.measurement.startsWith('_')) {
        errorMessage = "Measurement name cannot start with underscore (reserved namespace)";
        return false;
    }
    
    // 3. At least one field is required
    if (dataPoint.fields.isEmpty()) {
        errorMessage = "At least one field is required";
        return false;
    }
    
    // 4. Validate tag keys and values
    for (auto it = dataPoint.tags.constBegin(); it != dataPoint.tags.constEnd(); ++it) {
        const QString &tagKey = it.key();
        const QString &tagValue = it.value();
        
        if (tagKey.isEmpty()) {
            errorMessage = "Tag key cannot be empty";
            return false;
        }
        
        if (tagKey.startsWith('_')) {
            errorMessage = QString("Tag key '%1' cannot start with underscore (reserved namespace)").arg(tagKey);
            return false;
        }
        
        // Tag values can be empty (they will be omitted), but if present, validate them
        if (!tagValue.isEmpty() && tagValue.contains('\n')) {
            errorMessage = QString("Tag value '%1' cannot contain newline characters").arg(tagValue);
            return false;
        }
    }
    
    // 5. Validate field keys and values
    for (auto it = dataPoint.fields.constBegin(); it != dataPoint.fields.constEnd(); ++it) {
        const QString &fieldKey = it.key();
        const QVariant &fieldValue = it.value();
        
        if (fieldKey.isEmpty()) {
            errorMessage = "Field key cannot be empty";
            return false;
        }
        
        if (fieldKey.startsWith('_')) {
            errorMessage = QString("Field key '%1' cannot start with underscore (reserved namespace)").arg(fieldKey);
            return false;
        }
        
        // Reserved field keys
        if (fieldKey == "_field" || fieldKey == "_measurement" || fieldKey == "time") {
            errorMessage = QString("Field key '%1' is reserved").arg(fieldKey);
            return false;
        }
        
        FieldType fieldType = detectFieldType(fieldValue);
        if (!isValidFieldValue(fieldValue, fieldType)) {
            errorMessage = QString("Invalid field value for key '%1'").arg(fieldKey);
            return false;
        }
        
        // Check for newlines in string field values
        if (fieldType == FieldType::String && fieldValue.toString().contains('\n')) {
            errorMessage = QString("String field value '%1' cannot contain newline characters").arg(fieldKey);
            return false;
        }
    }
    
    // 6. Validate timestamp range if provided
    if (dataPoint.timestamp != 0) {
        // InfluxDB timestamp range: -9223372036854775806 to 9223372036854775806
        qint64 convertedTimestamp = convertTimestamp(dataPoint.timestamp, dataPoint.precision);
        if (convertedTimestamp < -9223372036854775806LL || convertedTimestamp > 9223372036854775806LL) {
            errorMessage = "Timestamp is outside valid InfluxDB range";
            return false;
        }
    }
    
    return true;
}

QString InfluxDBLineProtocol::escapeMeasurement(const QString &measurement)
{
    // Measurement names need commas and spaces escaped
    return escapeSpecialChars(measurement, ", ");
}

QString InfluxDBLineProtocol::escapeTagKey(const QString &tagKey)
{
    // Tag keys need commas, equals signs, and spaces escaped
    return escapeSpecialChars(tagKey, ",= ");
}

QString InfluxDBLineProtocol::escapeTagValue(const QString &tagValue)
{
    // Tag values need commas, equals signs, and spaces escaped
    return escapeSpecialChars(tagValue, ",= ");
}

QString InfluxDBLineProtocol::escapeFieldKey(const QString &fieldKey)
{
    // Field keys need commas, equals signs, and spaces escaped
    return escapeSpecialChars(fieldKey, ",= ");
}

QString InfluxDBLineProtocol::formatFieldValue(const QVariant &value, FieldType type)
{
    switch (type) {
        case FieldType::Float: {
            double doubleValue = value.toDouble();
            // Check for NaN and infinity (not supported by InfluxDB)
            if (std::isnan(doubleValue) || std::isinf(doubleValue)) {
                qWarning() << "[InfluxDB Line Protocol] NaN or Infinity values are not supported";
                return QString();
            }
            // Use 'g' format to avoid unnecessary decimal places, limit precision to 15 digits
            return QString::number(doubleValue, 'g', 15);
        }
        
        case FieldType::Integer: {
            qint64 intValue = value.toLongLong();
            // Check integer range: -9223372036854775808 to 9223372036854775807
            return QString::number(intValue) + "i";
        }
        
        case FieldType::UInteger: {
            quint64 uintValue = value.toULongLong();
            // Check unsigned integer range: 0 to 18446744073709551615
            return QString::number(uintValue) + "u";
        }
        
        case FieldType::String: {
            QString stringValue = value.toString();
            // String field values must be double-quoted
            // Escape double quotes and backslashes within the string
            stringValue.replace("\\", "\\\\"); // Escape backslashes first
            stringValue.replace("\"", "\\\"");     // Escape double quotes
            return "\"" + stringValue + "\"";
        }
        
        case FieldType::Boolean: {
            bool boolValue = value.toBool();
            // Boolean values are not quoted and use lowercase
            return boolValue ? "true" : "false";
        }
    }
    
    return QString();
}

InfluxDBLineProtocol::FieldType InfluxDBLineProtocol::detectFieldType(const QVariant &value)
{
    switch (value.typeId()) {
        case QMetaType::Bool:
            return FieldType::Boolean;
            
        case QMetaType::Int:
        case QMetaType::LongLong:
        case QMetaType::Short:
        case QMetaType::SChar:
            return FieldType::Integer;
            
        case QMetaType::UInt:
        case QMetaType::ULongLong:
        case QMetaType::UShort:
        case QMetaType::UChar:
            return FieldType::UInteger;
            
        case QMetaType::Double:
        case QMetaType::Float:
            return FieldType::Float;
            
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        default:
            return FieldType::String;
    }
}

qint64 InfluxDBLineProtocol::convertTimestamp(qint64 timestamp, TimestampPrecision precision)
{
    // Input timestamp is assumed to be in milliseconds (Qt standard)
    switch (precision) {
        case TimestampPrecision::Nanoseconds:
            return timestamp * 1000000LL; // ms to ns
            
        case TimestampPrecision::Microseconds:
            return timestamp * 1000LL;    // ms to μs
            
        case TimestampPrecision::Milliseconds:
            return timestamp;             // ms to ms (no conversion)
            
        case TimestampPrecision::Seconds:
            return timestamp / 1000LL;    // ms to s
    }
    
    return timestamp;
}

bool InfluxDBLineProtocol::isValidIdentifier(const QString &identifier)
{
    if (identifier.isEmpty()) {
        return false;
    }
    
    // Cannot start with underscore (reserved namespace)
    if (identifier.startsWith('_')) {
        return false;
    }
    
    // Cannot contain newlines
    if (identifier.contains('\n')) {
        return false;
    }
    
    return true;
}

QString InfluxDBLineProtocol::escapeSpecialChars(const QString &input, const QString &escapeChars)
{
    QString result = input;
    
    // Escape each character that needs escaping
    for (const QChar &ch : escapeChars) {
        result.replace(ch, QString("\\") + ch);
    }
    
    return result;
}

bool InfluxDBLineProtocol::isValidFieldValue(const QVariant &value, FieldType type)
{
    if (!value.isValid()) {
        return false;
    }
    
    switch (type) {
        case FieldType::Float: {
            bool ok;
            double doubleValue = value.toDouble(&ok);
            if (!ok) return false;
            // Check for NaN and infinity
            return !std::isnan(doubleValue) && !std::isinf(doubleValue);
        }
        
        case FieldType::Integer: {
            bool ok;
            value.toLongLong(&ok);
            return ok;
        }
        
        case FieldType::UInteger: {
            bool ok;
            value.toULongLong(&ok);
            return ok;
        }
        
        case FieldType::String: {
            QString stringValue = value.toString();
            // String length limit is 64KB
            return stringValue.length() <= 65536;
        }
        
        case FieldType::Boolean: {
            // QVariant can convert most types to bool, so this is generally valid
            return true;
        }
    }
    
    return false;
}
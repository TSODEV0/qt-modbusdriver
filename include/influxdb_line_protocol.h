#ifndef INFLUXDB_LINE_PROTOCOL_H
#define INFLUXDB_LINE_PROTOCOL_H

#include <QString>
#include <QVariant>
#include <QMap>
#include <QByteArray>
#include <QDateTime>

/**
 * @brief InfluxDB Line Protocol Handler
 * 
 * This class provides compliant InfluxDB line protocol formatting according to:
 * https://docs.influxdata.com/influxdb/cloud/reference/syntax/line-protocol/
 * 
 * Key features:
 * - Proper escaping for measurement names, tag keys, tag values, and field keys
 * - Correct data type handling (integers with 'i' suffix, unsigned with 'u', booleans)
 * - Nanosecond timestamp precision by default
 * - Field and tag validation
 * - Performance optimized for high-frequency data writing
 */
class InfluxDBLineProtocol
{
public:
    /**
     * @brief Data types supported by InfluxDB
     */
    enum class FieldType {
        Float,      // IEEE-754 64-bit floating-point (default numeric type)
        Integer,    // Signed 64-bit integer (requires 'i' suffix)
        UInteger,   // Unsigned 64-bit integer (requires 'u' suffix)
        String,     // String value (must be quoted)
        Boolean     // Boolean value (true/false, not quoted)
    };

    /**
     * @brief Timestamp precision options
     */
    enum class TimestampPrecision {
        Nanoseconds,    // Default precision (ns)
        Microseconds,   // μs
        Milliseconds,   // ms
        Seconds         // s
    };

    /**
     * @brief Represents a single data point for InfluxDB
     */
    struct DataPoint {
        QString measurement;                    // Required: measurement name
        QMap<QString, QString> tags;           // Optional: tag key-value pairs
        QMap<QString, QVariant> fields;        // Required: field key-value pairs (at least one)
        qint64 timestamp;                      // Optional: Unix timestamp (0 = use server time)
        TimestampPrecision precision;          // Timestamp precision
        
        DataPoint() : timestamp(0), precision(TimestampPrecision::Nanoseconds) {}
    };

    /**
     * @brief Format a data point into InfluxDB line protocol
     * @param dataPoint The data point to format
     * @return Formatted line protocol string as QByteArray
     */
    static QByteArray formatLineProtocol(const DataPoint &dataPoint);

    /**
     * @brief Format multiple data points into InfluxDB line protocol
     * @param dataPoints Vector of data points to format
     * @return Formatted line protocol string as QByteArray (newline separated)
     */
    static QByteArray formatLineProtocol(const QVector<DataPoint> &dataPoints);

    /**
     * @brief Validate a data point for InfluxDB compliance
     * @param dataPoint The data point to validate
     * @param errorMessage Output parameter for error details
     * @return true if valid, false otherwise
     */
    static bool validateDataPoint(const DataPoint &dataPoint, QString &errorMessage);

    /**
     * @brief Escape measurement name according to InfluxDB line protocol rules
     * @param measurement The measurement name to escape
     * @return Escaped measurement name
     */
    static QString escapeMeasurement(const QString &measurement);

    /**
     * @brief Escape tag key according to InfluxDB line protocol rules
     * @param tagKey The tag key to escape
     * @return Escaped tag key
     */
    static QString escapeTagKey(const QString &tagKey);

    /**
     * @brief Escape tag value according to InfluxDB line protocol rules
     * @param tagValue The tag value to escape
     * @return Escaped tag value
     */
    static QString escapeTagValue(const QString &tagValue);

    /**
     * @brief Escape field key according to InfluxDB line protocol rules
     * @param fieldKey The field key to escape
     * @return Escaped field key
     */
    static QString escapeFieldKey(const QString &fieldKey);

    /**
     * @brief Format field value according to InfluxDB data type rules
     * @param value The field value to format
     * @param type The InfluxDB field type
     * @return Formatted field value string
     */
    static QString formatFieldValue(const QVariant &value, FieldType type);

    /**
     * @brief Detect the appropriate InfluxDB field type for a QVariant
     * @param value The value to analyze
     * @return Detected InfluxDB field type
     */
    static FieldType detectFieldType(const QVariant &value);

    /**
     * @brief Convert timestamp to specified precision
     * @param timestamp Unix timestamp in milliseconds
     * @param precision Target precision
     * @return Converted timestamp
     */
    static qint64 convertTimestamp(qint64 timestamp, TimestampPrecision precision);

    /**
     * @brief Check if a string is a valid InfluxDB identifier
     * @param identifier The identifier to check
     * @return true if valid, false otherwise
     */
    static bool isValidIdentifier(const QString &identifier);

private:
    /**
     * @brief Internal helper to escape special characters
     * @param input The string to escape
     * @param escapeChars Characters that need escaping
     * @return Escaped string
     */
    static QString escapeSpecialChars(const QString &input, const QString &escapeChars);

    /**
     * @brief Validate field value for InfluxDB compliance
     * @param value The field value to validate
     * @param type The expected field type
     * @return true if valid, false otherwise
     */
    static bool isValidFieldValue(const QVariant &value, FieldType type);
};

#endif // INFLUXDB_LINE_PROTOCOL_H
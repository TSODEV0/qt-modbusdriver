#include <QCoreApplication>
#include <QTest>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

#include "test_modbus_worker.h"
#include "test_modbus_worker_manager.h"

class TestRunner
{
public:
    static int runTests(int argc, char *argv[])
    {
        QCoreApplication app(argc, argv);
        
        // Set up test environment
        setupTestEnvironment();
        
        int totalFailures = 0;
        QStringList testResults;
        
        // Run ModbusWorker tests
        qDebug() << "\n=== Running ModbusWorker Tests ===";
        TestModbusWorker modbusWorkerTest;
        int workerFailures = QTest::qExec(&modbusWorkerTest, argc, argv);
        totalFailures += workerFailures;
        testResults << QString("ModbusWorker Tests: %1 failures").arg(workerFailures);
        
        // Run ModbusWorkerManager tests
        qDebug() << "\n=== Running ModbusWorkerManager Tests ===";
        TestModbusWorkerManager managerTest;
        int managerFailures = QTest::qExec(&managerTest, argc, argv);
        totalFailures += managerFailures;
        testResults << QString("ModbusWorkerManager Tests: %1 failures").arg(managerFailures);
        
        // Print summary
        printTestSummary(testResults, totalFailures);
        
        // Generate test report
        generateTestReport(testResults, totalFailures);
        
        return totalFailures;
    }
    
private:
    static void setupTestEnvironment()
    {
        // Set up any global test configuration
        qDebug() << "Setting up test environment...";
        
        // Create test output directory if it doesn't exist
        QDir testDir;
        if (!testDir.exists("test_output")) {
            testDir.mkdir("test_output");
        }
        
        // Set Qt test output format
        qputenv("QTEST_COLORED", "1");
    }
    
    static void printTestSummary(const QStringList& results, int totalFailures)
    {
        qDebug() << "\n" << QString("=").repeated(50);
        qDebug() << "TEST SUMMARY";
        qDebug() << QString("=").repeated(50);
        
        for (const QString& result : results) {
            qDebug() << result;
        }
        
        qDebug() << QString("-").repeated(50);
        if (totalFailures == 0) {
            qDebug() << "🎉 ALL TESTS PASSED!";
        } else {
            qDebug() << QString("❌ TOTAL FAILURES: %1").arg(totalFailures);
        }
        qDebug() << QString("=").repeated(50);
    }
    
    static void generateTestReport(const QStringList& results, int totalFailures)
    {
        QString reportPath = "test_output/test_report.txt";
        QFile reportFile(reportPath);
        
        if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&reportFile);
            
            out << "Modbus Driver Test Report\n";
            out << "Generated: " << QDateTime::currentDateTime().toString() << "\n";
            out << QString("=").repeated(50) << "\n";
            
            for (const QString& result : results) {
                out << result << "\n";
            }
            
            out << QString("-").repeated(50) << "\n";
            if (totalFailures == 0) {
                out << "Status: ALL TESTS PASSED\n";
            } else {
                out << QString("Status: %1 FAILURES\n").arg(totalFailures);
            }
            
            reportFile.close();
            qDebug() << "Test report generated:" << QDir::current().absoluteFilePath(reportPath);
        } else {
            qWarning() << "Failed to generate test report at:" << reportPath;
        }
    }
};

int main(int argc, char *argv[])
{
    return TestRunner::runTests(argc, argv);
}
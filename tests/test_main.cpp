#include <QtTest/QtTest>
#include <QCoreApplication>
#include "test_modbus_worker.h"
#include "test_modbus_worker_manager.h"
#include "test_scada_core_service.h"
#include "test_modbus_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    int result = 0;
    
    // Run ModbusWorker tests
    {
        TestModbusWorker testWorker;
        result |= QTest::qExec(&testWorker, argc, argv);
    }
    
    // Run ModbusWorkerManager tests
    {
        TestModbusWorkerManager testWorkerManager;
        result |= QTest::qExec(&testWorkerManager, argc, argv);
    }
    
    // Run ScadaCoreService tests
    {
        TestScadaCoreService testScadaService;
        result |= QTest::qExec(&testScadaService, argc, argv);
    }
    
    // Run ModbusManager tests
    {
        TestModbusManager testModbusManager;
        result |= QTest::qExec(&testModbusManager, argc, argv);
    }
    
    return result;
}
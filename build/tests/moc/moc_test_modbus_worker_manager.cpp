/****************************************************************************
** Meta object code from reading C++ file 'test_modbus_worker_manager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../tests/test_modbus_worker_manager.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'test_modbus_worker_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS = QtMocHelpers::stringData(
    "TestModbusWorkerManager",
    "initTestCase",
    "",
    "cleanupTestCase",
    "init",
    "cleanup",
    "testConstructor",
    "testCreateDeviceKey",
    "testGetOrCreateWorker",
    "testGetWorker",
    "testRemoveWorker",
    "testRemoveAllWorkers",
    "testGetWorkerCount",
    "testGetActiveDevices",
    "testGetConnectedDevices",
    "testGlobalStatistics",
    "testResetGlobalStatistics",
    "testStatisticsUpdates",
    "testDefaultPollInterval",
    "testSetWorkerPollInterval",
    "testStartAllWorkers",
    "testStopAllWorkers",
    "testConnectAllDevices",
    "testDisconnectAllDevices",
    "testWorkerCreatedSignal",
    "testWorkerRemovedSignal",
    "testGlobalStatisticsSignal",
    "testConcurrentWorkerAccess",
    "testConcurrentStatisticsAccess",
    "testConcurrentWorkerRemoval",
    "testConcurrentPollIntervalChanges",
    "testConcurrentWorkerLifecycle"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS_t {
    uint offsetsAndSizes[64];
    char stringdata0[24];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[5];
    char stringdata5[8];
    char stringdata6[16];
    char stringdata7[20];
    char stringdata8[22];
    char stringdata9[14];
    char stringdata10[17];
    char stringdata11[21];
    char stringdata12[19];
    char stringdata13[21];
    char stringdata14[24];
    char stringdata15[21];
    char stringdata16[26];
    char stringdata17[22];
    char stringdata18[24];
    char stringdata19[26];
    char stringdata20[20];
    char stringdata21[19];
    char stringdata22[22];
    char stringdata23[25];
    char stringdata24[24];
    char stringdata25[24];
    char stringdata26[27];
    char stringdata27[27];
    char stringdata28[31];
    char stringdata29[28];
    char stringdata30[34];
    char stringdata31[30];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS_t qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 23),  // "TestModbusWorkerManager"
        QT_MOC_LITERAL(24, 12),  // "initTestCase"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 15),  // "cleanupTestCase"
        QT_MOC_LITERAL(54, 4),  // "init"
        QT_MOC_LITERAL(59, 7),  // "cleanup"
        QT_MOC_LITERAL(67, 15),  // "testConstructor"
        QT_MOC_LITERAL(83, 19),  // "testCreateDeviceKey"
        QT_MOC_LITERAL(103, 21),  // "testGetOrCreateWorker"
        QT_MOC_LITERAL(125, 13),  // "testGetWorker"
        QT_MOC_LITERAL(139, 16),  // "testRemoveWorker"
        QT_MOC_LITERAL(156, 20),  // "testRemoveAllWorkers"
        QT_MOC_LITERAL(177, 18),  // "testGetWorkerCount"
        QT_MOC_LITERAL(196, 20),  // "testGetActiveDevices"
        QT_MOC_LITERAL(217, 23),  // "testGetConnectedDevices"
        QT_MOC_LITERAL(241, 20),  // "testGlobalStatistics"
        QT_MOC_LITERAL(262, 25),  // "testResetGlobalStatistics"
        QT_MOC_LITERAL(288, 21),  // "testStatisticsUpdates"
        QT_MOC_LITERAL(310, 23),  // "testDefaultPollInterval"
        QT_MOC_LITERAL(334, 25),  // "testSetWorkerPollInterval"
        QT_MOC_LITERAL(360, 19),  // "testStartAllWorkers"
        QT_MOC_LITERAL(380, 18),  // "testStopAllWorkers"
        QT_MOC_LITERAL(399, 21),  // "testConnectAllDevices"
        QT_MOC_LITERAL(421, 24),  // "testDisconnectAllDevices"
        QT_MOC_LITERAL(446, 23),  // "testWorkerCreatedSignal"
        QT_MOC_LITERAL(470, 23),  // "testWorkerRemovedSignal"
        QT_MOC_LITERAL(494, 26),  // "testGlobalStatisticsSignal"
        QT_MOC_LITERAL(521, 26),  // "testConcurrentWorkerAccess"
        QT_MOC_LITERAL(548, 30),  // "testConcurrentStatisticsAccess"
        QT_MOC_LITERAL(579, 27),  // "testConcurrentWorkerRemoval"
        QT_MOC_LITERAL(607, 33),  // "testConcurrentPollIntervalCha..."
        QT_MOC_LITERAL(641, 29)   // "testConcurrentWorkerLifecycle"
    },
    "TestModbusWorkerManager",
    "initTestCase",
    "",
    "cleanupTestCase",
    "init",
    "cleanup",
    "testConstructor",
    "testCreateDeviceKey",
    "testGetOrCreateWorker",
    "testGetWorker",
    "testRemoveWorker",
    "testRemoveAllWorkers",
    "testGetWorkerCount",
    "testGetActiveDevices",
    "testGetConnectedDevices",
    "testGlobalStatistics",
    "testResetGlobalStatistics",
    "testStatisticsUpdates",
    "testDefaultPollInterval",
    "testSetWorkerPollInterval",
    "testStartAllWorkers",
    "testStopAllWorkers",
    "testConnectAllDevices",
    "testDisconnectAllDevices",
    "testWorkerCreatedSignal",
    "testWorkerRemovedSignal",
    "testGlobalStatisticsSignal",
    "testConcurrentWorkerAccess",
    "testConcurrentStatisticsAccess",
    "testConcurrentWorkerRemoval",
    "testConcurrentPollIntervalChanges",
    "testConcurrentWorkerLifecycle"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTestModbusWorkerManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  194,    2, 0x08,    1 /* Private */,
       3,    0,  195,    2, 0x08,    2 /* Private */,
       4,    0,  196,    2, 0x08,    3 /* Private */,
       5,    0,  197,    2, 0x08,    4 /* Private */,
       6,    0,  198,    2, 0x08,    5 /* Private */,
       7,    0,  199,    2, 0x08,    6 /* Private */,
       8,    0,  200,    2, 0x08,    7 /* Private */,
       9,    0,  201,    2, 0x08,    8 /* Private */,
      10,    0,  202,    2, 0x08,    9 /* Private */,
      11,    0,  203,    2, 0x08,   10 /* Private */,
      12,    0,  204,    2, 0x08,   11 /* Private */,
      13,    0,  205,    2, 0x08,   12 /* Private */,
      14,    0,  206,    2, 0x08,   13 /* Private */,
      15,    0,  207,    2, 0x08,   14 /* Private */,
      16,    0,  208,    2, 0x08,   15 /* Private */,
      17,    0,  209,    2, 0x08,   16 /* Private */,
      18,    0,  210,    2, 0x08,   17 /* Private */,
      19,    0,  211,    2, 0x08,   18 /* Private */,
      20,    0,  212,    2, 0x08,   19 /* Private */,
      21,    0,  213,    2, 0x08,   20 /* Private */,
      22,    0,  214,    2, 0x08,   21 /* Private */,
      23,    0,  215,    2, 0x08,   22 /* Private */,
      24,    0,  216,    2, 0x08,   23 /* Private */,
      25,    0,  217,    2, 0x08,   24 /* Private */,
      26,    0,  218,    2, 0x08,   25 /* Private */,
      27,    0,  219,    2, 0x08,   26 /* Private */,
      28,    0,  220,    2, 0x08,   27 /* Private */,
      29,    0,  221,    2, 0x08,   28 /* Private */,
      30,    0,  222,    2, 0x08,   29 /* Private */,
      31,    0,  223,    2, 0x08,   30 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject TestModbusWorkerManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTestModbusWorkerManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TestModbusWorkerManager, std::true_type>,
        // method 'initTestCase'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cleanupTestCase'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'init'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cleanup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConstructor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testCreateDeviceKey'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGetOrCreateWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGetWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testRemoveWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testRemoveAllWorkers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGetWorkerCount'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGetActiveDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGetConnectedDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGlobalStatistics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testResetGlobalStatistics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testStatisticsUpdates'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testDefaultPollInterval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testSetWorkerPollInterval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testStartAllWorkers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testStopAllWorkers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConnectAllDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testDisconnectAllDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testWorkerCreatedSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testWorkerRemovedSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGlobalStatisticsSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentWorkerAccess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentStatisticsAccess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentWorkerRemoval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentPollIntervalChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentWorkerLifecycle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void TestModbusWorkerManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TestModbusWorkerManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->initTestCase(); break;
        case 1: _t->cleanupTestCase(); break;
        case 2: _t->init(); break;
        case 3: _t->cleanup(); break;
        case 4: _t->testConstructor(); break;
        case 5: _t->testCreateDeviceKey(); break;
        case 6: _t->testGetOrCreateWorker(); break;
        case 7: _t->testGetWorker(); break;
        case 8: _t->testRemoveWorker(); break;
        case 9: _t->testRemoveAllWorkers(); break;
        case 10: _t->testGetWorkerCount(); break;
        case 11: _t->testGetActiveDevices(); break;
        case 12: _t->testGetConnectedDevices(); break;
        case 13: _t->testGlobalStatistics(); break;
        case 14: _t->testResetGlobalStatistics(); break;
        case 15: _t->testStatisticsUpdates(); break;
        case 16: _t->testDefaultPollInterval(); break;
        case 17: _t->testSetWorkerPollInterval(); break;
        case 18: _t->testStartAllWorkers(); break;
        case 19: _t->testStopAllWorkers(); break;
        case 20: _t->testConnectAllDevices(); break;
        case 21: _t->testDisconnectAllDevices(); break;
        case 22: _t->testWorkerCreatedSignal(); break;
        case 23: _t->testWorkerRemovedSignal(); break;
        case 24: _t->testGlobalStatisticsSignal(); break;
        case 25: _t->testConcurrentWorkerAccess(); break;
        case 26: _t->testConcurrentStatisticsAccess(); break;
        case 27: _t->testConcurrentWorkerRemoval(); break;
        case 28: _t->testConcurrentPollIntervalChanges(); break;
        case 29: _t->testConcurrentWorkerLifecycle(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *TestModbusWorkerManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestModbusWorkerManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTestModbusWorkerManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TestModbusWorkerManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 30)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 30;
    }
    return _id;
}
QT_WARNING_POP

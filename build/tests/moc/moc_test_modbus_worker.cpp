/****************************************************************************
** Meta object code from reading C++ file 'test_modbus_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../tests/test_modbus_worker.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'test_modbus_worker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS = QtMocHelpers::stringData(
    "TestModbusWorker",
    "initTestCase",
    "",
    "cleanupTestCase",
    "init",
    "cleanup",
    "testConstructor",
    "testDeviceProperties",
    "testGetDeviceKey",
    "testConnectionState",
    "testStartStopWorker",
    "testQueueReadRequest",
    "testQueueWriteRequest",
    "testRequestPriority",
    "testClearRequestQueue",
    "testStatisticsTracking",
    "testResetStatistics",
    "testPollInterval",
    "testPollTimer",
    "testErrorHandling",
    "testRequestInterruption",
    "testSignalEmission",
    "testConcurrentRequestQueuing",
    "testConcurrentStatisticsAccess",
    "testThreadSafeRequestInterruption",
    "testConcurrentPollIntervalChanges"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS_t {
    uint offsetsAndSizes[52];
    char stringdata0[17];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[5];
    char stringdata5[8];
    char stringdata6[16];
    char stringdata7[21];
    char stringdata8[17];
    char stringdata9[20];
    char stringdata10[20];
    char stringdata11[21];
    char stringdata12[22];
    char stringdata13[20];
    char stringdata14[22];
    char stringdata15[23];
    char stringdata16[20];
    char stringdata17[17];
    char stringdata18[14];
    char stringdata19[18];
    char stringdata20[24];
    char stringdata21[19];
    char stringdata22[29];
    char stringdata23[31];
    char stringdata24[34];
    char stringdata25[34];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS_t qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "TestModbusWorker"
        QT_MOC_LITERAL(17, 12),  // "initTestCase"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 15),  // "cleanupTestCase"
        QT_MOC_LITERAL(47, 4),  // "init"
        QT_MOC_LITERAL(52, 7),  // "cleanup"
        QT_MOC_LITERAL(60, 15),  // "testConstructor"
        QT_MOC_LITERAL(76, 20),  // "testDeviceProperties"
        QT_MOC_LITERAL(97, 16),  // "testGetDeviceKey"
        QT_MOC_LITERAL(114, 19),  // "testConnectionState"
        QT_MOC_LITERAL(134, 19),  // "testStartStopWorker"
        QT_MOC_LITERAL(154, 20),  // "testQueueReadRequest"
        QT_MOC_LITERAL(175, 21),  // "testQueueWriteRequest"
        QT_MOC_LITERAL(197, 19),  // "testRequestPriority"
        QT_MOC_LITERAL(217, 21),  // "testClearRequestQueue"
        QT_MOC_LITERAL(239, 22),  // "testStatisticsTracking"
        QT_MOC_LITERAL(262, 19),  // "testResetStatistics"
        QT_MOC_LITERAL(282, 16),  // "testPollInterval"
        QT_MOC_LITERAL(299, 13),  // "testPollTimer"
        QT_MOC_LITERAL(313, 17),  // "testErrorHandling"
        QT_MOC_LITERAL(331, 23),  // "testRequestInterruption"
        QT_MOC_LITERAL(355, 18),  // "testSignalEmission"
        QT_MOC_LITERAL(374, 28),  // "testConcurrentRequestQueuing"
        QT_MOC_LITERAL(403, 30),  // "testConcurrentStatisticsAccess"
        QT_MOC_LITERAL(434, 33),  // "testThreadSafeRequestInterrup..."
        QT_MOC_LITERAL(468, 33)   // "testConcurrentPollIntervalCha..."
    },
    "TestModbusWorker",
    "initTestCase",
    "",
    "cleanupTestCase",
    "init",
    "cleanup",
    "testConstructor",
    "testDeviceProperties",
    "testGetDeviceKey",
    "testConnectionState",
    "testStartStopWorker",
    "testQueueReadRequest",
    "testQueueWriteRequest",
    "testRequestPriority",
    "testClearRequestQueue",
    "testStatisticsTracking",
    "testResetStatistics",
    "testPollInterval",
    "testPollTimer",
    "testErrorHandling",
    "testRequestInterruption",
    "testSignalEmission",
    "testConcurrentRequestQueuing",
    "testConcurrentStatisticsAccess",
    "testThreadSafeRequestInterruption",
    "testConcurrentPollIntervalChanges"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTestModbusWorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  158,    2, 0x08,    1 /* Private */,
       3,    0,  159,    2, 0x08,    2 /* Private */,
       4,    0,  160,    2, 0x08,    3 /* Private */,
       5,    0,  161,    2, 0x08,    4 /* Private */,
       6,    0,  162,    2, 0x08,    5 /* Private */,
       7,    0,  163,    2, 0x08,    6 /* Private */,
       8,    0,  164,    2, 0x08,    7 /* Private */,
       9,    0,  165,    2, 0x08,    8 /* Private */,
      10,    0,  166,    2, 0x08,    9 /* Private */,
      11,    0,  167,    2, 0x08,   10 /* Private */,
      12,    0,  168,    2, 0x08,   11 /* Private */,
      13,    0,  169,    2, 0x08,   12 /* Private */,
      14,    0,  170,    2, 0x08,   13 /* Private */,
      15,    0,  171,    2, 0x08,   14 /* Private */,
      16,    0,  172,    2, 0x08,   15 /* Private */,
      17,    0,  173,    2, 0x08,   16 /* Private */,
      18,    0,  174,    2, 0x08,   17 /* Private */,
      19,    0,  175,    2, 0x08,   18 /* Private */,
      20,    0,  176,    2, 0x08,   19 /* Private */,
      21,    0,  177,    2, 0x08,   20 /* Private */,
      22,    0,  178,    2, 0x08,   21 /* Private */,
      23,    0,  179,    2, 0x08,   22 /* Private */,
      24,    0,  180,    2, 0x08,   23 /* Private */,
      25,    0,  181,    2, 0x08,   24 /* Private */,

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

       0        // eod
};

Q_CONSTINIT const QMetaObject TestModbusWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTestModbusWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TestModbusWorker, std::true_type>,
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
        // method 'testDeviceProperties'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testGetDeviceKey'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConnectionState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testStartStopWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testQueueReadRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testQueueWriteRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testRequestPriority'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testClearRequestQueue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testStatisticsTracking'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testResetStatistics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testPollInterval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testPollTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testErrorHandling'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testRequestInterruption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testSignalEmission'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentRequestQueuing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentStatisticsAccess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testThreadSafeRequestInterruption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testConcurrentPollIntervalChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void TestModbusWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TestModbusWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->initTestCase(); break;
        case 1: _t->cleanupTestCase(); break;
        case 2: _t->init(); break;
        case 3: _t->cleanup(); break;
        case 4: _t->testConstructor(); break;
        case 5: _t->testDeviceProperties(); break;
        case 6: _t->testGetDeviceKey(); break;
        case 7: _t->testConnectionState(); break;
        case 8: _t->testStartStopWorker(); break;
        case 9: _t->testQueueReadRequest(); break;
        case 10: _t->testQueueWriteRequest(); break;
        case 11: _t->testRequestPriority(); break;
        case 12: _t->testClearRequestQueue(); break;
        case 13: _t->testStatisticsTracking(); break;
        case 14: _t->testResetStatistics(); break;
        case 15: _t->testPollInterval(); break;
        case 16: _t->testPollTimer(); break;
        case 17: _t->testErrorHandling(); break;
        case 18: _t->testRequestInterruption(); break;
        case 19: _t->testSignalEmission(); break;
        case 20: _t->testConcurrentRequestQueuing(); break;
        case 21: _t->testConcurrentStatisticsAccess(); break;
        case 22: _t->testThreadSafeRequestInterruption(); break;
        case 23: _t->testConcurrentPollIntervalChanges(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *TestModbusWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestModbusWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTestModbusWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TestModbusWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    return _id;
}
QT_WARNING_POP

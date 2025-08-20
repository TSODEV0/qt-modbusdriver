/****************************************************************************
** Meta object code from reading C++ file 'scada_service_test.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/scada_service_test.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scada_service_test.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSScadaServiceTestENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSScadaServiceTestENDCLASS = QtMocHelpers::stringData(
    "ScadaServiceTest",
    "onWorkerAdded",
    "",
    "workerId",
    "onWorkerRemoved",
    "onGlobalStatisticsUpdated",
    "ModbusWorkerManager::GlobalStatistics",
    "stats",
    "onWorkerReadCompleted",
    "requestId",
    "ModbusReadResult",
    "result",
    "onWorkerWriteCompleted",
    "ModbusWriteResult",
    "onWorkerError",
    "error",
    "stopTest",
    "onDataReceived",
    "deviceId",
    "value",
    "onConnectionStatusChanged",
    "connected",
    "onReadCompleted",
    "success",
    "onErrorOccurred",
    "onStatisticsUpdated",
    "ScadaCoreService::ServiceStatistics",
    "onWriteCompleted",
    "testWriteOperationsFromDatabase",
    "onWorkerCreated",
    "onDatabaseConnected",
    "onDatabaseError",
    "onServiceStarted",
    "onServiceStopped",
    "onDataPointAcquired",
    "AcquiredDataPoint",
    "dataPoint",
    "onDataPointSentToInflux",
    "measurement"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSScadaServiceTestENDCLASS_t {
    uint offsetsAndSizes[78];
    char stringdata0[17];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[16];
    char stringdata5[26];
    char stringdata6[38];
    char stringdata7[6];
    char stringdata8[22];
    char stringdata9[10];
    char stringdata10[17];
    char stringdata11[7];
    char stringdata12[23];
    char stringdata13[18];
    char stringdata14[14];
    char stringdata15[6];
    char stringdata16[9];
    char stringdata17[15];
    char stringdata18[9];
    char stringdata19[6];
    char stringdata20[26];
    char stringdata21[10];
    char stringdata22[16];
    char stringdata23[8];
    char stringdata24[16];
    char stringdata25[20];
    char stringdata26[36];
    char stringdata27[17];
    char stringdata28[32];
    char stringdata29[16];
    char stringdata30[20];
    char stringdata31[16];
    char stringdata32[17];
    char stringdata33[17];
    char stringdata34[20];
    char stringdata35[18];
    char stringdata36[10];
    char stringdata37[24];
    char stringdata38[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSScadaServiceTestENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSScadaServiceTestENDCLASS_t qt_meta_stringdata_CLASSScadaServiceTestENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "ScadaServiceTest"
        QT_MOC_LITERAL(17, 13),  // "onWorkerAdded"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 8),  // "workerId"
        QT_MOC_LITERAL(41, 15),  // "onWorkerRemoved"
        QT_MOC_LITERAL(57, 25),  // "onGlobalStatisticsUpdated"
        QT_MOC_LITERAL(83, 37),  // "ModbusWorkerManager::GlobalSt..."
        QT_MOC_LITERAL(121, 5),  // "stats"
        QT_MOC_LITERAL(127, 21),  // "onWorkerReadCompleted"
        QT_MOC_LITERAL(149, 9),  // "requestId"
        QT_MOC_LITERAL(159, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(176, 6),  // "result"
        QT_MOC_LITERAL(183, 22),  // "onWorkerWriteCompleted"
        QT_MOC_LITERAL(206, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(224, 13),  // "onWorkerError"
        QT_MOC_LITERAL(238, 5),  // "error"
        QT_MOC_LITERAL(244, 8),  // "stopTest"
        QT_MOC_LITERAL(253, 14),  // "onDataReceived"
        QT_MOC_LITERAL(268, 8),  // "deviceId"
        QT_MOC_LITERAL(277, 5),  // "value"
        QT_MOC_LITERAL(283, 25),  // "onConnectionStatusChanged"
        QT_MOC_LITERAL(309, 9),  // "connected"
        QT_MOC_LITERAL(319, 15),  // "onReadCompleted"
        QT_MOC_LITERAL(335, 7),  // "success"
        QT_MOC_LITERAL(343, 15),  // "onErrorOccurred"
        QT_MOC_LITERAL(359, 19),  // "onStatisticsUpdated"
        QT_MOC_LITERAL(379, 35),  // "ScadaCoreService::ServiceStat..."
        QT_MOC_LITERAL(415, 16),  // "onWriteCompleted"
        QT_MOC_LITERAL(432, 31),  // "testWriteOperationsFromDatabase"
        QT_MOC_LITERAL(464, 15),  // "onWorkerCreated"
        QT_MOC_LITERAL(480, 19),  // "onDatabaseConnected"
        QT_MOC_LITERAL(500, 15),  // "onDatabaseError"
        QT_MOC_LITERAL(516, 16),  // "onServiceStarted"
        QT_MOC_LITERAL(533, 16),  // "onServiceStopped"
        QT_MOC_LITERAL(550, 19),  // "onDataPointAcquired"
        QT_MOC_LITERAL(570, 17),  // "AcquiredDataPoint"
        QT_MOC_LITERAL(588, 9),  // "dataPoint"
        QT_MOC_LITERAL(598, 23),  // "onDataPointSentToInflux"
        QT_MOC_LITERAL(622, 11)   // "measurement"
    },
    "ScadaServiceTest",
    "onWorkerAdded",
    "",
    "workerId",
    "onWorkerRemoved",
    "onGlobalStatisticsUpdated",
    "ModbusWorkerManager::GlobalStatistics",
    "stats",
    "onWorkerReadCompleted",
    "requestId",
    "ModbusReadResult",
    "result",
    "onWorkerWriteCompleted",
    "ModbusWriteResult",
    "onWorkerError",
    "error",
    "stopTest",
    "onDataReceived",
    "deviceId",
    "value",
    "onConnectionStatusChanged",
    "connected",
    "onReadCompleted",
    "success",
    "onErrorOccurred",
    "onStatisticsUpdated",
    "ScadaCoreService::ServiceStatistics",
    "onWriteCompleted",
    "testWriteOperationsFromDatabase",
    "onWorkerCreated",
    "onDatabaseConnected",
    "onDatabaseError",
    "onServiceStarted",
    "onServiceStopped",
    "onDataPointAcquired",
    "AcquiredDataPoint",
    "dataPoint",
    "onDataPointSentToInflux",
    "measurement"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSScadaServiceTestENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  140,    2, 0x0a,    1 /* Public */,
       4,    1,  143,    2, 0x0a,    3 /* Public */,
       5,    1,  146,    2, 0x0a,    5 /* Public */,
       8,    2,  149,    2, 0x0a,    7 /* Public */,
      12,    2,  154,    2, 0x0a,   10 /* Public */,
      14,    2,  159,    2, 0x0a,   13 /* Public */,
      16,    0,  164,    2, 0x0a,   16 /* Public */,
      17,    2,  165,    2, 0x0a,   17 /* Public */,
      20,    2,  170,    2, 0x0a,   20 /* Public */,
      22,    4,  175,    2, 0x0a,   23 /* Public */,
      24,    1,  184,    2, 0x0a,   28 /* Public */,
      25,    1,  187,    2, 0x0a,   30 /* Public */,
      27,    4,  190,    2, 0x0a,   32 /* Public */,
      28,    0,  199,    2, 0x0a,   37 /* Public */,
      29,    1,  200,    2, 0x0a,   38 /* Public */,
      30,    0,  203,    2, 0x0a,   40 /* Public */,
      31,    1,  204,    2, 0x0a,   41 /* Public */,
      32,    0,  207,    2, 0x0a,   43 /* Public */,
      33,    0,  208,    2, 0x0a,   44 /* Public */,
      34,    1,  209,    2, 0x0a,   45 /* Public */,
      37,    2,  212,    2, 0x0a,   47 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 10,    9,   11,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 13,    9,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   18,   19,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   18,   21,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::Bool, QMetaType::QVariant,    9,   18,   23,   19,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, 0x80000000 | 26,    7,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::Bool, QMetaType::QString,    9,   18,   23,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 35,   36,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   38,   23,

       0        // eod
};

Q_CONSTINIT const QMetaObject ScadaServiceTest::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSScadaServiceTestENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSScadaServiceTestENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSScadaServiceTestENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScadaServiceTest, std::true_type>,
        // method 'onWorkerAdded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkerRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onGlobalStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWorkerManager::GlobalStatistics &, std::false_type>,
        // method 'onWorkerReadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'onWorkerWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>,
        // method 'onWorkerError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'stopTest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'onConnectionStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onReadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'onErrorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ScadaCoreService::ServiceStatistics &, std::false_type>,
        // method 'onWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'testWriteOperationsFromDatabase'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onWorkerCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onDatabaseConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDatabaseError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onServiceStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onServiceStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDataPointAcquired'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AcquiredDataPoint &, std::false_type>,
        // method 'onDataPointSentToInflux'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void ScadaServiceTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScadaServiceTest *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onWorkerAdded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onWorkerRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onGlobalStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<ModbusWorkerManager::GlobalStatistics>>(_a[1]))); break;
        case 3: _t->onWorkerReadCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[2]))); break;
        case 4: _t->onWorkerWriteCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[2]))); break;
        case 5: _t->onWorkerError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->stopTest(); break;
        case 7: _t->onDataReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 8: _t->onConnectionStatusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 9: _t->onReadCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[4]))); break;
        case 10: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<ScadaCoreService::ServiceStatistics>>(_a[1]))); break;
        case 12: _t->onWriteCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 13: _t->testWriteOperationsFromDatabase(); break;
        case 14: _t->onWorkerCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->onDatabaseConnected(); break;
        case 16: _t->onDatabaseError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->onServiceStarted(); break;
        case 18: _t->onServiceStopped(); break;
        case 19: _t->onDataPointAcquired((*reinterpret_cast< std::add_pointer_t<AcquiredDataPoint>>(_a[1]))); break;
        case 20: _t->onDataPointSentToInflux((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *ScadaServiceTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScadaServiceTest::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSScadaServiceTestENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScadaServiceTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 21;
    }
    return _id;
}
QT_WARNING_POP

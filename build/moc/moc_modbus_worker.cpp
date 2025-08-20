/****************************************************************************
** Meta object code from reading C++ file 'modbus_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/modbus_worker.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbus_worker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusWorkerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusWorkerENDCLASS = QtMocHelpers::stringData(
    "ModbusWorker",
    "readCompleted",
    "",
    "requestId",
    "ModbusReadResult",
    "result",
    "writeCompleted",
    "ModbusWriteResult",
    "connectionStateChanged",
    "deviceKey",
    "connected",
    "errorOccurred",
    "error",
    "errorOccurredClassified",
    "ModbusErrorType",
    "errorType",
    "requestInterrupted",
    "reason",
    "statisticsUpdated",
    "WorkerStatistics",
    "stats",
    "workerStarted",
    "workerStopped",
    "startWorker",
    "stopWorker",
    "initializeTimers",
    "connectToDevice",
    "disconnectFromDevice",
    "interruptCurrentRequest",
    "clearRequestQueue",
    "setPollInterval",
    "intervalMs",
    "setPollingEnabled",
    "enabled",
    "onRequestTimeout",
    "processRequestQueue",
    "onPollTimer",
    "onModbusReadCompleted",
    "onModbusWriteCompleted",
    "onModbusConnectionStateChanged",
    "onModbusError",
    "onBatchTimeout",
    "onHealthCheckTimer",
    "onHeartbeatTimer"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusWorkerENDCLASS_t {
    uint offsetsAndSizes[88];
    char stringdata0[13];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[17];
    char stringdata5[7];
    char stringdata6[15];
    char stringdata7[18];
    char stringdata8[23];
    char stringdata9[10];
    char stringdata10[10];
    char stringdata11[14];
    char stringdata12[6];
    char stringdata13[24];
    char stringdata14[16];
    char stringdata15[10];
    char stringdata16[19];
    char stringdata17[7];
    char stringdata18[18];
    char stringdata19[17];
    char stringdata20[6];
    char stringdata21[14];
    char stringdata22[14];
    char stringdata23[12];
    char stringdata24[11];
    char stringdata25[17];
    char stringdata26[16];
    char stringdata27[21];
    char stringdata28[24];
    char stringdata29[18];
    char stringdata30[16];
    char stringdata31[11];
    char stringdata32[18];
    char stringdata33[8];
    char stringdata34[17];
    char stringdata35[20];
    char stringdata36[12];
    char stringdata37[22];
    char stringdata38[23];
    char stringdata39[31];
    char stringdata40[14];
    char stringdata41[15];
    char stringdata42[19];
    char stringdata43[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusWorkerENDCLASS_t qt_meta_stringdata_CLASSModbusWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "ModbusWorker"
        QT_MOC_LITERAL(13, 13),  // "readCompleted"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 9),  // "requestId"
        QT_MOC_LITERAL(38, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(55, 6),  // "result"
        QT_MOC_LITERAL(62, 14),  // "writeCompleted"
        QT_MOC_LITERAL(77, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(95, 22),  // "connectionStateChanged"
        QT_MOC_LITERAL(118, 9),  // "deviceKey"
        QT_MOC_LITERAL(128, 9),  // "connected"
        QT_MOC_LITERAL(138, 13),  // "errorOccurred"
        QT_MOC_LITERAL(152, 5),  // "error"
        QT_MOC_LITERAL(158, 23),  // "errorOccurredClassified"
        QT_MOC_LITERAL(182, 15),  // "ModbusErrorType"
        QT_MOC_LITERAL(198, 9),  // "errorType"
        QT_MOC_LITERAL(208, 18),  // "requestInterrupted"
        QT_MOC_LITERAL(227, 6),  // "reason"
        QT_MOC_LITERAL(234, 17),  // "statisticsUpdated"
        QT_MOC_LITERAL(252, 16),  // "WorkerStatistics"
        QT_MOC_LITERAL(269, 5),  // "stats"
        QT_MOC_LITERAL(275, 13),  // "workerStarted"
        QT_MOC_LITERAL(289, 13),  // "workerStopped"
        QT_MOC_LITERAL(303, 11),  // "startWorker"
        QT_MOC_LITERAL(315, 10),  // "stopWorker"
        QT_MOC_LITERAL(326, 16),  // "initializeTimers"
        QT_MOC_LITERAL(343, 15),  // "connectToDevice"
        QT_MOC_LITERAL(359, 20),  // "disconnectFromDevice"
        QT_MOC_LITERAL(380, 23),  // "interruptCurrentRequest"
        QT_MOC_LITERAL(404, 17),  // "clearRequestQueue"
        QT_MOC_LITERAL(422, 15),  // "setPollInterval"
        QT_MOC_LITERAL(438, 10),  // "intervalMs"
        QT_MOC_LITERAL(449, 17),  // "setPollingEnabled"
        QT_MOC_LITERAL(467, 7),  // "enabled"
        QT_MOC_LITERAL(475, 16),  // "onRequestTimeout"
        QT_MOC_LITERAL(492, 19),  // "processRequestQueue"
        QT_MOC_LITERAL(512, 11),  // "onPollTimer"
        QT_MOC_LITERAL(524, 21),  // "onModbusReadCompleted"
        QT_MOC_LITERAL(546, 22),  // "onModbusWriteCompleted"
        QT_MOC_LITERAL(569, 30),  // "onModbusConnectionStateChanged"
        QT_MOC_LITERAL(600, 13),  // "onModbusError"
        QT_MOC_LITERAL(614, 14),  // "onBatchTimeout"
        QT_MOC_LITERAL(629, 18),  // "onHealthCheckTimer"
        QT_MOC_LITERAL(648, 16)   // "onHeartbeatTimer"
    },
    "ModbusWorker",
    "readCompleted",
    "",
    "requestId",
    "ModbusReadResult",
    "result",
    "writeCompleted",
    "ModbusWriteResult",
    "connectionStateChanged",
    "deviceKey",
    "connected",
    "errorOccurred",
    "error",
    "errorOccurredClassified",
    "ModbusErrorType",
    "errorType",
    "requestInterrupted",
    "reason",
    "statisticsUpdated",
    "WorkerStatistics",
    "stats",
    "workerStarted",
    "workerStopped",
    "startWorker",
    "stopWorker",
    "initializeTimers",
    "connectToDevice",
    "disconnectFromDevice",
    "interruptCurrentRequest",
    "clearRequestQueue",
    "setPollInterval",
    "intervalMs",
    "setPollingEnabled",
    "enabled",
    "onRequestTimeout",
    "processRequestQueue",
    "onPollTimer",
    "onModbusReadCompleted",
    "onModbusWriteCompleted",
    "onModbusConnectionStateChanged",
    "onModbusError",
    "onBatchTimeout",
    "onHealthCheckTimer",
    "onHeartbeatTimer"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusWorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,  188,    2, 0x06,    1 /* Public */,
       6,    2,  193,    2, 0x06,    4 /* Public */,
       8,    2,  198,    2, 0x06,    7 /* Public */,
      11,    2,  203,    2, 0x06,   10 /* Public */,
      13,    3,  208,    2, 0x06,   13 /* Public */,
      16,    2,  215,    2, 0x06,   17 /* Public */,
      18,    2,  220,    2, 0x06,   20 /* Public */,
      21,    1,  225,    2, 0x06,   23 /* Public */,
      22,    1,  228,    2, 0x06,   25 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      23,    0,  231,    2, 0x0a,   27 /* Public */,
      24,    0,  232,    2, 0x0a,   28 /* Public */,
      25,    0,  233,    2, 0x0a,   29 /* Public */,
      26,    0,  234,    2, 0x0a,   30 /* Public */,
      27,    0,  235,    2, 0x0a,   31 /* Public */,
      28,    1,  236,    2, 0x0a,   32 /* Public */,
      28,    0,  239,    2, 0x2a,   34 /* Public | MethodCloned */,
      29,    0,  240,    2, 0x0a,   35 /* Public */,
      30,    1,  241,    2, 0x0a,   36 /* Public */,
      32,    1,  244,    2, 0x0a,   38 /* Public */,
      34,    0,  247,    2, 0x08,   40 /* Private */,
      35,    0,  248,    2, 0x08,   41 /* Private */,
      36,    0,  249,    2, 0x08,   42 /* Private */,
      37,    1,  250,    2, 0x08,   43 /* Private */,
      38,    1,  253,    2, 0x08,   45 /* Private */,
      39,    1,  256,    2, 0x08,   47 /* Private */,
      40,    1,  259,    2, 0x08,   49 /* Private */,
      41,    0,  262,    2, 0x08,   51 /* Private */,
      42,    0,  263,    2, 0x08,   52 /* Private */,
      43,    0,  264,    2, 0x08,   53 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 7,    3,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 14,    9,   12,   15,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString,    3,   17,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 19,    9,   20,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   31,
    QMetaType::Void, QMetaType::Bool,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    5,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusWorker, std::true_type>,
        // method 'readCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>,
        // method 'connectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'errorOccurredClassified'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<ModbusErrorType, std::false_type>,
        // method 'requestInterrupted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'statisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const WorkerStatistics &, std::false_type>,
        // method 'workerStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'workerStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'startWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopWorker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'initializeTimers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectToDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnectFromDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'interruptCurrentRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'interruptCurrentRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearRequestQueue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setPollInterval'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setPollingEnabled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onRequestTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'processRequestQueue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPollTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onModbusReadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'onModbusWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>,
        // method 'onModbusConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onModbusError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onBatchTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onHealthCheckTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onHeartbeatTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ModbusWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->readCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[2]))); break;
        case 1: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[2]))); break;
        case 2: _t->connectionStateChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->errorOccurredClassified((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<ModbusErrorType>>(_a[3]))); break;
        case 5: _t->requestInterrupted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->statisticsUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<WorkerStatistics>>(_a[2]))); break;
        case 7: _t->workerStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->workerStopped((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->startWorker(); break;
        case 10: _t->stopWorker(); break;
        case 11: _t->initializeTimers(); break;
        case 12: _t->connectToDevice(); break;
        case 13: _t->disconnectFromDevice(); break;
        case 14: _t->interruptCurrentRequest((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->interruptCurrentRequest(); break;
        case 16: _t->clearRequestQueue(); break;
        case 17: _t->setPollInterval((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->setPollingEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->onRequestTimeout(); break;
        case 20: _t->processRequestQueue(); break;
        case 21: _t->onPollTimer(); break;
        case 22: _t->onModbusReadCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 23: _t->onModbusWriteCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        case 24: _t->onModbusConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 25: _t->onModbusError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->onBatchTimeout(); break;
        case 27: _t->onHealthCheckTimer(); break;
        case 28: _t->onHeartbeatTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ModbusErrorType >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusWorker::*)(qint64 , const ModbusReadResult & );
            if (_t _q_method = &ModbusWorker::readCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(qint64 , const ModbusWriteResult & );
            if (_t _q_method = &ModbusWorker::writeCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & , bool );
            if (_t _q_method = &ModbusWorker::connectionStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & , const QString & );
            if (_t _q_method = &ModbusWorker::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & , const QString & , ModbusErrorType );
            if (_t _q_method = &ModbusWorker::errorOccurredClassified; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(qint64 , const QString & );
            if (_t _q_method = &ModbusWorker::requestInterrupted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & , const WorkerStatistics & );
            if (_t _q_method = &ModbusWorker::statisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & );
            if (_t _q_method = &ModbusWorker::workerStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & );
            if (_t _q_method = &ModbusWorker::workerStopped; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject *ModbusWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void ModbusWorker::readCompleted(qint64 _t1, const ModbusReadResult & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModbusWorker::writeCompleted(qint64 _t1, const ModbusWriteResult & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModbusWorker::connectionStateChanged(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModbusWorker::errorOccurred(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ModbusWorker::errorOccurredClassified(const QString & _t1, const QString & _t2, ModbusErrorType _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ModbusWorker::requestInterrupted(qint64 _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ModbusWorker::statisticsUpdated(const QString & _t1, const WorkerStatistics & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ModbusWorker::workerStarted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ModbusWorker::workerStopped(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_WARNING_POP

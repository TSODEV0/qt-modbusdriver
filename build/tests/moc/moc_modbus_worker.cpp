/****************************************************************************
** Meta object code from reading C++ file 'modbus_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/modbus_worker.h"
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
    "requestInterrupted",
    "reason",
    "statisticsUpdated",
    "WorkerStatistics",
    "stats",
    "workerStarted",
    "workerStopped",
    "startWorker",
    "stopWorker",
    "connectToDevice",
    "disconnectFromDevice",
    "interruptCurrentRequest",
    "clearRequestQueue",
    "setPollInterval",
    "intervalMs",
    "processRequestQueue",
    "onPollTimer",
    "onModbusReadCompleted",
    "onModbusWriteCompleted",
    "onModbusConnectionStateChanged",
    "onModbusError"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusWorkerENDCLASS_t {
    uint offsetsAndSizes[68];
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
    char stringdata13[19];
    char stringdata14[7];
    char stringdata15[18];
    char stringdata16[17];
    char stringdata17[6];
    char stringdata18[14];
    char stringdata19[14];
    char stringdata20[12];
    char stringdata21[11];
    char stringdata22[16];
    char stringdata23[21];
    char stringdata24[24];
    char stringdata25[18];
    char stringdata26[16];
    char stringdata27[11];
    char stringdata28[20];
    char stringdata29[12];
    char stringdata30[22];
    char stringdata31[23];
    char stringdata32[31];
    char stringdata33[14];
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
        QT_MOC_LITERAL(158, 18),  // "requestInterrupted"
        QT_MOC_LITERAL(177, 6),  // "reason"
        QT_MOC_LITERAL(184, 17),  // "statisticsUpdated"
        QT_MOC_LITERAL(202, 16),  // "WorkerStatistics"
        QT_MOC_LITERAL(219, 5),  // "stats"
        QT_MOC_LITERAL(225, 13),  // "workerStarted"
        QT_MOC_LITERAL(239, 13),  // "workerStopped"
        QT_MOC_LITERAL(253, 11),  // "startWorker"
        QT_MOC_LITERAL(265, 10),  // "stopWorker"
        QT_MOC_LITERAL(276, 15),  // "connectToDevice"
        QT_MOC_LITERAL(292, 20),  // "disconnectFromDevice"
        QT_MOC_LITERAL(313, 23),  // "interruptCurrentRequest"
        QT_MOC_LITERAL(337, 17),  // "clearRequestQueue"
        QT_MOC_LITERAL(355, 15),  // "setPollInterval"
        QT_MOC_LITERAL(371, 10),  // "intervalMs"
        QT_MOC_LITERAL(382, 19),  // "processRequestQueue"
        QT_MOC_LITERAL(402, 11),  // "onPollTimer"
        QT_MOC_LITERAL(414, 21),  // "onModbusReadCompleted"
        QT_MOC_LITERAL(436, 22),  // "onModbusWriteCompleted"
        QT_MOC_LITERAL(459, 30),  // "onModbusConnectionStateChanged"
        QT_MOC_LITERAL(490, 13)   // "onModbusError"
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
    "requestInterrupted",
    "reason",
    "statisticsUpdated",
    "WorkerStatistics",
    "stats",
    "workerStarted",
    "workerStopped",
    "startWorker",
    "stopWorker",
    "connectToDevice",
    "disconnectFromDevice",
    "interruptCurrentRequest",
    "clearRequestQueue",
    "setPollInterval",
    "intervalMs",
    "processRequestQueue",
    "onPollTimer",
    "onModbusReadCompleted",
    "onModbusWriteCompleted",
    "onModbusConnectionStateChanged",
    "onModbusError"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusWorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,  146,    2, 0x06,    1 /* Public */,
       6,    2,  151,    2, 0x06,    4 /* Public */,
       8,    2,  156,    2, 0x06,    7 /* Public */,
      11,    2,  161,    2, 0x06,   10 /* Public */,
      13,    2,  166,    2, 0x06,   13 /* Public */,
      15,    2,  171,    2, 0x06,   16 /* Public */,
      18,    1,  176,    2, 0x06,   19 /* Public */,
      19,    1,  179,    2, 0x06,   21 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      20,    0,  182,    2, 0x0a,   23 /* Public */,
      21,    0,  183,    2, 0x0a,   24 /* Public */,
      22,    0,  184,    2, 0x0a,   25 /* Public */,
      23,    0,  185,    2, 0x0a,   26 /* Public */,
      24,    1,  186,    2, 0x0a,   27 /* Public */,
      24,    0,  189,    2, 0x2a,   29 /* Public | MethodCloned */,
      25,    0,  190,    2, 0x0a,   30 /* Public */,
      26,    1,  191,    2, 0x0a,   31 /* Public */,
      28,    0,  194,    2, 0x08,   33 /* Private */,
      29,    0,  195,    2, 0x08,   34 /* Private */,
      30,    1,  196,    2, 0x08,   35 /* Private */,
      31,    1,  199,    2, 0x08,   37 /* Private */,
      32,    1,  202,    2, 0x08,   39 /* Private */,
      33,    1,  205,    2, 0x08,   41 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 7,    3,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   12,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString,    3,   14,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 16,    9,   17,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    5,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::QString,   12,

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
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
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
        case 4: _t->requestInterrupted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->statisticsUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<WorkerStatistics>>(_a[2]))); break;
        case 6: _t->workerStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->workerStopped((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->startWorker(); break;
        case 9: _t->stopWorker(); break;
        case 10: _t->connectToDevice(); break;
        case 11: _t->disconnectFromDevice(); break;
        case 12: _t->interruptCurrentRequest((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->interruptCurrentRequest(); break;
        case 14: _t->clearRequestQueue(); break;
        case 15: _t->setPollInterval((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->processRequestQueue(); break;
        case 17: _t->onPollTimer(); break;
        case 18: _t->onModbusReadCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 19: _t->onModbusWriteCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        case 20: _t->onModbusConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 21: _t->onModbusError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
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
            using _t = void (ModbusWorker::*)(qint64 , const QString & );
            if (_t _q_method = &ModbusWorker::requestInterrupted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & , const WorkerStatistics & );
            if (_t _q_method = &ModbusWorker::statisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & );
            if (_t _q_method = &ModbusWorker::workerStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ModbusWorker::*)(const QString & );
            if (_t _q_method = &ModbusWorker::workerStopped; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
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
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
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
void ModbusWorker::requestInterrupted(qint64 _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ModbusWorker::statisticsUpdated(const QString & _t1, const WorkerStatistics & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ModbusWorker::workerStarted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ModbusWorker::workerStopped(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS = QtMocHelpers::stringData(
    "ModbusWorkerManager",
    "workerCreated",
    "",
    "deviceKey",
    "workerRemoved",
    "globalStatisticsUpdated",
    "GlobalStatistics",
    "stats",
    "startAllWorkers",
    "stopAllWorkers",
    "connectAllDevices",
    "disconnectAllDevices",
    "onWorkerStarted",
    "onWorkerStopped",
    "onWorkerConnectionStateChanged",
    "connected",
    "onWorkerStatisticsUpdated",
    "ModbusWorker::WorkerStatistics",
    "updateGlobalStatistics"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS_t {
    uint offsetsAndSizes[38];
    char stringdata0[20];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[14];
    char stringdata5[24];
    char stringdata6[17];
    char stringdata7[6];
    char stringdata8[16];
    char stringdata9[15];
    char stringdata10[18];
    char stringdata11[21];
    char stringdata12[16];
    char stringdata13[16];
    char stringdata14[31];
    char stringdata15[10];
    char stringdata16[26];
    char stringdata17[31];
    char stringdata18[23];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS_t qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 19),  // "ModbusWorkerManager"
        QT_MOC_LITERAL(20, 13),  // "workerCreated"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 9),  // "deviceKey"
        QT_MOC_LITERAL(45, 13),  // "workerRemoved"
        QT_MOC_LITERAL(59, 23),  // "globalStatisticsUpdated"
        QT_MOC_LITERAL(83, 16),  // "GlobalStatistics"
        QT_MOC_LITERAL(100, 5),  // "stats"
        QT_MOC_LITERAL(106, 15),  // "startAllWorkers"
        QT_MOC_LITERAL(122, 14),  // "stopAllWorkers"
        QT_MOC_LITERAL(137, 17),  // "connectAllDevices"
        QT_MOC_LITERAL(155, 20),  // "disconnectAllDevices"
        QT_MOC_LITERAL(176, 15),  // "onWorkerStarted"
        QT_MOC_LITERAL(192, 15),  // "onWorkerStopped"
        QT_MOC_LITERAL(208, 30),  // "onWorkerConnectionStateChanged"
        QT_MOC_LITERAL(239, 9),  // "connected"
        QT_MOC_LITERAL(249, 25),  // "onWorkerStatisticsUpdated"
        QT_MOC_LITERAL(275, 30),  // "ModbusWorker::WorkerStatistics"
        QT_MOC_LITERAL(306, 22)   // "updateGlobalStatistics"
    },
    "ModbusWorkerManager",
    "workerCreated",
    "",
    "deviceKey",
    "workerRemoved",
    "globalStatisticsUpdated",
    "GlobalStatistics",
    "stats",
    "startAllWorkers",
    "stopAllWorkers",
    "connectAllDevices",
    "disconnectAllDevices",
    "onWorkerStarted",
    "onWorkerStopped",
    "onWorkerConnectionStateChanged",
    "connected",
    "onWorkerStatisticsUpdated",
    "ModbusWorker::WorkerStatistics",
    "updateGlobalStatistics"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusWorkerManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       4,    1,   89,    2, 0x06,    3 /* Public */,
       5,    1,   92,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   95,    2, 0x0a,    7 /* Public */,
       9,    0,   96,    2, 0x0a,    8 /* Public */,
      10,    0,   97,    2, 0x0a,    9 /* Public */,
      11,    0,   98,    2, 0x0a,   10 /* Public */,
      12,    1,   99,    2, 0x08,   11 /* Private */,
      13,    1,  102,    2, 0x08,   13 /* Private */,
      14,    2,  105,    2, 0x08,   15 /* Private */,
      16,    2,  110,    2, 0x08,   18 /* Private */,
      18,    0,  115,    2, 0x08,   21 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,   15,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 17,    3,    7,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusWorkerManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusWorkerManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusWorkerManager, std::true_type>,
        // method 'workerCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'workerRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'globalStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const GlobalStatistics &, std::false_type>,
        // method 'startAllWorkers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopAllWorkers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectAllDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnectAllDevices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onWorkerStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkerStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkerConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onWorkerStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWorker::WorkerStatistics &, std::false_type>,
        // method 'updateGlobalStatistics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ModbusWorkerManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusWorkerManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->workerCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->workerRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->globalStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<GlobalStatistics>>(_a[1]))); break;
        case 3: _t->startAllWorkers(); break;
        case 4: _t->stopAllWorkers(); break;
        case 5: _t->connectAllDevices(); break;
        case 6: _t->disconnectAllDevices(); break;
        case 7: _t->onWorkerStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onWorkerStopped((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onWorkerConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 10: _t->onWorkerStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusWorker::WorkerStatistics>>(_a[2]))); break;
        case 11: _t->updateGlobalStatistics(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusWorkerManager::*)(const QString & );
            if (_t _q_method = &ModbusWorkerManager::workerCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusWorkerManager::*)(const QString & );
            if (_t _q_method = &ModbusWorkerManager::workerRemoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusWorkerManager::*)(const GlobalStatistics & );
            if (_t _q_method = &ModbusWorkerManager::globalStatisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *ModbusWorkerManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusWorkerManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusWorkerManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ModbusWorkerManager::workerCreated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModbusWorkerManager::workerRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModbusWorkerManager::globalStatisticsUpdated(const GlobalStatistics & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP

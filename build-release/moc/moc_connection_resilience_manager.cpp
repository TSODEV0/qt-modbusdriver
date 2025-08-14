/****************************************************************************
** Meta object code from reading C++ file 'connection_resilience_manager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/connection_resilience_manager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connection_resilience_manager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS = QtMocHelpers::stringData(
    "ConnectionResilienceManager",
    "configurationChanged",
    "",
    "description",
    "monitoringStarted",
    "host",
    "port",
    "monitoringStopped",
    "connectionQualityChanged",
    "ConnectionQuality",
    "quality",
    "connectionLost",
    "connectionRestored",
    "reconnectionAttempt",
    "attempt",
    "maxAttempts",
    "reconnectionSuccessful",
    "reconnectionFailed",
    "errorDetected",
    "error",
    "consecutiveFailures",
    "responseTimeUpdated",
    "responseTime",
    "averageTime",
    "performHeartbeat",
    "retryConnection",
    "assessConnectionQuality",
    "onConnectionStateChanged",
    "connected",
    "onModbusError",
    "onOperationCompleted",
    "ModbusReadResult",
    "result",
    "onWriteCompleted",
    "ModbusWriteResult",
    "Unknown",
    "Excellent",
    "Good",
    "Fair",
    "Poor"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS_t {
    uint offsetsAndSizes[80];
    char stringdata0[28];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[18];
    char stringdata5[5];
    char stringdata6[5];
    char stringdata7[18];
    char stringdata8[25];
    char stringdata9[18];
    char stringdata10[8];
    char stringdata11[15];
    char stringdata12[19];
    char stringdata13[20];
    char stringdata14[8];
    char stringdata15[12];
    char stringdata16[23];
    char stringdata17[19];
    char stringdata18[14];
    char stringdata19[6];
    char stringdata20[20];
    char stringdata21[20];
    char stringdata22[13];
    char stringdata23[12];
    char stringdata24[17];
    char stringdata25[16];
    char stringdata26[24];
    char stringdata27[25];
    char stringdata28[10];
    char stringdata29[14];
    char stringdata30[21];
    char stringdata31[17];
    char stringdata32[7];
    char stringdata33[17];
    char stringdata34[18];
    char stringdata35[8];
    char stringdata36[10];
    char stringdata37[5];
    char stringdata38[5];
    char stringdata39[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS_t qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 27),  // "ConnectionResilienceManager"
        QT_MOC_LITERAL(28, 20),  // "configurationChanged"
        QT_MOC_LITERAL(49, 0),  // ""
        QT_MOC_LITERAL(50, 11),  // "description"
        QT_MOC_LITERAL(62, 17),  // "monitoringStarted"
        QT_MOC_LITERAL(80, 4),  // "host"
        QT_MOC_LITERAL(85, 4),  // "port"
        QT_MOC_LITERAL(90, 17),  // "monitoringStopped"
        QT_MOC_LITERAL(108, 24),  // "connectionQualityChanged"
        QT_MOC_LITERAL(133, 17),  // "ConnectionQuality"
        QT_MOC_LITERAL(151, 7),  // "quality"
        QT_MOC_LITERAL(159, 14),  // "connectionLost"
        QT_MOC_LITERAL(174, 18),  // "connectionRestored"
        QT_MOC_LITERAL(193, 19),  // "reconnectionAttempt"
        QT_MOC_LITERAL(213, 7),  // "attempt"
        QT_MOC_LITERAL(221, 11),  // "maxAttempts"
        QT_MOC_LITERAL(233, 22),  // "reconnectionSuccessful"
        QT_MOC_LITERAL(256, 18),  // "reconnectionFailed"
        QT_MOC_LITERAL(275, 13),  // "errorDetected"
        QT_MOC_LITERAL(289, 5),  // "error"
        QT_MOC_LITERAL(295, 19),  // "consecutiveFailures"
        QT_MOC_LITERAL(315, 19),  // "responseTimeUpdated"
        QT_MOC_LITERAL(335, 12),  // "responseTime"
        QT_MOC_LITERAL(348, 11),  // "averageTime"
        QT_MOC_LITERAL(360, 16),  // "performHeartbeat"
        QT_MOC_LITERAL(377, 15),  // "retryConnection"
        QT_MOC_LITERAL(393, 23),  // "assessConnectionQuality"
        QT_MOC_LITERAL(417, 24),  // "onConnectionStateChanged"
        QT_MOC_LITERAL(442, 9),  // "connected"
        QT_MOC_LITERAL(452, 13),  // "onModbusError"
        QT_MOC_LITERAL(466, 20),  // "onOperationCompleted"
        QT_MOC_LITERAL(487, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(504, 6),  // "result"
        QT_MOC_LITERAL(511, 16),  // "onWriteCompleted"
        QT_MOC_LITERAL(528, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(546, 7),  // "Unknown"
        QT_MOC_LITERAL(554, 9),  // "Excellent"
        QT_MOC_LITERAL(564, 4),  // "Good"
        QT_MOC_LITERAL(569, 4),  // "Fair"
        QT_MOC_LITERAL(574, 4)   // "Poor"
    },
    "ConnectionResilienceManager",
    "configurationChanged",
    "",
    "description",
    "monitoringStarted",
    "host",
    "port",
    "monitoringStopped",
    "connectionQualityChanged",
    "ConnectionQuality",
    "quality",
    "connectionLost",
    "connectionRestored",
    "reconnectionAttempt",
    "attempt",
    "maxAttempts",
    "reconnectionSuccessful",
    "reconnectionFailed",
    "errorDetected",
    "error",
    "consecutiveFailures",
    "responseTimeUpdated",
    "responseTime",
    "averageTime",
    "performHeartbeat",
    "retryConnection",
    "assessConnectionQuality",
    "onConnectionStateChanged",
    "connected",
    "onModbusError",
    "onOperationCompleted",
    "ModbusReadResult",
    "result",
    "onWriteCompleted",
    "ModbusWriteResult",
    "Unknown",
    "Excellent",
    "Good",
    "Fair",
    "Poor"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSConnectionResilienceManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       1,  168, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  122,    2, 0x06,    2 /* Public */,
       4,    2,  125,    2, 0x06,    4 /* Public */,
       7,    0,  130,    2, 0x06,    7 /* Public */,
       8,    1,  131,    2, 0x06,    8 /* Public */,
      11,    0,  134,    2, 0x06,   10 /* Public */,
      12,    0,  135,    2, 0x06,   11 /* Public */,
      13,    2,  136,    2, 0x06,   12 /* Public */,
      16,    0,  141,    2, 0x06,   15 /* Public */,
      17,    0,  142,    2, 0x06,   16 /* Public */,
      18,    2,  143,    2, 0x06,   17 /* Public */,
      21,    2,  148,    2, 0x06,   20 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      24,    0,  153,    2, 0x08,   23 /* Private */,
      25,    0,  154,    2, 0x08,   24 /* Private */,
      26,    0,  155,    2, 0x08,   25 /* Private */,
      27,    1,  156,    2, 0x08,   26 /* Private */,
      29,    1,  159,    2, 0x08,   28 /* Private */,
      30,    1,  162,    2, 0x08,   30 /* Private */,
      33,    1,  165,    2, 0x08,   32 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    5,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   19,   20,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,   22,   23,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   28,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, 0x80000000 | 31,   32,
    QMetaType::Void, 0x80000000 | 34,   32,

 // enums: name, alias, flags, count, data
       9,    9, 0x2,    5,  173,

 // enum data: key, value
      35, uint(ConnectionResilienceManager::ConnectionQuality::Unknown),
      36, uint(ConnectionResilienceManager::ConnectionQuality::Excellent),
      37, uint(ConnectionResilienceManager::ConnectionQuality::Good),
      38, uint(ConnectionResilienceManager::ConnectionQuality::Fair),
      39, uint(ConnectionResilienceManager::ConnectionQuality::Poor),

       0        // eod
};

Q_CONSTINIT const QMetaObject ConnectionResilienceManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSConnectionResilienceManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS_t,
        // enum 'ConnectionQuality'
        QtPrivate::TypeAndForceComplete<ConnectionResilienceManager::ConnectionQuality, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ConnectionResilienceManager, std::true_type>,
        // method 'configurationChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'monitoringStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'monitoringStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectionQualityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ConnectionQuality, std::false_type>,
        // method 'connectionLost'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectionRestored'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reconnectionAttempt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'reconnectionSuccessful'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reconnectionFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'errorDetected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'responseTimeUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'performHeartbeat'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'retryConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'assessConnectionQuality'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onModbusError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onOperationCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'onWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>
    >,
    nullptr
} };

void ConnectionResilienceManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConnectionResilienceManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->configurationChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->monitoringStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->monitoringStopped(); break;
        case 3: _t->connectionQualityChanged((*reinterpret_cast< std::add_pointer_t<ConnectionQuality>>(_a[1]))); break;
        case 4: _t->connectionLost(); break;
        case 5: _t->connectionRestored(); break;
        case 6: _t->reconnectionAttempt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->reconnectionSuccessful(); break;
        case 8: _t->reconnectionFailed(); break;
        case 9: _t->errorDetected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 10: _t->responseTimeUpdated((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 11: _t->performHeartbeat(); break;
        case 12: _t->retryConnection(); break;
        case 13: _t->assessConnectionQuality(); break;
        case 14: _t->onConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->onModbusError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->onOperationCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 17: _t->onWriteCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConnectionResilienceManager::*)(const QString & );
            if (_t _q_method = &ConnectionResilienceManager::configurationChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)(const QString & , int );
            if (_t _q_method = &ConnectionResilienceManager::monitoringStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)();
            if (_t _q_method = &ConnectionResilienceManager::monitoringStopped; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)(ConnectionQuality );
            if (_t _q_method = &ConnectionResilienceManager::connectionQualityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)();
            if (_t _q_method = &ConnectionResilienceManager::connectionLost; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)();
            if (_t _q_method = &ConnectionResilienceManager::connectionRestored; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)(int , int );
            if (_t _q_method = &ConnectionResilienceManager::reconnectionAttempt; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)();
            if (_t _q_method = &ConnectionResilienceManager::reconnectionSuccessful; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)();
            if (_t _q_method = &ConnectionResilienceManager::reconnectionFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)(const QString & , int );
            if (_t _q_method = &ConnectionResilienceManager::errorDetected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ConnectionResilienceManager::*)(qint64 , qint64 );
            if (_t _q_method = &ConnectionResilienceManager::responseTimeUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
    }
}

const QMetaObject *ConnectionResilienceManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConnectionResilienceManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSConnectionResilienceManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ConnectionResilienceManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void ConnectionResilienceManager::configurationChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ConnectionResilienceManager::monitoringStarted(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ConnectionResilienceManager::monitoringStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ConnectionResilienceManager::connectionQualityChanged(ConnectionQuality _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ConnectionResilienceManager::connectionLost()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ConnectionResilienceManager::connectionRestored()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ConnectionResilienceManager::reconnectionAttempt(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ConnectionResilienceManager::reconnectionSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ConnectionResilienceManager::reconnectionFailed()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ConnectionResilienceManager::errorDetected(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ConnectionResilienceManager::responseTimeUpdated(qint64 _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_WARNING_POP

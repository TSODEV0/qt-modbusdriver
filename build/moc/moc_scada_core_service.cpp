/****************************************************************************
** Meta object code from reading C++ file 'scada_core_service.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/scada_core_service.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scada_core_service.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS = QtMocHelpers::stringData(
    "ScadaCoreService",
    "serviceStarted",
    "",
    "serviceStopped",
    "dataPointAcquired",
    "AcquiredDataPoint",
    "dataPoint",
    "dataPointSentToInflux",
    "pointName",
    "success",
    "writeCompleted",
    "operation",
    "error",
    "errorOccurred",
    "statisticsUpdated",
    "ServiceStatistics",
    "stats",
    "alertGenerated",
    "SystemAlert",
    "alert",
    "alertCleared",
    "AlertType",
    "alertType",
    "source",
    "healthStatusChanged",
    "healthy",
    "dataLossDetected",
    "missedReadings",
    "onPollTimer",
    "onModbusReadCompleted",
    "ModbusReadResult",
    "result",
    "onModbusWriteCompleted",
    "ModbusWriteResult",
    "onModbusConnectionStateChanged",
    "connected",
    "onModbusError",
    "onInfluxWriteCompleted"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t {
    uint offsetsAndSizes[76];
    char stringdata0[17];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[18];
    char stringdata5[18];
    char stringdata6[10];
    char stringdata7[22];
    char stringdata8[10];
    char stringdata9[8];
    char stringdata10[15];
    char stringdata11[10];
    char stringdata12[6];
    char stringdata13[14];
    char stringdata14[18];
    char stringdata15[18];
    char stringdata16[6];
    char stringdata17[15];
    char stringdata18[12];
    char stringdata19[6];
    char stringdata20[13];
    char stringdata21[10];
    char stringdata22[10];
    char stringdata23[7];
    char stringdata24[20];
    char stringdata25[8];
    char stringdata26[17];
    char stringdata27[15];
    char stringdata28[12];
    char stringdata29[22];
    char stringdata30[17];
    char stringdata31[7];
    char stringdata32[23];
    char stringdata33[18];
    char stringdata34[31];
    char stringdata35[10];
    char stringdata36[14];
    char stringdata37[23];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "ScadaCoreService"
        QT_MOC_LITERAL(17, 14),  // "serviceStarted"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 14),  // "serviceStopped"
        QT_MOC_LITERAL(48, 17),  // "dataPointAcquired"
        QT_MOC_LITERAL(66, 17),  // "AcquiredDataPoint"
        QT_MOC_LITERAL(84, 9),  // "dataPoint"
        QT_MOC_LITERAL(94, 21),  // "dataPointSentToInflux"
        QT_MOC_LITERAL(116, 9),  // "pointName"
        QT_MOC_LITERAL(126, 7),  // "success"
        QT_MOC_LITERAL(134, 14),  // "writeCompleted"
        QT_MOC_LITERAL(149, 9),  // "operation"
        QT_MOC_LITERAL(159, 5),  // "error"
        QT_MOC_LITERAL(165, 13),  // "errorOccurred"
        QT_MOC_LITERAL(179, 17),  // "statisticsUpdated"
        QT_MOC_LITERAL(197, 17),  // "ServiceStatistics"
        QT_MOC_LITERAL(215, 5),  // "stats"
        QT_MOC_LITERAL(221, 14),  // "alertGenerated"
        QT_MOC_LITERAL(236, 11),  // "SystemAlert"
        QT_MOC_LITERAL(248, 5),  // "alert"
        QT_MOC_LITERAL(254, 12),  // "alertCleared"
        QT_MOC_LITERAL(267, 9),  // "AlertType"
        QT_MOC_LITERAL(277, 9),  // "alertType"
        QT_MOC_LITERAL(287, 6),  // "source"
        QT_MOC_LITERAL(294, 19),  // "healthStatusChanged"
        QT_MOC_LITERAL(314, 7),  // "healthy"
        QT_MOC_LITERAL(322, 16),  // "dataLossDetected"
        QT_MOC_LITERAL(339, 14),  // "missedReadings"
        QT_MOC_LITERAL(354, 11),  // "onPollTimer"
        QT_MOC_LITERAL(366, 21),  // "onModbusReadCompleted"
        QT_MOC_LITERAL(388, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(405, 6),  // "result"
        QT_MOC_LITERAL(412, 22),  // "onModbusWriteCompleted"
        QT_MOC_LITERAL(435, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(453, 30),  // "onModbusConnectionStateChanged"
        QT_MOC_LITERAL(484, 9),  // "connected"
        QT_MOC_LITERAL(494, 13),  // "onModbusError"
        QT_MOC_LITERAL(508, 22)   // "onInfluxWriteCompleted"
    },
    "ScadaCoreService",
    "serviceStarted",
    "",
    "serviceStopped",
    "dataPointAcquired",
    "AcquiredDataPoint",
    "dataPoint",
    "dataPointSentToInflux",
    "pointName",
    "success",
    "writeCompleted",
    "operation",
    "error",
    "errorOccurred",
    "statisticsUpdated",
    "ServiceStatistics",
    "stats",
    "alertGenerated",
    "SystemAlert",
    "alert",
    "alertCleared",
    "AlertType",
    "alertType",
    "source",
    "healthStatusChanged",
    "healthy",
    "dataLossDetected",
    "missedReadings",
    "onPollTimer",
    "onModbusReadCompleted",
    "ModbusReadResult",
    "result",
    "onModbusWriteCompleted",
    "ModbusWriteResult",
    "onModbusConnectionStateChanged",
    "connected",
    "onModbusError",
    "onInfluxWriteCompleted"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSScadaCoreServiceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  122,    2, 0x06,    1 /* Public */,
       3,    0,  123,    2, 0x06,    2 /* Public */,
       4,    1,  124,    2, 0x06,    3 /* Public */,
       7,    2,  127,    2, 0x06,    5 /* Public */,
      10,    3,  132,    2, 0x06,    8 /* Public */,
      10,    2,  139,    2, 0x26,   12 /* Public | MethodCloned */,
      13,    1,  144,    2, 0x06,   15 /* Public */,
      14,    1,  147,    2, 0x06,   17 /* Public */,
      17,    1,  150,    2, 0x06,   19 /* Public */,
      20,    2,  153,    2, 0x06,   21 /* Public */,
      24,    1,  158,    2, 0x06,   24 /* Public */,
      26,    2,  161,    2, 0x06,   26 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      28,    0,  166,    2, 0x08,   29 /* Private */,
      29,    1,  167,    2, 0x08,   30 /* Private */,
      32,    1,  170,    2, 0x08,   32 /* Private */,
      34,    1,  173,    2, 0x08,   34 /* Private */,
      36,    1,  176,    2, 0x08,   36 /* Private */,
      37,    3,  179,    2, 0x08,   38 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   11,    9,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   11,    9,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, 0x80000000 | 21, QMetaType::QString,   22,   23,
    QMetaType::Void, QMetaType::Bool,   25,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong,    8,   27,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void, 0x80000000 | 33,   31,
    QMetaType::Void, QMetaType::Bool,   35,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,    8,    9,   12,

       0        // eod
};

Q_CONSTINIT const QMetaObject ScadaCoreService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSScadaCoreServiceENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScadaCoreService, std::true_type>,
        // method 'serviceStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'serviceStopped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dataPointAcquired'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AcquiredDataPoint &, std::false_type>,
        // method 'dataPointSentToInflux'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'statisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ServiceStatistics &, std::false_type>,
        // method 'alertGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const SystemAlert &, std::false_type>,
        // method 'alertCleared'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<AlertType, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'healthStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'dataLossDetected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
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
        // method 'onInfluxWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ScadaCoreService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScadaCoreService *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->serviceStarted(); break;
        case 1: _t->serviceStopped(); break;
        case 2: _t->dataPointAcquired((*reinterpret_cast< std::add_pointer_t<AcquiredDataPoint>>(_a[1]))); break;
        case 3: _t->dataPointSentToInflux((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 4: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 5: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->statisticsUpdated((*reinterpret_cast< std::add_pointer_t<ServiceStatistics>>(_a[1]))); break;
        case 8: _t->alertGenerated((*reinterpret_cast< std::add_pointer_t<SystemAlert>>(_a[1]))); break;
        case 9: _t->alertCleared((*reinterpret_cast< std::add_pointer_t<AlertType>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->healthStatusChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->dataLossDetected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 12: _t->onPollTimer(); break;
        case 13: _t->onModbusReadCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 14: _t->onModbusWriteCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        case 15: _t->onModbusConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->onModbusError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->onInfluxWriteCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScadaCoreService::*)();
            if (_t _q_method = &ScadaCoreService::serviceStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)();
            if (_t _q_method = &ScadaCoreService::serviceStopped; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const AcquiredDataPoint & );
            if (_t _q_method = &ScadaCoreService::dataPointAcquired; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & , bool );
            if (_t _q_method = &ScadaCoreService::dataPointSentToInflux; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & , bool , const QString & );
            if (_t _q_method = &ScadaCoreService::writeCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & );
            if (_t _q_method = &ScadaCoreService::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const ServiceStatistics & );
            if (_t _q_method = &ScadaCoreService::statisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const SystemAlert & );
            if (_t _q_method = &ScadaCoreService::alertGenerated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(AlertType , const QString & );
            if (_t _q_method = &ScadaCoreService::alertCleared; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(bool );
            if (_t _q_method = &ScadaCoreService::healthStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & , qint64 );
            if (_t _q_method = &ScadaCoreService::dataLossDetected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
    }
}

const QMetaObject *ScadaCoreService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScadaCoreService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScadaCoreService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ScadaCoreService::serviceStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ScadaCoreService::serviceStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ScadaCoreService::dataPointAcquired(const AcquiredDataPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ScadaCoreService::dataPointSentToInflux(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ScadaCoreService::writeCompleted(const QString & _t1, bool _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 6
void ScadaCoreService::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ScadaCoreService::statisticsUpdated(const ServiceStatistics & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ScadaCoreService::alertGenerated(const SystemAlert & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ScadaCoreService::alertCleared(AlertType _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ScadaCoreService::healthStatusChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ScadaCoreService::dataLossDetected(const QString & _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS = QtMocHelpers::stringData(
    "InfluxDBWorker",
    "writeCompleted",
    "",
    "pointName",
    "success",
    "error",
    "statisticsUpdated",
    "socketErrors",
    "writeDataPoint",
    "AcquiredDataPoint",
    "dataPoint",
    "processBufferedDataPoint",
    "BufferedDataPoint",
    "bufferedPoint",
    "stop"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[15];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[8];
    char stringdata5[6];
    char stringdata6[18];
    char stringdata7[13];
    char stringdata8[15];
    char stringdata9[18];
    char stringdata10[10];
    char stringdata11[25];
    char stringdata12[18];
    char stringdata13[14];
    char stringdata14[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS_t qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "InfluxDBWorker"
        QT_MOC_LITERAL(15, 14),  // "writeCompleted"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 9),  // "pointName"
        QT_MOC_LITERAL(41, 7),  // "success"
        QT_MOC_LITERAL(49, 5),  // "error"
        QT_MOC_LITERAL(55, 17),  // "statisticsUpdated"
        QT_MOC_LITERAL(73, 12),  // "socketErrors"
        QT_MOC_LITERAL(86, 14),  // "writeDataPoint"
        QT_MOC_LITERAL(101, 17),  // "AcquiredDataPoint"
        QT_MOC_LITERAL(119, 9),  // "dataPoint"
        QT_MOC_LITERAL(129, 24),  // "processBufferedDataPoint"
        QT_MOC_LITERAL(154, 17),  // "BufferedDataPoint"
        QT_MOC_LITERAL(172, 13),  // "bufferedPoint"
        QT_MOC_LITERAL(186, 4)   // "stop"
    },
    "InfluxDBWorker",
    "writeCompleted",
    "",
    "pointName",
    "success",
    "error",
    "statisticsUpdated",
    "socketErrors",
    "writeDataPoint",
    "AcquiredDataPoint",
    "dataPoint",
    "processBufferedDataPoint",
    "BufferedDataPoint",
    "bufferedPoint",
    "stop"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSInfluxDBWorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   44,    2, 0x06,    1 /* Public */,
       6,    1,   51,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    1,   54,    2, 0x0a,    7 /* Public */,
      11,    1,   57,    2, 0x0a,    9 /* Public */,
      14,    0,   60,    2, 0x0a,   11 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::LongLong,    7,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject InfluxDBWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSInfluxDBWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<InfluxDBWorker, std::true_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'statisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'writeDataPoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AcquiredDataPoint &, std::false_type>,
        // method 'processBufferedDataPoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const BufferedDataPoint &, std::false_type>,
        // method 'stop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void InfluxDBWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<InfluxDBWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->statisticsUpdated((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 2: _t->writeDataPoint((*reinterpret_cast< std::add_pointer_t<AcquiredDataPoint>>(_a[1]))); break;
        case 3: _t->processBufferedDataPoint((*reinterpret_cast< std::add_pointer_t<BufferedDataPoint>>(_a[1]))); break;
        case 4: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (InfluxDBWorker::*)(const QString & , bool , const QString & );
            if (_t _q_method = &InfluxDBWorker::writeCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (InfluxDBWorker::*)(qint64 );
            if (_t _q_method = &InfluxDBWorker::statisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *InfluxDBWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InfluxDBWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSInfluxDBWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int InfluxDBWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void InfluxDBWorker::writeCompleted(const QString & _t1, bool _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void InfluxDBWorker::statisticsUpdated(qint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP

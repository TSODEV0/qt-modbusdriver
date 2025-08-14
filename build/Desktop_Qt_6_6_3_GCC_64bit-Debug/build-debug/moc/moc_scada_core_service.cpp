/****************************************************************************
** Meta object code from reading C++ file 'scada_core_service.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/scada_core_service.h"
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
    "onPollTimer",
    "onModbusReadCompleted",
    "ModbusReadResult",
    "result",
    "onModbusWriteCompleted",
    "ModbusWriteResult",
    "onModbusConnectionStateChanged",
    "connected",
    "onModbusError"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t {
    uint offsetsAndSizes[52];
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
    char stringdata17[12];
    char stringdata18[22];
    char stringdata19[17];
    char stringdata20[7];
    char stringdata21[23];
    char stringdata22[18];
    char stringdata23[31];
    char stringdata24[10];
    char stringdata25[14];
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
        QT_MOC_LITERAL(221, 11),  // "onPollTimer"
        QT_MOC_LITERAL(233, 21),  // "onModbusReadCompleted"
        QT_MOC_LITERAL(255, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(272, 6),  // "result"
        QT_MOC_LITERAL(279, 22),  // "onModbusWriteCompleted"
        QT_MOC_LITERAL(302, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(320, 30),  // "onModbusConnectionStateChanged"
        QT_MOC_LITERAL(351, 9),  // "connected"
        QT_MOC_LITERAL(361, 13)   // "onModbusError"
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
    "onPollTimer",
    "onModbusReadCompleted",
    "ModbusReadResult",
    "result",
    "onModbusWriteCompleted",
    "ModbusWriteResult",
    "onModbusConnectionStateChanged",
    "connected",
    "onModbusError"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSScadaCoreServiceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x06,    1 /* Public */,
       3,    0,   93,    2, 0x06,    2 /* Public */,
       4,    1,   94,    2, 0x06,    3 /* Public */,
       7,    2,   97,    2, 0x06,    5 /* Public */,
      10,    3,  102,    2, 0x06,    8 /* Public */,
      10,    2,  109,    2, 0x26,   12 /* Public | MethodCloned */,
      13,    1,  114,    2, 0x06,   15 /* Public */,
      14,    1,  117,    2, 0x06,   17 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      17,    0,  120,    2, 0x08,   19 /* Private */,
      18,    1,  121,    2, 0x08,   20 /* Private */,
      21,    1,  124,    2, 0x08,   22 /* Private */,
      23,    1,  127,    2, 0x08,   24 /* Private */,
      25,    1,  130,    2, 0x08,   26 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   11,    9,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   11,    9,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, 0x80000000 | 15,   16,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, 0x80000000 | 22,   20,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void, QMetaType::QString,   12,

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
        case 8: _t->onPollTimer(); break;
        case 9: _t->onModbusReadCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 10: _t->onModbusWriteCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        case 11: _t->onModbusConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->onModbusError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
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
QT_WARNING_POP

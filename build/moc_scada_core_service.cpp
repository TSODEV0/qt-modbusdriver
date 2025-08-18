/****************************************************************************
** Meta object code from reading C++ file 'scada_core_service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/scada_core_service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scada_core_service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ScadaCoreService_t {
    QByteArrayData data[26];
    char stringdata0[375];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScadaCoreService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScadaCoreService_t qt_meta_stringdata_ScadaCoreService = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ScadaCoreService"
QT_MOC_LITERAL(1, 17, 14), // "serviceStarted"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 14), // "serviceStopped"
QT_MOC_LITERAL(4, 48, 17), // "dataPointAcquired"
QT_MOC_LITERAL(5, 66, 17), // "AcquiredDataPoint"
QT_MOC_LITERAL(6, 84, 9), // "dataPoint"
QT_MOC_LITERAL(7, 94, 21), // "dataPointSentToInflux"
QT_MOC_LITERAL(8, 116, 9), // "pointName"
QT_MOC_LITERAL(9, 126, 7), // "success"
QT_MOC_LITERAL(10, 134, 14), // "writeCompleted"
QT_MOC_LITERAL(11, 149, 9), // "operation"
QT_MOC_LITERAL(12, 159, 5), // "error"
QT_MOC_LITERAL(13, 165, 13), // "errorOccurred"
QT_MOC_LITERAL(14, 179, 17), // "statisticsUpdated"
QT_MOC_LITERAL(15, 197, 17), // "ServiceStatistics"
QT_MOC_LITERAL(16, 215, 5), // "stats"
QT_MOC_LITERAL(17, 221, 11), // "onPollTimer"
QT_MOC_LITERAL(18, 233, 21), // "onModbusReadCompleted"
QT_MOC_LITERAL(19, 255, 16), // "ModbusReadResult"
QT_MOC_LITERAL(20, 272, 6), // "result"
QT_MOC_LITERAL(21, 279, 22), // "onModbusWriteCompleted"
QT_MOC_LITERAL(22, 302, 17), // "ModbusWriteResult"
QT_MOC_LITERAL(23, 320, 30), // "onModbusConnectionStateChanged"
QT_MOC_LITERAL(24, 351, 9), // "connected"
QT_MOC_LITERAL(25, 361, 13) // "onModbusError"

    },
    "ScadaCoreService\0serviceStarted\0\0"
    "serviceStopped\0dataPointAcquired\0"
    "AcquiredDataPoint\0dataPoint\0"
    "dataPointSentToInflux\0pointName\0success\0"
    "writeCompleted\0operation\0error\0"
    "errorOccurred\0statisticsUpdated\0"
    "ServiceStatistics\0stats\0onPollTimer\0"
    "onModbusReadCompleted\0ModbusReadResult\0"
    "result\0onModbusWriteCompleted\0"
    "ModbusWriteResult\0onModbusConnectionStateChanged\0"
    "connected\0onModbusError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScadaCoreService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    0,   80,    2, 0x06 /* Public */,
       4,    1,   81,    2, 0x06 /* Public */,
       7,    2,   84,    2, 0x06 /* Public */,
      10,    3,   89,    2, 0x06 /* Public */,
      10,    2,   96,    2, 0x26 /* Public | MethodCloned */,
      13,    1,  101,    2, 0x06 /* Public */,
      14,    1,  104,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,  107,    2, 0x08 /* Private */,
      18,    1,  108,    2, 0x08 /* Private */,
      21,    1,  111,    2, 0x08 /* Private */,
      23,    1,  114,    2, 0x08 /* Private */,
      25,    1,  117,    2, 0x08 /* Private */,

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

void ScadaCoreService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScadaCoreService *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->serviceStarted(); break;
        case 1: _t->serviceStopped(); break;
        case 2: _t->dataPointAcquired((*reinterpret_cast< const AcquiredDataPoint(*)>(_a[1]))); break;
        case 3: _t->dataPointSentToInflux((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->writeCompleted((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 5: _t->writeCompleted((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->statisticsUpdated((*reinterpret_cast< const ServiceStatistics(*)>(_a[1]))); break;
        case 8: _t->onPollTimer(); break;
        case 9: _t->onModbusReadCompleted((*reinterpret_cast< const ModbusReadResult(*)>(_a[1]))); break;
        case 10: _t->onModbusWriteCompleted((*reinterpret_cast< const ModbusWriteResult(*)>(_a[1]))); break;
        case 11: _t->onModbusConnectionStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->onModbusError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScadaCoreService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::serviceStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::serviceStopped)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const AcquiredDataPoint & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::dataPointAcquired)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::dataPointSentToInflux)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & , bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::writeCompleted)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::errorOccurred)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const ServiceStatistics & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScadaCoreService::statisticsUpdated)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ScadaCoreService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ScadaCoreService.data,
    qt_meta_data_ScadaCoreService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ScadaCoreService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScadaCoreService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScadaCoreService.stringdata0))
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
            *reinterpret_cast<int*>(_a[0]) = -1;
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
QT_END_MOC_NAMESPACE

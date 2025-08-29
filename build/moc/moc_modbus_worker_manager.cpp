/****************************************************************************
** Meta object code from reading C++ file 'modbus_worker_manager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/modbus_worker_manager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbus_worker_manager.h' doesn't include <QObject>."
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
    "ModbusWorkerTypes::WorkerStatistics",
    "updateGlobalStatistics",
    "handleDelayedStartup"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusWorkerManagerENDCLASS_t {
    uint offsetsAndSizes[40];
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
    char stringdata17[36];
    char stringdata18[23];
    char stringdata19[21];
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
        QT_MOC_LITERAL(275, 35),  // "ModbusWorkerTypes::WorkerStat..."
        QT_MOC_LITERAL(311, 22),  // "updateGlobalStatistics"
        QT_MOC_LITERAL(334, 20)   // "handleDelayedStartup"
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
    "ModbusWorkerTypes::WorkerStatistics",
    "updateGlobalStatistics",
    "handleDelayedStartup"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusWorkerManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   92,    2, 0x06,    1 /* Public */,
       4,    1,   95,    2, 0x06,    3 /* Public */,
       5,    1,   98,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,  101,    2, 0x0a,    7 /* Public */,
       9,    0,  102,    2, 0x0a,    8 /* Public */,
      10,    0,  103,    2, 0x0a,    9 /* Public */,
      11,    0,  104,    2, 0x0a,   10 /* Public */,
      12,    1,  105,    2, 0x08,   11 /* Private */,
      13,    1,  108,    2, 0x08,   13 /* Private */,
      14,    2,  111,    2, 0x08,   15 /* Private */,
      16,    2,  116,    2, 0x08,   18 /* Private */,
      18,    0,  121,    2, 0x08,   21 /* Private */,
      19,    1,  122,    2, 0x08,   22 /* Private */,

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
    QMetaType::Void, QMetaType::QString,    3,

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
        QtPrivate::TypeAndForceComplete<const ModbusWorkerTypes::WorkerStatistics &, std::false_type>,
        // method 'updateGlobalStatistics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDelayedStartup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
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
        case 10: _t->onWorkerStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusWorkerTypes::WorkerStatistics>>(_a[2]))); break;
        case 11: _t->updateGlobalStatistics(); break;
        case 12: _t->handleDelayedStartup((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
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

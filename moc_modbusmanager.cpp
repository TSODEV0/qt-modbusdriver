/****************************************************************************
** Meta object code from reading C++ file 'modbusmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "modbusmanager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbusmanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusManagerENDCLASS = QtMocHelpers::stringData(
    "ModbusManager",
    "readCompleted",
    "",
    "ModbusReadResult",
    "result",
    "writeCompleted",
    "ModbusWriteResult",
    "connectionStateChanged",
    "connected",
    "errorOccurred",
    "error",
    "onReadReady",
    "onWriteReady",
    "onStateChanged",
    "QModbusDevice::State",
    "state",
    "onErrorOccurred",
    "QModbusDevice::Error"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusManagerENDCLASS_t {
    uint offsetsAndSizes[36];
    char stringdata0[14];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[7];
    char stringdata5[15];
    char stringdata6[18];
    char stringdata7[23];
    char stringdata8[10];
    char stringdata9[14];
    char stringdata10[6];
    char stringdata11[12];
    char stringdata12[13];
    char stringdata13[15];
    char stringdata14[21];
    char stringdata15[6];
    char stringdata16[16];
    char stringdata17[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusManagerENDCLASS_t qt_meta_stringdata_CLASSModbusManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13),  // "ModbusManager"
        QT_MOC_LITERAL(14, 13),  // "readCompleted"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(46, 6),  // "result"
        QT_MOC_LITERAL(53, 14),  // "writeCompleted"
        QT_MOC_LITERAL(68, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(86, 22),  // "connectionStateChanged"
        QT_MOC_LITERAL(109, 9),  // "connected"
        QT_MOC_LITERAL(119, 13),  // "errorOccurred"
        QT_MOC_LITERAL(133, 5),  // "error"
        QT_MOC_LITERAL(139, 11),  // "onReadReady"
        QT_MOC_LITERAL(151, 12),  // "onWriteReady"
        QT_MOC_LITERAL(164, 14),  // "onStateChanged"
        QT_MOC_LITERAL(179, 20),  // "QModbusDevice::State"
        QT_MOC_LITERAL(200, 5),  // "state"
        QT_MOC_LITERAL(206, 15),  // "onErrorOccurred"
        QT_MOC_LITERAL(222, 20)   // "QModbusDevice::Error"
    },
    "ModbusManager",
    "readCompleted",
    "",
    "ModbusReadResult",
    "result",
    "writeCompleted",
    "ModbusWriteResult",
    "connectionStateChanged",
    "connected",
    "errorOccurred",
    "error",
    "onReadReady",
    "onWriteReady",
    "onStateChanged",
    "QModbusDevice::State",
    "state",
    "onErrorOccurred",
    "QModbusDevice::Error"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    1 /* Public */,
       5,    1,   65,    2, 0x06,    3 /* Public */,
       7,    1,   68,    2, 0x06,    5 /* Public */,
       9,    1,   71,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,   74,    2, 0x08,    9 /* Private */,
      12,    0,   75,    2, 0x08,   10 /* Private */,
      13,    1,   76,    2, 0x08,   11 /* Private */,
      16,    1,   79,    2, 0x08,   13 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    4,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::QString,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusManager, std::true_type>,
        // method 'readCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>,
        // method 'connectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onReadReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onWriteReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModbusDevice::State, std::false_type>,
        // method 'onErrorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModbusDevice::Error, std::false_type>
    >,
    nullptr
} };

void ModbusManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->readCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 1: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        case 2: _t->connectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onReadReady(); break;
        case 5: _t->onWriteReady(); break;
        case 6: _t->onStateChanged((*reinterpret_cast< std::add_pointer_t<QModbusDevice::State>>(_a[1]))); break;
        case 7: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QModbusDevice::Error>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusManager::*)(const ModbusReadResult & );
            if (_t _q_method = &ModbusManager::readCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(const ModbusWriteResult & );
            if (_t _q_method = &ModbusManager::writeCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(bool );
            if (_t _q_method = &ModbusManager::connectionStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(const QString & );
            if (_t _q_method = &ModbusManager::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *ModbusManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ModbusManager::readCompleted(const ModbusReadResult & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModbusManager::writeCompleted(const ModbusWriteResult & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModbusManager::connectionStateChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModbusManager::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP

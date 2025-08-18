/****************************************************************************
** Meta object code from reading C++ file 'modbusmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/modbusmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbusmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ModbusManager_t {
    QByteArrayData data[20];
    char stringdata0[279];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModbusManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModbusManager_t qt_meta_stringdata_ModbusManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ModbusManager"
QT_MOC_LITERAL(1, 14, 13), // "readCompleted"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 16), // "ModbusReadResult"
QT_MOC_LITERAL(4, 46, 6), // "result"
QT_MOC_LITERAL(5, 53, 14), // "writeCompleted"
QT_MOC_LITERAL(6, 68, 17), // "ModbusWriteResult"
QT_MOC_LITERAL(7, 86, 22), // "connectionStateChanged"
QT_MOC_LITERAL(8, 109, 9), // "connected"
QT_MOC_LITERAL(9, 119, 13), // "errorOccurred"
QT_MOC_LITERAL(10, 133, 5), // "error"
QT_MOC_LITERAL(11, 139, 11), // "onReadReady"
QT_MOC_LITERAL(12, 151, 12), // "onWriteReady"
QT_MOC_LITERAL(13, 164, 14), // "onStateChanged"
QT_MOC_LITERAL(14, 179, 20), // "QModbusDevice::State"
QT_MOC_LITERAL(15, 200, 5), // "state"
QT_MOC_LITERAL(16, 206, 15), // "onErrorOccurred"
QT_MOC_LITERAL(17, 222, 20), // "QModbusDevice::Error"
QT_MOC_LITERAL(18, 243, 18), // "processNextRequest"
QT_MOC_LITERAL(19, 262, 16) // "onRequestTimeout"

    },
    "ModbusManager\0readCompleted\0\0"
    "ModbusReadResult\0result\0writeCompleted\0"
    "ModbusWriteResult\0connectionStateChanged\0"
    "connected\0errorOccurred\0error\0onReadReady\0"
    "onWriteReady\0onStateChanged\0"
    "QModbusDevice::State\0state\0onErrorOccurred\0"
    "QModbusDevice::Error\0processNextRequest\0"
    "onRequestTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModbusManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       5,    1,   67,    2, 0x06 /* Public */,
       7,    1,   70,    2, 0x06 /* Public */,
       9,    1,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   76,    2, 0x08 /* Private */,
      12,    0,   77,    2, 0x08 /* Private */,
      13,    1,   78,    2, 0x08 /* Private */,
      16,    1,   81,    2, 0x08 /* Private */,
      18,    0,   84,    2, 0x08 /* Private */,
      19,    0,   85,    2, 0x08 /* Private */,

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
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ModbusManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->readCompleted((*reinterpret_cast< const ModbusReadResult(*)>(_a[1]))); break;
        case 1: _t->writeCompleted((*reinterpret_cast< const ModbusWriteResult(*)>(_a[1]))); break;
        case 2: _t->connectionStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onReadReady(); break;
        case 5: _t->onWriteReady(); break;
        case 6: _t->onStateChanged((*reinterpret_cast< QModbusDevice::State(*)>(_a[1]))); break;
        case 7: _t->onErrorOccurred((*reinterpret_cast< QModbusDevice::Error(*)>(_a[1]))); break;
        case 8: _t->processNextRequest(); break;
        case 9: _t->onRequestTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusManager::*)(const ModbusReadResult & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusManager::readCompleted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(const ModbusWriteResult & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusManager::writeCompleted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusManager::connectionStateChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusManager::errorOccurred)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ModbusManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ModbusManager.data,
    qt_meta_data_ModbusManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ModbusManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ModbusManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
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
QT_END_MOC_NAMESPACE

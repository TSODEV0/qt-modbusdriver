/****************************************************************************
** Meta object code from reading C++ file 'modbus_bool_test_module.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/modbus_bool_test_module.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbus_bool_test_module.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS = QtMocHelpers::stringData(
    "ModbusBoolTestModule",
    "testStarted",
    "",
    "testCompleted",
    "testProgress",
    "current",
    "total",
    "addressTestCompleted",
    "DiscreteInputTestResult",
    "result",
    "errorOccurred",
    "error",
    "onModbusReadCompleted",
    "ModbusReadResult",
    "onModbusError",
    "onConnectionStateChanged",
    "connected",
    "onTestTimeout",
    "processNextAddress"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS_t {
    uint offsetsAndSizes[38];
    char stringdata0[21];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[13];
    char stringdata5[8];
    char stringdata6[6];
    char stringdata7[21];
    char stringdata8[24];
    char stringdata9[7];
    char stringdata10[14];
    char stringdata11[6];
    char stringdata12[22];
    char stringdata13[17];
    char stringdata14[14];
    char stringdata15[25];
    char stringdata16[10];
    char stringdata17[14];
    char stringdata18[19];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS_t qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS = {
    {
        QT_MOC_LITERAL(0, 20),  // "ModbusBoolTestModule"
        QT_MOC_LITERAL(21, 11),  // "testStarted"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 13),  // "testCompleted"
        QT_MOC_LITERAL(48, 12),  // "testProgress"
        QT_MOC_LITERAL(61, 7),  // "current"
        QT_MOC_LITERAL(69, 5),  // "total"
        QT_MOC_LITERAL(75, 20),  // "addressTestCompleted"
        QT_MOC_LITERAL(96, 23),  // "DiscreteInputTestResult"
        QT_MOC_LITERAL(120, 6),  // "result"
        QT_MOC_LITERAL(127, 13),  // "errorOccurred"
        QT_MOC_LITERAL(141, 5),  // "error"
        QT_MOC_LITERAL(147, 21),  // "onModbusReadCompleted"
        QT_MOC_LITERAL(169, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(186, 13),  // "onModbusError"
        QT_MOC_LITERAL(200, 24),  // "onConnectionStateChanged"
        QT_MOC_LITERAL(225, 9),  // "connected"
        QT_MOC_LITERAL(235, 13),  // "onTestTimeout"
        QT_MOC_LITERAL(249, 18)   // "processNextAddress"
    },
    "ModbusBoolTestModule",
    "testStarted",
    "",
    "testCompleted",
    "testProgress",
    "current",
    "total",
    "addressTestCompleted",
    "DiscreteInputTestResult",
    "result",
    "errorOccurred",
    "error",
    "onModbusReadCompleted",
    "ModbusReadResult",
    "onModbusError",
    "onConnectionStateChanged",
    "connected",
    "onTestTimeout",
    "processNextAddress"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusBoolTestModuleENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    1 /* Public */,
       3,    0,   75,    2, 0x06,    2 /* Public */,
       4,    2,   76,    2, 0x06,    3 /* Public */,
       7,    1,   81,    2, 0x06,    6 /* Public */,
      10,    1,   84,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    1,   87,    2, 0x08,   10 /* Private */,
      14,    1,   90,    2, 0x08,   12 /* Private */,
      15,    1,   93,    2, 0x08,   14 /* Private */,
      17,    0,   96,    2, 0x08,   16 /* Private */,
      18,    0,   97,    2, 0x08,   17 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 13,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusBoolTestModule::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusBoolTestModuleENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusBoolTestModule, std::true_type>,
        // method 'testStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'addressTestCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DiscreteInputTestResult &, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onModbusReadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'onModbusError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onTestTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'processNextAddress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ModbusBoolTestModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusBoolTestModule *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->testStarted(); break;
        case 1: _t->testCompleted(); break;
        case 2: _t->testProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->addressTestCompleted((*reinterpret_cast< std::add_pointer_t<DiscreteInputTestResult>>(_a[1]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onModbusReadCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 6: _t->onModbusError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->onTestTimeout(); break;
        case 9: _t->processNextAddress(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusBoolTestModule::*)();
            if (_t _q_method = &ModbusBoolTestModule::testStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusBoolTestModule::*)();
            if (_t _q_method = &ModbusBoolTestModule::testCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusBoolTestModule::*)(int , int );
            if (_t _q_method = &ModbusBoolTestModule::testProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusBoolTestModule::*)(const DiscreteInputTestResult & );
            if (_t _q_method = &ModbusBoolTestModule::addressTestCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ModbusBoolTestModule::*)(const QString & );
            if (_t _q_method = &ModbusBoolTestModule::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *ModbusBoolTestModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusBoolTestModule::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusBoolTestModuleENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusBoolTestModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ModbusBoolTestModule::testStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModbusBoolTestModule::testCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusBoolTestModule::testProgress(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModbusBoolTestModule::addressTestCompleted(const DiscreteInputTestResult & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ModbusBoolTestModule::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP

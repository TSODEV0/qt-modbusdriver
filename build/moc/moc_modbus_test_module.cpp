/****************************************************************************
** Meta object code from reading C++ file 'modbus_test_module.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/modbus_test_module.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbus_test_module.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusTestModuleENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusTestModuleENDCLASS = QtMocHelpers::stringData(
    "ModbusTestModule",
    "connectionStateChanged",
    "",
    "connected",
    "testStarted",
    "testCompleted",
    "QList<ModbusTestResult>",
    "results",
    "testProgress",
    "address",
    "success",
    "message",
    "errorOccurred",
    "error",
    "onModbusStateChanged",
    "QModbusDevice::State",
    "state",
    "performNextTest",
    "testDiscreteInputAddress",
    "oneBasedAddress",
    "readZeroBasedAddress",
    "ModbusTestResult",
    "result"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusTestModuleENDCLASS_t {
    uint offsetsAndSizes[46];
    char stringdata0[17];
    char stringdata1[23];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[12];
    char stringdata5[14];
    char stringdata6[24];
    char stringdata7[8];
    char stringdata8[13];
    char stringdata9[8];
    char stringdata10[8];
    char stringdata11[8];
    char stringdata12[14];
    char stringdata13[6];
    char stringdata14[21];
    char stringdata15[21];
    char stringdata16[6];
    char stringdata17[16];
    char stringdata18[25];
    char stringdata19[16];
    char stringdata20[21];
    char stringdata21[17];
    char stringdata22[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusTestModuleENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusTestModuleENDCLASS_t qt_meta_stringdata_CLASSModbusTestModuleENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "ModbusTestModule"
        QT_MOC_LITERAL(17, 22),  // "connectionStateChanged"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 9),  // "connected"
        QT_MOC_LITERAL(51, 11),  // "testStarted"
        QT_MOC_LITERAL(63, 13),  // "testCompleted"
        QT_MOC_LITERAL(77, 23),  // "QList<ModbusTestResult>"
        QT_MOC_LITERAL(101, 7),  // "results"
        QT_MOC_LITERAL(109, 12),  // "testProgress"
        QT_MOC_LITERAL(122, 7),  // "address"
        QT_MOC_LITERAL(130, 7),  // "success"
        QT_MOC_LITERAL(138, 7),  // "message"
        QT_MOC_LITERAL(146, 13),  // "errorOccurred"
        QT_MOC_LITERAL(160, 5),  // "error"
        QT_MOC_LITERAL(166, 20),  // "onModbusStateChanged"
        QT_MOC_LITERAL(187, 20),  // "QModbusDevice::State"
        QT_MOC_LITERAL(208, 5),  // "state"
        QT_MOC_LITERAL(214, 15),  // "performNextTest"
        QT_MOC_LITERAL(230, 24),  // "testDiscreteInputAddress"
        QT_MOC_LITERAL(255, 15),  // "oneBasedAddress"
        QT_MOC_LITERAL(271, 20),  // "readZeroBasedAddress"
        QT_MOC_LITERAL(292, 16),  // "ModbusTestResult"
        QT_MOC_LITERAL(309, 6)   // "result"
    },
    "ModbusTestModule",
    "connectionStateChanged",
    "",
    "connected",
    "testStarted",
    "testCompleted",
    "QList<ModbusTestResult>",
    "results",
    "testProgress",
    "address",
    "success",
    "message",
    "errorOccurred",
    "error",
    "onModbusStateChanged",
    "QModbusDevice::State",
    "state",
    "performNextTest",
    "testDiscreteInputAddress",
    "oneBasedAddress",
    "readZeroBasedAddress",
    "ModbusTestResult",
    "result"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusTestModuleENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       4,    0,   71,    2, 0x06,    3 /* Public */,
       5,    1,   72,    2, 0x06,    4 /* Public */,
       8,    3,   75,    2, 0x06,    6 /* Public */,
      12,    1,   82,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    1,   85,    2, 0x08,   12 /* Private */,
      17,    0,   88,    2, 0x08,   14 /* Private */,
      18,    1,   89,    2, 0x08,   15 /* Private */,
      20,    1,   92,    2, 0x08,   17 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::QString,    9,   10,   11,
    QMetaType::Void, QMetaType::QString,   13,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, 0x80000000 | 21,   22,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusTestModule::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusTestModuleENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusTestModuleENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusTestModuleENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusTestModule, std::true_type>,
        // method 'connectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'testStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<ModbusTestResult> &, std::false_type>,
        // method 'testProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onModbusStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModbusDevice::State, std::false_type>,
        // method 'performNextTest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'testDiscreteInputAddress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'readZeroBasedAddress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ModbusTestResult, std::false_type>
    >,
    nullptr
} };

void ModbusTestModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusTestModule *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->testStarted(); break;
        case 2: _t->testCompleted((*reinterpret_cast< std::add_pointer_t<QList<ModbusTestResult>>>(_a[1]))); break;
        case 3: _t->testProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onModbusStateChanged((*reinterpret_cast< std::add_pointer_t<QModbusDevice::State>>(_a[1]))); break;
        case 6: _t->performNextTest(); break;
        case 7: _t->testDiscreteInputAddress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->readZeroBasedAddress((*reinterpret_cast< std::add_pointer_t<ModbusTestResult>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusTestModule::*)(bool );
            if (_t _q_method = &ModbusTestModule::connectionStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusTestModule::*)();
            if (_t _q_method = &ModbusTestModule::testStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusTestModule::*)(const QVector<ModbusTestResult> & );
            if (_t _q_method = &ModbusTestModule::testCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusTestModule::*)(int , bool , const QString & );
            if (_t _q_method = &ModbusTestModule::testProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ModbusTestModule::*)(const QString & );
            if (_t _q_method = &ModbusTestModule::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *ModbusTestModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusTestModule::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusTestModuleENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusTestModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ModbusTestModule::connectionStateChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModbusTestModule::testStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusTestModule::testCompleted(const QVector<ModbusTestResult> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModbusTestModule::testProgress(int _t1, bool _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ModbusTestModule::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP

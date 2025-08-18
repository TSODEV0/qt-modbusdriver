/****************************************************************************
** Meta object code from reading C++ file 'modbus_bool_test_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/modbus_bool_test_window.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbus_bool_test_window.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS = QtMocHelpers::stringData(
    "ModbusBoolTestWindow",
    "onStartTest",
    "",
    "onStopTest",
    "onConnectModbus",
    "onDisconnectModbus",
    "onTestCompleted",
    "QList<ModbusTestResult>",
    "results",
    "onTestProgress",
    "address",
    "success",
    "message",
    "onConnectionStateChanged",
    "connected",
    "onAutoTestTimer"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS_t {
    uint offsetsAndSizes[32];
    char stringdata0[21];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[16];
    char stringdata5[19];
    char stringdata6[16];
    char stringdata7[24];
    char stringdata8[8];
    char stringdata9[15];
    char stringdata10[8];
    char stringdata11[8];
    char stringdata12[8];
    char stringdata13[25];
    char stringdata14[10];
    char stringdata15[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS_t qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 20),  // "ModbusBoolTestWindow"
        QT_MOC_LITERAL(21, 11),  // "onStartTest"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 10),  // "onStopTest"
        QT_MOC_LITERAL(45, 15),  // "onConnectModbus"
        QT_MOC_LITERAL(61, 18),  // "onDisconnectModbus"
        QT_MOC_LITERAL(80, 15),  // "onTestCompleted"
        QT_MOC_LITERAL(96, 23),  // "QList<ModbusTestResult>"
        QT_MOC_LITERAL(120, 7),  // "results"
        QT_MOC_LITERAL(128, 14),  // "onTestProgress"
        QT_MOC_LITERAL(143, 7),  // "address"
        QT_MOC_LITERAL(151, 7),  // "success"
        QT_MOC_LITERAL(159, 7),  // "message"
        QT_MOC_LITERAL(167, 24),  // "onConnectionStateChanged"
        QT_MOC_LITERAL(192, 9),  // "connected"
        QT_MOC_LITERAL(202, 15)   // "onAutoTestTimer"
    },
    "ModbusBoolTestWindow",
    "onStartTest",
    "",
    "onStopTest",
    "onConnectModbus",
    "onDisconnectModbus",
    "onTestCompleted",
    "QList<ModbusTestResult>",
    "results",
    "onTestProgress",
    "address",
    "success",
    "message",
    "onConnectionStateChanged",
    "connected",
    "onAutoTestTimer"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSModbusBoolTestWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    0,   63,    2, 0x08,    2 /* Private */,
       4,    0,   64,    2, 0x08,    3 /* Private */,
       5,    0,   65,    2, 0x08,    4 /* Private */,
       6,    1,   66,    2, 0x08,    5 /* Private */,
       9,    3,   69,    2, 0x08,    7 /* Private */,
      13,    1,   76,    2, 0x08,   11 /* Private */,
      15,    0,   79,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::QString,   10,   11,   12,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ModbusBoolTestWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSModbusBoolTestWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ModbusBoolTestWindow, std::true_type>,
        // method 'onStartTest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStopTest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnectModbus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnectModbus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTestCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<ModbusTestResult> &, std::false_type>,
        // method 'onTestProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onAutoTestTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ModbusBoolTestWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusBoolTestWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onStartTest(); break;
        case 1: _t->onStopTest(); break;
        case 2: _t->onConnectModbus(); break;
        case 3: _t->onDisconnectModbus(); break;
        case 4: _t->onTestCompleted((*reinterpret_cast< std::add_pointer_t<QList<ModbusTestResult>>>(_a[1]))); break;
        case 5: _t->onTestProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->onConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->onAutoTestTimer(); break;
        default: ;
        }
    }
}

const QMetaObject *ModbusBoolTestWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusBoolTestWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSModbusBoolTestWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ModbusBoolTestWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP

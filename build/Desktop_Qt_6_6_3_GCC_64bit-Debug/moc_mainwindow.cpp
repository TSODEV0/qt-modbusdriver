/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "connectToServer",
    "",
    "disconnectFromServer",
    "onConnectionStateChanged",
    "connected",
    "onReadCompleted",
    "ModbusReadResult",
    "result",
    "onWriteCompleted",
    "ModbusWriteResult",
    "onErrorOccurred",
    "error",
    "readSingleRegister",
    "readMultipleRegisters",
    "readSingleCoil",
    "readMultipleCoils",
    "readFloat32",
    "readDouble64",
    "readLong32",
    "readLong64",
    "readFloat32Array",
    "readDouble64Array",
    "writeSingleRegister",
    "writeMultipleRegisters",
    "writeSingleCoil",
    "writeMultipleCoils",
    "writeFloat32",
    "writeDouble64",
    "writeLong32",
    "writeLong64",
    "clearLog",
    "updateConnectionStatus"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[66];
    char stringdata0[11];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[21];
    char stringdata4[25];
    char stringdata5[10];
    char stringdata6[16];
    char stringdata7[17];
    char stringdata8[7];
    char stringdata9[17];
    char stringdata10[18];
    char stringdata11[16];
    char stringdata12[6];
    char stringdata13[19];
    char stringdata14[22];
    char stringdata15[15];
    char stringdata16[18];
    char stringdata17[12];
    char stringdata18[13];
    char stringdata19[11];
    char stringdata20[11];
    char stringdata21[17];
    char stringdata22[18];
    char stringdata23[20];
    char stringdata24[23];
    char stringdata25[16];
    char stringdata26[19];
    char stringdata27[13];
    char stringdata28[14];
    char stringdata29[12];
    char stringdata30[12];
    char stringdata31[9];
    char stringdata32[23];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 15),  // "connectToServer"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 20),  // "disconnectFromServer"
        QT_MOC_LITERAL(49, 24),  // "onConnectionStateChanged"
        QT_MOC_LITERAL(74, 9),  // "connected"
        QT_MOC_LITERAL(84, 15),  // "onReadCompleted"
        QT_MOC_LITERAL(100, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(117, 6),  // "result"
        QT_MOC_LITERAL(124, 16),  // "onWriteCompleted"
        QT_MOC_LITERAL(141, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(159, 15),  // "onErrorOccurred"
        QT_MOC_LITERAL(175, 5),  // "error"
        QT_MOC_LITERAL(181, 18),  // "readSingleRegister"
        QT_MOC_LITERAL(200, 21),  // "readMultipleRegisters"
        QT_MOC_LITERAL(222, 14),  // "readSingleCoil"
        QT_MOC_LITERAL(237, 17),  // "readMultipleCoils"
        QT_MOC_LITERAL(255, 11),  // "readFloat32"
        QT_MOC_LITERAL(267, 12),  // "readDouble64"
        QT_MOC_LITERAL(280, 10),  // "readLong32"
        QT_MOC_LITERAL(291, 10),  // "readLong64"
        QT_MOC_LITERAL(302, 16),  // "readFloat32Array"
        QT_MOC_LITERAL(319, 17),  // "readDouble64Array"
        QT_MOC_LITERAL(337, 19),  // "writeSingleRegister"
        QT_MOC_LITERAL(357, 22),  // "writeMultipleRegisters"
        QT_MOC_LITERAL(380, 15),  // "writeSingleCoil"
        QT_MOC_LITERAL(396, 18),  // "writeMultipleCoils"
        QT_MOC_LITERAL(415, 12),  // "writeFloat32"
        QT_MOC_LITERAL(428, 13),  // "writeDouble64"
        QT_MOC_LITERAL(442, 11),  // "writeLong32"
        QT_MOC_LITERAL(454, 11),  // "writeLong64"
        QT_MOC_LITERAL(466, 8),  // "clearLog"
        QT_MOC_LITERAL(475, 22)   // "updateConnectionStatus"
    },
    "MainWindow",
    "connectToServer",
    "",
    "disconnectFromServer",
    "onConnectionStateChanged",
    "connected",
    "onReadCompleted",
    "ModbusReadResult",
    "result",
    "onWriteCompleted",
    "ModbusWriteResult",
    "onErrorOccurred",
    "error",
    "readSingleRegister",
    "readMultipleRegisters",
    "readSingleCoil",
    "readMultipleCoils",
    "readFloat32",
    "readDouble64",
    "readLong32",
    "readLong64",
    "readFloat32Array",
    "readDouble64Array",
    "writeSingleRegister",
    "writeMultipleRegisters",
    "writeSingleCoil",
    "writeMultipleCoils",
    "writeFloat32",
    "writeDouble64",
    "writeLong32",
    "writeLong64",
    "clearLog",
    "updateConnectionStatus"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  170,    2, 0x08,    1 /* Private */,
       3,    0,  171,    2, 0x08,    2 /* Private */,
       4,    1,  172,    2, 0x08,    3 /* Private */,
       6,    1,  175,    2, 0x08,    5 /* Private */,
       9,    1,  178,    2, 0x08,    7 /* Private */,
      11,    1,  181,    2, 0x08,    9 /* Private */,
      13,    0,  184,    2, 0x08,   11 /* Private */,
      14,    0,  185,    2, 0x08,   12 /* Private */,
      15,    0,  186,    2, 0x08,   13 /* Private */,
      16,    0,  187,    2, 0x08,   14 /* Private */,
      17,    0,  188,    2, 0x08,   15 /* Private */,
      18,    0,  189,    2, 0x08,   16 /* Private */,
      19,    0,  190,    2, 0x08,   17 /* Private */,
      20,    0,  191,    2, 0x08,   18 /* Private */,
      21,    0,  192,    2, 0x08,   19 /* Private */,
      22,    0,  193,    2, 0x08,   20 /* Private */,
      23,    0,  194,    2, 0x08,   21 /* Private */,
      24,    0,  195,    2, 0x08,   22 /* Private */,
      25,    0,  196,    2, 0x08,   23 /* Private */,
      26,    0,  197,    2, 0x08,   24 /* Private */,
      27,    0,  198,    2, 0x08,   25 /* Private */,
      28,    0,  199,    2, 0x08,   26 /* Private */,
      29,    0,  200,    2, 0x08,   27 /* Private */,
      30,    0,  201,    2, 0x08,   28 /* Private */,
      31,    0,  202,    2, 0x08,   29 /* Private */,
      32,    0,  203,    2, 0x08,   30 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10,    8,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'connectToServer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnectFromServer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onReadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'onWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>,
        // method 'onErrorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'readSingleRegister'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readMultipleRegisters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readSingleCoil'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readMultipleCoils'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readFloat32'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readDouble64'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readLong32'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readLong64'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readFloat32Array'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readDouble64Array'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeSingleRegister'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeMultipleRegisters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeSingleCoil'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeMultipleCoils'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeFloat32'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeDouble64'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeLong32'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'writeLong64'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearLog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateConnectionStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectToServer(); break;
        case 1: _t->disconnectFromServer(); break;
        case 2: _t->onConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->onReadCompleted((*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[1]))); break;
        case 4: _t->onWriteCompleted((*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[1]))); break;
        case 5: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->readSingleRegister(); break;
        case 7: _t->readMultipleRegisters(); break;
        case 8: _t->readSingleCoil(); break;
        case 9: _t->readMultipleCoils(); break;
        case 10: _t->readFloat32(); break;
        case 11: _t->readDouble64(); break;
        case 12: _t->readLong32(); break;
        case 13: _t->readLong64(); break;
        case 14: _t->readFloat32Array(); break;
        case 15: _t->readDouble64Array(); break;
        case 16: _t->writeSingleRegister(); break;
        case 17: _t->writeMultipleRegisters(); break;
        case 18: _t->writeSingleCoil(); break;
        case 19: _t->writeMultipleCoils(); break;
        case 20: _t->writeFloat32(); break;
        case 21: _t->writeDouble64(); break;
        case 22: _t->writeLong32(); break;
        case 23: _t->writeLong64(); break;
        case 24: _t->clearLog(); break;
        case 25: _t->updateConnectionStatus(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 26;
    }
    return _id;
}
QT_WARNING_POP

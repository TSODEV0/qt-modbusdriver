/****************************************************************************
** Meta object code from reading C++ file 'data_processing_task.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/data_processing_task.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'data_processing_task.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS = QtMocHelpers::stringData(
    "DataProcessingTask",
    "dataProcessingCompleted",
    "",
    "requestId",
    "AcquiredDataPoint",
    "dataPoint",
    "deviceKey",
    "dataProcessingFailed",
    "errorMessage"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS_t {
    uint offsetsAndSizes[18];
    char stringdata0[19];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[18];
    char stringdata5[10];
    char stringdata6[10];
    char stringdata7[21];
    char stringdata8[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS_t qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS = {
    {
        QT_MOC_LITERAL(0, 18),  // "DataProcessingTask"
        QT_MOC_LITERAL(19, 23),  // "dataProcessingCompleted"
        QT_MOC_LITERAL(43, 0),  // ""
        QT_MOC_LITERAL(44, 9),  // "requestId"
        QT_MOC_LITERAL(54, 17),  // "AcquiredDataPoint"
        QT_MOC_LITERAL(72, 9),  // "dataPoint"
        QT_MOC_LITERAL(82, 9),  // "deviceKey"
        QT_MOC_LITERAL(92, 20),  // "dataProcessingFailed"
        QT_MOC_LITERAL(113, 12)   // "errorMessage"
    },
    "DataProcessingTask",
    "dataProcessingCompleted",
    "",
    "requestId",
    "AcquiredDataPoint",
    "dataPoint",
    "deviceKey",
    "dataProcessingFailed",
    "errorMessage"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDataProcessingTaskENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   26,    2, 0x06,    1 /* Public */,
       7,    3,   33,    2, 0x06,    5 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 4, QMetaType::QString,    3,    5,    6,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::QString,    3,    8,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject DataProcessingTask::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDataProcessingTaskENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DataProcessingTask, std::true_type>,
        // method 'dataProcessingCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AcquiredDataPoint &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'dataProcessingFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void DataProcessingTask::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataProcessingTask *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataProcessingCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AcquiredDataPoint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->dataProcessingFailed((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataProcessingTask::*)(qint64 , const AcquiredDataPoint & , const QString & );
            if (_t _q_method = &DataProcessingTask::dataProcessingCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataProcessingTask::*)(qint64 , const QString & , const QString & );
            if (_t _q_method = &DataProcessingTask::dataProcessingFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *DataProcessingTask::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataProcessingTask::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDataProcessingTaskENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QRunnable"))
        return static_cast< QRunnable*>(this);
    return QObject::qt_metacast(_clname);
}

int DataProcessingTask::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DataProcessingTask::dataProcessingCompleted(qint64 _t1, const AcquiredDataPoint & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataProcessingTask::dataProcessingFailed(qint64 _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS = QtMocHelpers::stringData(
    "ParallelDataProcessor",
    "taskCompleted",
    "",
    "requestId",
    "AcquiredDataPoint",
    "dataPoint",
    "deviceKey",
    "taskFailed",
    "errorMessage",
    "onTaskCompleted",
    "onTaskFailed"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[22];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[18];
    char stringdata5[10];
    char stringdata6[10];
    char stringdata7[11];
    char stringdata8[13];
    char stringdata9[16];
    char stringdata10[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS_t qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS = {
    {
        QT_MOC_LITERAL(0, 21),  // "ParallelDataProcessor"
        QT_MOC_LITERAL(22, 13),  // "taskCompleted"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 9),  // "requestId"
        QT_MOC_LITERAL(47, 17),  // "AcquiredDataPoint"
        QT_MOC_LITERAL(65, 9),  // "dataPoint"
        QT_MOC_LITERAL(75, 9),  // "deviceKey"
        QT_MOC_LITERAL(85, 10),  // "taskFailed"
        QT_MOC_LITERAL(96, 12),  // "errorMessage"
        QT_MOC_LITERAL(109, 15),  // "onTaskCompleted"
        QT_MOC_LITERAL(125, 12)   // "onTaskFailed"
    },
    "ParallelDataProcessor",
    "taskCompleted",
    "",
    "requestId",
    "AcquiredDataPoint",
    "dataPoint",
    "deviceKey",
    "taskFailed",
    "errorMessage",
    "onTaskCompleted",
    "onTaskFailed"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSParallelDataProcessorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   38,    2, 0x06,    1 /* Public */,
       7,    3,   45,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    3,   52,    2, 0x08,    9 /* Private */,
      10,    3,   59,    2, 0x08,   13 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 4, QMetaType::QString,    3,    5,    6,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::QString,    3,    8,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 4, QMetaType::QString,    3,    5,    6,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::QString,    3,    8,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject ParallelDataProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSParallelDataProcessorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ParallelDataProcessor, std::true_type>,
        // method 'taskCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AcquiredDataPoint &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'taskFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTaskCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AcquiredDataPoint &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTaskFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ParallelDataProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ParallelDataProcessor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->taskCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AcquiredDataPoint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->taskFailed((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->onTaskCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AcquiredDataPoint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 3: _t->onTaskFailed((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ParallelDataProcessor::*)(qint64 , const AcquiredDataPoint & , const QString & );
            if (_t _q_method = &ParallelDataProcessor::taskCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ParallelDataProcessor::*)(qint64 , const QString & , const QString & );
            if (_t _q_method = &ParallelDataProcessor::taskFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *ParallelDataProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ParallelDataProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSParallelDataProcessorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ParallelDataProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ParallelDataProcessor::taskCompleted(qint64 _t1, const AcquiredDataPoint & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ParallelDataProcessor::taskFailed(qint64 _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP

/****************************************************************************
** Meta object code from reading C++ file 'scada_core_service.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/scada_core_service.h"
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
    "workerCreated",
    "deviceId",
    "workerRemoved",
    "requestCompleted",
    "requestId",
    "requestInterrupted",
    "reason",
    "writeCompleted",
    "operation",
    "error",
    "errorOccurred",
    "statisticsUpdated",
    "ServiceStatistics",
    "stats",
    "globalStatisticsUpdated",
    "ModbusWorkerManager::GlobalStatistics",
    "onPollTimer",
    "onWorkerReadCompleted",
    "ModbusReadResult",
    "result",
    "onWorkerWriteCompleted",
    "ModbusWriteResult",
    "onWorkerConnectionStateChanged",
    "connected",
    "onWorkerError",
    "onWorkerRequestInterrupted",
    "onWorkerStatisticsUpdated",
    "ModbusWorker::WorkerStatistics",
    "onWorkerCreated",
    "deviceKey",
    "onWorkerRemoved",
    "onGlobalStatisticsUpdated"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSScadaCoreServiceENDCLASS_t {
    uint offsetsAndSizes[84];
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
    char stringdata10[14];
    char stringdata11[9];
    char stringdata12[14];
    char stringdata13[17];
    char stringdata14[10];
    char stringdata15[19];
    char stringdata16[7];
    char stringdata17[15];
    char stringdata18[10];
    char stringdata19[6];
    char stringdata20[14];
    char stringdata21[18];
    char stringdata22[18];
    char stringdata23[6];
    char stringdata24[24];
    char stringdata25[38];
    char stringdata26[12];
    char stringdata27[22];
    char stringdata28[17];
    char stringdata29[7];
    char stringdata30[23];
    char stringdata31[18];
    char stringdata32[31];
    char stringdata33[10];
    char stringdata34[14];
    char stringdata35[27];
    char stringdata36[26];
    char stringdata37[31];
    char stringdata38[16];
    char stringdata39[10];
    char stringdata40[16];
    char stringdata41[26];
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
        QT_MOC_LITERAL(134, 13),  // "workerCreated"
        QT_MOC_LITERAL(148, 8),  // "deviceId"
        QT_MOC_LITERAL(157, 13),  // "workerRemoved"
        QT_MOC_LITERAL(171, 16),  // "requestCompleted"
        QT_MOC_LITERAL(188, 9),  // "requestId"
        QT_MOC_LITERAL(198, 18),  // "requestInterrupted"
        QT_MOC_LITERAL(217, 6),  // "reason"
        QT_MOC_LITERAL(224, 14),  // "writeCompleted"
        QT_MOC_LITERAL(239, 9),  // "operation"
        QT_MOC_LITERAL(249, 5),  // "error"
        QT_MOC_LITERAL(255, 13),  // "errorOccurred"
        QT_MOC_LITERAL(269, 17),  // "statisticsUpdated"
        QT_MOC_LITERAL(287, 17),  // "ServiceStatistics"
        QT_MOC_LITERAL(305, 5),  // "stats"
        QT_MOC_LITERAL(311, 23),  // "globalStatisticsUpdated"
        QT_MOC_LITERAL(335, 37),  // "ModbusWorkerManager::GlobalSt..."
        QT_MOC_LITERAL(373, 11),  // "onPollTimer"
        QT_MOC_LITERAL(385, 21),  // "onWorkerReadCompleted"
        QT_MOC_LITERAL(407, 16),  // "ModbusReadResult"
        QT_MOC_LITERAL(424, 6),  // "result"
        QT_MOC_LITERAL(431, 22),  // "onWorkerWriteCompleted"
        QT_MOC_LITERAL(454, 17),  // "ModbusWriteResult"
        QT_MOC_LITERAL(472, 30),  // "onWorkerConnectionStateChanged"
        QT_MOC_LITERAL(503, 9),  // "connected"
        QT_MOC_LITERAL(513, 13),  // "onWorkerError"
        QT_MOC_LITERAL(527, 26),  // "onWorkerRequestInterrupted"
        QT_MOC_LITERAL(554, 25),  // "onWorkerStatisticsUpdated"
        QT_MOC_LITERAL(580, 30),  // "ModbusWorker::WorkerStatistics"
        QT_MOC_LITERAL(611, 15),  // "onWorkerCreated"
        QT_MOC_LITERAL(627, 9),  // "deviceKey"
        QT_MOC_LITERAL(637, 15),  // "onWorkerRemoved"
        QT_MOC_LITERAL(653, 25)   // "onGlobalStatisticsUpdated"
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
    "workerCreated",
    "deviceId",
    "workerRemoved",
    "requestCompleted",
    "requestId",
    "requestInterrupted",
    "reason",
    "writeCompleted",
    "operation",
    "error",
    "errorOccurred",
    "statisticsUpdated",
    "ServiceStatistics",
    "stats",
    "globalStatisticsUpdated",
    "ModbusWorkerManager::GlobalStatistics",
    "onPollTimer",
    "onWorkerReadCompleted",
    "ModbusReadResult",
    "result",
    "onWorkerWriteCompleted",
    "ModbusWriteResult",
    "onWorkerConnectionStateChanged",
    "connected",
    "onWorkerError",
    "onWorkerRequestInterrupted",
    "onWorkerStatisticsUpdated",
    "ModbusWorker::WorkerStatistics",
    "onWorkerCreated",
    "deviceKey",
    "onWorkerRemoved",
    "onGlobalStatisticsUpdated"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSScadaCoreServiceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  152,    2, 0x06,    1 /* Public */,
       3,    0,  153,    2, 0x06,    2 /* Public */,
       4,    1,  154,    2, 0x06,    3 /* Public */,
       7,    2,  157,    2, 0x06,    5 /* Public */,
      10,    1,  162,    2, 0x06,    8 /* Public */,
      12,    1,  165,    2, 0x06,   10 /* Public */,
      13,    2,  168,    2, 0x06,   12 /* Public */,
      15,    2,  173,    2, 0x06,   15 /* Public */,
      17,    4,  178,    2, 0x06,   18 /* Public */,
      17,    3,  187,    2, 0x26,   23 /* Public | MethodCloned */,
      20,    1,  194,    2, 0x06,   27 /* Public */,
      21,    1,  197,    2, 0x06,   29 /* Public */,
      24,    1,  200,    2, 0x06,   31 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      26,    0,  203,    2, 0x08,   33 /* Private */,
      27,    2,  204,    2, 0x08,   34 /* Private */,
      30,    2,  209,    2, 0x08,   37 /* Private */,
      32,    2,  214,    2, 0x08,   40 /* Private */,
      34,    2,  219,    2, 0x08,   43 /* Private */,
      35,    2,  224,    2, 0x08,   46 /* Private */,
      36,    2,  229,    2, 0x08,   49 /* Private */,
      38,    1,  234,    2, 0x08,   52 /* Private */,
      40,    1,  237,    2, 0x08,   54 /* Private */,
      41,    1,  240,    2, 0x08,   56 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::LongLong, QMetaType::Bool,   14,    9,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString,   14,   16,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   14,   18,    9,   19,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString, QMetaType::Bool,   14,   18,    9,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void, 0x80000000 | 25,   23,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 28,   14,   29,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 31,   14,   29,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   11,   33,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   19,
    QMetaType::Void, QMetaType::LongLong, QMetaType::QString,   14,   16,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 37,   11,   23,
    QMetaType::Void, QMetaType::QString,   39,
    QMetaType::Void, QMetaType::QString,   39,
    QMetaType::Void, 0x80000000 | 25,   23,

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
        // method 'workerCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'workerRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'requestCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'requestInterrupted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'writeCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'statisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ServiceStatistics &, std::false_type>,
        // method 'globalStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWorkerManager::GlobalStatistics &, std::false_type>,
        // method 'onPollTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onWorkerReadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusReadResult &, std::false_type>,
        // method 'onWorkerWriteCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWriteResult &, std::false_type>,
        // method 'onWorkerConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onWorkerError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkerRequestInterrupted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkerStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWorker::WorkerStatistics &, std::false_type>,
        // method 'onWorkerCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onWorkerRemoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onGlobalStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ModbusWorkerManager::GlobalStatistics &, std::false_type>
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
        case 4: _t->workerCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->workerRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->requestCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->requestInterrupted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 9: _t->writeCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 10: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->statisticsUpdated((*reinterpret_cast< std::add_pointer_t<ServiceStatistics>>(_a[1]))); break;
        case 12: _t->globalStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<ModbusWorkerManager::GlobalStatistics>>(_a[1]))); break;
        case 13: _t->onPollTimer(); break;
        case 14: _t->onWorkerReadCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusReadResult>>(_a[2]))); break;
        case 15: _t->onWorkerWriteCompleted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusWriteResult>>(_a[2]))); break;
        case 16: _t->onWorkerConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 17: _t->onWorkerError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 18: _t->onWorkerRequestInterrupted((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 19: _t->onWorkerStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ModbusWorker::WorkerStatistics>>(_a[2]))); break;
        case 20: _t->onWorkerCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->onWorkerRemoved((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->onGlobalStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<ModbusWorkerManager::GlobalStatistics>>(_a[1]))); break;
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
            using _t = void (ScadaCoreService::*)(const QString & );
            if (_t _q_method = &ScadaCoreService::workerCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & );
            if (_t _q_method = &ScadaCoreService::workerRemoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(qint64 , bool );
            if (_t _q_method = &ScadaCoreService::requestCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(qint64 , const QString & );
            if (_t _q_method = &ScadaCoreService::requestInterrupted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(qint64 , const QString & , bool , const QString & );
            if (_t _q_method = &ScadaCoreService::writeCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const QString & );
            if (_t _q_method = &ScadaCoreService::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const ServiceStatistics & );
            if (_t _q_method = &ScadaCoreService::statisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ScadaCoreService::*)(const ModbusWorkerManager::GlobalStatistics & );
            if (_t _q_method = &ScadaCoreService::globalStatisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
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
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 23;
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
void ScadaCoreService::workerCreated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ScadaCoreService::workerRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ScadaCoreService::requestCompleted(qint64 _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ScadaCoreService::requestInterrupted(qint64 _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ScadaCoreService::writeCompleted(qint64 _t1, const QString & _t2, bool _t3, const QString & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 10
void ScadaCoreService::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ScadaCoreService::statisticsUpdated(const ServiceStatistics & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void ScadaCoreService::globalStatisticsUpdated(const ModbusWorkerManager::GlobalStatistics & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP

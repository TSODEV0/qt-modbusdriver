#ifndef MODBUS_WORKER_MANAGER_H
#define MODBUS_WORKER_MANAGER_H

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QStringList>

// Forward declarations
class ModbusWorker;

// Forward declaration of nested struct
namespace ModbusWorkerTypes {
    struct WorkerStatistics {
        qint64 totalRequests;
        qint64 successfulRequests;
        qint64 failedRequests;
        qint64 interruptedRequests;
        qint64 highPriorityRequests;
        double averageResponseTime;
        qint64 lastActivityTime;
        bool isConnected;
    };
}

class ModbusWorkerManager : public QObject
{
    Q_OBJECT

public:
    struct GlobalStatistics {
        int activeWorkers;
        int connectedDevices;
        qint64 totalRequests;
        qint64 totalSuccessfulRequests;
        qint64 totalFailedRequests;
        qint64 totalInterruptedRequests;
        double globalAverageResponseTime;
        qint64 lastUpdateTime;
    };

    struct WorkerInfo {
        ModbusWorker* worker;
        QThread* thread;
        bool isConnected;
        ModbusWorkerTypes::WorkerStatistics lastStats;
    };

    explicit ModbusWorkerManager(QObject *parent = nullptr);
    ~ModbusWorkerManager();

    // Worker management
    ModbusWorker* getOrCreateWorker(const QString &host, int port, int unitId = 1);
    ModbusWorker* getWorker(const QString &deviceKey) const;
    void removeWorker(const QString &deviceKey);
    void removeAllWorkers();

    // Device management
    QStringList getActiveDevices() const;
    QStringList getConnectedDevices() const;
    int getWorkerCount() const;

    // Statistics
    GlobalStatistics getGlobalStatistics() const;
    void resetGlobalStatistics();

    // Configuration
    void setDefaultPollInterval(int intervalMs);
    void setWorkerPollInterval(const QString &deviceKey, int intervalMs);

    // Load balancing and coordination
    QString getLeastLoadedWorker() const;
    void distributeLoad();
    void setLoadBalancingEnabled(bool enabled);
    bool isLoadBalancingEnabled() const;
    double getWorkerLoad(const QString &deviceKey) const;
    void optimizeWorkerDistribution();

public slots:
    void startAllWorkers();
    void stopAllWorkers();
    void connectAllDevices();
    void disconnectAllDevices();

signals:
    void workerCreated(const QString &deviceKey);
    void workerRemoved(const QString &deviceKey);
    void globalStatisticsUpdated(const GlobalStatistics &stats);

private slots:
    void onWorkerStarted(const QString &deviceKey);
    void onWorkerStopped(const QString &deviceKey);
    void onWorkerConnectionStateChanged(const QString &deviceKey, bool connected);
    void onWorkerStatisticsUpdated(const QString &deviceKey, const ModbusWorkerTypes::WorkerStatistics &stats);
    void updateGlobalStatistics();
    void handleDelayedStartup(const QString &deviceKey);

private:
    // Helper methods
    QString createDeviceKey(const QString &host, int port, int unitId) const;
    void connectWorkerSignals(ModbusWorker* worker);
    void cleanupWorker(const QString &deviceKey);
    void calculateWorkerLoads() const;
    void rebalanceWorkerLoads();

    // Member variables
    QHash<QString, WorkerInfo> m_workers;
    mutable QMutex m_workersMutex;
    int m_defaultPollInterval;
    GlobalStatistics m_globalStats;
    mutable QMutex m_globalStatsMutex;
    QTimer* m_statsUpdateTimer;
    bool m_loadBalancingEnabled;
    QTimer* m_loadBalancingTimer;
    mutable QMutex m_loadBalancingMutex;
    
    // Pending startup tracking
    struct PendingStartup {
        ModbusWorker* worker;
        QString deviceKey;
    };
    QHash<QString, PendingStartup> m_pendingStartups;
    mutable QMutex m_pendingStartupsMutex;
};

#endif // MODBUS_WORKER_MANAGER_H
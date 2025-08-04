#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include "modbusmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectToServer();
    void disconnectFromServer();
    void onConnectionStateChanged(bool connected);
    void onReadCompleted(const ModbusReadResult &result);
    void onWriteCompleted(const ModbusWriteResult &result);
    void onErrorOccurred(const QString &error);
    
    // Read operations
    void readSingleRegister();
    void readMultipleRegisters();
    void readSingleCoil();
    void readMultipleCoils();
    void readFloat32();
    void readDouble64();
    void readLong32();
    void readLong64();
    void readFloat32Array();
    void readDouble64Array();
    
    // Write operations
    void writeSingleRegister();
    void writeMultipleRegisters();
    void writeSingleCoil();
    void writeMultipleCoils();
    void writeFloat32();
    void writeDouble64();
    void writeLong32();
    void writeLong64();
    
    void clearLog();
    void updateConnectionStatus();

private:
    void setupUI();
    void setupConnectionGroup();
    void setupReadGroup(QWidget *readWidget = nullptr);
    void setupWriteGroup(QWidget *writeWidget = nullptr);
    void setupLogGroup();
    void logMessage(const QString &message);
    void updateUIState(bool connected);
    
    Ui::MainWindow *ui;
    ModbusManager *m_modbusManager;
    
    // Connection widgets
    QLineEdit *m_hostEdit;
    QSpinBox *m_portSpinBox;
    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;
    QLabel *m_statusLabel;
    
    // Read operation widgets
    QSpinBox *m_readAddressSpinBox;
    QSpinBox *m_readCountSpinBox;
    QComboBox *m_readDataTypeCombo;
    QPushButton *m_readSingleButton;
    QPushButton *m_readMultipleButton;
    QPushButton *m_readCoilButton;
    QPushButton *m_readMultipleCoilsButton;
    QPushButton *m_readFloat32Button;
    QPushButton *m_readDouble64Button;
    QPushButton *m_readLong32Button;
    QPushButton *m_readLong64Button;
    QPushButton *m_readFloat32ArrayButton;
    QPushButton *m_readDouble64ArrayButton;
    
    // Write operation widgets
    QSpinBox *m_writeAddressSpinBox;
    QSpinBox *m_writeValueSpinBox;
    QDoubleSpinBox *m_writeFloatSpinBox;
    QDoubleSpinBox *m_writeDoubleSpinBox;
    QSpinBox *m_writeLong32SpinBox;
    QLineEdit *m_writeLong64Edit;
    QCheckBox *m_writeCoilCheckBox;
    QLineEdit *m_writeMultipleEdit;
    QPushButton *m_writeSingleButton;
    QPushButton *m_writeMultipleButton;
    QPushButton *m_writeCoilButton;
    QPushButton *m_writeMultipleCoilsButton;
    QPushButton *m_writeFloat32Button;
    QPushButton *m_writeDouble64Button;
    QPushButton *m_writeLong32Button;
    QPushButton *m_writeLong64Button;
    
    // Log widget
    QTextEdit *m_logTextEdit;
    QPushButton *m_clearLogButton;
    
    QTimer *m_statusTimer;
};
#endif // MAINWINDOW_H

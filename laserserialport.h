#ifndef LASERSERIALPORT_H
#define LASERSERIALPORT_H

#include <QMainWindow>
#include <QWidget>
#include <QIntValidator>
#include <QLineEdit>
#include <QDialog>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class LaserSerialPort;
}

class LaserSerialPort : public QMainWindow
{
    Q_OBJECT

public:
    QByteArray getData(){return data;}
    void putData();
    explicit LaserSerialPort(QWidget *parent = 0);
    ~LaserSerialPort();

private:
    Ui::LaserSerialPort *ui;
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    Settings UARTsettings;
    QSerialPort *serial;
    QByteArray data;
    bool serialPortConnected;
    bool laserClosed;
    void fillPortsParameters();
    void fillPortsInfo();  
    void openSerialPort();
    void closeSerialPort();

private slots:
    void showPortInfo(int idx);
    void disconnet2com();
    void connet2com();
    bool writeData();
    void readData();
    void updateUARTSettings();
    void updateSendGroup(int index);
    void shutDownLaser();
};

#endif // LASERSERIALPORT_H

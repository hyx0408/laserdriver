#ifndef LASERSERIALPORT_H
#define LASERSERIALPORT_H

#include <QMainWindow>
#include <QWidget>
#include <QIntValidator>
#include <QLineEdit>
#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QQueue>
#include <QTextStream>
#include <iostream>
#include "wpid.h"
#include "qcustomplot.h"

namespace Ui {
class LaserSerialPort;
}

class wpid;
class test;
class SortDialog;

class LaserSerialPort : public QMainWindow
{
    Q_OBJECT

public:
    QByteArray getData(){return data;}
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
    wpid *piddialog;
    bool serialPortConnected;
    bool boardConnected;
//    QTimer *dataTimer;
//    QQueue<int> *TData;
    void fillPortsParameters();
    void fillPortsInfo();  
    bool openSerialPort();
    void closeSerialPort();
    void initialPlot();
    void processData();
    void dataPlot(int value);


private slots:
    void showPortInfo(int idx);
    void disconnet2com();
    void connet2com();
    bool writeData();
    void readData();
    void updateUARTSettings();
    void shutDownLaser();
    void setPID();
    void openhelpurl();
    void setDefaultValue();
    void setValue();
};

#endif // LASERSERIALPORT_H

#include "laserserialport.h"
#include "ui_laserserialport.h"
#include <QString>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QByteArray>
#include <QObject>

LaserSerialPort::LaserSerialPort(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LaserSerialPort)
{
    ui->setupUi(this);
    laserClosed = false;
    serialPortConnected = 0;
    serial = new QSerialPort(this);
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));
    connect(ui->connectButton, SIGNAL(clicked()),
            this, SLOT(connet2com()));
    connect(ui->disconnectButton, SIGNAL(clicked()),
            this, SLOT(disconnet2com()));
    connect(ui->clearButton, SIGNAL(clicked()),
            ui->textBrowser, SLOT(clear()));
    connect(ui->sendButtonL1, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonL2, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonL3A, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonL3B, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonT1, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonT2, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(serial, SIGNAL(readyRead()),
            this, SLOT(readData()));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUARTSettings()));
    connect(ui->dataBitsBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUARTSettings()));
    connect(ui->parityBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUARTSettings()));
    connect(ui->stopBitsBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUARTSettings()));
    connect(ui->flowControlBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateUARTSettings()));
    connect(ui->shutDownButton,SIGNAL(clicked()),
            this,SLOT(shutDownLaser()));
    ui->textBrowser->insertPlainText(tr("你好,欢迎使用hyx串口助手!\r\n"));
    fillPortsParameters();
    fillPortsInfo();
    updateUARTSettings();
}

void LaserSerialPort::connet2com()
{
    ui->connectButton->setDisabled(true);
    ui->disconnectButton->setEnabled(true);
    ui->selectBox->setDisabled(true);
    ui->parametersBox->setDisabled(true);
    openSerialPort();
}

void LaserSerialPort::disconnet2com()
{
    ui->connectButton->setEnabled(true);
    ui->selectBox->setEnabled(true);
    ui->parametersBox->setEnabled(true);
    ui->disconnectButton->setDisabled(true);
    ui->sendButtonL1->setDisabled(true);
    ui->sendButtonL2->setDisabled(true);
    ui->sendButtonT1->setDisabled(true);
    ui->sendButtonT2->setDisabled(true);
    ui->sendButtonL3A->setDisabled(true);
    ui->sendButtonL3B->setDisabled(true);
    ui->shutDownButton->setDisabled(true);
    closeSerialPort();
}

bool LaserSerialPort::writeData()
{
    QByteArray senddata = QByteArray();
    char command;
    char dacnumber;
    char channel;
    short v;
    char first4bits;
    char last8bits;
    char address = 0;
    command = char(0x60);
    unsigned int ID = sender()->property("userButtonID").toUInt();
    if(ID == 1 || ID == 2)
        dacnumber = char(0x02);
    if(ID == 3 || ID == 4)
        dacnumber = char(0x01);
    if(ID == 5 || ID == 6)
        dacnumber = char(0x04);
    if(ID%2)
        channel = char(0x00);
    else
        channel = char(0x10);
//    command = (ui->commandBox->itemData(ui->commandBox->currentIndex())).toInt();
//    dacnumber = (ui->dacNumberBox->itemData(ui->dacNumberBox->currentIndex())).toInt();
//    channel = (ui->channelBox->itemData(ui->channelBox->currentIndex())).toInt();
    switch(ID){
    case 1:v = ui->voltageSpinBoxL1->value();
        ui->spinBox_1->setValue(v);
        break;
    case 2:v = ui->voltageSpinBoxT1->value();
        ui->spinBox_2->setValue(v);
        break;
    case 3:v = ui->voltageSpinBoxL2->value();
        ui->spinBox_3->setValue(v);
        break;
    case 4:v = ui->voltageSpinBoxT2->value();
        ui->spinBox_4->setValue(v);
        break;
    case 5:v = ui->voltageSpinBoxL3A->value();
        ui->spinBox_5->setValue(v);
        break;
    case 6:v = ui->voltageSpinBoxL3B->value();
        ui->spinBox_6->setValue(v);
        break;
    default:break;
    }
    first4bits = (v>>8)&(short(0x000F));
    last8bits = v&(short(0x00FF));
//    switch (dacnumber|channel) {
//    case char(0x01):
//        address = char(0x10);
//        break;
//    case char(0x11):
//        address = char(0x40);
//        break;
//    case char(0x02):
//        address = char(0x20);
//        break;
//    case char(0x12):
//        address = char(0x50);
//        break;
//    case char(0x04):
//        address = char(0x30);
//        break;
//    case char(0x14):
//        address = char(0x60);
//        break;
//    default:
//        break;
//    }
    switch(command){
    case char(0x60):
        senddata.append(command|dacnumber);
        senddata.append(channel|first4bits);
        senddata.append(last8bits);
        break;
//    case char(0x80):
//        senddata.append(command|char(0x01));
//        senddata.append(address);
//        senddata.append(channel|first4bits);
//        senddata.append(last8bits);
//        break;
//    case char(0x00):
//        senddata.append(command|(channel>>1)|dacnumber);
//        break;
    default:
        break;
    }
    if(!senddata.isEmpty())
        serial->write(senddata);
    return 1;
}

void LaserSerialPort::readData()
{
    data = serial->readAll();
    putData();
}


void LaserSerialPort::fillPortsParameters()
{
    //ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    //ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    //ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    //ui->baudRateBox->setCurrentIndex(1);

    //ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    //ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    //ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    //ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    //ui->parityBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    //ui->parityBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    //ui->parityBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    //ui->parityBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    //ui->stopBitsBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    //ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    //ui->flowControlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    //ui->flowControlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);

    //ui->commandBox->addItem(tr("Set Voltage"),0x60);
    //ui->commandBox->addItem(tr("Set Default Voltage"),0x80);
    //ui->commandBox->addItem(tr("Set To Default Voltage"),0x00);

    //ui->dacNumberBox->addItem(tr("First Stage"),0x02);
    //ui->dacNumberBox->addItem(tr("Sencond Stage"),0x01);
    //ui->dacNumberBox->addItem(tr("Third Stage"),0x04);

//    ui->channelBox->addItem(tr("A Channel"),qint8(0x00));
//    ui->channelBox->addItem(tr("B Channel"),qint8(0x10));
}

void LaserSerialPort::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
}

void LaserSerialPort::showPortInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("描述:%1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("制造商:%1").arg(list.at(2)));
        ui->serialNumberLabel->setText(tr("序列编号:%1").arg(list.at(3)));
        ui->locationLabel->setText(tr("位置:%1").arg(list.at(4)));
        ui->vidLabel->setText(tr("VID:%1").arg(list.at(5)));
        ui->pidLabel->setText(tr("PID:%1").arg(list.at(6)));
    }
}

void LaserSerialPort::updateUARTSettings()
{
    if (ui->baudRateBox->currentIndex() == 4) {
        UARTsettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        UARTsettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    UARTsettings.stringBaudRate = QString::number(UARTsettings.baudRate);

    UARTsettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    UARTsettings.stringDataBits = ui->dataBitsBox->currentText();

    UARTsettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    UARTsettings.stringParity = ui->parityBox->currentText();

    UARTsettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    UARTsettings.stringStopBits = ui->stopBitsBox->currentText();

    UARTsettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    UARTsettings.stringFlowControl = ui->flowControlBox->currentText();
}

void LaserSerialPort::openSerialPort()
{
    serial->setPortName(ui->serialPortInfoListBox->currentText());
    serial->setBaudRate(UARTsettings.baudRate);
    serial->setDataBits(UARTsettings.dataBits);
    serial->setParity(UARTsettings.parity);
    serial->setStopBits(UARTsettings.stopBits);
    serial->setFlowControl(UARTsettings.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        if(laserClosed){
            ui->sendButtonL1->setDisabled(true);
            ui->sendButtonL2->setDisabled(true);
            ui->sendButtonL3A->setDisabled(true);
            ui->sendButtonL3B->setDisabled(true);
            ui->sendButtonT1->setDisabled(true);
            ui->sendButtonT2->setDisabled(true);
        }
        else{
            ui->sendButtonL1->setEnabled(true);
            ui->sendButtonL2->setEnabled(true);
            ui->sendButtonL3A->setEnabled(true);
            ui->sendButtonL3B->setEnabled(true);
            ui->sendButtonT1->setEnabled(true);
            ui->sendButtonT2->setEnabled(true);
        }
        ui->shutDownButton->setEnabled(true);
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }
}

void LaserSerialPort::closeSerialPort()
{
    serial->close();
}

void LaserSerialPort::putData()
{
    char *temp = data.data();
    while(*temp)
    {
        ui->textBrowser->insertPlainText(tr("0b'"));
        char a = *temp;
        QString b = QString();
        for(int i = 0; i<8; i++)
        {
            if(a&0x80)
                b += "1";
            else
                b += "0";
            a <<= 1;
        }
        b.insert(4,",");
        ui->textBrowser->insertPlainText(b);
        temp++;
        ui->textBrowser->insertPlainText(tr("  "));
    }
}

void LaserSerialPort::updateSendGroup(int index)
{
//    switch(index){
//    case 0:
//    case 1:
//        ui->dacNumberBox->setEnabled(true);
//        ui->channelBox->setEnabled(true);
//        ui->voltageSlider->setEnabled(true);
//        ui->voltageSpinBox->setEnabled(true);
//        break;
//    case 2:
//        ui->dacNumberBox->setEnabled(true);
//        ui->channelBox->setEnabled(true);
//        ui->voltageSlider->setDisabled(true);
//        ui->voltageSpinBox->setDisabled(true);
//        break;
//    default:
//        break;
//    }

}

void LaserSerialPort::shutDownLaser()
{
    if(laserClosed)
    {
        QByteArray senddata = QByteArray();
        senddata.append(char(0xcc));
        serial->write(senddata);
        ui->shutDownButton->setText(tr("关断"));
        ui->sendButtonL1->setEnabled(true);
        ui->sendButtonL2->setEnabled(true);
        ui->sendButtonL3A->setEnabled(true);
        ui->sendButtonL3B->setEnabled(true);
        ui->sendButtonT1->setEnabled(true);
        ui->sendButtonT2->setEnabled(true);
    } else {
        QByteArray senddata = QByteArray();
        senddata.append(char(0x33));
        serial->write(senddata);
        ui->shutDownButton->setText(tr("打开"));
        ui->sendButtonL1->setDisabled(true);
        ui->sendButtonL2->setDisabled(true);
        ui->sendButtonL3A->setDisabled(true);
        ui->sendButtonL3B->setDisabled(true);
        ui->sendButtonT1->setDisabled(true);
        ui->sendButtonT2->setDisabled(true);
        ui->spinBox_1->setValue(0);
        ui->spinBox_3->setValue(0);
    }
    laserClosed = !laserClosed;
}

LaserSerialPort::~LaserSerialPort()
{
    delete ui;
}

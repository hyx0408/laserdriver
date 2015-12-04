#include "laserserialport.h"
#include "ui_laserserialport.h"
#include <QString>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QByteArray>
#include <QObject>
#include <QPalette>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

LaserSerialPort::LaserSerialPort(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LaserSerialPort)
{
    ui->setupUi(this);
    serialPortConnected = false;
    boardConnected = false;
    serial = new QSerialPort(this);
    piddialog = new wpid(this);
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
    connect(ui->sendButtonL3, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonT1, SIGNAL(clicked()),
            this, SLOT(writeData()));
    connect(ui->sendButtonT2Open, SIGNAL(toggled(bool)),
            this, SLOT(writeData()));
    connect(piddialog, SIGNAL(setPIDTDefaultValue()),
            this, SLOT(setDefaultValue()));
    connect(piddialog, SIGNAL(setPIDTValue()),
            this, SLOT(setValue()));
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
    connect(ui->setPIDAction, SIGNAL(triggered()),
            this, SLOT(setPID()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(openhelpurl()));
    QString * info = new QString();
    QTextStream infostram(info);
    infostram << "LaserDriver v2.0!" << endl;
    ui->textBrowser->insertPlainText(*info);
    fillPortsParameters();
    fillPortsInfo();
    initialPlot();
    updateUARTSettings();
    //
//    TData = new QQueue<qint16>;
//    dataTimer = new QTimer(this);
//    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
//    dataTimer->start(100);
}

void LaserSerialPort::initialPlot()
{
    //set up ui->customPlot
    ui->customPlot->setBackground(this->palette().color(QPalette::Window));
    ui->customPlot->axisRect()->setBackground(Qt::white);

    ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis2); // blue line
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setAntialiasedFill(false);

    ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis2); // blue dot
    ui->customPlot->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot->xAxis->setDateTimeFormat("mm:ss");
    ui->customPlot->xAxis->setAutoTickStep(false);
    ui->customPlot->xAxis->setTickStep(10);
    ui->customPlot->yAxis->setAutoTickStep(true);
    ui->customPlot->yAxis2->setAutoTickStep(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis->setTickLabels(false);
    //ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->xAxis->setLabel(tr("时间"));
    ui->customPlot->yAxis2->setLabel(tr("温度"));
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setVisible(true);
    // make ticks on bottom axis go outward:
    ui->customPlot->xAxis->setTickLength(0, 5);
    ui->customPlot->xAxis->setSubTickLength(0, 3);
    // make ticks on right axis go inward and outward:
    ui->customPlot->yAxis2->setTickLength(3, 3);
    ui->customPlot->yAxis2->setSubTickLength(1, 1);
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis, SLOT(setRange(QCPRange)));
}

void LaserSerialPort::connet2com()
{
    QByteArray senddata = QByteArray();
    if(openSerialPort()){
        ui->connectButton->setDisabled(true);
        ui->disconnectButton->setEnabled(true);
        ui->selectBox->setDisabled(true);
        ui->parametersBox->setDisabled(true);
        ui->sendButtonL1->setEnabled(true);
        ui->sendButtonL2->setEnabled(true);
        ui->sendButtonT2Close->setEnabled(true);
        ui->sendButtonL3->setEnabled(true);
        ui->sendButtonT1->setEnabled(true);
        ui->sendButtonT2Open->setEnabled(true);
        ui->shutDownButton->setEnabled(true);
        senddata.append(0x80);
        senddata.append(0x18);
        senddata.append(0x38);
        serial->write(senddata);
    }
}

void LaserSerialPort::disconnet2com()
{
    QByteArray senddata = QByteArray();
    if(ui->spinBoxL1->value() == 0 && ui->spinBoxL2->value() == 0 && ui->spinBoxL3->value() == 0){
        ui->connectButton->setEnabled(true);
        ui->selectBox->setEnabled(true);
        ui->parametersBox->setEnabled(true);
        ui->disconnectButton->setDisabled(true);
        ui->sendButtonL1->setDisabled(true);
        ui->sendButtonL2->setDisabled(true);
        ui->sendButtonT1->setDisabled(true);
        ui->sendButtonT2Open->setDisabled(true);
        ui->sendButtonL3->setDisabled(true);
        ui->sendButtonT2Close->setDisabled(true);
        ui->shutDownButton->setDisabled(true);
        senddata.append(0x84);
        serial->write(senddata);
        serial->flush();
        if(!boardConnected)
            closeSerialPort();
    } else {
        QMessageBox::warning(this, tr("Warning!"), tr("Laser Driver is not completely closed!"),QMessageBox::Cancel);
    }
}

bool LaserSerialPort::writeData()
{
    QByteArray senddata = QByteArray();
    int v;
    unsigned int ID = sender()->property("userButtonID").toUInt();
    switch(ID){
    case 2: senddata.append(0x58);
            v = ui->voltageSliderT1->value();
            senddata.append((v>>8)&(0x0F));
            senddata.append(v&(0xFF));
            break;
    case 3: senddata.append(0x50);
            v = ui->voltageSliderL1->value();
            senddata.append((v>>8)&(0x0F));
            senddata.append(v&(0xFF));
            break;
    case 4: senddata.append(0x60);
            v = ui->voltageSliderL2->value();
            senddata.append((v>>8)&(0x0F));
            senddata.append(v&(0xFF));
            break;
    case 5: senddata.append(0x68);
            v = ui->voltageSliderL3->value();
            senddata.append((v>>8)&(0x0F));
            senddata.append(v&(0xFF));
            break;
    case 7: if(ui->sendButtonT2Open->isChecked())
                senddata.append(0x70);
            else
                senddata.append(0x78);
            break;
    default:ui->textBrowser->insertPlainText(tr("Error Command!\r\n"));
            break;
    }
    if(!senddata.isEmpty())
    {
        serial->write(senddata);
        return 1;
    }
    else
    {
        return 0;
    }
}

void LaserSerialPort::setDefaultValue(){
    QByteArray senddata = QByteArray();
    double t;
    if(piddialog->kEnable) {
        senddata.append(0x20);
        senddata.append((piddialog->k1)>>8);
        senddata.append((piddialog->k1)&0xFF);
        senddata.append((piddialog->k2)>>8);
        senddata.append((piddialog->k2)&0xFF);
        senddata.append((piddialog->k3)>>8);
        senddata.append((piddialog->k3)&0xFF);
        senddata.append(0x40);
        t = piddialog->getT();
        double k;
        k = qExp((1/(t+273.15)-2.765e-3)/2.553e-4);
        int v;
        v = (819200-16384*k)/(300+3*k);
        senddata.append((v>>8)&(0x0F));
        senddata.append(v&(0xFF));
     }
    if(!senddata.isEmpty())
    {
        serial->write(senddata);
    }
}

void LaserSerialPort::setValue(){
    QByteArray senddata = QByteArray();
    double t;
    if(piddialog->kEnable) {
        senddata.append(0x10);
        senddata.append((piddialog->k1)>>8);
        senddata.append((piddialog->k1)&0xFF);
        senddata.append((piddialog->k2)>>8);
        senddata.append((piddialog->k2)&0xFF);
        senddata.append((piddialog->k3)>>8);
        senddata.append((piddialog->k3)&0xFF);
        senddata.append(0x30);
        t = piddialog->getT();
        double k;
        k = qExp((1/(t+273.15)-2.765e-3)/2.553e-4);
        int v;
        v = (819200-16384*k)/(300+3*k);
        senddata.append((v>>8)&(0x0F));
        senddata.append(v&(0xFF));
     }
    if(!senddata.isEmpty())
    {
        serial->write(senddata);
    }
}

void LaserSerialPort::readData()
{
    QByteArray temp = serial->readAll();
    if(temp.size() != 0)
    {
        data.append(temp);
        processData();
    }
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

bool LaserSerialPort::openSerialPort()
{
    serial->setPortName(ui->serialPortInfoListBox->currentText());
    serial->setBaudRate(UARTsettings.baudRate);
    serial->setDataBits(UARTsettings.dataBits);
    serial->setParity(UARTsettings.parity);
    serial->setStopBits(UARTsettings.stopBits);
    serial->setFlowControl(UARTsettings.flowControl);
    bool openSuccess = serial->open(QIODevice::ReadWrite);
    if (openSuccess) {
        ui->statusBar->showMessage(tr("Serial port open success."));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Serial port open error!!!"));
    }
    return openSuccess;
}

void LaserSerialPort::closeSerialPort()
{
    serial->close();
    ui->statusBar->showMessage(tr("Serial port close success."));
}

void LaserSerialPort::processData()
{
    int needProcess = data.size();
    while(needProcess){
        switch ((unsigned char) data.at(0)) {
        case 0x18:
            if(needProcess < 7)
                needProcess = 0;
            else{
                int k1 = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                int k2 = -(((unsigned char) (~data.at(3)))*256+((unsigned char) (~data.at(4)))+1);
                int k3 = ((unsigned char) data.at(5))*256+((unsigned char) data.at(6));
                piddialog->showPIDValue(k1,k2,k3);
                data.remove(0,7);
                needProcess = data.size();
            }
            break;
        case 0x28:
            if(needProcess < 7)
                needProcess = 0;
            else{
                int k1 = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                int k2 = -(((unsigned char) (~data.at(3)))*256+((unsigned char) (~data.at(4)))+1);
                int k3 = ((unsigned char) data.at(5))*256+((unsigned char) data.at(6));
                if(piddialog->checkK(k1,k2,k3))
                    piddialog->setTips(tr("Setting default PID value success!\r\n"));
                else
                    piddialog->setTips(tr("Setting default PID value error!!!\r\n"));
                data.remove(0,7);
                needProcess = data.size();
            }
            break;
        case 0x38:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int t = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                piddialog->showTValue(t);
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        case 0x48:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int t = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                if(piddialog->checkT(t))
                    piddialog->setTips(tr("Setting default Temperature value success!\r\n"));
                else
                    piddialog->setTips(tr("Setting default Temperature value error!!!\r\n"));
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        case 0x50:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int v = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                ui->spinBoxL1->setValue(v);
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        case 0x58:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int v = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                ui->spinBoxT1->setValue(v);
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        case 0x60:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int v = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                ui->spinBoxL2->setValue(v);
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        case 0x68:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int v = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                ui->spinBoxL3->setValue(v);
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        case 0x70:
            data.remove(0,1);
            ui->textBrowser->insertPlainText(tr("Temperature controller is endabled!\r\n"));
            needProcess = data.size();
            break;
        case 0x78:
            data.remove(0,1);
            ui->textBrowser->insertPlainText(tr("Temperature controller is disabled!\r\n"));
            needProcess = data.size();
            break;
        case 0x80:
            data.remove(0,1);
            boardConnected = true;
            ui->textBrowser->insertPlainText(tr("System is enabled!\r\n"));
            needProcess = data.size();
            break;
        case 0x84:
            data.remove(0,1);
            if(boardConnected)
                closeSerialPort();
            boardConnected = false;
            ui->textBrowser->insertPlainText(tr("System is disabled!\r\n"));
            needProcess = data.size();
            break;
        case 0x88:
            data.remove(0,1);
            ui->textBrowser->insertPlainText(tr("Laser driver is closed!\r\n"));
            ui->spinBoxL1->setValue(0);
            ui->spinBoxL2->setValue(0);
            ui->spinBoxL3->setValue(0);
            needProcess = data.size();
            break;
        case 0x98:
            data.remove(0,2);
            needProcess = data.size();
            break;
        case 0xc0:
            if(needProcess < 3)
                needProcess = 0;
            else{
                int v = ((unsigned char) data.at(1))*256+((unsigned char) data.at(2));
                dataPlot(v);
                data.remove(0,3);
                needProcess = data.size();
            }
            break;
        default:
            data.remove(0,1);
            ui->textBrowser->insertPlainText(tr("Error Byte!!!\r\n"));
            needProcess = data.size();
            break;
        }
    }
}

void LaserSerialPort::shutDownLaser()
{
    QByteArray senddata = QByteArray();
    senddata.append(0x88);
    serial->write(senddata);
}

void LaserSerialPort::setPID()
{
   if(!piddialog)
       piddialog = new wpid(this);
   piddialog->show();
   piddialog->activateWindow();
}

void LaserSerialPort::dataPlot(int value)
{
    // calculate two new data points:
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    double tvalue;
    tvalue = 1/((2.765e-3)+(2.553e-4)*qLn((double)(819200-300*value)/(16384+3*value)))-273.15;
    // add data to lines:
    ui->lcdNumber->display(tvalue);
    ui->customPlot->graph(0)->addData(key, tvalue);
    // set data of dots:
    ui->customPlot->graph(1)->clearData();
    ui->customPlot->graph(1)->addData(key, tvalue);
    // remove data of lines that's outside visible range:
    ui->customPlot->graph(0)->removeDataBefore(key-60);
    // rescale value (vertical) axis to fit the current data:
    ui->customPlot->graph(0)->rescaleValueAxis();
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key+1, 60, Qt::AlignRight);
    ui->customPlot->replot();
}

void LaserSerialPort::openhelpurl(){
    QDesktopServices::openUrl(QUrl("http://www.baidu.com"));
    QDesktopServices::openUrl(QUrl(QDir::currentPath()+"/1.mp3", QUrl::TolerantMode));
}


LaserSerialPort::~LaserSerialPort()
{
    delete ui;
//    delete TData;
}

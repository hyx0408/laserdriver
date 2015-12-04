#include "wpid.h"
#include "ui_wpid.h"
#include <QtMath>

wpid::wpid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wpid)
{
    ui->setupUi(this);
    k1 = 20080;
    k2 = -20032;
    k3 = 16;
    kEnable = true;
    connect(ui->iBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateTipLabel()));
    connect(ui->pBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateTipLabel()));
    connect(ui->dBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateTipLabel()));
    connect(ui->setButton, SIGNAL(clicked()),
            this, SLOT(setDefaultValue()));
    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(setValue()));
}

wpid::~wpid()
{
    delete ui;
}

bool wpid::setPID(int p, int i, int d)
{
    qint32 tempk1,tempk2,tempk3;
    tempk1 = p + i + d;
    tempk2 = p + 2*d;
    tempk3 = d;
    qint32 uplimiter = qPow(2,16);
    if(tempk1>=uplimiter || tempk2>=uplimiter || tempk3>=uplimiter)
    {
        return false;
    }
    else
    {
        k1 = (int) tempk1;
        k2 = (int) (~tempk2+1);
        k3 = (int) tempk3;
    }
    return true;
}

void wpid::updateTipLabel()
{
    QString tips("");
    qint16 p = ui->pBox->value();
    qint16 i = ui->iBox->value();
    qint16 d = ui->dBox->value();
    if(setPID(p,i,d))
    {
        tips += tr("K1=0x");
        tips += QString::number(k1,16).rightJustified(4,'0').toUpper();
        tips += tr(";  ");
        tips += tr("K2=0x");
        tips += QString::number(k2,16).right(4).toUpper();
        tips += tr(";  ");
        tips += tr("K3=0x");
        tips += QString::number(k3,16).rightJustified(4,'0').toUpper();
        tips += tr(";  ");
        kEnable = true;
    }
    else
    {
        tips += tr("Error PID setting! No effect!");
        kEnable = false;
    }
    ui->tipLable->setText(tips);
}

void wpid::showPIDValue(int a, int b, int c){
    int p,i,d;
    d = c;
    p = -b-2*c;
    i = a-d-p;
    ui->pValueBox->setValue(p);
    ui->dValueBox->setValue(d);
    ui->iValueBox->setValue(i);
}
bool wpid::checkK(int a, int b, int c){
    if(k1 == a && k2 == b && k3 == c)
        return true;
    else
        return false;
}

void wpid::showTValue(int t){
    ui->tValueBox->setValue(t);
}

void wpid::setTips(QString s){
    ui->tipLable->setText(s);
}

bool wpid::checkT(int t){
    return (getT() == t);
}

int wpid::getT()
{
    return ui->tBox->value();
}

void wpid::setDefaultValue(){
    emit setPIDTDefaultValue();
}

void wpid::setValue(){
    emit setPIDTValue();
}

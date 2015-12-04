#ifndef WPID_H
#define WPID_H

#include <QDialog>

namespace Ui {
class wpid;
}

class wpid : public QDialog {
    Q_OBJECT

public:
    wpid(QWidget *parent = 0);
    int k1,k2,k3;
    bool kEnable;
    void showPIDValue(int a, int b, int c);
    void showTValue(int t);
    bool checkK(int a, int b, int c);
    bool checkT(int t);
    void setTips(QString s);
    int getT();
    ~wpid();

private:
    bool setPID(int p, int i, int d);
    Ui::wpid *ui;

signals:
    void setPIDTValue();
    void setPIDTDefaultValue();

public slots:
    void updateTipLabel();
    void setDefaultValue();
    void setValue();
};

#endif // WPID_H

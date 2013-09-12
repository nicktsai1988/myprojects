#ifndef LOOKMESSAGE_H
#define LOOKMESSAGE_H

#include <QDialog>
#include "data.h"
#include "readthread.h"

namespace Ui {
class LookMessage;
}

class LookMessage : public QDialog
{
    Q_OBJECT
    
public:
    explicit LookMessage(const Message &m, QWidget *parent = 0);
    ~LookMessage();
    
private slots:
    void on_closeButton_clicked();

    void on_readButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::LookMessage *ui;

    ReadThread read;    //read用于读信息的线程，注意它不能定义为局部变量，否则会导致过早释放。
    Message my;
};

#endif // LOOKMESSAGE_H

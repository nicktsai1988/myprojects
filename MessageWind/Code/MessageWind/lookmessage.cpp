#include "lookmessage.h"
#include "ui_lookmessage.h"

#include <QMessageBox>

LookMessage::LookMessage(const Message &m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LookMessage)
{
    ui->setupUi(this);
    setWindowTitle(tr("查看信息"));
    setFixedSize(width(),height());
    my = m;

    QString names = numbersToNames(m.number);
    ui->contactLineEdit->setText(names);
    ui->contactLineEdit->setEnabled(false);
    ui->textBrowser->setText(my.content);

    ui->stopButton->setEnabled(false);

}

LookMessage::~LookMessage()
{
    delete ui;
}

void LookMessage::on_closeButton_clicked()
{
    if(read.isFinished())
    {
        read.stop();
        read.wait();
    }
    close();
}

void LookMessage::on_readButton_clicked()
{

    read.setContent(my.content);
    if(read.isRunning())
        QMessageBox::information(this, tr("警告"),tr("正在朗读，请稍后..."));
    else
        read.start();
   ui->stopButton->setEnabled(true);
}

void LookMessage::on_stopButton_clicked()
{
    read.stop();
    read.wait();
    ui->stopButton->setEnabled(false);
}

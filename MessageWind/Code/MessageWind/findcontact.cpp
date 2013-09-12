#include "findcontact.h"
#include "ui_findcontact.h"
#include <QMessageBox>

FindContact::FindContact(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindContact)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("查找"));
    this->setFixedSize(width(), height());
}

FindContact::~FindContact()
{
    delete ui;
}

QString FindContact::findText()
{
    return ui->nameLineEdit->text();
}

void FindContact::on_findButton_clicked()
{
    QString text = ui->nameLineEdit->text();
    if(text.isEmpty())
    {
        QMessageBox::information(this, tr("警告"),tr("请输入姓名！"));
        reject();
    }else{
        accept();
    }
}

void FindContact::on_closeButton_clicked()
{
    close();
}

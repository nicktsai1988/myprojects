#include "setmyinfo.h"
#include "ui_setmyinfo.h"
#include <QMessageBox>

SetMyInfo::SetMyInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetMyInfo)
{
    ui->setupUi(this);
    setWindowTitle(tr("个人信息设置"));
    setFixedSize(width(),height());
    ui->nameLineEdit->setText(myName);
    ui->numberLineEdit->setText(myNumber);
    ui->numberLineEdit->setEnabled(false);
}

SetMyInfo::~SetMyInfo()
{
    delete ui;
}

void SetMyInfo::on_confirmButton_clicked()
{
    myName = ui->nameLineEdit->text();
    QMessageBox::information(this, tr("成功"),tr("个人信息设置成功"));
}

void SetMyInfo::on_exitButton_clicked()
{
    close();
}

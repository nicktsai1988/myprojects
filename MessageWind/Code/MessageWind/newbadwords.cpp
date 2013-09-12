#include "newbadwords.h"
#include "ui_newbadwords.h"
#include <QMessageBox>

NewBadwords::NewBadwords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBadwords)
{
    ui->setupUi(this);
    setWindowTitle(tr("新过滤词"));
    setFixedSize(width(),height());
}

NewBadwords::~NewBadwords()
{
    delete ui;
}

QString NewBadwords::getNewWords()
{
    return ui->lineEdit->text();
}

void NewBadwords::on_confirmButton_clicked()
{
    if(ui->lineEdit->text() == "")
    {
        QMessageBox::information(this, tr("警告"),tr("过滤词不能为空"));
        return;
    }
    close();
}

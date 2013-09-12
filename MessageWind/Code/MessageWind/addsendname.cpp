#include "addsendname.h"
#include "ui_addsendname.h"
#include <QMessageBox>

//str传递已有的默认发送人
AddSendName::AddSendName(QString str, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSendName)
{
    ui->setupUi(this);
    setWindowTitle(tr("添加发送人"));
    if(str != "")
        sendName = str.split(";", QString::SkipEmptyParts);

    for(int i = 0; i < allContacts.size(); i++)
    {
        if(allContacts[i].isWhite)
        {
            if(sendName.contains(allContacts[i].name))
                ui->sendNameListWidget->addItem("*" + allContacts[i].name);
            else
                ui->sendNameListWidget->addItem(allContacts[i].name);
        }
    }
}

AddSendName::~AddSendName()
{
    delete ui;
}

void AddSendName::on_addButton_clicked()
{
    QListWidgetItem * curItem = ui->sendNameListWidget->currentItem();
    if(curItem == NULL)
    {
        QMessageBox::information(this, tr("警告"),tr("没有选中名字"));
        return;
    }
    QString curName = curItem->text();
    if(curName.startsWith('*'))
    {
        QMessageBox::information(this, tr("警告"),tr("%1 已添加！").arg(curName));
        return;
    }
    sendName.append(curName);
    curItem->setText("*"+curName);
}

void AddSendName::on_moveButton_clicked()
{
    QListWidgetItem * curItem = ui->sendNameListWidget->currentItem();
    if(curItem == NULL)
    {
        QMessageBox::information(this, tr("警告"),tr("没有选中名字"));
        return;
    }

    QString curName = curItem->text();
    if(curName.startsWith('*'))
    {
        QString temp = curName.right(curName.size()-1);
        sendName.removeAll(temp);
        curItem->setText(temp);
    }
    else{
        QMessageBox::information(this, tr("警告"),tr("没有添加 %1").arg(curName));
        return;
    }
}

void AddSendName::on_confirmButton_clicked()
{
    close();
}

QStringList AddSendName::getAllName()
{
    return sendName;
}

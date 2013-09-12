#include "newmessage.h"
#include "ui_newmessage.h"
#include <QMessageBox>
#include "addsendname.h"

NewMessage::NewMessage(QString _name, QString _content, QWidget *parent):
    QDialog(parent),
    ui(new Ui::NewMessage)
{
    ui->setupUi(this);
    setWindowTitle(tr("新信息"));
    setFixedSize(width(),height());
    ui->lineEdit->setText(_name);
    ui->textEdit->setText(_content);
}

NewMessage::NewMessage(int i, QList<Message> *list,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewMessage)
{
    ui->setupUi(this);
    ui->lineEdit->setText(numbersToNames((*list)[i].number));
    ui->textEdit->setText((*list)[i].content);
    list->removeAt(i);
}

NewMessage::~NewMessage()
{
    delete ui;
}

void NewMessage::on_sendButton_clicked()
{
    if(ui->textEdit->toPlainText() == "" ||  ui->lineEdit->text() == "")
    {
        QMessageBox::information(this, tr("警告"),tr("内容或发送人不能为空"));
        return;
    }
    QString names = ui->lineEdit->text();
    QString numbers = namesToNumbers(names);
    QStringList numberList = numbers.split(";", QString::SkipEmptyParts);
    QString toSendContent = (tr("<Sms><Number>")+myNumber+tr("</Number><Content>###")+ui->textEdit->toPlainText()+tr("</Content></Sms>"));
    //实现对numberList中的每个号码发送短信
    sendMessage(toSendContent, numberList);
    ///////////////////////////
}

void NewMessage::sendMessage(QString & content, QStringList &addrList)
{
    for(int i=0;i<addrList.count();i++)
    {
        Message newmessage;
        sender.abort();
        sender.connectToHost(addrList[i],1573);
        if(sender.waitForConnected(500))
        {
            QByteArray block;
            QDataStream out(&block,QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_8);
            out<<quint16(0)<<content;
            out.device()->seek(0);
            out<<quint16(block.size()-sizeof(quint16));
            sender.write(block);
            sender.waitForBytesWritten(1000);
            sender.close();
            newmessage.number=addrList[i];
            newmessage.content=ui->textEdit->toPlainText();
            newmessage.time=QDateTime::currentDateTime();
            newmessage.from=0;
            newmessage.now=0;
            allMessages.prepend(newmessage);   //存放到收件箱

        }
        else  //发送失败，存到草稿箱，下次对方上线时，自动发送
        {
            newmessage.number=addrList[i];
            newmessage.content=ui->textEdit->toPlainText();
            newmessage.time=QDateTime::currentDateTime();
            newmessage.from=4;
            newmessage.now=-1;
            allMessages.prepend(newmessage);
            QMessageBox::information(this, tr("错误"),tr("发送到")+numberToName(addrList[i])+tr("失败，信息已放入草稿箱"));
        }
    }
    QMessageBox::information(this,tr("提示"),tr("信息发送完毕"));
}

void NewMessage::on_draftButton_clicked()  //自己保存的草稿，下次对方上线时，不发送
{
    if(ui->textEdit->toPlainText() == "")
    {
        QMessageBox::information(this, tr("警告"),tr("内容不能为空"));
        return;
    }
    Message n;
    QString names = ui->lineEdit->text();
    QString numbers = namesToNumbers(names);
    n.number = numbers;
    n.content = ui->textEdit->toPlainText();
    n.time=QDateTime::currentDateTime();
    n.now = -1;
    n.from = -1;
    allMessages.append(n);

    QMessageBox::information(this, tr("提示"),tr("信息已放入草稿箱!"));
}

void NewMessage::on_closeButton_clicked()
{
    close();
}

void NewMessage::on_addButton_clicked()
{
    AddSendName sendName(ui->lineEdit->text());
    sendName.exec();
    QStringList allName = sendName.getAllName();
    QString names = allName.join(";");
    if(names.endsWith(';'))     //去掉最后一个分号
        names = names.left(names.length()-1);
    ui->lineEdit->setText(names);
}

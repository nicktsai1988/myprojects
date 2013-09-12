#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QPixmap>
#include "QMessageBox"

ChatDialog::ChatDialog(QWidget *parent):QWidget(parent),ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    setFixedSize(width(),height());
    ui->allMessageTextEdit->setReadOnly(true);
    setWindowTitle(tr("信风"));
    QPixmap pixmap(":/icon/icons/user_offline.png");
    ui->pictureLabel->setPixmap(pixmap);
    connect(ui->sendButton,SIGNAL(clicked()),this,SLOT(sendmessage()));
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(&sender,SIGNAL(disconnected()),this,SLOT(disconnect()));
    connect(&sender,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(sockerror()));
    connect(&sender,SIGNAL(connected()),this,SLOT(haveconected()));
}

void ChatDialog::setcontact(const Contact &contact) //设置联系人
{
    this->contact=contact;
    ui->informationLabel->setText(contact.name+":"+contact.number);
    iswhitecontact();  //判断联系人是否来自白名单，并设置是否可以发送消息
    sender.connectToHost(contact.number,1573);

}

ChatDialog::~ChatDialog()
{
    delete ui;
}
void ChatDialog::closeEvent(QCloseEvent *)
{
    sender.close();
    emit chatfinished();  //聊天窗口关闭时，发送此信号
}

Contact& ChatDialog::getcontact()  //返回联系人
{
    return contact;
}

void ChatDialog::sockerror()  //连接出现错误
{
    QPixmap pixmap(":/icon/icons/user_offline.png");
    ui->pictureLabel->setPixmap(pixmap);
    sender.close();
}

void ChatDialog::updatepanel(const QString &name,const Message &newsms) //添加新信息到面板
{
    ui->allMessageTextEdit->append(name+"-["+newsms.time.toString(dateTimeFormat)+"]:");
    ui->allMessageTextEdit->append(tr("<h2><font color=red>%1</font></h2>").arg(newsms.content));
}

void ChatDialog::haveconected()  //连接成功
{

    QPixmap pixmap(":/icon/icons/user.png");
    ui->pictureLabel->setPixmap(pixmap);
    if(couldsend)
    {
        ui->sendButton->setEnabled(true);
        ui->sendButton->setToolTip("Ctrl+Enter"); //显示快捷键
    }
    else
    {
        ui->sendButton->setEnabled(false);
        ui->sendButton->setToolTip(tr("必须加为好友才能发送信息"));
    }
}

void ChatDialog::disconnect()  //连接断开
{
    QPixmap pixmap(":/icon/icons/user_offline.png");
    ui->pictureLabel->setPixmap(pixmap);
    sender.close();
}

void ChatDialog::sendmessage()  //发送消息
{
    if(sender.state()==QTcpSocket::ConnectedState)
    {
        if(!ui->sendMessageTextEdit->toPlainText().isEmpty())
        {

            QString message=createmessage();
            Message newsms;
            newsms.number=contact.number;
            newsms.content=ui->sendMessageTextEdit->toPlainText();
            newsms.time=QDateTime::currentDateTime();
            newsms.from=newsms.now=0;  //发送成功，信息保存到收件箱
            QByteArray block;
            QDataStream out(&block,QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_8);
            out<<quint16(0)<<message;
            out.device()->seek(0);
            out<<quint16(block.size()-sizeof(quint16));
            updatepanel(myName,newsms);
            sender.write(block);
            sender.waitForBytesWritten(2000);
            ui->sendMessageTextEdit->clear();
            newsms.from=newsms.now=0;
            allMessages.prepend(newsms);
        }
        else
            QMessageBox::information(this,tr("提示"),tr("内容为空"));

    }
    else //连接断开，保存在草稿箱
    {
        if(!ui->sendMessageTextEdit->toPlainText().isEmpty())
        {
            Message newsms;
            newsms.number=contact.number;
            newsms.content=ui->sendMessageTextEdit->toPlainText();
            newsms.time=QDateTime::currentDateTime();
            newsms.now=-1;
            newsms.from=4;
            updatepanel(myName,newsms);
            allMessages.prepend(newsms);
            ui->sendMessageTextEdit->clear();
            QMessageBox::information(this,tr("提示"),tr("%1处于离线状态，信息保存到草稿箱").arg(contact.name));
        }
        else
            QMessageBox::information(this,tr("提示"),tr("内容为空"));
    }

}

QString ChatDialog::createmessage()  //根据内容，生成发送的报文
{
    return tr("<Sms><Number>")+myNumber+tr("</Number><Content>")+"###"+ui->sendMessageTextEdit->toPlainText()+tr("</Content></Sms>");
}

void ChatDialog::contactonline()  //当联系人由离线转为上线时，重建到联系人的连接
{
    sender.connectToHost(contact.number,1573);
}

bool ChatDialog::iswhitecontact()
{
    for(int i=0;i<allContacts.count();i++)
    {
        if(allContacts[i].number==contact.number&&allContacts[i].name==contact.name&&allContacts[i].isWhite)
        {
            couldsend=true;
            return true;
        }
    }
    couldsend=false;
    return false;
}

void ChatDialog::setwhite()
{
    couldsend=true;
    if(sender.state()==QTcpSocket::ConnectedState)
    {
        ui->sendButton->setEnabled(true);
        ui->sendButton->setToolTip("Ctrl+Enter"); //显示快捷键
    }
}

void ChatDialog::setdark()
{
    couldsend=false;
    if(sender.state()==QTcpSocket::ConnectedState)
    {
        ui->sendButton->setEnabled(false);
        ui->sendButton->setToolTip(tr("必须加为好友才能发送信息"));
    }
}

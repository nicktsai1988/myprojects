#include "replythemessage.h"
#include "ui_replythemessage.h"
#include <QDateTime>
#include <QMessageBox>

ReplyTheMessage::ReplyTheMessage(const Message &m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplyTheMessage)
{
    ui->setupUi(this);
    setWindowTitle(tr("回复"));
    setFixedSize(width(),height());

    curMessage = m;
    QString tempName = numberToName(curMessage.number);  //实际上，收件箱中的信息一定是单个号码发送过来的

    this->setFixedSize(400, 300);
    ui->namelineEdit->setText(tempName);
    ui->namelineEdit->setEnabled(false);
    ui->contentTextEdit->setText(curMessage.content);
    ui->contentTextEdit->setEnabled(false);

    ui->replyTextEdit->hide();
    ui->newlabel->hide();
}

ReplyTheMessage::~ReplyTheMessage()
{
    delete ui;
}

void ReplyTheMessage::on_replyButton_clicked()
{
    if(ui->replyButton->text() == tr("回复"))
        replayButtonClicked();
    else
        sendButtonClicked();
}

void ReplyTheMessage::replayButtonClicked()
{
    this->setFixedSize(400, 480);
    ui->replyButton->setGeometry(80,430,75,23);
    ui->exitButton->setGeometry(260,430,75,23);
    ui->replyTextEdit->show();
    ui->newlabel->show();
    ui->replyButton->setText(tr("发送"));
}

void ReplyTheMessage::sendButtonClicked()
{
    QString toContent = ui->replyTextEdit->toPlainText();
    if(toContent.isEmpty())
    {
        QMessageBox::information(this,tr("提示"),tr("回复内容不能为空！"));
        return;
    }
    QString toSendContent = ("<Sms><Number>"+myNumber+"</Number><Content>###"+toContent+"</Content></Sms>");
    /////回复短信
    sender.abort();
    sender.connectToHost(curMessage.number,1573);
    if(sender.waitForConnected(500))
    {
        sendMessage(toSendContent);
        Message newmessage;
        newmessage.number=curMessage.number;
        newmessage.content=toContent;
        newmessage.from=0; //信息存入发件箱
        newmessage.now=0;
        allMessages.prepend(newmessage);
        QMessageBox::information(this, tr("提示"),tr("信息发送成功"));
    }
    else
    {
        Message newmessage;
        newmessage.number=curMessage.number;
        newmessage.content=toContent;
        newmessage.from=-1;
        newmessage.now=-1;
        allMessages.prepend(newmessage);
        QMessageBox::information(this, tr("错误"),tr("发送失败，信息存入草稿箱"));
    }

}

void ReplyTheMessage::sendMessage(const QString &content)
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
}


void ReplyTheMessage::on_exitButton_clicked()
{
    close();
}

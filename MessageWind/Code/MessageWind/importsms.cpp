/*
用于从其它文件中导入短信

*/
#include "importsms.h"
#include "ui_importsms.h"
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

ImportSms::ImportSms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportSms)
{
    ui->setupUi(this);
    setWindowTitle(tr("导入短信"));
    setFixedSize(width(),height());
    ui->smsnameEdit->setEnabled(false);
    connect(ui->importButton,SIGNAL(clicked()),this,SLOT(importButtonClicked()));
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
}

ImportSms::~ImportSms()
{
    delete ui;
}

void ImportSms::importButtonClicked()   //导入短信，并给出提示
{
    QString filename = QFileDialog::getOpenFileName(this, tr("打开短信文件"), "", tr("短信文件(*.xml);; 所有文件(*.*)"));
    if(filename.isEmpty())
        return;
    ui->smsnameEdit->setText(filename);
    if(readSmsFromFile(filename))
    {
        QMessageBox::information(this,tr("提示"),tr("短信导入成功"));
        ui->importButton->setText(tr("继续导入"));
    }
    else
        QMessageBox::information(this,tr("错误"),tr("短信导入失败"));

}

bool ImportSms::readSmsFromFile(const QString &filename)//从文件filename导入
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::critical(this,tr("Error"),tr("can not read %1").arg(filename));
        return false;
    }
    reader.clear();
    reader.setDevice(&file);
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            if(reader.name()==tr("SmsCollection"))  //当前标签为SmsCollection
            {
                readMessage();
            }
            else
            {
                QMessageBox::critical(this,tr("Error"),tr("Not a message file"));
                return false;
            }
        }
        else
            reader.readNext();
    }
    file.close();
    if(reader.hasError())
    {
        QMessageBox::critical(this,tr("Error"),reader.errorString());
        return false;
    }
    else
        if(file.error()!=QFile::NoError)
        {
            QMessageBox::critical(this,tr("Error"),file.errorString());
            return false;
        }
    return true;
}

void ImportSms::readMessage()       //开始读取消息
{
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }
        if(reader.isStartElement())
        {
            if(reader.name()==tr("Sms"))  //当前标签为Sms
                readSms();
            else
            {
                QMessageBox::critical(this,tr("Error"),tr("sms file error"));
                exit(1);
            }
        }
        else
            reader.readNext();
    }
}

void ImportSms::readSms()          //提取消息，并保存消息
{
    Message newsms;
    newsms.time=QDateTime::currentDateTime();
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isEndElement())   //读到</Sms>
        {
            insertSms(newsms); //此时一个Sms已经读完
            reader.readNext();
            break;
        }
        if(reader.isStartElement())
        {
            if(reader.name()==tr("Number"))
                newsms.number=readElement();
            else
                newsms.content=readElement();
        }
        else
            reader.readNext();
    }

}
QString ImportSms::readElement()         //读取当前元素值
{
    QString number=reader.readElementText();
    if(reader.isEndElement())     //读到</Number>
        reader.readNext();       //current element is <Sms>
    return number;
}

void ImportSms::insertSms(Message &newsms)  //根据是否含有敏感词和是否来自黑名单的短信，进行合适的保存
{
    if(isRubishMessage(newsms))
    {
        newsms.now=2;
        newsms.from=1;
    }
    else
    {
        newsms.from=newsms.now=1;
    }
    allMessages.prepend(newsms);
}



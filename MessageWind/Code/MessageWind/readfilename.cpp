#include "readfilename.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

ReadFileName::ReadFileName(QWidget * parent):QWidget(parent)
{
}

void ReadFileName::ReadALLFile(const QString &message, const QString &contact, const QString &filter, const QString & directoryName)
{
    readFilterFile(filter);
    readContactFile(contact);
    readMessageFile(message);
    readDirectoryFile(directoryName);
}

void ReadFileName::readDirectoryFile(const QString &directoryFileName)
{
    QFile file(directoryFileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    QTextStream in(&file);
    QString temp;
    int count;
    while(!in.atEnd())
    {
        in >> temp >>count;
        directory.insert(temp, count);
    }
}

void ReadFileName::readFilterFile(const QString &filter)
{
    QFile file(filter);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
         QMessageBox::critical(this,tr("Error"),tr("can not read %1").arg(filter));
         exit(1);
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString temp;
        in >> temp;

        if(temp != QString::null)
            badWords.append(temp);
    }
    file.close();
}


void ReadFileName::readContactFile(const QString & contact)
{
    QFile file(contact);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this,tr("Error"),tr("can not read %1").arg(contact));
        exit(1);
    }
    reader.clear();
    reader.setDevice(&file);
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isStartElement())     //meet <People>
        {
            if(reader.name() == tr("People"))
                readPeople();
            else
            {
                QMessageBox::critical(this, tr("Error"),tr("contact file error"));
                exit(1);
            }
        }
        else
           reader.readNext();
    }

    file.close();
}

void ReadFileName::readPeople()
{
    reader.readNext();  //over <People>
    while(!reader.atEnd())
    {
        if(reader.isEndElement())  //meet </People>
        {
            reader.readNext();
            break;
        }
        if(reader.isStartElement()) //meet <Contact>
        {
            if(reader.name() == tr("Contact"))
                readContact();
            else
            {
                QMessageBox::critical(this, tr("Error"),tr("contact file error"));
                exit(1);
            }
        }
        else
            reader.readNext();  //开始一直少写了这句，导致reader流没有向前移动
    }
}

void ReadFileName::readContact()
{
    Contact newContact;
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isEndElement())   //meet </Contact>
        {
            newContact.onLine = false;
            allContacts.append(newContact);
            reader.readNext();
            break;
        }
        if(reader.isStartElement())
        {
            if(reader.name() == tr("Number"))
                newContact.number = readElement();
            else if(reader.name() == tr("Name"))
                newContact.name = readElement();
            else
                newContact.isWhite = (readElement() == "true");
        }
        else
            reader.readNext();
    }
}

void ReadFileName::readMessageFile(const QString & message)
{
    QFile file(message);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::critical(this,tr("Error"),tr("can not read %1").arg(message));
        exit(1);
    }
    reader.clear();
    reader.setDevice(&file);
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            if(reader.name()==tr("SmsCollection"))
                readMessage();
            else
            {
                QMessageBox::critical(this,tr("Error"),tr("Not a message file"));
                exit(1);
            }
        }
        else
            reader.readNext();
    }
    file.close();
    if(reader.hasError())
    {
        QMessageBox::critical(this,tr("Error"),reader.errorString());
        exit(1);
    }
    else
        if(file.error()!=QFile::NoError)
        {
            QMessageBox::critical(this,tr("Error"),file.errorString());
            exit(1);
        }
    file.close();
}

void ReadFileName::readMessage()
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
            if(reader.name() == tr("Sms"))
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

void ReadFileName::readSms()
{
    Message newMessage;
    newMessage.now = -2;    //-2说明目前还不知道信息在哪个信箱内
    newMessage.from = 1;    //1表示默认新信息在收件箱内并且已读

    QString temp=reader.attributes().value(tr("datetime")).toString();
    newMessage.time=QDateTime::fromString(temp,dateTimeFormat);
    reader.readNext();
    while(!reader.atEnd())
    {
        if(reader.isEndElement())   // </Sms>
        {
            if(newMessage.now == -2)    //对于还没有确定消息位置的，根据号码和过滤词来判断放入收件箱还是垃圾箱
                insertMessage(newMessage);
            else
                if(newMessage.from==3)
                    unreadsms.append(newMessage);
                else
                    allMessages.append(newMessage);
            reader.readNext();
            break;
        }
        if(reader.isStartElement())
        {
            if(reader.name()==tr("Number"))
                newMessage.number= readElement();
            else if(reader.name() == tr("Content"))
                newMessage.content= readElement();
            else if(reader.name() == tr("Now"))
                newMessage.now = readElement().toInt();
            else if(reader.name() == tr("From"))
                newMessage.from = readElement().toInt();
        }
        else
            reader.readNext();
    }
}

void ReadFileName::insertMessage(Message newMessage)
{
    //这里开始另recogniseNumber是否等于1，导致不认识的号码，直接放入垃圾箱中
    //改为recogniseNumber!= -1,则可以将不认识号码的短信也放入收件箱中，只要不含敏感词即可
    if(recogniseNumber(newMessage.number) != -1 && !hasBadWords(newMessage.content))
        newMessage.now = 1;
    else
        newMessage.now = 2;
    allMessages.append(newMessage);
}

QString ReadFileName::readElement()
{
    QString str=reader.readElementText();
    if(reader.isEndElement())     //</Number>
        reader.readNext();       //current element is <Sms>
    return str;
}

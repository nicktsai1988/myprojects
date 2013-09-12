#include "writetofile.h"
#include<QMessageBox>
#include<QFile>
#include<cstdlib>
#include<QTextStream>

WriteToFile::WriteToFile(QWidget *parent) :
    QWidget(parent)
{
}

void WriteToFile::writeAllFile(const QString &message, const QString &contact, const QString &filter)
{
    writeMessageFile(message);
    writeContactFile(contact);
    writeFilterFile(filter);
}

void WriteToFile::writeMessageFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::critical(this,tr("Error"),QString(filename).append(":").append(file.errorString()));
        exit(1);
    }
    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setCodec("utf-8");
    writer.writeStartDocument();
    writer.writeStartElement(tr("SmsCollection"));
    for(int i = 0; i < allMessages.size(); i++)
        writeSms(allMessages[i]);
    for(int i=0;i< unreadsms.size();i++)
        writeSms(unreadsms[i]);
    writer.writeEndDocument();
    file.close();
    if(file.error())
    {
        QMessageBox::critical(this,tr("error"),tr("cannot write file ").append(filename).append(file.errorString()));
        exit(1);
    }
}

void WriteToFile::writeSms(const Message &newMessage)
{
    writer.writeStartElement(tr("Sms"));
    writer.writeAttribute(tr("datetime"),newMessage.time.toString(dateTimeFormat));
    writer.writeTextElement(tr("Number"), newMessage.number);
    writer.writeTextElement(tr("Content"), newMessage.content);
    writer.writeTextElement(tr("Now"), QString::number(newMessage.now));
    writer.writeTextElement(tr("From"), QString::number(newMessage.from));
    writer.writeEndElement();
}

void WriteToFile::writeContactFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::critical(this,tr("Error"),QString(filename).append(":").append(file.errorString()));
        exit(1);
    }
    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setCodec("utf-8");
    writer.writeStartDocument();
    writer.writeStartElement(tr("People"));
    for(int i=0;i<allContacts.size();i++)
        writePeople(allContacts[i]);
    writer.writeEndDocument();
    file.close();
    if(file.error())
    {
        QMessageBox::critical(this,tr("error"),tr("cannot write file ").append(filename).append(file.errorString()));
        exit(1);
    }
}

void WriteToFile::writePeople(const Contact &newcontact) //写People标签的内容
{
    writer.writeStartElement(tr("Contact"));
    writer.writeTextElement(tr("Number"),newcontact.number);
    writer.writeTextElement(tr("Name"),newcontact.name);
    writer.writeTextElement(tr("White"),boolToQString(newcontact.isWhite));
    writer.writeEndElement();
}

void WriteToFile::writeFilterFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::critical(this,tr("Error"),tr("can not write %1").arg(filename));
        exit(1);
    }
    QTextStream out(&file);
    for(int i=0; i < badWords.size();i++)
        out<<badWords[i]<<" ";
    file.close();
}



QString WriteToFile::boolToQString(bool b)
{
    QString temp;
    if(b)
        temp = tr("true");
    else
        temp = tr("false");
    return temp;
}

#include "informthread.h"
#include <QMessageBox>

InformThread::InformThread(QObject *parent):QThread(parent)
{
    flags="+++";
    exit=false;
}

InformThread::~InformThread()
{
}

void InformThread::run()
{
    sender=new QTcpSocket();
    connect(this,SIGNAL(finished()),sender,SLOT(deleteLater()));

    for(int i=0;i<allContacts.count();i++)
    {
        if(exit==false&&allContacts[i].isWhite) //只向好友发送在线信息
            sendto(allContacts[i]);
        else
            if(exit&&allContacts[i].onLine&&allContacts[i].isWhite) //只向在线好友发送下线信息
                sendto(allContacts[i]);
    }
}

void InformThread::sendto(const Contact &contact)
{
    sender->abort();
    sender->connectToHost(contact.number,1573);
    if(sender->waitForConnected(500))
    {
        QString message=createmessage();
        sendmessage(message);
    }
    else
    {
        sender->close();
    }
}

void InformThread::sendmessage(const QString &message)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out<<quint16(0)<<message;
    out.device()->seek(0);
    out<<quint16(block.size()-sizeof(quint16));
    sender->write(block);
    sender->waitForBytesWritten(2000);
    sender->close();
}

void InformThread::setoffline()
{
    flags="---";
    exit=true;
}

QString InformThread::createmessage()
{
    return ("<Sms><Number>"+myNumber+"</Number><Content>"+flags+"</Content></Sms>");
}





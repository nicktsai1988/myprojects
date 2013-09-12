/*
  这是一个客户类，用于从socket提取消息，并生成短信，当短信生成成功后，改变自己的状态，
  并发信号havereadmessage()通知其它 外部函数来提取短信
  */

#include "clientsocket.h"
ClientSocket::ClientSocket(QTcpSocket *socket)  //构造哈数，socket为要提取短信的连接
{
    this->socket=socket;
    readstate=false;
    socketclosed=false;
    blocksizereceive=0;
    QObject::connect(socket,SIGNAL(readyRead()),this,SLOT(readmessage()));  //readyRead信号到来时，调用readmessage()读取消息
    QObject::connect(socket,SIGNAL(disconnected()),this,SLOT(clientclosed()));   //客户断开连接
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(clientclosed()));  //连接出错
    connect(socket,SIGNAL(disconnected()),socket,SLOT(deleteLater()));
}

ClientSocket::~ClientSocket()
{

}
bool ClientSocket::haveread()           //是否已经读完
{
    return readstate;
}
QList<Message> * ClientSocket::getSms()           //提取消息
{
    readstate=false;
    return &smsset;
}

bool ClientSocket::haveclosed()
{
    return socketclosed;
}

void ClientSocket::readmessage()     //readyRead信号到来时，读取消息
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_8);
    if(blocksizereceive==0)
     {
         if(socket->bytesAvailable()<sizeof(quint16))
                return;
         in>>blocksizereceive;
     }
     if(socket->bytesAvailable()<blocksizereceive)
         return;
     QString message;
     in>>message;
     createSms(message);
}

void ClientSocket::createSms(const QString &message)      //分析报文，生成短信
{
    Message newsms;
    int index1=message.indexOf(tr("<Sms><Number>"),0)+tr("<Sms><Number>").length();
    int index2=message.indexOf(tr("</Number><Content>"),index1);
    newsms.number=message.mid(index1,index2-index1);
    int index3=index2+tr("</Number><Content>").length();
    int index4=message.indexOf(tr("</Content></Sms>"),index3);
    newsms.content=message.mid(index3,index4-index3);
    newsms.from=newsms.now=3;   //标记为未读
    newsms.time=QDateTime::currentDateTime();
    smsset.append(newsms);
    readstate=true;  //改变状态
    blocksizereceive=0;
    emit havereadmessage();  //发出信息已读完，可以来提取信息的信号
}

void ClientSocket::clientclosed()
{
    socketclosed=true;
    emit haveclientclosed();
}



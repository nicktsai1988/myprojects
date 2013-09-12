/*
  这是一个客户类，用于从socket提取消息，并生成短信，当短信生成成功后，改变自己的状态，
  并发信号havereadmessage()通知其它 外部函数来提取短信
  */


#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include <QTcpSocket>
#include "data.h"
#include <QString>
#include <QList>
class ClientSocket : public QObject
{
    Q_OBJECT
public:
    ClientSocket(QTcpSocket *);
    ~ClientSocket();
    bool haveread();   //消息是否读完
    bool haveclosed(); //连接是否断开
    QList<Message> * getSms();      //提取消息
signals:
    void havereadmessage();   //通知外部函数，提取短信
    void haveclientclosed();
private slots:
    void readmessage();      //提取消息
    void clientclosed();
private:
    void createSms(const QString &);//生成短信
    bool readstate;              //标志当前状态
    bool socketclosed;
    QList<Message> smsset;             //保存短信内容，供外部函数提取
    QTcpSocket *socket;
    quint16 blocksizereceive;
};

#endif // CLIENTSOCKET_H

/*

  这个是一个子线程。用于发送上线的信息，当上线时，对所有好友发送上线信息，下线时，对所有在线好友发送下线信息

  */


#ifndef INFROMTHREAD_H
#define INFORMTHREAD_H
#include<QTcpSocket>
#include<QList>
#include<QThread>
#include "data.h"

class InformThread:public QThread
{
    Q_OBJECT
public:
    InformThread(QObject *parent=0);
    ~InformThread();
    void run();
    void setoffline();
private:
    void sendto(const Contact&);
    void sendmessage(const QString &);
    QString createmessage();
    QTcpSocket *sender;
    bool exit;
    QString flags;
};

#endif // INFORMTHREAD_H

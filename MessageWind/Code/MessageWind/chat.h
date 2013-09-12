/*
  这个是一个聊天的类，里面有一个chatdialog类，是聊天的主窗口。之所以加了这个壳
  是因为当聊天窗口关闭时，设置标记，让MessageWind类的方法来处理

  */


#ifndef CHAT_H
#define CHAT_H
#include "chatdialog.h"
#include "data.h"
class Chat:public QObject
{
    Q_OBJECT
public:
    Chat(const Contact&);
    ~Chat();
    bool dialog_have_closed();
    Contact& getcontact();
    void activateWindow();
    void addnewmessage(const QString &,const Message &);
    void close();
    void contatonline();
    void setwhite();
    void setdark();
signals:
    void have_chat_dialog_closed();
private slots:
    void on_chat_dialog_closed();
private:
    ChatDialog chatdialog;
    bool closed;
};

#endif // CHAT_H

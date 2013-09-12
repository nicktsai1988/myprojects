#ifndef MESSAGEWIND_H
#define MESSAGEWIND_H

#include <QMainWindow>
#include <QTimer>
#include <QTreeWidgetItem>
#include "informthread.h"
#include <QList>
#include "data.h"
#include <QTcpSocket>
#include <QTcpServer>
#include "clientsocket.h"
#include "parentblink.h"
#include "childblink.h"
#include "chat.h"
namespace Ui {
class MessageWind;
}

class MessageWind : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MessageWind(QWidget *parent = 0);
    ~MessageWind();
signals:
    void newSms();   //当收到新消息后，通知其他函数进行必要的更新
private slots:
    void DoubleClicked(QTreeWidgetItem *,int);  //双击联系人列表里的项目时
    void Collapsed(QTreeWidgetItem *);        //节点收起时调用，收起时父节点闪烁，
    void Expanded(QTreeWidgetItem*);          //展开时子节点闪烁，父节点停止闪烁

    //////////////////////////////////////////
    void havenewSms(); //用于与ClientSocket类的havereadmessage()信号连接，提取新的短信
    void haveclientclosed();  //当有客户连接断开时调用，用于删除掉断开的客户对象
    void havechatdialogclosed(); //当有聊天窗口关闭时，处理已关闭的聊天窗口
    void receiveBoxButtonClicked();  //显示收件箱
    void writeButtonClicked();//发送新信息
    void sendBoxButtonClicked(); //显示发件箱
    void draftBoxButtonClicked(); //显示草稿箱
    void settingButtonClicked(); //显示设置面板
    void newclient();         //处理新的TCP连接
    void on_action_Add_triggered();  //添加新联系人

    void on_action_Edit_triggered();  //编辑联系人

    void on_action_Move_to_dark_triggered(); //移动联系人到黑名单

    void on_action_Move_to_white_triggered(); //移动联系人到白名单

    void on_action_Delete_triggered();        //删除联系人

    void on_action_Send_Message_triggered();   //发消息

    void on_action_Find_triggered();         //查找联系人

protected:
    virtual void contextMenuEvent (QContextMenuEvent *);//右键菜单
    virtual void	closeEvent (QCloseEvent *);    //关闭主窗口时，将数据写回文件
private:
    void init();         //初始化界面和发出上线信息
    void update();          //更新主窗口
    Contact* indexofcontact(const QString&,QList<Contact> **contactlist=NULL,int *index=NULL);
    bool chatdialogexist(const Contact&,Chat **);   //聊天窗口已经存在
    void addchildblink(QTreeWidgetItem *);          //让当前联系人闪烁
    void contact_online(const QString &,bool);      //收到联系人在线的信息时，标示联系人在线
    void contact_offline(const QString &);          //收到联系人的离线消息时，表示联系人离线
    Contact texttocontact(const QString&);          //提取文本中的联系人信息
    void initserver();                      //初始化服务器
    void insertSms(Message);          //根据短信号码和短信内容，将此短信保存到合适的集合中
    QString getmyownnumber();     //获取我自己的号码
    void sendmessage(const QString &,const QString &); //收到联系人的上线消息时，发回响应信息
    int totalunreadsms();        //未读短信的个数
    int totalunreadrubishsms();        //未读垃圾短信的个数
    void newblinkparent(QTreeWidgetItem*);  //当有子节点闪烁时，判断父节点是否闪烁
    QTreeWidgetItem* finditem(const QString &);  //根据号码查找对应的对象

    Ui::MessageWind *ui;
    QList<ChildBlink*> childBlinkItems;     //子节点闪烁集合
    QList<ParentBlink*> parentBlinkItems;    //父节点闪烁集合
    InformThread informer;                 //通知线程，用于通知上线和下线和刷新联系人列表
    QTcpServer *server;    //服务器，用于接收短信
    QList<ClientSocket *> clients; //存放TCP连接的集合
    QList<Chat*> chats;
};

#endif // MESSAGEWIND_H

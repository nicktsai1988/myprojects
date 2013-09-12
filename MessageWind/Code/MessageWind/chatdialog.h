#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include "data.h"
#include <QTcpSocket>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit ChatDialog(QWidget* parent=0);
    void setcontact(const Contact&);
    Contact& getcontact();   //返回联系人
    void updatepanel(const QString &,const Message &); //将消息添加到已发送窗口
    void contactonline(); //党联系人重新上线时，重建连接
    ~ChatDialog();
    void setwhite();   //当移动联系人呢时，更新其状态
    void setdark();
signals:
    void chatfinished();    //当窗口关闭时，发送此信号
protected:
    virtual void closeEvent (QCloseEvent *);
private slots:
    bool iswhitecontact();  //判断联系人是否是来自白名单
    void sockerror();
    void haveconected();     //已经连接
    void disconnect();   //连接断开
    void sendmessage();  // 发送消息
private:
    QString createmessage();
    Ui::ChatDialog *ui;
    Contact contact;
    QTcpSocket sender;
    bool couldsend;
};

#endif // CHATDIALOG_H

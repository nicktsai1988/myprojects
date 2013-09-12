#include "messagewind.h"
#include "ui_messagewind.h"
#include <QTreeWidgetItem>
#include <QStringList>
#include "childblink.h"
#include "parentblink.h"
#include "chatdialog.h"
//////////////////////////////
#include "clientsocket.h"
#include "newcontact.h"
#include "readfilename.h"
#include "writetofile.h"
#include "newmessage.h"
#include "basemessagebox.h"
#include "setting.h"
#include "editcontact.h"
#include "findcontact.h"
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QMenu>
#include <QNetworkInterface>

MessageWind::MessageWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MessageWind)
{
    ui->setupUi(this);
    setFixedSize(width(),height());
    setWindowTitle(tr("信风"));
    ReadFileName readfile;
    readfile.ReadALLFile("messages.xml", "contacts.xml", "key.txt", "directory.txt"); //从文件中读取信息
    initserver();
    myName=tr("我");
    myNumber=getmyownnumber();
    connect(ui->contactsTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(DoubleClicked(QTreeWidgetItem*,int)));
    connect(ui->contactsTreeWidget,SIGNAL(itemCollapsed(QTreeWidgetItem*)),this,SLOT(Collapsed(QTreeWidgetItem*)));
    connect(ui->contactsTreeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(Expanded(QTreeWidgetItem*)));
    connect(ui->receiveBoxButton,SIGNAL(clicked()),this,SLOT(receiveBoxButtonClicked()));
    connect(ui->writeButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui->settingButton,SIGNAL(clicked()),this,SLOT(settingButtonClicked()));
    connect(ui->sendBoxButton,SIGNAL(clicked()),this,SLOT(sendBoxButtonClicked()));
    connect(ui->draftBoxButton,SIGNAL(clicked()),this,SLOT(draftBoxButtonClicked()));
    init();
    update();
}

MessageWind::~MessageWind()
{
    delete ui;
    for(int i=0;i<childBlinkItems.count();i++) //删除子节点对象
        delete childBlinkItems[i];
    for(int j=0;j<parentBlinkItems.count();j++)//删除父节点对象
        delete parentBlinkItems[j];
    server->close();        //关闭服务器
    delete server;          //删除服务器对象
}

void MessageWind::init()
{
    ui->contactsTreeWidget->setHeaderLabel(tr("联系人"));
    QTreeWidgetItem *whitecontacts=new QTreeWidgetItem(QStringList()<<"我的好友");
    QTreeWidgetItem *unknowncontacts=new QTreeWidgetItem(QStringList()<<"陌生人");
    QTreeWidgetItem *darkcontacts=new QTreeWidgetItem(QStringList()<<"黑名单");
    ui->contactsTreeWidget->addTopLevelItem(whitecontacts);
    whitecontacts->setExpanded(true);  //将白名单设置为展开
    ui->contactsTreeWidget->addTopLevelItem(unknowncontacts);
    ui->contactsTreeWidget->addTopLevelItem(darkcontacts);
    whitecontacts->setWhatsThis(0,QString::number(0)); //将白名单的子闪烁集合设置为0
    parentBlinkItems.append(new ParentBlink(whitecontacts));
    unknowncontacts->setWhatsThis(0,QString::number(0));//将未知联系人的子闪烁集合设置为0
    parentBlinkItems.append(new ParentBlink(unknowncontacts));
    darkcontacts->setWhatsThis(0,QString::number(0));  //将黑名单的子闪烁集合设置为0
    parentBlinkItems.append(new ParentBlink(darkcontacts));

    for(int i=0;i<allContacts.count();i++)
    {
        QTreeWidgetItem *child=new QTreeWidgetItem(QStringList()<<(allContacts[i].name+":"+allContacts[i].number)); //显示联系人信息
        child->setWhatsThis(0,allContacts[i].name+":"+allContacts[i].number);//因为闪烁的时候文本会改变，所以将联系人的信息保存在whatsthis 中
        child->setIcon(0,QIcon(":/icon/icons/user_offline.png")); //初始化时设置为离线
        if(allContacts[i].isWhite)
            ui->contactsTreeWidget->topLevelItem(0)->addChild(child); //白名单添加到白名单中
        else
            ui->contactsTreeWidget->topLevelItem(2)->addChild(child);  //黑名单添加到黑名单中
    }
    for(int i=0;i<unreadsms.count();i++)   //若短信文件中有未读短信，则对应的联系人闪烁
    {
        QTreeWidgetItem *item=finditem(unreadsms[i].number);
        if(item!=NULL) //存在联系人
            addchildblink(item);
    }
    ui->receiveBoxButton->setToolTip(tr("收件箱")); //设置对应按钮的提示
    ui->writeButton->setToolTip(tr("写信息"));
    ui->sendBoxButton->setToolTip(tr("发件箱"));
    ui->draftBoxButton->setToolTip(tr("草稿箱"));
    ui->settingButton->setToolTip(tr("设置"));       
    informer.start();//通知所有联系人，我已上线
}

void MessageWind::DoubleClicked(QTreeWidgetItem *item, int row)
{
    if(item->parent()==NULL)      //顶级节点,不做任何响应
    {
        return;
    }
    for(int i=0;i<childBlinkItems.count();i++)  //删除对应联系人的闪烁
    {
        if(item==childBlinkItems[i]->getitem())
        {
            Blink *temp=childBlinkItems[i];
            childBlinkItems.removeAt(i);
            delete temp;
            i--;          //因为删除后，余下闪烁项目的未知会改变，所以要-1
        }
    }
    Contact contact=texttocontact(item->whatsThis(0)); //提取联系人信息
    Chat *chat;
    if(chatdialogexist(contact,&chat)) //存在联系人的聊天窗口
    {
        chat->activateWindow();        //将窗口激活
    }
    else               //不存在联系人的聊天窗口
    {
        chat=new Chat(contact); //则打开此联系人的新的聊天窗口
        if(chat!=NULL)
        {
            connect(chat,SIGNAL(have_chat_dialog_closed()),this,SLOT(havechatdialogclosed()));
            chats.append(chat);
        }
    }
    for(int i=0;i<unreadsms.count();i++) //若有此联系人的未读信息，则将这些未读信息显示在聊天窗口中，并将这些信息移到收件箱或者垃圾箱
    {
        QString number=chat->getcontact().number;

        if(unreadsms[i].number==number)
        {
            Message temp;
            temp=unreadsms[i];
            unreadsms.removeAt(i);
            i--;
            chat->addnewmessage(numberToName(number),temp);
            if(temp.now==2)  //来自黑名单，放入垃圾箱
            {
                temp.now=temp.from=2;
                allMessages.prepend(temp);
            }
            else
            {
                temp.now=temp.from=1;       //否则放入收件箱
                allMessages.prepend(temp);
            }
        }
    }
    update();   //刷新界面
}

void MessageWind::Expanded(QTreeWidgetItem *item) //父节点展开，父节点停止闪烁
{
    for(int i=0;i<parentBlinkItems.count();i++)
    {
        if(parentBlinkItems[i]->getitem()==item)
            parentBlinkItems[i]->stopblink();
    }
}

void MessageWind::Collapsed(QTreeWidgetItem *item) //父节点卷起，若仍有子节点在闪烁，则父节点闪烁
{
    int total=item->whatsThis(0).toInt();
    if(total>0)
    {
        for(int i=0;i<parentBlinkItems.count();i++)
        {
            if(parentBlinkItems[i]->getitem()==item)
                parentBlinkItems[i]->startblink();
        }
    }
}

void MessageWind::newblinkparent(QTreeWidgetItem *item) //当添加子节点的闪烁对象后，根据实际情况，处理父节点的闪烁
{
    int total=item->whatsThis(0).toInt();
    if(total>0&&!item->isExpanded()) //存在子节点的闪烁，并且此父节点卷起，则此父节点闪烁
    {
        for(int i=0;i<parentBlinkItems.count();i++)
        {
            if(parentBlinkItems[i]->getitem()==item)
                parentBlinkItems[i]->startblink();
        }
    }
}
void MessageWind::update()  //更新面板
{
    //resetitem();
    ui->nameLabel->setText(tr("姓名：")+myName);
    ui->numberLabel->setText(tr("号码：")+myNumber);
    ui->totalUnreadSmsLabel->setText(tr("未读消息：")+QString::number(totalunreadsms()));
    ui->totalRubishSmsLabel->setText(tr("未读垃圾消息:")+QString::number(totalunreadrubishsms()));
    for(int i=0;i<childBlinkItems.count();i++)//若信息在收件箱或者垃圾箱中查看了，则让对应的子节点和父节点停止闪烁
    {
        bool find=false;
        QString number=childBlinkItems[i]->getnumber();
        for(int j=0;j<unreadsms.count();j++)
        {
            if(unreadsms[j].number==number)//存在对应号码的未读短信
            {
                find=true;
                break;
            }
        }
        if(find==false)             //不存在对应号码的短信
        {
            Blink *temp=childBlinkItems[i];
            childBlinkItems.removeAt(i);//删除对应的闪烁
            delete temp;
            i--;
        }
    }
}

void MessageWind::receiveBoxButtonClicked() //打开收件箱
{
    BaseMessageBox mb(1);
    mb.exec();
    update();
}

void MessageWind::sendBoxButtonClicked() //打开发件箱
{
    BaseMessageBox mb(0);
    mb.exec();
    update();
}

void MessageWind::draftBoxButtonClicked()//打开草稿箱
{
    BaseMessageBox mb(-1);
    mb.exec();
    update();
}

void MessageWind::closeEvent(QCloseEvent *event)   //退出程序时，将数据写回文件
{
    informer.setoffline();
    informer.start();  //通知所有人，我已下线
    WriteToFile writefile;
    writefile.writeAllFile("messages.xml", "contacts.xml", "key.txt");
    for(int i=0;i<chats.count();i++)
    {
        disconnect(chats[i],SIGNAL(have_chat_dialog_closed()),this,SLOT(havechatdialogclosed()));
        chats[i]->close();
        delete chats[i];
    }//关闭已经打开的聊天窗口
}

void MessageWind::writeButtonClicked()//写新信息
{
    NewMessage sm;
    sm.exec();
}

void MessageWind::settingButtonClicked()//打开设置面板
{
    Setting set;
    set.exec();
    update();
}

void MessageWind::contextMenuEvent (QContextMenuEvent * event)  //右键菜单
{
    QMenu popMenu(this);
    popMenu.addAction(ui->action_Add); //增加联系人
    popMenu.addAction(ui->action_Find);//查找联系人
    popMenu.addSeparator();
    QTreeWidgetItem *item=ui->contactsTreeWidget->itemAt(ui->contactsTreeWidget->viewport()->mapFromGlobal(QCursor::pos()));
    if(item!=NULL) //如果有item则弹出菜单
    { 

        if(item->parent()==ui->contactsTreeWidget->topLevelItem(0))
        {
            popMenu.addAction(ui->action_Send_Message);
            popMenu.addSeparator();
            popMenu.addAction(ui->action_Edit);
            popMenu.addAction(ui->action_Delete);
            popMenu.addAction(ui->action_Move_to_dark);
        }
        else
            if(item->parent()==ui->contactsTreeWidget->topLevelItem(1))
            {
                popMenu.addAction(ui->action_Edit);
                popMenu.addAction(ui->action_Delete);
                popMenu.addAction(ui->action_Move_to_white);
                popMenu.addAction(ui->action_Move_to_dark);
            }
            else
                if(item->parent()==ui->contactsTreeWidget->topLevelItem(2))
                {
                    popMenu.addAction(ui->action_Edit);
                    popMenu.addAction(ui->action_Delete);
                    popMenu.addAction(ui->action_Move_to_white);
                }

    }
    popMenu.exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

void MessageWind::newclient()       //当有新的TCP连接时调用，让其自己去提取消息，提取完成后，用信号通知函数去取短信，
{                                    //连接断开时，用信号通知函数去处理断开连接的客户对象
    QTcpSocket *socket=server->nextPendingConnection();
    ClientSocket *newclient=new ClientSocket(socket);
    connect(newclient,SIGNAL(havereadmessage()),this,SLOT(havenewSms()));
    connect(newclient,SIGNAL(haveclientclosed()),this,SLOT(haveclientclosed()));
    clients.append(newclient);
}

void MessageWind::initserver()     //初始化服务器
{
    server=new QTcpServer(this);
    if(!server->listen(QHostAddress::Any,1573))
        {
            QMessageBox::critical(this,tr("服务器监听错误！"),server->errorString());
            exit(1);
        }
    connect(server,SIGNAL(newConnection()),this,SLOT(newclient()));
}

void MessageWind::insertSms(Message newsms)  //将消息保存到合适的短信集合
{
    if(newsms.content=="+++"||newsms.content=="---"||newsms.content=="+-+")//若为上线或者下线的消息
    {
       if(newsms.content=="+++") //对方发来对方的上线消息
        {
            contact_online(newsms.number,true);//标示对方上线，并且回发响应信息和对方的离线消息
            Chat *chat;
            if(chatdialogexist(*(indexofcontact(newsms.number)),&chat))
                chat->contatonline();//存在对应联系人的聊天窗口，则重建到对应联系人的连接
        }
        else
            if(newsms.content=="---") //离线消息，标志对方离线
                contact_offline(newsms.number);
            else//收到+-+            //收到对方发送的我上线的响应消息，则只需要将对方标记为上线，并发送对方的离线消息
            {
                contact_online(newsms.number,false);
            }
        return;
    }
    //若消息为正常消息
    newsms.content=newsms.content.mid(3,newsms.content.length()-3);//去除消息的头部
    if(hasBadWords(newsms.content)) //还有敏感词汇，直接扔进垃圾箱
    {
        newsms.from=3;//设置为未读
        newsms.now=2;
        allMessages.prepend(newsms);
        update();
        return;
    }
    else
    {
       QTreeWidgetItem *item=finditem(newsms.number);//查找对应号码的子节点
        if(item!=NULL)//存在对应的联系人
        {
            Contact contact=texttocontact(item->whatsThis(0)); //提取联系人信息
            if(contact.number==newsms.number)
            {
                Chat *chat;
                if(chatdialogexist(contact,&chat))//联系人已有聊天窗口打开
                {
                    chat->activateWindow();       //则将窗口激活
                    chat->addnewmessage(numberToName(newsms.number),newsms);//在聊天窗口中显示此消息
                    if(recogniseNumber(newsms.number)==-1)//来自黑名单
                        newsms.from=newsms.now=2;   //放入垃圾箱
                    else
                        newsms.from=newsms.now=1;    // 放入收件箱
                    allMessages.prepend(newsms);
                    update();
                    return;
                }
                else           //没有聊天窗口打开
                {
                    if(recogniseNumber(newsms.number)==-1)//来自黑名单
                        newsms.now=2;   //查看后放入垃圾箱
                    else
                        newsms.now=1;    // 查看后放入收件箱
                    unreadsms.append(newsms); //保存在未读短信集合中
                    addchildblink(item);       //让对应的子节点闪烁
                    update();
                    return;
                }
            }
        }
        else
        {
                //没有对应此号码的联系人
            Contact newcontact;
            newcontact.name=QString::null;
            newcontact.number=newsms.number;
            newcontact.onLine=true;    //将联系人设置为在线
            strangers.append(newcontact); //保存未知联系人
            QTreeWidgetItem *child=new QTreeWidgetItem(QStringList()<<(newcontact.name+":"+newcontact.number));
            child->setIcon(0,QIcon(":/icon/icons/user.png"));
            child->setWhatsThis(0,newcontact.name+":"+newcontact.number);
            ui->contactsTreeWidget->topLevelItem(1)->addChild(child);  //在联系人列表显示未知联系人
            newsms.now=1;  //将陌生人的信息存到收件箱
            unreadsms.append(newsms);  //添加未读消息
            addchildblink(child);   //设置为闪烁
            update();
        }
    }
}

void MessageWind::addchildblink(QTreeWidgetItem *item)
{
    for(int i=0;i<childBlinkItems.count();i++) //若已经存在对应节点的闪烁，则不添加，否则添加闪烁，并且处理对应的父节点
    {
        if(childBlinkItems[i]->getitem()==item)
            return;
    }
    childBlinkItems.append(new ChildBlink(item));
    newblinkparent(item->parent());
}

void MessageWind::havenewSms()         //提取新消息
{
    QList<Message> *smsset;
    for(int i=0;i<clients.count();i++)
    {
        if(clients[i]->haveread())
        {
            smsset=clients[i]->getSms();
            for(int j=0;j<smsset->count();j++)
            {
                insertSms((*smsset)[j]);
                smsset->removeAt(j);
                j--;
            }
        }
    }
}

void MessageWind::haveclientclosed() //处理已经断开连接的对象
{
    QList<Message> *smsset;
    for(int i=0;i<clients.count();i++)
    {
        if(clients[i]->haveread())  //若存在未读消息，则提取
        {
            smsset=clients[i]->getSms();
            for(int j=0;j<smsset->count();j++)
            {
                insertSms((*smsset)[j]);
                smsset->removeAt(j);
                j--;
            }
        }
        if(clients[i]->haveclosed())
        {
            ClientSocket *temp=clients[i];
            clients.removeAt(i);
            delete temp;
            i--;
        }
    }
}

QString MessageWind::getmyownnumber()  //获取本机的IP地址作为我自己的号码
{
    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    QHostAddress result;
    foreach(QHostAddress address, AddressList){
    if(address.protocol() == QAbstractSocket::IPv4Protocol &&
    address != QHostAddress::Null &&
    address != QHostAddress::LocalHost)
    {
    if (address.toString().contains("127.0."))
    {
        continue;
    }
    result = address;
    break;
    }
    }
    return result.toString();
}
Contact* MessageWind::indexofcontact(const QString &number,QList<Contact> **contactlist,int *index)
{
    //根据号码,查找对应的联系人，查找成功返回联系人的地址，查找失败，返回NULL
    for(int i=0;i<allContacts.count();i++)
    {
        if(number==allContacts[i].number)
        {
            if(contactlist!=NULL&&index!=NULL)
            {
                *index=i;
                *contactlist=&allContacts;
            }
            return &(allContacts[i]);
        }
    }
    for(int i=0;i<strangers.count();i++)
    {
        if(number==strangers[i].number)
        {
            if(contactlist!=NULL&&index!=NULL)
            {
                *index=i;
                *contactlist=&strangers;
            }
            return &(strangers[i]);
        }
    }
    return NULL;
}


void MessageWind::on_action_Add_triggered()//增加联系人
{

    NewContact newcontact;
    newcontact.exec();
    QList<Contact> *contacts=newcontact.getnewcontacts();
    for(int i=0;i<contacts->count();i++)
    {
        if((*contacts)[i].isWhite)//添加到白名单中
        {
            QTreeWidgetItem *child=new QTreeWidgetItem(QStringList()<<((*contacts)[i].name+":"+(*contacts)[i].number));
            child->setWhatsThis(0,(*contacts)[i].name+":"+(*contacts)[i].number);
            child->setIcon(0,QIcon(":/icon/icons/user_offline.png"));
            ui->contactsTreeWidget->topLevelItem(0)->addChild(child);
        }
        else  //添加到黑名单中
        {
            QTreeWidgetItem *child=new QTreeWidgetItem(QStringList()<<((*contacts)[i].name+":"+(*contacts)[i].number));
            child->setWhatsThis(0,(*contacts)[i].name+":"+(*contacts)[i].number);
            child->setIcon(0,QIcon(":/icon/icons/user_offline.png"));
            ui->contactsTreeWidget->topLevelItem(2)->addChild(child);
        }
    }
    informer.start();  //刷新联系人列表
}

void MessageWind::on_action_Edit_triggered()//编辑联系人
{

    QTreeWidgetItem *item=ui->contactsTreeWidget->currentItem();
    Contact temp=texttocontact(item->whatsThis(0));
    Contact* contact=indexofcontact(temp.number);
    EditContact editcontact(contact);
    editcontact.exec();
    item->setText(0,contact->name+":"+contact->number); //更新为新的联系人的信息
    item->setWhatsThis(0,contact->name+":"+contact->number);
    informer.start();      //刷新联系人列表
    for(int i=0;i<childBlinkItems.count();i++) //若存在对应的联系人的闪烁，也更新闪烁时显示的信息
    {
        if(childBlinkItems[i]->getitem()==item)
            childBlinkItems[i]->settext(item->whatsThis(0));
    }

}

void MessageWind::on_action_Move_to_dark_triggered() //移动到黑名单
{
    QTreeWidgetItem *item=ui->contactsTreeWidget->currentItem();

    Contact temp=texttocontact(item->whatsThis(0));
    QList<Contact> *contactlist;
    int position;
    indexofcontact(temp.number,&contactlist,&position);
    if(contactlist==&strangers)  //若原来为陌生人，则将此人从陌生人集合中移动到黑名单集合
    {
        temp.onLine=(*contactlist)[position].onLine;
        temp.isWhite=false;
        allContacts.append(temp);
        contactlist->removeAt(position);

    }
    else
        (*contactlist)[position].isWhite=false;
    ui->contactsTreeWidget->removeItemWidget(item,0);
    QTreeWidgetItem *item2=new QTreeWidgetItem(*item);
    ui->contactsTreeWidget->topLevelItem(2)->addChild(item2);
    for(int i=0;i<childBlinkItems.count();i++) //若有存在的闪烁，则更新闪烁
    {
        if(childBlinkItems[i]->getitem()==item)
        {
            childBlinkItems[i]->setitem(item2);
            newblinkparent(item2->parent());
            i--;
        }
    }
    Chat *chat;
    if(chatdialogexist(temp,&chat))
    {
        chat->setdark();
    }
    delete item;
}

void MessageWind::on_action_Move_to_white_triggered() //移动到白名单，与移动到黑名单类似
{
    QTreeWidgetItem *item=ui->contactsTreeWidget->currentItem();
    Contact temp=texttocontact(item->whatsThis(0));
    QList<Contact> *contactlist;
    int position;
    indexofcontact(temp.number,&contactlist,&position);
    if(contactlist==&strangers)
    {
        temp.onLine=(*contactlist)[position].onLine;
        temp.isWhite=true;
        allContacts.append(temp);
        contactlist->removeAt(position);

    }
    else
        (*contactlist)[position].isWhite=true;
    ui->contactsTreeWidget->removeItemWidget(item,0);
    QTreeWidgetItem *item2=new QTreeWidgetItem(*item);
    ui->contactsTreeWidget->topLevelItem(0)->addChild(item2);
    for(int i=0;i<childBlinkItems.count();i++)
    {
        if(childBlinkItems[i]->getitem()==item)
        {
            childBlinkItems[i]->setitem(item2);
            newblinkparent(item2->parent());
            i--;
        }
    }
    Chat *chat;
    if(chatdialogexist(temp,&chat))
    {
        chat->setwhite();
    }
    delete item;
}

void MessageWind::on_action_Delete_triggered()  //删除联系人
{
    QTreeWidgetItem *item= ui->contactsTreeWidget->currentItem();
    Contact temp=texttocontact(item->whatsThis(0));
    QList<Contact> *contactlist;
    int position;
    indexofcontact(temp.number,&contactlist,&position);
    contactlist->removeAt(position); //删除联系人对象
    ui->contactsTreeWidget->removeItemWidget(item,0);
    for(int i=0;i<childBlinkItems.count();i++)  //删除存在的闪烁
    {
        if(childBlinkItems[i]->getitem()==item)
        {
            Blink *tmp=childBlinkItems[i];
            childBlinkItems.removeAt(i);
            delete tmp;
            i--;
        }
    }

    for(int i=0;i<chats.count();i++)    //删除该联系人的聊天窗口
    {
        if(chats[i]->getcontact().name==temp.name&&chats[i]->getcontact().number==temp.number)
        {
            Chat *chat=chats[i];
            chats.removeAt(i);
            delete chat;
            i--;
        }
    }
    delete item;
}

void MessageWind::on_action_Send_Message_triggered() //发送消息
{

    QTreeWidgetItem* item=ui->contactsTreeWidget->currentItem();
    Contact contact=texttocontact(item->whatsThis(0));
    NewMessage newmessage(numberToName(contact.number));
    newmessage.exec();
}

void MessageWind::havechatdialogclosed()  //当有聊天窗口关闭时，处理关闭的聊天窗口
{
    for(int i=0;i<chats.count();i++)
    {
        if(chats[i]->dialog_have_closed())
        {
            Chat *temp=chats[i];
            chats.removeAt(i);
            delete temp;
        }
    }
}

bool MessageWind::chatdialogexist(const Contact &contact,Chat **chat) //查看对应联系人的聊天窗口是否存在
{
    for(int i=0;i<chats.count();i++)
    {
        if((chats[i]->getcontact().name==contact.name)&&(chats[i]->getcontact().number==contact.number))
        {

            *chat=chats[i];
            return true;
        }
    }
    *chat=NULL;
    return false;
}
void MessageWind::contact_online(const QString &number,bool reply)//联系人上线
{
    QTcpSocket socket;
    socket.connectToHost(number,1573);
    Contact *contact=indexofcontact(number,NULL,NULL);
    if(contact!=NULL)//已经存在的联系人，发回响应消息，并且标示联系人在线
    {
        if(reply) //如果收到的是+++,则发回响应消息，如果收到的是+-+,则不发回响应消息
        {
            sendmessage(number,"+-+");
        }
        for(int i=0;i<allMessages.count();i++)//查看是否存在对方的离线消息
        {
            if(allMessages[i].number==number&&allMessages[i].from==4)
            {
                sendmessage(number,"###"+allMessages[i].content); //发送离线消息
                allMessages[i].from=0;  //将信息从草稿箱转移到发件箱
                allMessages[i].now=0;
            }
        }
        contact->onLine=true;  //设置为在线
        QTreeWidgetItem *item=finditem(number);
        for(int i=0;i<childBlinkItems.count();i++)//闪烁时，设置为在线时的图标
        {
            if(childBlinkItems[i]->getitem()==item)
                childBlinkItems[i]->setonlineicon();
        }
        item->setIcon(0,QIcon(":/icon/icons/user.png"));
        return;

    }
    else//未知联系人,只发回响应消息
    {
        if(reply)
            sendmessage(number,"+-+");
    }
}

void MessageWind::contact_offline(const QString &number)//联系人离线
{
    Contact *contact=indexofcontact(number,NULL,NULL);
    if(contact!=NULL)  //只标示联系人离线
    {
        contact->onLine=false;
        QTreeWidgetItem *item=finditem(number);
        for(int i=0;i<childBlinkItems.count();i++)//闪烁时，设置为离线时的图标
        {
            if(childBlinkItems[i]->getitem()==item)
                childBlinkItems[i]->setofflineicon();
        }
        item->setIcon(0,QIcon(":/icon/icons/user_offline.png"));
        return;
    }
}

Contact MessageWind::texttocontact(const QString &text) //提取文本中的联系人信息
{
    Contact contact;
    int position=text.indexOf(":");
    contact.name=text.left(position);
    contact.number=text.mid(position+1,text.length()-position);
    return contact;
}

void MessageWind::sendmessage(const QString &number,const QString &content)  //回发在线的响应消息
{
    QTcpSocket sender;
    sender.connectToHost(number,1573);
    if(sender.waitForConnected(500)) // 等待连接的时间为500ms
    {
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out<<quint16(0)<<("<Sms><Number>"+myNumber+"</Number><Content>"+content+"</Content></Sms>");
    out.device()->seek(0);
    out<<quint16(block.size()-sizeof(quint16));
    sender.write(block);
    sender.waitForBytesWritten(2000);
    sender.close();
    }
}

int MessageWind::totalunreadsms() //未读短信条数
{
    int total=0;
    for(int i=0;i<unreadsms.count();i++)
    {
        if(unreadsms[i].now!=2)
            total++;
    }
    return total;
}

int MessageWind::totalunreadrubishsms()//未读垃圾短信条数
{
    int total=0;
    for(int i=0;i<allMessages.count();i++)
    {
        if(allMessages[i].now==2&&allMessages[i].from==3)  //具有敏感词汇，且未读的短信
            total++;
    }
    for(int i=0;i<unreadsms.count();i++)
    {
        if(unreadsms[i].now==2)  //来自黑名单的短信
            total++;
    }
    return total;
}

QTreeWidgetItem* MessageWind::finditem(const QString &number) //根据号码查找对应的联系人的子节点
{
    int i,j;
    for(i=0;i<ui->contactsTreeWidget->topLevelItemCount();i++)
        for(j=0;j<ui->contactsTreeWidget->topLevelItem(i)->childCount();j++)
        {
            QTreeWidgetItem *item=ui->contactsTreeWidget->topLevelItem(i)->child(j);
            Contact contact=texttocontact(item->whatsThis(0));
            if(contact.number==number)
                return item;
        }
    return NULL;
}



void MessageWind::on_action_Find_triggered()  // c查找联系人
{
    FindContact fc;
        if(fc.exec() == QDialog::Accepted)
            {
                QString findName = fc.findText();
                QTreeWidgetItem * findItem = NULL;
                for(int i = 0; i < ui->contactsTreeWidget->topLevelItemCount(); i++)
                {
                    for(int j = 0; j < ui->contactsTreeWidget->topLevelItem(i)->childCount(); j++)
                    {
                        QString text = ui->contactsTreeWidget->topLevelItem(i)->child(j)->whatsThis(0);
                        if(text.contains(findName))
                        {
                            findItem = ui->contactsTreeWidget->topLevelItem(i)->child(j);
                            break;
                        }
                    }
                    if(findItem)    //若findItem不为空，说明找到
                        break;
                }

                if(!findItem)
                {
                    QMessageBox::information(this, tr("没有找到联系人"),tr("电话本中没有 %1").arg(findName));
                    return;
                }else{
                    ui->contactsTreeWidget->setCurrentItem(findItem);  //将光标这是为查找到的联系人
                }
            }
}

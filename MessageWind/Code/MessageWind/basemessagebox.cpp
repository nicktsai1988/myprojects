#include "basemessagebox.h"
#include "ui_basemessagebox.h"
#include "lookmessage.h"
#include <QMenu>
#include "lookmessage.h"
#include "newmessage.h"

#include <QMessageBox>
#include "replythemessage.h"

BaseMessageBox::BaseMessageBox(int w, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseMessageBox)
{

    ui->setupUi(this);
    setFixedSize(width(),height());
    which = w;
    switch(which){
    case -1: setWindowTitle(tr("草稿箱"));
             connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_actionEdit_triggered()));
             break;
    case 0: setWindowTitle(tr("发件箱"));
            connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_actionLook_triggered()));
            break;
    case 1: setWindowTitle(tr("收件箱"));
            connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_actionReply_triggered()));
            break;
    case 2: setWindowTitle(tr("垃圾箱"));
            connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_actionLook_triggered()));
            break;
    }  
    update();
}

BaseMessageBox::~BaseMessageBox()
{
    delete ui;
}

void BaseMessageBox::update()
{
    ui->listWidget->clear();
    for(int i=0;i < unreadsms.count();i++)
    {
        if(unreadsms[i].now==which&&(which==1||which==2))
        {
            QListWidgetItem *item=new QListWidgetItem("*" + numbersToNames(unreadsms[i].number)+ "#" + unreadsms[i].content);
            item->setWhatsThis(unreadsms[i].time.toString(dateTimeFormat));
            ui->listWidget->addItem(item);
        }
    }
    for(int i = 0; i < allMessages.size(); i++)
    {
        if(allMessages[i].now == which)     //选择属于which信息箱内的文件
        {
            QListWidgetItem *item;
            if(allMessages[i].from == 3)    //显示垃圾箱里面的短信未读
                item=new QListWidgetItem("*" + numbersToNames(allMessages[i].number)+ "#" + allMessages[i].content);
            else
                item=new QListWidgetItem(numbersToNames(allMessages[i].number)+ "#" + allMessages[i].content);
            item->setWhatsThis(allMessages[i].time.toString(dateTimeFormat));
            ui->listWidget->addItem(item);
        }
    }
    ui->label->setText(tr("总数：%1").arg(ui->listWidget->count()));
}

int BaseMessageBox::analysis(QListWidgetItem *item,QList<Message> **list)
{
    QString str = item->text();
    int position = str.indexOf("#");
    QString tempNames = str.left(position);     //这里在查找时，要保证号码和内容均相同
    QString tempContent = str.right(str.size()-position-1);
    if(tempNames.startsWith("*"))
        tempNames= tempNames.mid(1);
    QString tempNumbers = namesToNumbers(tempNames);
    QString datetime=item->whatsThis();
    for(int i = 0; i < allMessages.size(); i++)
        if(allMessages[i].number == tempNumbers && allMessages[i].content == tempContent&& allMessages[i].time.toString(dateTimeFormat)==datetime)
        {
            *list=&allMessages;
            return i;
        }
    for(int i=0;i<unreadsms.size();i++)
        if(unreadsms[i].number == tempNumbers && unreadsms[i].content == tempContent&& unreadsms[i].time.toString(dateTimeFormat)==datetime)
        {
            *list=&unreadsms;
            return i;
        }
    *list==NULL;
    return -1;
}

void BaseMessageBox::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu popMenu(this);
    if(ui->listWidget->itemAt(ui->listWidget->viewport()->mapFromGlobal(QCursor::pos())))   //设定各个信箱所支持的操作
    {
        switch(which){
        case -1:
            popMenu.addAction(ui->actionEdit);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionRead);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionMove);
            break;
        case 0:
            popMenu.addAction(ui->actionLook);
            popMenu.addAction(ui->actionSend);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionRead);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionMove);
            break;
        case 1:
            popMenu.addAction(ui->actionLook);
            popMenu.addAction(ui->actionReply);
            popMenu.addAction(ui->actionSend);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionRead);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionMove);
            break;
         case 2:
            popMenu.addAction(ui->actionLook);
            popMenu.addAction(ui->actionBack);
            popMenu.addSeparator();
            popMenu.addAction(ui->actionRead);
            popMenu.addSeparator();
        }
        popMenu.addAction(ui->actionDelete);    
    }
    popMenu.addAction(ui->actionDeleteALL);

    popMenu.exec(QCursor::pos());
}

void BaseMessageBox::on_actionLook_triggered()  //信息查看：发件箱，收件箱，垃圾箱
{
    QListWidgetItem * curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);
    LookMessage lm((*list)[i]);
    if(list==&unreadsms)  //如果是未读消息，则将其移动到对应的短信箱子
    {

        Message temp=unreadsms[i];
        //////////////////////////
        //qDebug()<<temp.number+":"+temp.content;
        unreadsms.removeAt(i);
        temp.from=temp.now;
        allMessages.prepend(temp);
    }
    else //消息在allmessages中
    {
        if((*list)[i].from==3)    //将收到的该消息标记为已读
        (*list)[i].from=(*list)[i].now;
    }
    lm.exec();
    update();
}

void BaseMessageBox::on_actionEdit_triggered()    //编辑：草稿箱
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);
    NewMessage sm(i,list);
    sm.exec();
    update();
}

void BaseMessageBox::on_actionSend_triggered()      //转发：发件箱，收件箱
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);
    NewMessage sm("",((*list)[i]).content);
    sm.exec();
    if(list==&unreadsms)  //如果是未读消息，则将其移动到对应的短信箱子
    {
        Message temp=unreadsms[i];
        unreadsms.removeAt(i);
        temp.from=temp.now;
        allMessages.prepend(temp);
    }
    else //消息在allmessages中
    {
        if((*list)[i].from==3)    //将收到的该消息标记为已读
        (*list)[i].from=(*list)[i].now;
    }
    update();
}

void BaseMessageBox::on_actionReply_triggered()     //回复：收件箱
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);
    Message curMessage = (*list)[i];
    ReplyTheMessage rm(curMessage);

    rm.exec();
    if(list==&unreadsms)  //如果是未读消息，则将其移动到对应的短信箱子
    {
        Message temp=unreadsms[i];
        unreadsms.removeAt(i);
        temp.from=temp.now;
        allMessages.prepend(temp);
    }
    else //消息在allmessages中
    {
        if((*list)[i].from==3)    //将收到的该消息标记为已读
        (*list)[i].from=(*list)[i].now;
    }
    update();
}

void BaseMessageBox::on_actionRead_triggered()      //朗读：草稿箱，发件箱，收件箱，垃圾箱
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);

    read.setContent((*list)[i].content);
    if(read.isRunning())
        QMessageBox::information(this, tr("警告"),tr("正在朗读，请稍后..."));
    else
        read.start();

}

void BaseMessageBox::on_actionMove_triggered()     //移动到垃圾箱：草稿箱，发件箱，收件箱
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);

    if((*list)[i].now == 1)
        study((*list)[i]);

    if(list==&unreadsms)  //如果是未读消息，则将其移动到对应的短信箱子
    {
        Message temp=unreadsms[i];
        unreadsms.removeAt(i);
        temp.from=temp.now;
        temp.now=2;
        allMessages.prepend(temp);
    }
    else //消息在allmessages中
    {
        if((*list)[i].from==3)    //将收到的该消息标记为已读
        (*list)[i].from=(*list)[i].now;
        (*list)[i].now=2;
    }
    //////////////分析用户放入垃圾箱的信息，实现自动学习
    update();
}

void BaseMessageBox::on_actionDelete_triggered()    //删除当前信息
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);

    list->removeAt(i);
    update();
}

void BaseMessageBox::on_actionDeleteALL_triggered()     //删除所有信息
{
    for(int i = 0; i < allMessages.size(); i++)
    {
        if(allMessages[i].now == which)
        {
            allMessages.removeAt(i);
            i--;
        }
    }
    for(int i=0;i<unreadsms.count();i++)
    {
        if(unreadsms[i].now=which)
        {
            unreadsms.removeAt(i);
            i--;
        }
    }
    update();
}

void BaseMessageBox::on_actionBack_triggered()      //垃圾箱中 还原操作
{
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    QList<Message> *list;
    int i = analysis(curItem,&list);
    if(list==(&unreadsms)&&((*list)[i].now==2))
            (*list)[i].now=1;
    else
        if((*list)[i].from!=3)
            (*list)[i].now = (*list)[i].from;
    update();
}

void BaseMessageBox::on_exitButton_clicked()
{
    if(read.isRunning())
    {
        read.stop();
        read.wait();
    }
    close();
}

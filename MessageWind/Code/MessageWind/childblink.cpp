#include "childblink.h"
#include <QTreeWidgetItem>
#include <QMessageBox>


ChildBlink::ChildBlink(QTreeWidgetItem *item):Blink(item)
{
    icon=item->icon(0);
    timer.start();
    parentadd();
    connect(&timer,SIGNAL(timeout()),this,SLOT(changeicon()));
}
ChildBlink::~ChildBlink()
{
    reset();
    parentdec();
}

void ChildBlink::changeicon()
{
    if(display)
    {
    item->setIcon(0,icon);
    item->setText(0,text);
    }
    else
    {
        item->setIcon(0,QIcon(":/icon/icons/user2.png"));
        item->setText(0,QString::null);
    }
    display=!display;

}

QString ChildBlink::getnumber()
{
    QString text=item->whatsThis(0);
    int position=text.indexOf(":");
    return text.mid(position+1,text.length()-position);
}

void ChildBlink::reset()
{
    item->setIcon(0,icon);
    item->setText(0,text);
}

void ChildBlink::parentadd()
{
    QTreeWidgetItem *parent=getparentitem();
    QString totalstr=parent->whatsThis(0);
    int total=totalstr.toInt();
    total++;
    parent->setWhatsThis(0,QString::number(total));
}

void ChildBlink::parentdec()
{
    QTreeWidgetItem *parent=getparentitem();
    QString totalstr=parent->whatsThis(0);
    int total=totalstr.toInt();
    total--;
    if(total<0)
    {
       total=0;
    }
    parent->setWhatsThis(0,QString::number(total));
}

void ChildBlink::setitem(QTreeWidgetItem *item)
{
    timer.stop();
    parentdec();
    this->item=item;
    parentadd();
    timer.start();
}

void ChildBlink::settext(const QString &text)
{
    this->text=text;
}

void ChildBlink::setonlineicon()
{
    icon=QIcon(":/icon/icons/user.png");
}

void ChildBlink::setofflineicon()
{
    icon=QIcon(":/icon/icons/user_offline.png");
}


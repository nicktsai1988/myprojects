#include "parentblink.h"
#include <QTreeWidgetItem>

ParentBlink::ParentBlink(QTreeWidgetItem *item):Blink(item)
{
    timer.stop();
    connect(&timer,SIGNAL(timeout()),this,SLOT(changeicon()));
}
ParentBlink::~ParentBlink()
{
    reset();
}

void ParentBlink::changeicon()
{
    if(display)
    {
    item->setText(0,text);
    }
    else
    {
        item->setText(0,QString::null);
    }
    display=!display;

}
void ParentBlink::reset()
{
    item->setText(0,text);
}


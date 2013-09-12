#include "blink.h"
#include <QTreeWidgetItem>

//#include <QTimer>

Blink::Blink(QTreeWidgetItem *item, QObject *parent) :
    QObject(parent)
{
    this->item=item;
    text=item->text(0);
    timer.setInterval(500);
    timer.setSingleShot(false);
    display=false;
}

Blink::~Blink()
{
}

QTreeWidgetItem* Blink::getitem()
{
    return item;
}
QTreeWidgetItem* Blink::getparentitem()
{
    return item->parent();
}
void Blink::startblink()
{
    display=false;
    timer.start();
}

void Blink::stopblink()
{
    timer.stop();
    reset();
}

void Blink::setitem(QTreeWidgetItem *item)
{
    this->item=item;
}



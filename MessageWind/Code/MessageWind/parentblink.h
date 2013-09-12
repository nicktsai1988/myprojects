#ifndef PARENTBLINK_H
#define PARENTBLINK_H
#include "blink.h"

class ParentBlink:public Blink
{
    Q_OBJECT
public:
    ParentBlink(QTreeWidgetItem *);
    virtual ~ParentBlink();
protected slots:
    virtual void changeicon(); //重写基类的方法
protected:
    virtual void reset();
};

#endif // PARENTBLINK_H

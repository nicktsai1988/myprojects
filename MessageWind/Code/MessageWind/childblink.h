#ifndef CHILDBLINK_H
#define CHILDBLINK_H
#include "blink.h"
#include "QIcon"

class ChildBlink:public Blink
{
    Q_OBJECT
public:
    ChildBlink(QTreeWidgetItem *);
    virtual ~ChildBlink();
    virtual void setitem(QTreeWidgetItem *);
    QString getnumber();          //返回其号码
    void settext(const QString&);  //闪烁的内容
    void setonlineicon();  //设置为上线时的图标
    void setofflineicon();  // 设置为下线的图标
protected slots:
    virtual void changeicon();
protected:
    virtual void parentadd();
    virtual void parentdec();
    virtual void reset();
private:
    QIcon icon;
};

#endif // CHILDBLINK_H

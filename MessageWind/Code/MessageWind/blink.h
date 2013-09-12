/*
  这个是父节点和子节点的闪烁的基类，子类必须实现其纯虚函数


  */

#ifndef BLINK_H
#define BLINK_H

#include <QObject>
#include <QTimer>
class QTreeWidgetItem;
class Blink : public QObject
{
    Q_OBJECT
public:
    explicit Blink(QTreeWidgetItem *,QObject *parent = 0);
    virtual ~Blink();
    QTreeWidgetItem* getitem();       //返回item
    virtual void setitem(QTreeWidgetItem *);  //更新item
    QTreeWidgetItem* getparentitem(); //返回item的父节点
    void startblink(); //开始闪烁
    void stopblink();   // 停止闪烁

signals:
    
protected slots:
    virtual void changeicon()=0;
protected:
    virtual void reset()=0;
    QTreeWidgetItem *item;
    QTimer timer;
    bool display;
    QString text;
};

#endif // BLINK_H

/*

  这个是增加新联系人的类
  点击增加联系人的按钮，可以增加联系人
  若联系人名字或者号码已经存在，则添加错误
  并且给出错误原因

  */


#ifndef NEWCONTACT_H
#define NEWCONTACT_H

#include <QDialog>
#include "data.h"

namespace Ui {
    class NewContact;
}

class NewContact : public QDialog
{
    Q_OBJECT

public:
    explicit NewContact(QWidget *parent = 0);
    ~NewContact();
   QList<Contact>* getnewcontacts();
private slots:
    void OkButtonClicked();     //确定按钮点击后调用

private:
    bool contatcCorrect(const Contact &);  //判断联系人和号码是否填写正确
    bool iscontainedperson(const QString &); //看看当前新联系人的姓名是否已经存在
    bool iscontainednumber(const QString &);//看看当前新联系人的号码是否已存在
    Ui::NewContact *ui;
    QList<Contact> newcontacts;
};

#endif // NEWCONTACT_H

/*
  该类用于编辑联系人并且保存
  */


#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "data.h"
namespace Ui {
    class EditContact;
}

class EditContact : public QDialog
{
    Q_OBJECT

public:
    explicit EditContact(Contact *,QWidget *parent = 0);
    ~EditContact();
protected slots:
    void editButtonClicked();//编辑联系人并保存

private:
    bool contatcCorrect(const Contact &);  //判断联系人和号码是否填写正确
    bool iscontainedperson(const QString &); //看看当前新联系人的姓名是否已经存在
    bool iscontainednumber(const QString &);//看看当前新联系人的号码是否已存在
    Ui::EditContact *ui;
    Contact *contact;
    bool editButtonClickednum;
    Contact precontact;

};

#endif // DIALOG_H


/*
  该类用于编辑联系人并且保存
  */

#include "editcontact.h"
#include "ui_editcontact.h"
#include <QMessageBox>
#include "data.h"

EditContact::EditContact(Contact *mycontact,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditContact)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("编辑联系人"));
    this->setFixedSize(width(),height());
    ui->nameEdit->setEnabled(false);
    ui->numberEdit->setEnabled(false);
    this->contact=mycontact;
    editButtonClickednum=true;
    ui->nameEdit->setText(contact->name);
    ui->numberEdit->setText(contact->number);
    precontact.name=contact->name;
    precontact.number=contact->number;
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(editButtonClicked()));
}

EditContact::~EditContact()
{
    delete ui;
}

void EditContact::editButtonClicked()   //修改联系人信息，并且保存
{
    if(editButtonClickednum==true)
    {
        ui->nameEdit->setEnabled(true);
        ui->numberEdit->setEnabled(true);
        ui->editButton->setText(tr("保存"));
        editButtonClickednum=false;
    }
    else
    {    
        Contact tempcontact;
        tempcontact.name=ui->nameEdit->text();
        tempcontact.number=ui->numberEdit->text();
        if(contatcCorrect(tempcontact)&&!iscontainedperson(tempcontact.name)&&!iscontainednumber(tempcontact.number))
        {
            contact->name=ui->nameEdit->text();
            contact->number=ui->numberEdit->text();
            ui->nameEdit->setEnabled(false);
            ui->numberEdit->setEnabled(false);
            editButtonClickednum=true;
            ui->editButton->setText(tr("编辑"));


        }

        else
            if(!contatcCorrect(tempcontact))
            {
                QMessageBox::critical(this,tr("错误"),tr("信息填写错误！"));
            }
            else
                if(iscontainedperson(tempcontact.name))
                {
                    QMessageBox::critical(this,tr("错误"),tempcontact.name+tr("已存在！"));
                }
                else
                {
                    QMessageBox::critical(this,tr("错误"),tempcontact.number+tr("已存在！"));
                }
    }
}

bool EditContact::contatcCorrect(const Contact &contact) //判断新联系人姓名或者号码是否填写正确
{
    if(!contact.name.isEmpty()&&!contact.number.isEmpty())
        return true;
    else
        return false;
}

bool EditContact::iscontainedperson(const QString &name)//判断姓名是否已存在
{
    int i;
    for(i=0;i<allContacts.count();i++)
    {
        if(allContacts[i].name!=precontact.name&&allContacts[i].name==name)
            return true;
    }
    for(i=0;i<strangers.count();i++)
    {
        if(strangers[i].name!=precontact.name&&strangers[i].name==name)
            return  true;
    }
    return false;
}

bool EditContact::iscontainednumber(const QString &number)//判断号码是否已存在
{
    int i;
    for(i=0;i<allContacts.count();i++)
    {
        if(allContacts[i].number!=precontact.number&&allContacts[i].number==number)
            return true;
    }
    for(i=0;i<strangers.count();i++)
    {
        if(strangers[i].number!=precontact.number&&strangers[i].number==number)
            return  true;
    }
    return false;
}


#include "chat.h"
Chat::Chat(const Contact &contact)
{
    chatdialog.setcontact(contact);
    this->closed=false;
    chatdialog.show();
    connect(&chatdialog,SIGNAL(chatfinished()),this,SLOT(on_chat_dialog_closed()));
}

Chat::~Chat()
{

}


void Chat::on_chat_dialog_closed()
{
    closed=true;
    emit have_chat_dialog_closed();
}

bool Chat::dialog_have_closed()
{
    return closed;
}

Contact& Chat::getcontact()
{
    return chatdialog.getcontact();
}

void Chat::activateWindow()
{
    chatdialog.setWindowState(Qt::WindowNoState);
    chatdialog.activateWindow();
}

void Chat::close()
{
    chatdialog.close();
}

void Chat::addnewmessage(const QString &name,const Message &sms)
{
    chatdialog.updatepanel(name,sms);
}
void Chat::contatonline()
{
    chatdialog.contactonline();
}

void Chat::setdark()
{
    chatdialog.setdark();
}

void Chat::setwhite()
{
    chatdialog.setwhite();
}

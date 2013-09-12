#ifndef NEWMESSAGE_H
#define NEWMESSAGE_H

#include <QDialog>
#include <QTcpSocket>
#include "data.h"

namespace Ui {
class NewMessage;
}

class NewMessage : public QDialog
{
    Q_OBJECT

public:
    explicit NewMessage(QString _name = "", QString _content = "", QWidget *parent = 0);
    NewMessage(int i, QList<Message> *list,QWidget *parent = 0);
    ~NewMessage();

private slots:
    void on_sendButton_clicked();

    void on_draftButton_clicked();

    void on_closeButton_clicked();

    void on_addButton_clicked();

private:
    Ui::NewMessage *ui;
    QTcpSocket sender;

    void sendMessage(QString & content, QStringList & addrList);

};

#endif // NEWMESSAGE_H

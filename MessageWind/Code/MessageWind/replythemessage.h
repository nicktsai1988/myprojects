#ifndef REPLYTHEMESSAGE_H
#define REPLYTHEMESSAGE_H

#include "data.h"
#include <QDialog>
#include <QTcpSocket>


namespace Ui {
class ReplyTheMessage;
}

class ReplyTheMessage : public QDialog
{
    Q_OBJECT

public:
    explicit ReplyTheMessage(const Message &m, QWidget *parent = 0);
    ~ReplyTheMessage();

private slots:
    void on_replyButton_clicked();

    void on_exitButton_clicked();

private:
    Ui::ReplyTheMessage *ui;
    Message curMessage; //当前要对该条信息进行回复
    QTcpSocket sender;

    void sendMessage(const QString & content);
    void replayButtonClicked();
    void sendButtonClicked();

};

#endif // REPLYMESSAGE_H

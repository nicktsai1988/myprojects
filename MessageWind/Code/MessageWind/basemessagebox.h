#ifndef BaseMessageBox_H
#define BaseMessageBox_H

#include <QDialog>
#include "data.h"
#include <QListWidgetItem>
#include <QThread>
#include "readthread.h"

namespace Ui {
class BaseMessageBox;
}

class BaseMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit BaseMessageBox(int w, QWidget *parent = 0);
    ~BaseMessageBox();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void on_actionLook_triggered();

    void on_actionEdit_triggered();

    void on_actionSend_triggered();

    void on_actionReply_triggered();

    void on_actionRead_triggered();

    void on_actionMove_triggered();

    void on_actionDelete_triggered();

    void on_actionDeleteALL_triggered();

    void on_actionBack_triggered();

    void on_exitButton_clicked();

private:
    Ui::BaseMessageBox *ui;
    int which;      //which指示当前信箱：-1为草稿箱，0为发件箱，1为收件箱，2为垃圾箱
    ReadThread read;    //read用于读新信息，不要定义为局部变量，否则会过早释放

    void update();
    int analysis(QListWidgetItem * item,QList<Message> **);    //分析该信息项，返回它在allMessages中的存储下标
};

#endif // BaseMessageBox_H

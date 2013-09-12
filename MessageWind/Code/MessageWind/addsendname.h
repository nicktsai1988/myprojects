#ifndef ADDSENDNAME_H
#define ADDSENDNAME_H

#include <QDialog>
#include "data.h"
#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
class AddSendName;
}

class AddSendName : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddSendName(QString str, QWidget *parent = 0); //str表示已经添加了的姓名
    ~AddSendName();

    QStringList getAllName();
    
private slots:
    void on_addButton_clicked();

    void on_moveButton_clicked();

    void on_confirmButton_clicked();

private:
    Ui::AddSendName *ui;

    QStringList sendName;
};

#endif // ADDSENDNAME_H

#ifndef SETMYINFO_H
#define SETMYINFO_H

#include <QDialog>
#include "data.h"

namespace Ui {
class SetMyInfo;
}

class SetMyInfo : public QDialog
{
    Q_OBJECT

public:
    explicit SetMyInfo(QWidget *parent = 0);
    ~SetMyInfo();

private slots:
    void on_confirmButton_clicked();

    void on_exitButton_clicked();

private:
    Ui::SetMyInfo *ui;
};

#endif // SETMYINFO_H

#ifndef SETTING_H
#define SETTING_H

#include <QDialog>
#include "setmyinfo.h"

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

private slots:
    void on_myInfoButton_clicked();

    void on_badwordsButton_clicked();

    void on_importMessageButton_clicked();

    void on_closeButton_clicked();

    void on_rubbishButton_clicked();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H

#ifndef BADWORDS_H
#define BADWORDS_H

#include <QDialog>
#include "data.h"

namespace Ui {
class BadWords;
}

class BadWords : public QDialog
{
    Q_OBJECT
    
public:
    explicit BadWords(QWidget *parent = 0);
    ~BadWords();
    
private slots:
    void on_addButton_clicked();

    void on_exitButton_clicked();

    void on_moveButton_clicked();

private:
    Ui::BadWords *ui;

    void updateBadwordsListWidget();
};

#endif // BADWORDS_H

#ifndef NEWBADWORDS_H
#define NEWBADWORDS_H

#include <QDialog>

namespace Ui {
class NewBadwords;
}

class NewBadwords : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewBadwords(QWidget *parent = 0);
    ~NewBadwords();

    QString getNewWords();
    
private slots:
    void on_confirmButton_clicked();

private:
    Ui::NewBadwords *ui;
};

#endif // NEWBADWORDS_H

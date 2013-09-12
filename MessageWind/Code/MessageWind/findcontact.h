#ifndef FINDCONTACT_H
#define FINDCONTACT_H

#include <QDialog>

namespace Ui {
class FindContact;
}

class FindContact : public QDialog
{
    Q_OBJECT
    
public:
    explicit FindContact(QWidget *parent = 0);
    ~FindContact();

    QString findText();
    
private slots:
    void on_findButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::FindContact *ui;
};

#endif // FINDCONTACT_H

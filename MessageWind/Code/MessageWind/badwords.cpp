#include "badwords.h"
#include "ui_badwords.h"
#include "newbadwords.h"
#include <QListWidgetItem>
#include <QMessageBox>

BadWords::BadWords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BadWords)
{
    ui->setupUi(this);
    setWindowTitle(tr("设置过滤词"));
    setFixedSize(width(),height());

    updateBadwordsListWidget();
}

BadWords::~BadWords()
{
    delete ui;
}

void BadWords::updateBadwordsListWidget()
{
    ui->badwordsListWidget->clear();
    for(int i = 0; i < badWords.size(); i++)
        ui->badwordsListWidget->addItem(badWords[i]);
}

void BadWords::on_addButton_clicked()
{
    NewBadwords nbw;
    nbw.exec();
    badWords.append(nbw.getNewWords());

    updateBadwordsListWidget();
}

void BadWords::on_exitButton_clicked()
{
    close();
}


void BadWords::on_moveButton_clicked()
{
    QListWidgetItem * curItem = ui->badwordsListWidget->currentItem();
    if(curItem == NULL)
    {
        QMessageBox::information(this, tr("警告"),tr("没有选中过滤词"));
        return;
    }
    QString str = curItem->text();
    badWords.removeAll(str);

    updateBadwordsListWidget();
}

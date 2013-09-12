#include "setting.h"
#include "ui_setting.h"
#include "basemessagebox.h"
#include "badwords.h"
#include "readfilename.h"
#include <QFileDialog>
#include "importsms.h"

Setting::Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    setWindowTitle(tr("设置"));
    setFixedSize(width(),height());
}

Setting::~Setting()
{
    delete ui;
}

void Setting::on_myInfoButton_clicked()
{
    SetMyInfo myinfo;
    myinfo.exec();
}

void Setting::on_rubbishButton_clicked()
{
    BaseMessageBox rubbishBox(2);
    rubbishBox.exec();
}

void Setting::on_badwordsButton_clicked()
{
    BadWords bw;
    bw.exec();
}

void Setting::on_importMessageButton_clicked()
{
    /*
    QString fileName = QFileDialog::getOpenFileName(this, tr("短信文件"),"", tr("file(*.xml);; all file(*)"));
    if(fileName.isEmpty())
        return;
    else
    {
        ReadFileName rf;
        rf.readMessageFile(fileName);
    }
    */
    ImportSms import;
    import.exec();
}

void Setting::on_closeButton_clicked()
{
    close();
}

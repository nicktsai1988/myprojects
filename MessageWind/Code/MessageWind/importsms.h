/*
用于从其它文件中导入短信

*/

#ifndef IMPORTSMS_H
#define IMPORTSMS_H

#include <QDialog>
#include "data.h"
#include <QList>
#include <QXmlStreamReader>

namespace Ui {
class ImportSms;
}

class ImportSms : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImportSms(QWidget *parent = 0);
    ~ImportSms();
private slots:
    void importButtonClicked();
    
private:
    bool readSmsFromFile(const QString &);//从文件中读取短信
    void readMessage();        //开始读取
    void readSms();           //提取消息
    void insertSms(Message &); //根据短信内容和号码，存放到合理的位置
    QString readElement();//提取当前元素
    Ui::ImportSms *ui;
    QXmlStreamReader reader;
};

#endif // IMPORTSMS_H

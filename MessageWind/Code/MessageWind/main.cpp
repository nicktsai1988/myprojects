#include <QtGui/QApplication>
#include "messagewind.h"
#include "chat.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");//设置字符集为UTF-8

    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    MessageWind w;
    w.show();
    //Chat chat;
    //chat.show();

    
    return a.exec();
}

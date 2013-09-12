#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QMap>

const QString dateTimeFormat="yyyy-MM-dd,HH:mm:ss";

extern QString myName;
extern QString myNumber;

struct Message
{
    QString number;
    QString content;
    QDateTime time;   //接收时间或发送时间
    int now;    //表示当前该信息位于哪个信箱内:-1为草稿箱，0为发件箱，1为收件箱，2为垃圾箱,3为未读
    int from;  //表示该信息的来源：-1为源于草稿箱，0为源于发件箱，1为源于收件箱已读,2表示垃圾箱已读,3为未读,4为离线消息
};

struct Contact
{
    QString name;
    QString number;
    bool isWhite;
    bool onLine;    //添加一个用户是否在线的标记，初始化时全部设定为false
};

extern QList<Message> allMessages;
extern QList<Message> unreadsms;
extern QStringList badWords;
extern QList<Contact> allContacts;
extern QList<Contact> strangers;    //用来表示陌生人集合

extern QMap<QString, int> directory;   //使用该中文词库进行自动学习，在系统启动时初始化
extern QMap<QString, int> numberCount;  //用于记录被放入垃圾箱中的号码出现次数，实现智能识别，在导入allMessages是初始化

int recogniseName(const QString name);    //识别名字来自白名单还是黑名单,黑名单返回-1，白名单返回1
int recogniseNumber(const QString number);    //识别号码来自黑名单还是白名单，黑名单返回-1，白名单返回1
bool hasBadWords(const QString content);

bool isRubishMessage(const Message & newMessage);  //根据号码和内容判断是不是应该放入垃圾箱

//下面两个函数实现名字和号码间的转换，若不存在，则返回相同名称，并设置isFind参数为false，
//第二个参数可以用来标记该名字或号码是否存在，若用户不关心这点，可以使用NULL来调用
QString nameToNumber(QString name, bool * isFind = NULL);
QString numberToName(QString number, bool * isFind = NULL);

//这两个函数用于转换多个名字或多个号码
QString namesToNumbers(QString names);
QString numbersToNames(QString numbers);

void study(const Message & newMessage);     //实现短信的智能分析

#endif // DATA_H

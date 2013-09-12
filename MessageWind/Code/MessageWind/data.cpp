#include"data.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "messagewind.h"

QString myName;
QString myNumber;

QList<Message> allMessages;
QStringList badWords;
QList<Message> unreadsms;
QList<Contact> allContacts;
QList<Contact> strangers;

QMap<QString, int> directory;
QMap<QString, int> numberCount;

int recogniseName(const QString name)
{
    return recogniseNumber(nameToNumber(name));
}

int recogniseNumber(const QString number)
{
    for(int i = 0; i < allContacts.size(); i++)
    {
        if(allContacts[i].number == number)
        {
            if(allContacts[i].isWhite == true)
                return 1;           //from whiteList
            else
                return -1;          //from darkList
        }
    }

    return 0;       //cannot find
}

bool hasBadWords(const QString content)
{
    for(int i = 0; i < badWords.size(); i++)
        if(content.contains(badWords[i]))
            return true;
    return false;
}

bool isRubishMessage(const Message & newMessage)
{
    if(recogniseNumber(newMessage.number) == -1)
        return true;
    if(hasBadWords(newMessage.content))
        return true;
    return false;
}

QString nameToNumber(QString name, bool *isFind)
{
    for(int i = 0; i < allContacts.size(); i++)
    {
        if(allContacts[i].name == name)
        {
            if(isFind != NULL)
                *isFind = true;
            return allContacts[i].number;
        }
    }

    for(int i=0;i<strangers.count();i++)
    {
        if(strangers[i].name == name)
        {
            if(isFind !=NULL)
                *isFind=true;
            return strangers[i].number;

        }
    }

    if(isFind != NULL)
        *isFind = false;
   return name;
}

QString numberToName(QString number, bool *isFind)
{
    for(int i = 0; i < allContacts.size(); i++)
    {
        if(allContacts[i].number == number)
        {
            if(isFind != NULL)
                *isFind = true;
            return allContacts[i].name;
        }
    }
    for(int i=0;i<strangers.count();i++)
    {
        if(strangers[i].number==number&&!strangers[i].name.isEmpty())
        {
            if(isFind!=NULL)
                *isFind=true;
            return strangers[i].name;
        }
    }
    if(isFind != NULL)
        *isFind = false;
    return number;
}

QString namesToNumbers(QString names)
{
    if(names == "")
        return "";
    QString temp;
    QStringList nameList = names.split(";", QString::SkipEmptyParts);
    for(int i = 0; i < nameList.size(); i++)
        temp += nameToNumber(nameList[i]) + ";";

    temp = temp.left(temp.length()-1);  //  去掉最后一个分号
    return temp;
}

QString numbersToNames(QString numbers)
{
    if(numbers == "")
        return "";
    QString temp;
    QStringList numberList = numbers.split(";", QString::SkipEmptyParts);
    for(int i = 0; i < numberList.size(); i++)
        temp += numberToName(numberList[i]) + ";";

    temp = temp.left(temp.length()-1);  //  去掉最后一个分号
    return temp;
}

void study(const Message & newMessage)
{

    //先对号码进行分析
        int val = numberCount.value(newMessage.number);
        if(val == 0)    //说明numberCount中还不存在该号码
            numberCount.insert(newMessage.number, 1);
        else
            numberCount[newMessage.number]++;
        if(numberCount[newMessage.number] >= 3)
        {
            QString tempName = numberToName(newMessage.number);
            QMessageBox::StandardButton rb = QMessageBox::question(NULL, QObject::tr("提示"), QObject::tr("系统检测到多次将 %1 的短信放入垃圾箱，是否将该号码放入黑名单？").arg(tempName), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if(rb == QMessageBox::Yes)
            {
                for(int i = 0; i < allContacts.size(); i++)
                {
                    if(allContacts[i].number == newMessage.number)
                    {
                        allContacts[i].isWhite = false;
                        numberCount.remove(newMessage.number);
                        return;
                    }
                }
                Contact newContact;
                newContact.number = newMessage.number;
                newContact.isWhite = false;
                newContact.onLine = true;
                numberCount.remove(newMessage.number);
                allContacts.append(newContact);
            }
            return;
        }
        QMutableMapIterator<QString,int> iter(directory);
        QString study ="";      //study存储本次学习的词语
        while(iter.hasNext())
        {
            iter.next();
            if(newMessage.content.contains(iter.key()))
            {
                iter.setValue(iter.value()+1);
                if(iter.value() >= 3)
                {
                    if(iter.key().length()>study.length())
                        study = iter.key();
                }
            }
        }
        if(study != "")     //如果study保持为“”，说明此次没有符合要求的词语
        {
            QMessageBox::StandardButton rb = QMessageBox::question(NULL, QObject::tr("提示"), QObject::tr("系统检测到多次将含有 %1 的短信放入垃圾箱，是否将该词语设为敏感词？").arg(study), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if(rb == QMessageBox::Yes)
                badWords.append(study);
            directory[study] = 0;
        }

}

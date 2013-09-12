#ifndef READFILENAME_H
#define READFILENAME_H

#include "data.h"
#include <QtXml/QXmlStreamReader>
#include <QWidget>

class ReadFileName: public QWidget
{
public:
    ReadFileName(QWidget *parent=0);

    void ReadALLFile(const QString & message, const QString & contact, const QString & filter, const QString & directoryName);
    void readMessageFile(const QString & message);
    void readContactFile(const QString & contact);
    void readFilterFile(const QString & filter);
    void readDirectoryFile(const QString & directoryFileName);
private:
    QXmlStreamReader reader;

    void readPeople();
    void readContact();

    void readMessage();
    void readSms();
    void insertMessage(Message newMessage);

    QString readElement();
};

#endif // READFILENAME_H

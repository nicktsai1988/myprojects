#ifndef WRITETOFILE_H
#define WRITETOFILE_H

#include <QWidget>
#include <QtXml/QXmlStreamWriter>
#include "data.h"

class WriteToFile : public QWidget
{
    Q_OBJECT
public:
    explicit WriteToFile(QWidget *parent = 0);

    void writeAllFile(const QString & message, const QString & contact, const QString & filter);
    void writeMessageFile(const QString & message);
    void writeContactFile(const QString & contact);
    void writeFilterFile(const QString & filter);
    
private:
    QXmlStreamWriter writer;

    void writeSms(const Message & newMessage);
    void writePeople(const Contact & newcontact);
    QString boolToQString(bool b);
};

#endif // WRITETOFILE_H

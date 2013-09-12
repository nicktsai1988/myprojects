#ifndef READTHREAD_H
#define READTHREAD_H

#include <windows.h>
#include <QThread>
#include <QAxObject>
#include "qt_windows.h"

class ReadThread : public QThread
{
    Q_OBJECT
public:
    explicit ReadThread( QObject *parent = 0);
    ~ReadThread();
    void setContent(const QString & str);
    void stop();

protected:
    void run();

private:
    QString content;

    QAxObject *speak;
    
};

#endif // READTHREAD_H

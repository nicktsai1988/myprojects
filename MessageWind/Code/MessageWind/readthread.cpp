#include "readthread.h"

ReadThread::ReadThread(QObject *parent) :
    QThread(parent)
{
}

ReadThread::~ReadThread()
{
//   delete speak;
}

void ReadThread::setContent(const QString &str)
{
    content = str;
}

void ReadThread::stop()
{
    content = "";
    speak->dynamicCall("Speak(QString,SpeechVoiceSpeakFlags)", content, 2);
}

void ReadThread::run()
{
    HRESULT r = OleInitialize(0);
        if (r != S_OK && r != S_FALSE) {
            qWarning("Qt: Could not initialize OLE (error %x)", (unsigned int)r);
    }
    speak = new QAxObject("SAPI.SpVoice");
    speak->dynamicCall("Speak(QString,SpeechVoiceSpeakFlags)", content, 1);
    OleUninitialize();
}

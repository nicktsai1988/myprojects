QT      += core gui \
        += network
CONFIG  += qaxcontainer

HEADERS += \
    parentblink.h \
    newcontact.h \
    messagewind.h \
    informthread.h \
    clientsocket.h \
    childblink.h \
    chatdialog.h \
    chat.h \
    blink.h \
    data.h \
    readfilename.h \
    writetofile.h \
    basemessagebox.h \
    badwords.h \
    addsendname.h \
    findcontact.h \
    lookmessage.h \
    newbadwords.h \
    newmessage.h \
    replythemessage.h \
    setmyinfo.h \
    setting.h \
    readthread.h \
    editcontact.h \
    importsms.h

SOURCES += \
    parentblink.cpp \
    newcontact.cpp \
    messagewind.cpp \
    informthread.cpp \
    clientsocket.cpp \
    childblink.cpp \
    chatdialog.cpp \
    chat.cpp \
    blink.cpp \
    data.cpp \
    readfilename.cpp \
    writetofile.cpp \
    main.cpp \
    basemessagebox.cpp \
    badwords.cpp \
    addsendname.cpp \
    findcontact.cpp \
    lookmessage.cpp \
    newbadwords.cpp \
    newmessage.cpp \
    replythemessage.cpp \
    setmyinfo.cpp \
    setting.cpp \
    readthread.cpp \
    editcontact.cpp \
    importsms.cpp

FORMS += \
    newcontact.ui \
    chatdialog.ui \
    messagewind.ui \
    basemessagebox.ui \
    badwords.ui \
    addsendname.ui \
    findcontact.ui \
    lookmessage.ui \
    newbadwords.ui \
    newmessage.ui \
    replythemessage.ui \
    setmyinfo.ui \
    setting.ui \
    editcontact.ui \
    importsms.ui

RESOURCES += \
    resource.qrc

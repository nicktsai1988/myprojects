TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    ../utils.c \
    ../transfileserver.c \
    ../transfile.c \
    ../daemon_init.c \
    ../client.c \
    ../filetransfer_conf.c \
    ../errorlog.c \
    ../error.c

HEADERS += \
    ../utils.h \
    ../filetransfer_conf.h \
    ../transfile.h


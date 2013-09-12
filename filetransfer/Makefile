TEST=test.o utils.o transfile.o filetransfer_conf.o error.o errlog.o
CLIENT=client.c utils.o transfile.o errorlog.o
SERVER=daemon_init.o error.o errorlog.o filetransfer_conf.o utils.o transfileserver.o transfile.o
CC=gcc
CFLAGS= -Wall -O3
INCLUDE=
LIB=
all:test client server
test:$(TSET)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB)-o test $(TEST)
client:$(CLIENT)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB)-o client $(CLIENT)
server:$(SERVER)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB)-o transfileserver $(SERVER)

clean:
	rm -fv *.o

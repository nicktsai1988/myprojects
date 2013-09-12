CONFIG = record.conf
NAME = srv_record
MAIN = srv_record.c
OTHER_SOURCE = parse.c urldecode.c urlrecord.c record_conf.c
OTHER_LIB = parse.o urldecode.o urlrecord.o record_conf.o
SO = srv_record.so
LA = srv_record.la

PREFIX = /usr/local/c-icap
DESTDIR = $(PREFIX)/lib/c_icap/
CFLAGS = -D_REENTRANT -g -O2 -Wall -D_FILE_OFFSET_BITS=64 -std=c99
INCLUDE = -I../../include
LIB = -lmysqlclient -lpthread

all:first second third forth fifth sixth
first:
	/bin/bash ../../libtool  --tag=CC   --mode=compile gcc -DHAVE_CONFIG_H -I. -I../.. -I../..   -I../../include/ -D_REENTRANT -g -O2 -Wall -D_FILE_OFFSET_BITS=64 -MT $(NAME)_la-$(NAME).lo -MD -MP -MF .deps/$(NAME)_la-$(NAME).Tpo -c -o $(NAME)_la-$(NAME).lo `test -f $(MAIN) || echo './'`$(MAIN)
second:
	gcc -DHAVE_CONFIG_H -I. -I../.. -I../.. -I../../include/ -D_REENTRANT -g -O2 -Wall -D_FILE_OFFSET_BITS=64 -MT $(NAME)_la-$(NAME).lo -MD -MP -MF .deps/$(NAME)_la-$(NAME).Tpo -c $(MAIN) -fPIC -DPIC -o .libs/$(NAME)_la-$(NAME).o
third:
	mv -f .deps/$(NAME)_la-$(NAME).Tpo .deps/$(NAME)_la-$(NAME).Plo
forth:
	/bin/bash ../../libtool --tag=CC   --mode=link gcc -I../../include/ -D_REENTRANT -g -O2 -Wall -D_FILE_OFFSET_BITS=64 -module -avoid-version  -o $(LA) -rpath $(DESTDIR) $(NAME)_la-$(NAME).lo  
fifth:
	gcc -c -fPIC $(OTHER_SOURCE) $(CFLAGS) $(INCLUDE)
sixth:
	gcc -shared  -fPIC -DPIC  .libs/$(NAME)_la-$(NAME).o $(OTHER_LIB)  $(LIB) -O2   -Wl,-soname -Wl,$(SO) -o .libs/$(SO)
clean:
	rm -f ./.libs/*
	rm -f ./.deps/*
	rm -f ./*.lo
	rm -f ./*.la
	rm -f ./*.o
install:
	cp ./.libs/$(SO) $(DESTDIR)
	cp ./$(LA) $(DESTDIR)
	@if [ ! -z $(CONFIG) ] && [ ! -e $(PREFIX)/etc/$(CONFIG) ]; \
		then \
		cp $(CONFIG) $(PREFIX)/etc/; \
	fi


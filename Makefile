# This program is free software; you can redistribute
# it and/or modify it under the terms of the GNU GPL

#### Start of system configuration section. ####
srcdir = .

SHELL = /bin/sh

CC = gcc -O
C++ = g++ -O
YACC = bison -y
INSTALL = /usr/local/bin/install -c
INSTALLDATA = /usr/local/bin/install -c -m 644

DEFS =  -DSIGTYPE=int -DDIRENT -DSTRSTR_MISSING \
        -DVPRINTF_MISSING -DBSD42 -D_GNU_SOURCE

WARNING = -Wall
CDEBUG = -g
LIBS = -L/usr/lib -L /lib
CFLAGS =   $(DEFS) $(LIBS) $(CDEBUG) -I/usr/include/json-c/ -I. -I$(srcdir) -fPIC -g
CPPFLAGS = $(CDEBUG) -I.  $(WARNING)
LDFLAGS = -lpthread -lmysqlclient -lutil -lcurl -ljson-c

binaries := translateExample01 translateWithLocalCache
objects := $(patsubst %.c,%.o,$(wildcard *.c))
sources := $(wildcard *.c) + $(wildcard *.cpp)

all: $(binaries)


translateExample01: translateExample01.o db.o
	$(CC) -Wall -o $@ $? $(LDFLAGS)

translateWithLocalCache: translateWithLocalCache.o db.o
	$(CC) -Wall -o $@ $? $(LDFLAGS)

.c:
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -o $@ $<

.cpp:
	$(C++)  $(CPPFLAGS) -o $@ $<

clean:
	rm -f *.o $(binaries) core core.* *.a

distclean: clean
	rm -f TAGS Makefile config.status

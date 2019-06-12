PREFIX=usr/bin
D=0
PRE="requirements.txt"
SRC=$(wildcard src/*.c)
LIBS=$(wildcard lib/*.c)
LB=lib
OBJECTS=*.o
TNAME=c_gopherd
# compiler
CC=gcc
CFLAGS=-Wall -D _GNU_SOURCE
DEBUG=
#WARNFLAGS=-Wsign-compare -Wint-conversion -fno-stack-protector
LDLIBS+=-lpthread -lgcc_s
INCLUDE=-I lib
OBJECTS=$(patsubst %.c, %.o, $(SRC))
OBJECTS_LB=$(patsubst %.c, %.o, $(LIBS))
#LDFLAGS+=

#Enable debug mode: to do that set D=1
ifeq ($(D),1)
	DEBUG:=-DDEBUG
endif

#just for installation purposes
TARGET=/usr/bin

#used in compiler directives for development purposes
ifneq ($(wildcard .git/.),)
	GIT_HASH:=$(shell git describe --abbrev=4 --dirty --always --tags)
	DEFINES+=-DGIT_HASH=\"$(GIT_HASH)\"
endif


all: clean config build $(OBJECTS) $(OBJECTS_LB)
	@echo Building the c_gopherd package
	$(CC) -o ${PREFIX}/$(TNAME) $(OBJECTS) $(OBJECTS_LB) $(LDLIBS) $(INCLUDE) $(WARNFLAGS)

config:
	cp $(LB)/defaults.def.h $(LB)/defaults.h

clean:
	@echo Removing build directories
	rm -rf ${PREFIX}
	find . -name "*.o" -exec rm {} \;
	find . -name "defaults.h" -exec rm {} \;


install:
	@echo Installing the c_gopherd package on the system
	cp ${PREFIX}/c_gopherd ${TARGET}


%.o: %.c
	$(CC) -o $@ -c $< $(DEBUG) $(INCLUDE) $(WARNFLAGS)


$OBJECTS: $(SRC)
	$(CC) -c $(SRC) $(LDLIBS) $(INCLUDE) $(DEBUG) $(WARNFLAGS)


$OBJECTS_LB: $(OBJECTS_LB)
	$(CC) -c $(OBJECTS_LB) $(LDLIBS) $(INCLUDE) $(DEBUG) $(WARNFLAGS)


build:
	@mkdir -p ${PREFIX}


executable:
	$(CC) $(SRC) -o $(PREFIX)/c_gopherd $(LDLIBS) $(INCLUDE) $(DEBUG)


.PHONY: install

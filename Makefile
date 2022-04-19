# imgtool makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
IDIR=-I.
LIBS=-lz -lpng -ljpeg
CC=gcc
NAME=imgtool
SRC=src/*.c src/gif/*.c

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)

LPATH=$(patsubst %,lib%.a,$(NAME))
LFLAGS=-L.
LFLAGS += $(patsubst %,-l%,$(NAME))
LFLAGS += $(LIBS)

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	OSFLAGS=-dynamiclib
	LIB=lib$(NAME).dylib
else
	OSFLAGS=-shared -fPIC
	LIB=lib$(NAME).so
endif

$(LPATH): $(SRC)
	$(CC) $(CFLAGS) -c $^ && ar -cr $@ *.o && rm *.o

$(NAME): $(LPATH) $(NAME).c
	$(CC) -o $@ $(NAME).c $(CFLAGS) $(LFLAGS)

shared: $(SRC)
	$(CC) -o $(LIB) $(SRC) $(CFLAGS) $(LIBS) $(OSFLAGS)

clean: build.sh
	./$^ $@

install: build.sh
	./$^ $@

uninstall: build.sh
	./$^ $@


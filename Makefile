# imgtool makefile

NAME = imgtool

CC = gcc
STD = -std=c99
WFLAGS = -Wall -Wextra -pedantic
OPT = -O2
INC = -I.
LIBS = -lz -lpng -ljpeg

SRCDIR = src
TMPDIR = tmp
BINDIR = bin

SCRIPT = build.sh
SRC = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.o,$(SRC))

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	DLIB = -dynamiclib
	SUFFIX = .dylib
else
	DLIB = -shared -fPIC
	SUFFIX = .so
endif

TARGET = $(BINDIR)/lib$(NAME)
LIBNAME = $(TARGET)$(SUFFIX)

CFLAGS = $(STD) $(WFLAGS) $(OPT) $(INC)

$(TARGET).a: $(BINDIR) $(OBJS)
	ar -cr $@ $(OBJS)

$(NAME): $(NAME).c $(TARGET).a
	$(CC) -o $@ $< $(CFLAGS) -Lbin -l$(NAME) $(LIBS)

.PHONY: exe shared all clean install uninstall

exe: $(NAME)

shared: $(LIBNAME)

all: $(LIBNAME) $(NAME)

$(LIBNAME): $(BINDIR) $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(DLIB) -o $@ $(OBJS)

$(TMPDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): | $(TMPDIR)

$(TMPDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

clean: $(SCRIPT)
	./$^ $@

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@

INCDIR=-I/usr/X11R6/include
LIBDIR=-L/usr/X11R6/lib
COMP=gcc
CFLAGS=-g -Wall -Wextra

INCDIR+=-I/usr/local/include
LIBDIR+=-L/usr/local/lib

LIBS=-lglut -lGL -lGLU -lX11 -lm -pthread

all: glwars

glwars: glwars.c glwars.h
	$(COMP) $(CFLAGS) $(INCDIR) $(LIBDIR) -o glwars glwars.c $(LIBS) $(OPTS)

clean:
	rm glwars

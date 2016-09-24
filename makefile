INCDIR=-I/usr/X11R6/include
LIBDIR=-L/usr/X11R6/lib
COMP=gcc
CFLAGS=-g -Wall

INCDIR+=-I/usr/local/include
LIBDIR+=-L/usr/local/lib

#--Truncates floats, its grphx, no one cares----#
#OPTS=-O3

#---watch the math lib, you may not need it----#
#-pthread is needed to link libc_r
#libc_r is needed to display correctly over ssh, and on unacceled things 
LIBS=-lglut -lGL -lGLU -lX11 -lXmu -lm -pthread

all: glwars

glwars: glwars.c glwars.h
	$(COMP) $(CFLAGS) $(INCDIR) $(LIBDIR) -o glwars glwars.c $(LIBS) $(OPTS)

clean:
	rm glwars

#FreeBSD
INCDIR=-I/usr/X11R6/include
LIBDIR=-L/usr/X11R6/lib
COMP=gcc
CFLAGS=-g -Wall

#Irix-6.5
INCDIR+=-I/usr/local/include
LIBDIR+=-L/usr/local/lib
#COMP=cc
#CFLAGS=-fullwarn 

#--Truncates floats, its grphx, no one cares----#
#OPTS=-O3

#---watch the math lib, you may not need it----#
#-pthread is needed to link libc_r
#libc_r is needed to display correctly over ssh, and on unacceled things 
LIBS=-lglut -lGL -lGLU -lX11 -lXmu -lm -pthread

all: test 

socktreads.o: sockthreads.c
	$(COMP) $(CFLAGS) -c sockthreads.c -pthread

glwars.o: glwars.c sockthreads.o
	$(COMP) $(CFLAGS) $(INCDIR) $(LIBDIR) -c sockthreads.o glwars.c $(LIBS) $(OPTS)

glwars: glwars.o sockthreads.o
	$(COMP) $(CFLAGS) $(INCDIR) $(LIBDIR) -o glwars sockthreads.o glwars.o $(LIBS) $(OPTS)

test: glwars.c
	$(COMP) $(CFLAGS) $(INCDIR) $(LIBDIR) -o glwars glwars.c $(LIBS) $(OPTS)

clean:
	rm *.o

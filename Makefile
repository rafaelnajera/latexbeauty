#
# Makefile for latexbeauty 
#
INSTALLDIR = ~/bin
GCC = gcc

MAIN= latexbeauty
# Normal
CFLAGS = 
# For debug
#CFLAGS = -g
OBJECTS = error.o block.o paragraph.o beautify.o utf8.o stringstack.o
HEADERS = error.h block.h paragraph.h beautify.h utf8.h stringstack.h
CUNIT_LIB = -lcunit

all : $(MAIN) testlatexbeauty

test : testlatexbeauty
	./testlatexbeauty

install : $(MAIN)
	cp $(MAIN) $(INSTALLDIR)

latexbeauty : latexbeauty.c $(HEADERS) $(OBJECTS)
	gcc -o latexbeauty latexbeauty.c $(OBJECTS)
	
latexbeauty.o : latexbeauty.c
	gcc $(CFLAGS) -c latexbeauty.c
	
beautify.o : beautify.c $(HEADERS)
	gcc $(CFLAGS) -c beautify.c		
	
block.o : block.c $(HEADERS)
	gcc $(CFLAGS) -c block.c

paragraph.o : paragraph.c $(HEADERS)
	gcc $(CFLAGS) -c paragraph.c

error.o : error.c error.h
	gcc $(CFLAGS) -c error.c
	
utf8.o : utf8.c utf8.h
	gcc $(CFLAGS) -c utf8.c
	
stringstack.o : stringstack.c stringstack.h
	gcc $(CFLAGS) -c stringstack.c

testlatexbeauty : testlatexbeauty.c $(OBJECTS)
	gcc -o testlatexbeauty testlatexbeauty.c $(OBJECTS) $(CUNIT_LIB)
	

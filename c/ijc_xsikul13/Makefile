#  // Makefile
#  // Øešení IJC-DU1,  23.3.2014
#  // Autor: Vojtìch Šikula, FIT
#  // Pøeloženo: GNU gcc compiler
#  // popis pøíkladu - poznámky, atd
CC=gcc
CFLAGS=-O2 -std=c99 -Wall -pedantic -lm

all: prvocisla-inline prvocisla steg-decode


#prelozeni modulu
prvocisla.o: prvocisla.c bit-array.h eratosthenes.h error.h
	$(CC) $(CFLAGS) -c prvocisla.c -o prvocisla.o

eratosthenes.o: eratosthenes.c eratosthenes.h error.h bit-array.h
	$(CC) $(CFLAGS) -c eratosthenes.c -o eratosthenes.o

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o error.o

ppm.o: ppm.c ppm.h bit-array.h error.h eratosthenes.h
	$(CC) $(CFLAGS) -c ppm.c -o ppm.o

steg-decode.o: steg-decode.c ppm.h bit-array.h eratosthenes.h error.h
	$(CC) $(CFLAGS) -c steg-decode.c -o steg-decode.o

#linkovani
prvocisla: prvocisla.o eratosthenes.o error.o
	$(CC) $(CFLAGS) prvocisla.o error.o eratosthenes.o -o prvocisla

prvocisla-inline: prvocisla.c error.o eratosthenes.c 
	gcc $(CFLAGS) -DUSE_INLINE prvocisla.c eratosthenes.c error.o -o prvocisla-inline

steg-decode: steg-decode.o ppm.o error.o eratosthenes.o
	gcc $(CFLAGS) steg-decode.o error.o eratosthenes.o ppm.o -o steg-decode

